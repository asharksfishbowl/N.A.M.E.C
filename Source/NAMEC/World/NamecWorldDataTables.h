#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NamecWorldDataTables.generated.h"

USTRUCT(BlueprintType)
struct FNamecVoxelMaterialRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName  MaterialName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool   bIsOre  = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool   bIsSolid = true;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText  DisplayName;
};

USTRUCT(BlueprintType)
struct FNamecClimateRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName RegionName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float CaveDensityThreshold    = 0.08f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float CaveDepthBandMinMetres  = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float CaveDepthBandMaxMetres  = 20.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float CaveTunnelRadiusMetres  = 2.f;
};

USTRUCT(BlueprintType)
struct FNamecOreVeinRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName  RegionName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName  OreMaterialRow;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float  VeinDensity      = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float  VeinRadiusMetres = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool   bCaveOnly        = false;
};
