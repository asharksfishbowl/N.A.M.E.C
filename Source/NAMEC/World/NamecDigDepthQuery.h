#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NamecDigDepthQuery.generated.h"

class UNamecVoxelMapAsset;

UCLASS()
class NAMEC_API UNamecDigDepthQuery : public UObject
{
    GENERATED_BODY()

public:
    // Returns whether Position is within dig depth limit using baked base data.
    // "Generated air" = air in the baked base data; player edits are ignored.
    UFUNCTION(BlueprintCallable)
    bool IsWithinDigDepth(FVector PositionMetres) const;

    UPROPERTY()
    TObjectPtr<UNamecVoxelMapAsset> MapAsset;
};
