#include "World/NamecDigDepthQuery.h"
#include "World/NamecVoxelTypes.h"
#include "World/Map/NamecVoxelMapAsset.h"
#include "Misc/Compression.h"

bool UNamecDigDepthQuery::IsWithinDigDepth(FVector PositionMetres) const
{
    if (!MapAsset) return false;

    const float VoxSize = MapAsset->VoxelResolutionCm / 100.f;

    const int32 VoxX = FMath::FloorToInt(PositionMetres.X / VoxSize);
    const int32 VoxY = FMath::FloorToInt(PositionMetres.Y / VoxSize);
    const int32 VoxZ = FMath::FloorToInt(PositionMetres.Z / VoxSize);

    const FIntPoint ChunkKey(FMath::DivideAndRoundDown(VoxX, NamecChunkH),
                             FMath::DivideAndRoundDown(VoxY, NamecChunkH));

    const FNamecChunkData* ChunkData = MapAsset->BakedChunks.Find(ChunkKey);
    if (!ChunkData || ChunkData->Bytes.IsEmpty()) return false;

    // Decompress
    int32 RawSize = 0;
    FMemory::Memcpy(&RawSize, ChunkData->Bytes.GetData(), sizeof(int32));
    TArray<uint8> Raw;
    Raw.SetNumUninitialized(RawSize);
    FCompression::UncompressMemory(NAME_Zlib,
        Raw.GetData(), RawSize,
        ChunkData->Bytes.GetData() + sizeof(int32),
        ChunkData->Bytes.Num() - sizeof(int32));

    // Layout: [SizeX:4][SizeY:4][SizeZ:4][data]
    if (Raw.Num() < 12) return false;
    int32 SzX, SzY, SzZ;
    FMemory::Memcpy(&SzX, Raw.GetData(),     4);
    FMemory::Memcpy(&SzY, Raw.GetData() + 4, 4);
    FMemory::Memcpy(&SzZ, Raw.GetData() + 8, 4);

    const int32 LocalX = VoxX - ChunkKey.X * NamecChunkH;
    const int32 LocalY = VoxY - ChunkKey.Y * NamecChunkH;
    if (LocalX < 0 || LocalX >= SzX || LocalY < 0 || LocalY >= SzY || VoxZ < 0 || VoxZ >= SzZ)
        return false;

    // A position is within dig depth if the voxel directly above it (in base data) is generated air.
    // Scan upward from VoxZ to find the first generated air voxel.
    const uint8* Voxels = Raw.GetData() + 12;
    auto Idx = [&](int32 X, int32 Y, int32 Z) { return Z + SzZ * (Y + SzY * X); };

    for (int32 Z2 = VoxZ + 1; Z2 < SzZ; ++Z2)
    {
        if (Voxels[Idx(LocalX, LocalY, Z2)] == 0) // generated air found above
            return true;
    }
    return false; // no generated air above — below bedrock or fully enclosed
}
