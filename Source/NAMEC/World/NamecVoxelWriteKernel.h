#pragma once

#include "CoreMinimal.h"
#include "World/NamecVoxelTypes.h"

// Plain C++ struct — no USTRUCT/GENERATED_BODY overhead since there are no UPROPERTY members.
struct NAMEC_API FNamecVoxelData
{
    // Defined in NamecVoxelWriteKernel.cpp — chunk-local voxel grid.
};

struct NAMEC_API FNamecVoxelWriteKernel
{
    // Writes a sphere of voxels. MaterialRow ignored for Dig mode.
    // Voxels written here are NOT flagged player-placed.
    static void Apply(FNamecVoxelData& Data, FVector CentreMetres, float RadiusMetres,
                      ENamecEditMode Mode, FName MaterialRow, int32 ToolTier);
};
