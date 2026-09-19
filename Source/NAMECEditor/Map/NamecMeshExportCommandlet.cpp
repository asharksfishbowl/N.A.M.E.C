#include "Map/NamecMeshExportCommandlet.h"
#include "World/NamecVoxelChunkComponent.h"
#include "World/NamecVoxelTypes.h"
#include "World/NamecVoxelSceneProxy.h"
#include "PackedNormal.h"
#include "World/Map/NamecVoxelMapAsset.h"
#include "Misc/Compression.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshAttributes.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"

// Duplicate of the decompressor in NamecVoxelWorld.cpp — kept local so this editor module
// does not pull in or modify the runtime module's internal helpers.
static FNamecVoxelData ExportDecompressChunk(const FNamecChunkData& ChunkData,
                                              FIntPoint ChunkKey, float VoxelSizeMetres)
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
    Out.WorldOriginMetres = FVector(ChunkKey.X * NamecChunkH, ChunkKey.Y * NamecChunkH, 0)
                            * VoxelSizeMetres;
    Out.VoxelSizeMetres = VoxelSizeMetres;
    return Out;
}

static UStaticMesh* BuildStaticMesh(const FNamecVoxelMeshData& MeshData,
                                     const FString& PackageName)
{
    if (MeshData.IsEmpty()) return nullptr;

    UPackage* Package = CreatePackage(*PackageName);
    Package->FullyLoad();
    FString AssetName = FPackageName::GetLongPackageAssetName(PackageName);
    UStaticMesh* SM = NewObject<UStaticMesh>(Package, *AssetName, RF_Public | RF_Standalone);

    FStaticMeshSourceModel& SrcModel = SM->AddSourceModel();
    SrcModel.BuildSettings.bRecomputeNormals        = false;
    SrcModel.BuildSettings.bRecomputeTangents       = true;
    SrcModel.BuildSettings.bGenerateLightmapUVs     = true;
    SrcModel.BuildSettings.bBuildReversedIndexBuffer = false;

    FMeshDescription* MD = SM->CreateMeshDescription(0);
    if (!MD) return nullptr;

    FStaticMeshAttributes Attrs(*MD);
    Attrs.Register();

    auto VertexPositions = Attrs.GetVertexPositions();
    auto InstanceNormals = Attrs.GetVertexInstanceNormals();
    auto InstanceUVs     = Attrs.GetVertexInstanceUVs();
    auto GroupNames      = Attrs.GetPolygonGroupMaterialSlotNames();

    FPolygonGroupID PGId = MD->CreatePolygonGroup();
    GroupNames[PGId] = FName(TEXT("Material"));

    TArray<FVertexID> VertIds;
    VertIds.SetNumUninitialized(MeshData.Vertices.Num());
    for (int32 i = 0; i < MeshData.Vertices.Num(); ++i)
    {
        VertIds[i] = MD->CreateVertex();
        VertexPositions[VertIds[i]] = MeshData.Vertices[i].Position;
    }

    TArray<FEdgeID> NewEdges;
    for (int32 t = 0; t + 2 < MeshData.Indices.Num(); t += 3)
    {
        TArray<FVertexInstanceID> ViIds;
        ViIds.SetNumUninitialized(3);
        for (int32 c = 0; c < 3; ++c)
        {
            int32 Vi = MeshData.Indices[t + c];
            FVertexInstanceID VIId = MD->CreateVertexInstance(VertIds[Vi]);
            InstanceNormals[VIId] = MeshData.Vertices[Vi].TangentZ.ToFVector3f();
            InstanceUVs.Set(VIId, 0, MeshData.Vertices[Vi].TextureCoordinate[0]);
            ViIds[c] = VIId;
        }
        NewEdges.Reset();
        MD->CreateTriangle(PGId, ViIds, &NewEdges);
    }

    SM->CommitMeshDescription(0);

    FStaticMaterial Mat;
    Mat.ImportedMaterialSlotName = FName(TEXT("Material"));
    SM->GetStaticMaterials().Add(Mat);

    SM->Build(true);
    SM->MarkPackageDirty();

    FString FilePath = FPackageName::LongPackageNameToFilename(
        PackageName, FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), true);

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    if (!UPackage::SavePackage(Package, SM, *FilePath, SaveArgs))
    {
        UE_LOG(LogTemp, Error, TEXT("NamecMeshExport: failed to save %s"), *PackageName);
        return nullptr;
    }

    FAssetRegistryModule& AssetReg =
        FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    AssetReg.Get().AssetCreated(SM);
    return SM;
}

int32 UNamecMeshExportCommandlet::Main(const FString& Params)
{
    FString ChunkStr;
    FParse::Value(*Params, TEXT("Chunk="), ChunkStr);
    TArray<FString> Parts;
    ChunkStr.ParseIntoArray(Parts, TEXT(","), true);

    FIntPoint ChunkKey(0, 0);
    if (Parts.Num() == 2)
    {
        ChunkKey.X = FCString::Atoi(*Parts[0]);
        ChunkKey.Y = FCString::Atoi(*Parts[1]);
    }

    FSoftObjectPath Ref(TEXT("/Game/Map/Baked/VMA_Benchmark.VMA_Benchmark"));
    UNamecVoxelMapAsset* MapAsset = Cast<UNamecVoxelMapAsset>(Ref.TryLoad());
    if (!MapAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("NamecMeshExport: could not load /Game/Map/Baked/VMA_Benchmark"));
        return 1;
    }

    const FNamecChunkData* ChunkData = MapAsset->BakedChunks.Find(ChunkKey);
    if (!ChunkData)
    {
        UE_LOG(LogTemp, Error, TEXT("NamecMeshExport: chunk (%d,%d) not in VMA_Benchmark"),
               ChunkKey.X, ChunkKey.Y);
        return 1;
    }

    const float VoxSize = MapAsset->VoxelResolutionCm / 100.f;
    FNamecVoxelData VoxelData = ExportDecompressChunk(*ChunkData, ChunkKey, VoxSize);
    if (VoxelData.SizeX == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("NamecMeshExport: decompression failed for chunk (%d,%d)"),
               ChunkKey.X, ChunkKey.Y);
        return 1;
    }

    FNamecVoxelMeshData Mesh = UNamecVoxelChunkComponent::BuildMesh(VoxelData);
    if (Mesh.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("NamecMeshExport: chunk (%d,%d) produced empty mesh."),
               ChunkKey.X, ChunkKey.Y);
        return 0;
    }

    const FString OutPackage = TEXT("/Game/Benchmark/TestChunk");
    UStaticMesh* SM = BuildStaticMesh(Mesh, OutPackage);
    if (!SM) return 1;

    UE_LOG(LogTemp, Display, TEXT("NamecMeshExport: saved %s (%d verts, %d tris)"),
           *OutPackage, Mesh.Vertices.Num(), Mesh.Indices.Num() / 3);
    return 0;
}
