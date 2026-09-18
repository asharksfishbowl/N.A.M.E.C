#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HAL/IConsoleManager.h"
#include "NamecScalabilitySubsystem.generated.h"

class UDataTable;
class ULocalPlayer;

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

    // Full-res LOD distance: chunks within this radius use LOD 0 (stride 1).
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float LODDistance0Metres = 40.f;

    // LOD 1 distance: chunks within this radius (beyond LOD0) use stride 2.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float LODDistance1Metres = 100.f;

    // LOD 2 distance: chunks beyond LOD1 distance use stride 4.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float LODDistance2Metres = 200.f;
};

UCLASS()
class NAMEC_API UNamecScalabilitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Returns the LOD distances (in metres) for the active tier.
    // Falls back to struct defaults when no tier is matched.
    void GetActiveLODDistances(float& Out0, float& Out1, float& Out2) const;

private:
    void OnLocalPlayerCountChanged(ULocalPlayer* LocalPlayer);
    void ApplyTierForViewportCount(int32 ViewportCount);
    void LogActiveTier() const;

    // FAutoConsoleCommand registered in Initialize — UFUNCTION(Exec) cannot reach UGameInstanceSubsystem.
    TOptional<FAutoConsoleCommand> LogActiveTierCmd;

    UPROPERTY()
    TObjectPtr<UDataTable> ScalabilityTable;

    FName ActiveTierName;
    FNamecScalabilityTierRow ActiveTierRow; // cached copy; updated on every tier change
};
