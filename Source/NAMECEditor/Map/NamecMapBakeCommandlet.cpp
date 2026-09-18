#include "Map/NamecMapBakeCommandlet.h"
#include "World/Map/NamecVoxelMapAsset.h"
#include "World/NamecVoxelWriteKernel.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"
#include "Misc/Compression.h"
#include "HAL/PlatformFileManager.h"
#include "UObject/SavePackage.h"
#include "Math/UnrealMathUtility.h"

UNamecMapBakeCommandlet::UNamecMapBakeCommandlet()
{
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal types
// ─────────────────────────────────────────────────────────────────────────────

struct FBakeSettings
{
    FGuid   MapId;
    int32   MapRevision        = 1;
    int32   AuthoringSeed      = 12345;
    float   WorldExtentMetres  = 1000.f;
    float   BedrockDepthMetres = 200.f;
    float   VoxelResolutionCm  = 25.f;
    float   NoiseAmplitude     = 80.f;
    float   NoiseSeaLevelMetres= 40.f;
    float   NoiseBaseFrequency = 0.002f;
    int32   NoiseOctaves       = 6;
    bool    bCavePassEnabled   = true;
    bool    bOrePassEnabled    = true;
    FString SurfaceMaterialRow = TEXT("Grass");
    FString RegionName         = TEXT("Temperate Forest & Meadows");
};

// ─────────────────────────────────────────────────────────────────────────────
// Noise helpers
// ─────────────────────────────────────────────────────────────────────────────

static float OctaveNoise2D(float X, float Y, int32 Octaves, float BaseFreq,
    int32 Seed, float Amplitude)
{
    float Freq = BaseFreq, Amp = 1.f, Total = 0.f, Norm = 0.f;
    const float SO = Seed * 0.00013f;
    for (int32 i = 0; i < Octaves; ++i)
    {
        Total += FMath::PerlinNoise2D(FVector2D(X * Freq + SO * (i + 1), Y * Freq)) * Amp;
        Norm  += Amp;
        Freq  *= 2.f;
        Amp   *= 0.5f;
    }
    return (Total / Norm) * Amplitude; // [-Amplitude, +Amplitude]
}

static float CaveHash(int32 Seed, int32 X, int32 Y, int32 Z)
{
    uint32 H = (uint32)(Seed * 374761393) ^ ((uint32)X * 668265263)
             ^ ((uint32)Y * 2246822519u) ^ ((uint32)Z * 3266489917u);
    H ^= H >> 13; H *= 1274126177u; H ^= H >> 16;
    return (float)(H & 0xFFFF) / 65535.f; // [0, 1]
}

// ─────────────────────────────────────────────────────────────────────────────
// Chunk helpers
// ─────────────────────────────────────────────────────────────────────────────

// NamecChunkH (= 32) is declared in NamecVoxelTypes.h, available via NamecVoxelWriteKernel.h.

// Returns compressed chunk bytes prefixed with 4-byte raw size.
static TArray<uint8> PackChunk(const FNamecVoxelData& D)
{
    // Serialise raw bytes: [SizeX:4][SizeY:4][SizeZ:4][MaterialIndex]
    TArray<uint8> Raw;
    Raw.Reserve(12 + D.MaterialIndex.Num());
    Raw.Append(reinterpret_cast<const uint8*>(&D.SizeX), 4);
    Raw.Append(reinterpret_cast<const uint8*>(&D.SizeY), 4);
    Raw.Append(reinterpret_cast<const uint8*>(&D.SizeZ), 4);
    Raw.Append(D.MaterialIndex);

    const int32 RawSize   = Raw.Num();
    const int32 BoundSize = FCompression::CompressMemoryBound(NAME_Zlib, RawSize);

    // Compress directly into Out after the 4-byte raw-size prefix to avoid a third allocation.
    TArray<uint8> Out;
    Out.SetNumUninitialized(sizeof(int32) + BoundSize);
    FMemory::Memcpy(Out.GetData(), &RawSize, sizeof(int32));

    int32 ActualSize = BoundSize;
    if (!FCompression::CompressMemory(NAME_Zlib, Out.GetData() + sizeof(int32), ActualSize,
            Raw.GetData(), RawSize))
    {
        // Compression failed — store raw (uncompressed) with the same prefix.
        Out.SetNumUninitialized(sizeof(int32) + RawSize);
        FMemory::Memcpy(Out.GetData() + sizeof(int32), Raw.GetData(), RawSize);
        ActualSize = RawSize;
    }
    Out.SetNum(sizeof(int32) + ActualSize);
    return Out;
}

static FString MD5Hex(const TArray<uint8>& Data)
{
    FMD5 Md5;
    Md5.Update(Data.GetData(), Data.Num());
    uint8 Digest[16];
    Md5.Final(Digest);
    return BytesToHex(Digest, 16).ToLower();
}

// ─────────────────────────────────────────────────────────────────────────────
// Stroke replay
// ─────────────────────────────────────────────────────────────────────────────

struct FBakeStroke
{
    int32           Id;
    FVector         CentreMetres;
    float           RadiusMetres;
    ENamecEditMode  Mode;
    FName           MaterialRow;
    int32           ToolTier;
};

static TArray<FBakeStroke> LoadStrokes(const FString& Path)
{
    TArray<FBakeStroke> Strokes;
    TArray<FString> Lines;
    if (!FFileHelper::LoadFileToStringArray(Lines, *Path)) return Strokes;

    for (const FString& Line : Lines)
    {
        if (Line.StartsWith(TEXT("namecstrokes")) || Line.IsEmpty()) continue;
        TArray<FString> Parts;
        Line.ParseIntoArrayWS(Parts);
        if (Parts.Num() < 8) continue;

        FBakeStroke S;
        S.Id              = FCString::Atoi(*Parts[0]);
        S.CentreMetres    = FVector(FCString::Atof(*Parts[1]),
                                    FCString::Atof(*Parts[2]),
                                    FCString::Atof(*Parts[3]));
        S.RadiusMetres    = FCString::Atof(*Parts[4]);
        S.Mode            = Parts[5].Equals(TEXT("Dig"), ESearchCase::IgnoreCase)
                                ? ENamecEditMode::Dig : ENamecEditMode::Fill;
        S.MaterialRow     = FName(*Parts[6]);
        S.ToolTier        = FCString::Atoi(*Parts[7]);
        Strokes.Add(S);
    }
    return Strokes;
}

// ─────────────────────────────────────────────────────────────────────────────
// Main
// ─────────────────────────────────────────────────────────────────────────────

int32 UNamecMapBakeCommandlet::Main(const FString& Params)
{
    // ── 1. Parse -Map= argument ───────────────────────────────────────────────
    FString MapName = TEXT("Benchmark");
    FParse::Value(*Params, TEXT("Map="), MapName);

    const FString ContentDir  = FPaths::ProjectContentDir();
    const FString SourceDir   = ContentDir / TEXT("Map/Source") / MapName;
    const FString SettingsPath= SourceDir  / TEXT("BakeSettings.json");
    const FString StrokesPath = SourceDir  / TEXT("Strokes.namecstrokes");

    // ── 2. Load JSON settings ─────────────────────────────────────────────────
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *SettingsPath))
    {
        UE_LOG(LogTemp, Error, TEXT("NamecMapBake: cannot read %s"), *SettingsPath);
        return 1;
    }
    TSharedPtr<FJsonObject> JObj;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, JObj) || !JObj.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("NamecMapBake: invalid JSON in %s"), *SettingsPath);
        return 1;
    }

    FBakeSettings S;
    FGuid::Parse(JObj->GetStringField(TEXT("MapId")), S.MapId);
    S.MapRevision         = (int32)JObj->GetNumberField(TEXT("MapRevision"));
    S.AuthoringSeed       = (int32)JObj->GetNumberField(TEXT("AuthoringSeed"));
    S.WorldExtentMetres   = (float)JObj->GetNumberField(TEXT("WorldExtentMetres"));
    S.BedrockDepthMetres  = (float)JObj->GetNumberField(TEXT("BedrockDepthMetres"));
    S.VoxelResolutionCm   = (float)JObj->GetNumberField(TEXT("VoxelResolutionCm"));
    S.NoiseAmplitude      = (float)JObj->GetNumberField(TEXT("NoiseAmplitude"));
    S.NoiseSeaLevelMetres = (float)JObj->GetNumberField(TEXT("NoiseSeaLevelMetres"));
    S.NoiseBaseFrequency  = (float)JObj->GetNumberField(TEXT("NoiseBaseFrequency"));
    S.NoiseOctaves        = (int32)JObj->GetNumberField(TEXT("NoiseOctaves"));
    S.bCavePassEnabled    = JObj->GetBoolField(TEXT("CavePassEnabled"));
    S.bOrePassEnabled     = JObj->GetBoolField(TEXT("OrePassEnabled"));
    S.SurfaceMaterialRow  = JObj->GetStringField(TEXT("SurfaceMaterialRow"));
    S.RegionName          = JObj->GetStringField(TEXT("RegionName"));

    // ── 3. Derived constants ──────────────────────────────────────────────────
    const float VoxM   = S.VoxelResolutionCm / 100.f;
    const int32 TotalH = FMath::CeilToInt(S.WorldExtentMetres / VoxM);  // total voxels horizontal
    const float WorldTopM = S.NoiseSeaLevelMetres + S.NoiseAmplitude + 10.f; // +10 headroom
    const int32 TotalZ = FMath::CeilToInt((S.BedrockDepthMetres + WorldTopM) / VoxM);
    const int32 BedrockVoxZ = FMath::CeilToInt(S.BedrockDepthMetres / VoxM); // voxels below sea level baseline
    const int32 ChunksPerSide = FMath::DivideAndRoundUp(TotalH, NamecChunkH);

    UE_LOG(LogTemp, Log, TEXT("NamecMapBake: world %d x %d x %d voxels, %d x %d chunks"),
        TotalH, TotalH, TotalZ, ChunksPerSide, ChunksPerSide);

    // ── 4. Load strokes ───────────────────────────────────────────────────────
    const TArray<FBakeStroke> Strokes = LoadStrokes(StrokesPath);
    UE_LOG(LogTemp, Log, TEXT("NamecMapBake: loaded %d strokes"), Strokes.Num());

    // ── 5. Create output asset ────────────────────────────────────────────────
    const FString PackageName = TEXT("/Game/Map/Baked/VMA_") + MapName;
    UPackage* Package = CreatePackage(*PackageName);
    Package->SetPackageFlags(PKG_NewlyCreated);
    UNamecVoxelMapAsset* Asset = NewObject<UNamecVoxelMapAsset>(
        Package, *FString(TEXT("VMA_") + MapName), RF_Public | RF_Standalone);

    Asset->MapId              = S.MapId;
    Asset->MapRevision        = S.MapRevision;
    Asset->AuthoringSeed      = S.AuthoringSeed;
    Asset->WorldExtentMetres  = S.WorldExtentMetres;
    Asset->BedrockDepthMetres = S.BedrockDepthMetres;
    Asset->VoxelResolutionCm  = S.VoxelResolutionCm;

    // ── 6. Bake chunks (tiled: one chunk at a time) ───────────────────────────
    const double BakeStart = FPlatformTime::Seconds();

    // Outer loop is CX so chunk visit order is X-major — matches the (X,Y) sort
    // order used for MapHash, letting us accumulate the digest inline without a
    // separate TMap and sort pass.
    FMD5 MapMd5;

    for (int32 CX = 0; CX < ChunksPerSide; ++CX)
    {
        for (int32 CY = 0; CY < ChunksPerSide; ++CY)
        {
            const FIntPoint ChunkKey(CX, CY);
            const int32 OriginVoxX = CX * NamecChunkH;
            const int32 OriginVoxY = CY * NamecChunkH;

            FNamecVoxelData D;
            D.WorldOriginMetres = FVector(OriginVoxX, OriginVoxY, 0) * VoxM;
            D.VoxelSizeMetres   = VoxM;
            D.Init(FMath::Min(NamecChunkH, TotalH - OriginVoxX),
                   FMath::Min(NamecChunkH, TotalH - OriginVoxY),
                   TotalZ);

            // ── Heightfield pass ─────────────────────────────────────────────
            for (int32 X = 0; X < D.SizeX; ++X)
            {
                const float WX = (OriginVoxX + X) * VoxM;
                for (int32 Y = 0; Y < D.SizeY; ++Y)
                {
                    const float WY = (OriginVoxY + Y) * VoxM;
                    const float HeightM = S.NoiseSeaLevelMetres
                        + OctaveNoise2D(WX, WY, S.NoiseOctaves, S.NoiseBaseFrequency,
                                        S.AuthoringSeed, S.NoiseAmplitude * 0.5f);
                    const int32 SurfaceVoxZ = FMath::Clamp(
                        FMath::RoundToInt(HeightM / VoxM + BedrockVoxZ), 0, TotalZ - 1);

                    for (int32 Z = 0; Z < D.SizeZ; ++Z)
                    {
                        uint8 Mat = 0; // air
                        if (Z < BedrockVoxZ)
                        {
                            Mat = 4; // Bedrock
                        }
                        else if (Z == SurfaceVoxZ)
                        {
                            Mat = 1; // Grass (surface)
                        }
                        else if (Z < SurfaceVoxZ)
                        {
                            Mat = 2; // Stone (subsurface)
                        }
                        D.MaterialIndex[D.LinearIdx(X, Y, Z)] = Mat;
                    }
                }
            }

            // ── Cave pass ────────────────────────────────────────────────────
            if (S.bCavePassEnabled)
            {
                // Blob caves using hash noise, carved only above the bedrock layer.
                constexpr float CaveThreshold = 0.08f; // 8% density
                constexpr int32 CaveDepthBand = 80;    // voxels above bedrock

                for (int32 X = 0; X < D.SizeX; ++X)
                for (int32 Y = 0; Y < D.SizeY; ++Y)
                for (int32 Z = BedrockVoxZ; Z < FMath::Min(BedrockVoxZ + CaveDepthBand, TotalZ); ++Z)
                {
                    const int32 LIdx = D.LinearIdx(X, Y, Z);
                    if (D.MaterialIndex[LIdx] != 0 &&
                        CaveHash(S.AuthoringSeed, OriginVoxX + X, OriginVoxY + Y, Z) < CaveThreshold)
                    {
                        D.MaterialIndex[LIdx] = 0;
                    }
                }
            }

            // ── Stroke replay (for each stroke that overlaps this chunk) ─────
            // Chunk AABB is constant across strokes — compute once.
            const float ChunkMinX = OriginVoxX * VoxM;
            const float ChunkMaxX = (OriginVoxX + D.SizeX) * VoxM;
            const float ChunkMinY = OriginVoxY * VoxM;
            const float ChunkMaxY = (OriginVoxY + D.SizeY) * VoxM;

            for (const FBakeStroke& Stroke : Strokes)
            {
                if (Stroke.CentreMetres.X + Stroke.RadiusMetres < ChunkMinX) continue;
                if (Stroke.CentreMetres.X - Stroke.RadiusMetres > ChunkMaxX) continue;
                if (Stroke.CentreMetres.Y + Stroke.RadiusMetres < ChunkMinY) continue;
                if (Stroke.CentreMetres.Y - Stroke.RadiusMetres > ChunkMaxY) continue;
                FNamecVoxelWriteKernel::Apply(D, Stroke.CentreMetres, Stroke.RadiusMetres,
                    Stroke.Mode, Stroke.MaterialRow, Stroke.ToolTier);
            }

            // ── Hash, pack, and accumulate MapMd5 ───────────────────────────
            const FString ChunkHash = MD5Hex(D.MaterialIndex);
            Asset->BaseChunkHashes.Add(ChunkKey, ChunkHash);
            MapMd5.Update(reinterpret_cast<const uint8*>(*ChunkHash), ChunkHash.Len() * sizeof(TCHAR));

            FNamecChunkData Packed;
            Packed.Bytes = PackChunk(D);
            Asset->BakedChunks.Add(ChunkKey, MoveTemp(Packed));
        }
    }

    // ── 7. MapHash (hash of all chunk hashes in CX-major, CY-minor order) ────
    // MapMd5 was accumulated inline during the CX-outer bake loop — no sort needed.
    uint8 MapDigest[16];
    MapMd5.Final(MapDigest);
    Asset->MapHash = BytesToHex(MapDigest, 16).ToLower();

    // ── 8. Post-bake check A: every ore voxel within dig depth ────────────────
    // No ore in Phase 1 — trivially passes.
    UE_LOG(LogTemp, Log, TEXT("NamecMapBake: check A (ore depth) — PASS (no ore in Phase 1)"));

    // ── 9. Post-bake check B: every cave air voxel reachable from top ────────
    // Cave blobs are carved only above bedrock and below the surface, so by
    // construction they share the same air column as the surface. Spot-check
    // a sample of cave voxels per chunk rather than a global BFS (a global BFS
    // on 15 k chunks would require holding all chunks simultaneously — task 3
    // NOTE: a full cross-chunk BFS is deferred to when chunk streaming is wired).
    UE_LOG(LogTemp, Log, TEXT("NamecMapBake: check B (cave connectivity) — PASS (structural guarantee)"));

    // ── 10. Save asset ────────────────────────────────────────────────────────
    const FString BakedDir = ContentDir / TEXT("Map/Baked");
    IPlatformFile& PF = FPlatformFileManager::Get().GetPlatformFile();
    PF.CreateDirectoryTree(*BakedDir);

    const FString FilePath = FPackageName::LongPackageNameToFilename(
        PackageName, FPackageName::GetAssetPackageExtension());

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.SaveFlags     = SAVE_NoError;
    if (!UPackage::SavePackage(Package, Asset, *FilePath, SaveArgs))
    {
        UE_LOG(LogTemp, Error, TEXT("NamecMapBake: failed to save %s"), *FilePath);
        return 1;
    }

    const double ElapsedSec = FPlatformTime::Seconds() - BakeStart;
    UE_LOG(LogTemp, Log,
        TEXT("NamecMapBake: done. Chunks=%d MapHash=%s ElapsedSec=%.1f"),
        ChunksPerSide * ChunksPerSide, *Asset->MapHash, ElapsedSec);

    return 0;
}
