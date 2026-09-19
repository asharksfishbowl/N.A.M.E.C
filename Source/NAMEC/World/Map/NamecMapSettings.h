#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NamecMapSettings.generated.h"

class UNamecVoxelMapAsset;

// Config/DefaultGame.ini [/Script/NAMEC.NamecMapSettings]. The shipped asset is the effective map
// asset unless a level overrides it (authored-map Requirement 4).
UCLASS(config = Game, defaultconfig)
class NAMEC_API UNamecMapSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(config, EditAnywhere, Category = "Map")
    TSoftObjectPtr<UNamecVoxelMapAsset> ShippedMapAsset;
};
