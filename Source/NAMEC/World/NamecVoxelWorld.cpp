#include "World/NamecVoxelWorld.h"
#include "World/Map/NamecVoxelMapAsset.h"
#include "World/NamecVoxelWriteKernel.h"
#include "World/NamecDigDepthQuery.h"
#include "Misc/Compression.h"

static FNamecVoxelData DecompressChunk(const FNamecChunkData& ChunkData, FIntPoint ChunkKey,
    float VoxelSizeMetres)
{
    constexpr int32 KRawSzBytes  = sizeof(int32);
    constexpr int32 KHeaderBytes = 12;

    FNamecVoxelData Out;
    if (ChunkData.Bytes.Num() < KRawSzBytes + KHeaderBytes) return Out;

    int32 RawSize = 0;
    FMemory::Memcpy(&RawSize, ChunkData.Bytes.GetData(), KRawSzBytes);
    TArray<uint8> Raw;
    Raw.SetNumUninitialized(RawSize);
    FCompression::UncompressMemory(NAME_Zlib,
        Raw.GetData(), RawSize,
        ChunkData.Bytes.GetData() + KRawSzBytes,
        ChunkData.Bytes.Num() - KRawSzBytes);

    if (Raw.Num() < KHeaderBytes) return Out;
    FMemory::Memcpy(&Out.SizeX, Raw.GetData(),     4);
    FMemory::Memcpy(&Out.SizeY, Raw.GetData() + 4, 4);
    FMemory::Memcpy(&Out.SizeZ, Raw.GetData() + 8, 4);

    Out.MaterialIndex = TArray<uint8>(Raw.GetData() + KHeaderBytes, Raw.Num() - KHeaderBytes);
    Out.WorldOriginMetres = FVector(ChunkKey.X * NamecChunkH, ChunkKey.Y * NamecChunkH, 0) * VoxelSizeMetres;
    Out.VoxelSizeMetres = VoxelSizeMetres;
    return Out;
}

static void MergeDeltas(TArray<FNamecVoxelDelta>& Stored, const TArray<FNamecVoxelDelta>& Incoming)
{
    for (const FNamecVoxelDelta& D : Incoming)
    {
        FNamecVoxelDelta* Ex = Stored.FindByPredicate(
            [&](const FNamecVoxelDelta& S){ return S.LinearIdx == D.LinearIdx; });
        if (Ex) Ex->Material = D.Material;
        else    Stored.Add(D);
    }
}

void UNamecVoxelWorld::AddOrUpdateChunk(FIntPoint ChunkCoord)
{
    if (!MapAssetOverride) return;
    if (!MapAssetOverride->BakedChunks.Contains(ChunkCoord)) return;
    OnTerrainEdited.Broadcast(ChunkCoord);
    // Listeners call GetChunkVoxelData(ChunkCoord) to get the current voxel state.
}

FNamecVoxelData UNamecVoxelWorld::GetChunkVoxelData(FIntPoint ChunkCoord) const
{
    if (!MapAssetOverride) return {};
    const FNamecChunkData* Packed = MapAssetOverride->BakedChunks.Find(ChunkCoord);
    if (!Packed) return {};

    const float VoxSize = MapAssetOverride->VoxelResolutionCm / 100.f;
    FNamecVoxelData Data = DecompressChunk(*Packed, ChunkCoord, VoxSize);

    if (const TArray<FNamecVoxelDelta>* Deltas = EditDeltas.Find(ChunkCoord))
        for (const FNamecVoxelDelta& D : *Deltas)
            if (Data.MaterialIndex.IsValidIndex(D.LinearIdx))
                Data.MaterialIndex[D.LinearIdx] = D.Material;

    return Data;
}

void UNamecVoxelWorld::ApplyTerrainEdit(FVector CentreMetres, float RadiusMetres,
    ENamecEditMode Mode, FName MaterialRow, int32 ToolTier,
    TArray<FIntPoint>& OutChunkCoords,
    TArray<TArray<FNamecVoxelDelta>>& OutPerChunkDeltas)
{
    if (!MapAssetOverride) return;
    if (Mode == ENamecEditMode::Dig && !IsWithinDigDepth(CentreMetres)) return;

    const float VoxSize     = MapAssetOverride->VoxelResolutionCm / 100.f;
    const float ChunkMetres = NamecChunkH * VoxSize;

    const int32 MinCX = FMath::FloorToInt((CentreMetres.X - RadiusMetres) / ChunkMetres);
    const int32 MaxCX = FMath::FloorToInt((CentreMetres.X + RadiusMetres) / ChunkMetres);
    const int32 MinCY = FMath::FloorToInt((CentreMetres.Y - RadiusMetres) / ChunkMetres);
    const int32 MaxCY = FMath::FloorToInt((CentreMetres.Y + RadiusMetres) / ChunkMetres);

    for (int32 CX = MinCX; CX <= MaxCX; ++CX)
    for (int32 CY = MinCY; CY <= MaxCY; ++CY)
    {
        const FIntPoint Coord(CX, CY);
        FNamecVoxelData Data = GetChunkVoxelData(Coord);
        if (Data.SizeX == 0) continue;

        TArray<uint8> Before = Data.MaterialIndex;
        FNamecVoxelWriteKernel::Apply(Data, CentreMetres, RadiusMetres, Mode, MaterialRow, ToolTier);

        TArray<FNamecVoxelDelta> NewDeltas;
        for (int32 i = 0; i < Before.Num(); ++i)
            if (Before[i] != Data.MaterialIndex[i])
                NewDeltas.Add({ i, Data.MaterialIndex[i] });

        if (NewDeltas.IsEmpty()) continue;

        MergeDeltas(EditDeltas.FindOrAdd(Coord), NewDeltas);
        OutChunkCoords.Add(Coord);
        OutPerChunkDeltas.Add(MoveTemp(NewDeltas));
        OnTerrainEdited.Broadcast(Coord);
    }
}

void UNamecVoxelWorld::ApplyDelta(FIntPoint ChunkCoord, const TArray<FNamecVoxelDelta>& Deltas)
{
    MergeDeltas(EditDeltas.FindOrAdd(ChunkCoord), Deltas);
    OnTerrainEdited.Broadcast(ChunkCoord);
}

void UNamecVoxelWorld::RevertArenaEdits(FVector ArenaCentreMetres, float ArenaRadiusMetres)
{
    if (!MapAssetOverride) return;
    const float VoxSize     = MapAssetOverride->VoxelResolutionCm / 100.f;
    const float ChunkMetres = NamecChunkH * VoxSize;
    const float R2          = ArenaRadiusMetres * ArenaRadiusMetres;

    TArray<FIntPoint> ToRevert;
    for (const auto& KV : EditDeltas)
    {
        // AABB vs circle overlap in the XY plane.
        const float MinX = KV.Key.X * ChunkMetres, MaxX = MinX + ChunkMetres;
        const float MinY = KV.Key.Y * ChunkMetres, MaxY = MinY + ChunkMetres;
        const float NearX = FMath::Clamp((float)ArenaCentreMetres.X, MinX, MaxX);
        const float NearY = FMath::Clamp((float)ArenaCentreMetres.Y, MinY, MaxY);
        if (FMath::Square(ArenaCentreMetres.X - NearX) +
            FMath::Square(ArenaCentreMetres.Y - NearY) <= R2)
            ToRevert.Add(KV.Key);
    }

    for (const FIntPoint& Coord : ToRevert)
    {
        EditDeltas.Remove(Coord);
        OnTerrainEdited.Broadcast(Coord); // re-mesh from baked base only
    }
}

bool UNamecVoxelWorld::IsWithinDigDepth(FVector PositionMetres) const
{
    return DigDepthQuery && DigDepthQuery->IsWithinDigDepth(PositionMetres);
}

void UNamecVoxelWorld::Load(UNamecVoxelMapAsset* MapAsset)
{
    MapAssetOverride = MapAsset;
    DigDepthQuery = NewObject<UNamecDigDepthQuery>(this);
    DigDepthQuery->MapAsset = MapAsset;
}
