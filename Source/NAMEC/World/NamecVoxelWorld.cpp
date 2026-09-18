#include "World/NamecVoxelWorld.h"
#include "World/Map/NamecVoxelMapAsset.h"
#include "World/NamecVoxelWriteKernel.h"
#include "World/NamecDigDepthQuery.h"
#include "Misc/Compression.h"

static FNamecVoxelData DecompressChunk(const FNamecChunkData& ChunkData, FIntPoint ChunkKey,
    float VoxelSizeMetres)
{
    // Wire format: [RawSize:4][Zlib-compressed [SizeX:4][SizeY:4][SizeZ:4][voxels]]
    constexpr int32 KRawSzBytes  = sizeof(int32); // leading raw-size prefix
    constexpr int32 KHeaderBytes = 12;            // SizeX(4)+SizeY(4)+SizeZ(4)

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

void UNamecVoxelWorld::AddOrUpdateChunk(FIntPoint ChunkCoord)
{
    if (!MapAssetOverride) return;

    const FNamecChunkData* Packed = MapAssetOverride->BakedChunks.Find(ChunkCoord);
    if (!Packed) return;

    const float VoxSize = MapAssetOverride->VoxelResolutionCm / 100.f;

    FNamecVoxelData BaseData = DecompressChunk(*Packed, ChunkCoord, VoxSize);
    // TODO (Task 6): apply stored edit deltas on top of BaseData before meshing.
    // For Phase 1 no runtime edits persist; base data is the authoritative state.

    OnTerrainEdited.Broadcast(ChunkCoord);
}

void UNamecVoxelWorld::ApplyTerrainEdit(FVector CentreMetres, float RadiusMetres,
    ENamecEditMode Mode, FName MaterialRow, int32 ToolTier) {}

bool UNamecVoxelWorld::IsWithinDigDepth(FVector PositionMetres) const
{
    // Delegate to the query object which owns the decompression logic.
    return DigDepthQuery && DigDepthQuery->IsWithinDigDepth(PositionMetres);
}

void UNamecVoxelWorld::Load(UNamecVoxelMapAsset* MapAsset)
{
    MapAssetOverride = MapAsset;

    DigDepthQuery = NewObject<UNamecDigDepthQuery>(this);
    DigDepthQuery->MapAsset = MapAsset;
}
