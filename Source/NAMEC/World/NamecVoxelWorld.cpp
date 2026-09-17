#include "World/NamecVoxelWorld.h"
#include "World/Map/NamecVoxelMapAsset.h"
#include "World/NamecVoxelWriteKernel.h"
#include "World/NamecDigDepthQuery.h"
#include "Misc/Compression.h"

static FNamecVoxelData DecompressChunk(const FNamecChunkData& ChunkData, FIntPoint ChunkKey,
    float VoxelSizeMetres, int32 ChunkH)
{
    FNamecVoxelData Out;
    if (ChunkData.Bytes.Num() < 4 + 12) return Out;

    int32 RawSize = 0;
    FMemory::Memcpy(&RawSize, ChunkData.Bytes.GetData(), sizeof(int32));
    TArray<uint8> Raw;
    Raw.SetNumUninitialized(RawSize);
    FCompression::UncompressMemory(NAME_Zlib,
        Raw.GetData(), RawSize,
        ChunkData.Bytes.GetData() + sizeof(int32),
        ChunkData.Bytes.Num() - sizeof(int32));

    if (Raw.Num() < 12) return Out;
    FMemory::Memcpy(&Out.SizeX, Raw.GetData(),     4);
    FMemory::Memcpy(&Out.SizeY, Raw.GetData() + 4, 4);
    FMemory::Memcpy(&Out.SizeZ, Raw.GetData() + 8, 4);

    Out.MaterialIndex = TArray<uint8>(Raw.GetData() + 12, Raw.Num() - 12);
    Out.WorldOriginMetres = FVector(ChunkKey.X * ChunkH, ChunkKey.Y * ChunkH, 0) * VoxelSizeMetres;
    Out.VoxelSizeMetres = VoxelSizeMetres;
    return Out;
}

void UNamecVoxelWorld::AddOrUpdateChunk(FIntPoint ChunkCoord)
{
    if (!MapAssetOverride) return;

    const FNamecChunkData* Packed = MapAssetOverride->BakedChunks.Find(ChunkCoord);
    if (!Packed) return;

    const float VoxSize = MapAssetOverride->VoxelResolutionCm / 100.f;
    constexpr int32 ChunkH = 32;

    FNamecVoxelData BaseData = DecompressChunk(*Packed, ChunkCoord, VoxSize, ChunkH);
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
