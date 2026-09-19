#pragma once

#include "CoreMinimal.h"
#include "Save/NamecVersionedSave.h"
#include "NamecWorldSave.generated.h"

USTRUCT()
struct NAMEC_API FNamecWorldSettings
{
    GENERATED_BODY()

    UPROPERTY()
    bool bFriendlyFire = false;

    UPROPERTY()
    bool bLanHosting = true;

    UPROPERTY()
    FString Password;

    UPROPERTY()
    bool bRaids = true;
};

UCLASS()
class NAMEC_API UNamecWorldSave : public UNamecVersionedSave
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FString WorldName;

    UPROPERTY()
    float VoxelResolutionCm = 25.f;

    UPROPERTY()
    FNamecWorldSettings Settings;
};
