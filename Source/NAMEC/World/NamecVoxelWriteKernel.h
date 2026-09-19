#pragma once

#include "CoreMinimal.h"
#include "World/NamecVoxelTypes.h"

struct NAMEC_API FNamecVoxelWriteKernel
{
    // Writes a sphere of voxels. MaterialRow ignored for Dig mode.
    // Voxels written here are NOT flagged player-placed.
    static void Apply(FNamecVoxelData& Data, FVector CentreMetres, float RadiusMetres,
                      ENamecEditMode Mode, FName MaterialRow, int32 ToolTier);
};
