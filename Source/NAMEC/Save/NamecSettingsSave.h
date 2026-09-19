#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Save/NamecVersionedSave.h"
#include "NamecSettingsSave.generated.h"

inline constexpr int32 NamecLocalPlayerSlotCount = 4;

// Belongs to a local player slot index, never to a character or a controller:
// whoever joins as local player 2 uses slot 2's section (game-foundation Requirement 6).
USTRUCT()
struct NAMEC_API FNamecLocalPlayerSettings
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<FName, FKey> GamepadRemaps;

    UPROPERTY()
    TMap<FName, FKey> KeyboardMouseRemaps;

    UPROPERTY()
    bool bNightEyes = true;

    UPROPERTY()
    bool bExecutionPrompts = true;
};

// The four graphics values use FNamecScalabilityTierRow's units, so the Split ceiling is a per-field min.
USTRUCT()
struct NAMEC_API FNamecMachineSettings
{
    GENERATED_BODY()

    UPROPERTY()
    float MasterVolume = 1.f;

    UPROPERTY()
    float MusicVolume = 1.f;

    UPROPERTY()
    float EffectsVolume = 1.f;

    UPROPERTY()
    int32 GIQuality = 3;

    UPROPERTY()
    int32 ShadowQuality = 3;

    UPROPERTY()
    float FoliageDensityPercent = 100.f;

    UPROPERTY()
    float ViewDistancePercent = 100.f;
};

USTRUCT()
struct NAMEC_API FNamecSettings
{
    GENERATED_BODY()

    UPROPERTY()
    FNamecLocalPlayerSettings Slots[NamecLocalPlayerSlotCount];

    UPROPERTY()
    FNamecMachineSettings Machine;

    // SlotNumber is the local player slot, 1 to 4.
    FNamecLocalPlayerSettings& GetSlot(int32 SlotNumber);
    const FNamecLocalPlayerSettings& GetSlot(int32 SlotNumber) const;

    // A settings file is outside input: every number is brought back into its range.
    void ClampToValidRanges();
};

UCLASS()
class NAMEC_API UNamecSettingsSave : public UNamecVersionedSave
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FNamecSettings Settings;
};
