#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Save/NamecSettingsSave.h"
#include "NamecSettingsService.generated.h"

class UNamecSaveFileService;

DECLARE_MULTICAST_DELEGATE_OneParam(FNamecOnSettingsApplied, const FNamecSettings&);

// Decided once, when the file is read. SessionOnly is a settings file from a newer build:
// it stays on disk untouched and nothing is written back (game-foundation Requirement 7).
enum class ENamecSettingsPersistence : uint8
{
    Writable,
    SessionOnly,
};

UCLASS()
class NAMEC_API UNamecSettingsService : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static const TCHAR* const SettingsFileName;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Public because a test has no subsystem collection to run Initialize for it.
    void LoadSettings(UNamecSaveFileService& InSaveFiles);

    const FNamecSettings& GetSettings() const;

    ENamecSettingsPersistence GetPersistence() const { return Persistence; }

    // Broadcasts whether or not anything is written: a SessionOnly session still changes.
    void Apply(const FNamecSettings& NewSettings);

    FNamecOnSettingsApplied OnSettingsApplied;

private:
    void Persist();

    UPROPERTY()
    TObjectPtr<UNamecSaveFileService> SaveFiles;

    UPROPERTY()
    TObjectPtr<UNamecSettingsSave> SettingsSave;

    ENamecSettingsPersistence Persistence = ENamecSettingsPersistence::Writable;
};
