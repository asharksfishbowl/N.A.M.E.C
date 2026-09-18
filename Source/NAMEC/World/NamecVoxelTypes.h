#pragma once

#include "CoreMinimal.h"
#include "NamecVoxelTypes.generated.h"

// Shared voxel types included by both write kernel and world subsystem headers.

UENUM(BlueprintType)
enum class ENamecEditMode : uint8
{
    Dig,
    Fill,
};

// Voxels per horizontal chunk side — shared by commandlet, world, and depth query.
inline constexpr int32 NamecChunkH = 32;

// Chunk-local voxel grid. MaterialIndex 0 = air; non-zero = opaque material index.
struct FNamecVoxelData
{
    FVector WorldOriginMetres = FVector::ZeroVector; // world-space position of voxel (0,0,0)
    float   VoxelSizeMetres   = 0.25f;
    int32   SizeX = 0, SizeY = 0, SizeZ = 0;        // X,Y horizontal; Z vertical
    TArray<uint8> MaterialIndex;                     // SizeX*SizeY*SizeZ, row-major [X][Y][Z]

    int32 LinearIdx(int32 X, int32 Y, int32 Z) const { return Z + SizeZ * (Y + SizeY * X); }

    void Init(int32 InX, int32 InY, int32 InZ)
    {
        SizeX = InX; SizeY = InY; SizeZ = InZ;
        MaterialIndex.SetNumZeroed(SizeX * SizeY * SizeZ);
    }
};
