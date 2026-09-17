#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HAL/IConsoleManager.h"
#include "NamecScalabilitySubsystem.generated.h"

USTRUCT(BlueprintType)
struct NAMEC_API FNamecScalabilityTierRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName TierName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MinViewports = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MaxViewports = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 LumenMaxViews = 2;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 GIQuality = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ShadowQuality = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 FoliageQuality = 2;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 PostProcessQuality = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 DynamicResOperationMode = 2;

    // LODDistance0/1/2Metres are added by Task 5 when the LOD columns land in DT_MP_SplitScreenScalability.
};

UCLASS()
class NAMEC_API UNamecScalabilitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:
    void OnLocalPlayerCountChanged();
    void ApplyTierForViewportCount(int32 ViewportCount);
    void LogActiveTier() const;

    // FAutoConsoleCommand registered in Initialize — UFUNCTION(Exec) cannot reach UGameInstanceSubsystem.
    TOptional<FAutoConsoleCommand> LogActiveTierCmd;
};
