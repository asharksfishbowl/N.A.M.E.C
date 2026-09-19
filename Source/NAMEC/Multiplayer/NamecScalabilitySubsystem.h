#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HAL/IConsoleManager.h"
#include "NamecScalabilitySubsystem.generated.h"

class UDataTable;
class ULocalPlayer;
class UNamecSettingsService;
struct FNamecMachineSettings;
struct FNamecSettings;

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

    // Scales cosmetic PCG placement only; anything with gameplay state or collision is identical in both tiers.
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float FoliageDensityPercent = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ViewDistancePercent = 100.f;

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

// The four user graphics settings as they are applied on this machine right now
// (engine-tech Requirement 7). GIQuality: 3 Lumen HWRT High, 2 Lumen Lite, 1 Lumen Medium, 0 Low with a skylight.
struct FNamecAppliedGraphics
{
    int32 GIQuality = 3;
    int32 ShadowQuality = 3;
    float FoliageDensityPercent = 100.f;
    float ViewDistancePercent = 100.f;
};

UCLASS()
class NAMEC_API UNamecScalabilitySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    static const FName HighTierRowName;

    // By value: nothing keeps the table loaded for a caller that holds a pointer into it.
    static TOptional<FNamecScalabilityTierRow> FindTierRow(FName RowName);

    // Pure. The High tier returns the user's settings unchanged; any other tier returns the lower
    // of user and row for each setting. Without hardware ray tracing GIQuality is 0 in every tier.
    static FNamecAppliedGraphics ComputeAppliedSettings(const FNamecMachineSettings& UserSettings, const FNamecScalabilityTierRow& TierRow, bool bSupportsHWRT);

    // Public because a test has no subsystem collection to run Initialize for it.
    void UseSettings(UNamecSettingsService& InSettings);
    void ApplyTierForViewportCount(int32 ViewportCount);

    const FNamecAppliedGraphics& GetAppliedGraphics() const { return AppliedGraphics; }
    FName GetActiveTierName() const { return ActiveTierName; }

    // Returns the LOD distances (in metres) for the active tier.
    // Falls back to struct defaults when no tier is matched.
    void GetActiveLODDistances(float& Out0, float& Out1, float& Out2) const;

private:
    static UDataTable* LoadTierTable();

    void OnLocalPlayerCountChanged(ULocalPlayer* LocalPlayer);
    void OnSettingsApplied(const FNamecSettings& Settings);
    void RecomputeAppliedSettings();
    void ApplyToEngine(const FNamecAppliedGraphics& Applied);
    void LogActiveTier() const;

    // FAutoConsoleCommand registered in Initialize — UFUNCTION(Exec) cannot reach UGameInstanceSubsystem.
    TOptional<FAutoConsoleCommand> LogActiveTierCmd;

    UPROPERTY()
    TObjectPtr<UDataTable> ScalabilityTable;

    UPROPERTY()
    TObjectPtr<UNamecSettingsService> Settings;

    FNamecAppliedGraphics AppliedGraphics;

    FName ActiveTierName;
    FNamecScalabilityTierRow ActiveTierRow; // cached copy; updated on every tier change
};
