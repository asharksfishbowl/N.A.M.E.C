#include "Save/NamecSettingsService.h"
#include "Save/NamecSaveFileService.h"
#include "Multiplayer/NamecScalabilitySubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogNamecSettings, Log, All);

const TCHAR* const UNamecSettingsService::SettingsFileName = TEXT("Settings.sav");

namespace
{
    // Default graphics are the High tier row, the configuration the benchmark tunes
    // (engine-tech roadmap, closed 2026-09-16). No hardware detection.
    UNamecSettingsSave* NewDefaultSettingsSave(UObject* Outer)
    {
        UNamecSettingsSave* Defaults = NewObject<UNamecSettingsSave>(Outer);
        const TOptional<FNamecScalabilityTierRow> HighRow = UNamecScalabilitySubsystem::FindTierRow(UNamecScalabilitySubsystem::HighTierRowName);
        if (HighRow.IsSet())
        {
            Defaults->Settings.Machine.GIQuality = HighRow->GIQuality;
            Defaults->Settings.Machine.ShadowQuality = HighRow->ShadowQuality;
            Defaults->Settings.Machine.FoliageDensityPercent = HighRow->FoliageDensityPercent;
            Defaults->Settings.Machine.ViewDistancePercent = HighRow->ViewDistancePercent;
        }
        else
        {
            UE_LOG(LogNamecSettings, Warning, TEXT("High tier row not found; default graphics settings are the compiled-in values"));
        }
        return Defaults;
    }
}

void UNamecSettingsService::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UNamecSaveFileService* SaveFileService = Collection.InitializeDependency<UNamecSaveFileService>();
    check(SaveFileService);
    LoadSettings(*SaveFileService);
}

void UNamecSettingsService::LoadSettings(UNamecSaveFileService& InSaveFiles)
{
    SaveFiles = &InSaveFiles;

    const FNamecSaveLoadOutcome Outcome = SaveFiles->Load(SettingsFileName, UNamecSettingsSave::StaticClass());
    Persistence = Outcome.Result == ENamecSaveLoadResult::NewerVersion ? ENamecSettingsPersistence::SessionOnly : ENamecSettingsPersistence::Writable;

    if (Outcome.Result == ENamecSaveLoadResult::Corrupt && !SaveFiles->MoveToBak(SettingsFileName))
    {
        UE_LOG(LogNamecSettings, Warning, TEXT("Corrupt settings file could not be set aside as .bak"));
    }

    if (Outcome.Result == ENamecSaveLoadResult::Ok)
    {
        SettingsSave = CastChecked<UNamecSettingsSave>(Outcome.Save);
        SettingsSave->Settings.ClampToValidRanges();
    }
    else
    {
        SettingsSave = NewDefaultSettingsSave(this);
    }
}

const FNamecSettings& UNamecSettingsService::GetSettings() const
{
    check(SettingsSave);
    return SettingsSave->Settings;
}

void UNamecSettingsService::Apply(const FNamecSettings& NewSettings)
{
    check(SettingsSave);
    SettingsSave->Settings = NewSettings;
    SettingsSave->Settings.ClampToValidRanges();
    OnSettingsApplied.Broadcast(SettingsSave->Settings);

    if (Persistence == ENamecSettingsPersistence::Writable)
    {
        Persist();
    }
}

void UNamecSettingsService::Persist()
{
    const ENamecSaveWriteResult Result = SaveFiles->Write(*SettingsSave, SettingsFileName);
    if (Result != ENamecSaveWriteResult::Ok)
    {
        UE_LOG(LogNamecSettings, Warning, TEXT("Settings were applied but not written (%s)"), LexToString(Result));
    }
}
