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

// Forward declaration only — full struct defined in NamecVoxelWriteKernel.cpp.
struct FNamecVoxelData;
