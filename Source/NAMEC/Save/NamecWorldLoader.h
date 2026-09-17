#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NamecWorldLoader.generated.h"

class UNamecVoxelMapAsset;

UCLASS()
class NAMEC_API UNamecWorldLoader : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void Load(UNamecVoxelMapAsset* MapAsset);
};
