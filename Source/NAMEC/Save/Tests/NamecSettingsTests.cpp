#include "Save/NamecSettingsService.h"
#include "Save/Tests/NamecSaveTestHelpers.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Multiplayer/NamecScalabilitySubsystem.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

namespace
{
    FString SettingsFilePath()
    {
        return UNamecSaveFileService::GetSaveFilePath(UNamecSettingsService::SettingsFileName);
    }

    FString SettingsBakPath()
    {
        return UNamecSaveFileService::GetBakFilePath(UNamecSettingsService::SettingsFileName);
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSettingsSlotTest, "Namec.Foundation.Settings.SlotTwoPersistsAndSlotOneIsUnchanged", NamecFoundationTestFlags)

bool FNamecSettingsSlotTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;

    UNamecSettingsService* FirstRun = StartSettingsService();
    FNamecSettings Edited = FirstRun->GetSettings();
    Edited.GetSlot(2).bNightEyes = false;
    Edited.GetSlot(2).bExecutionPrompts = false;
    Edited.GetSlot(2).GamepadRemaps.Add(TEXT("Jump"), EKeys::Gamepad_FaceButton_Right);
    FirstRun->Apply(Edited);

    const FNamecSettings& Reloaded = StartSettingsService()->GetSettings();
    TestFalse(TEXT("Slot 2 Night Eyes stays off"), Reloaded.GetSlot(2).bNightEyes);
    TestFalse(TEXT("Slot 2 Execution prompts stays off"), Reloaded.GetSlot(2).bExecutionPrompts);
    TestEqual(TEXT("Slot 2 remap"), Reloaded.GetSlot(2).GamepadRemaps.FindRef(TEXT("Jump")), EKeys::Gamepad_FaceButton_Right);
    for (const int32 OtherSlot : { 1, 3, 4 })
    {
        TestTrue(FString::Printf(TEXT("Slot %d Night Eyes unchanged"), OtherSlot), Reloaded.GetSlot(OtherSlot).bNightEyes);
        TestTrue(FString::Printf(TEXT("Slot %d Execution prompts unchanged"), OtherSlot), Reloaded.GetSlot(OtherSlot).bExecutionPrompts);
        TestEqual(FString::Printf(TEXT("Slot %d has no remaps"), OtherSlot), Reloaded.GetSlot(OtherSlot).GamepadRemaps.Num(), 0);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSettingsMachineTest, "Namec.Foundation.Settings.GraphicsChangePersistsMachineWide", NamecFoundationTestFlags)

bool FNamecSettingsMachineTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;

    UNamecSettingsService* FirstRun = StartSettingsService();
    int32 BroadcastCount = 0;
    FirstRun->OnSettingsApplied.AddLambda([&BroadcastCount](const FNamecSettings&) { ++BroadcastCount; });

    FNamecSettings Edited = FirstRun->GetSettings();
    Edited.Machine.GIQuality = 1;
    Edited.Machine.FoliageDensityPercent = 30.f;
    Edited.Machine.MusicVolume = 0.25f;
    Edited.Machine.ViewDistancePercent = 250.f;
    FirstRun->Apply(Edited);
    TestEqual(TEXT("OnSettingsApplied fires once per Apply"), BroadcastCount, 1);

    const FNamecMachineSettings& Reloaded = StartSettingsService()->GetSettings().Machine;
    TestEqual(TEXT("GIQuality"), Reloaded.GIQuality, 1);
    TestEqual(TEXT("FoliageDensityPercent"), Reloaded.FoliageDensityPercent, 30.f);
    TestEqual(TEXT("MusicVolume"), Reloaded.MusicVolume, 0.25f);
    TestEqual(TEXT("An out-of-range value is clamped"), Reloaded.ViewDistancePercent, 100.f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSettingsMissingTest, "Namec.Foundation.Settings.MissingFileUsesHighRowDefaultsAndWritesOnFirstApply", NamecFoundationTestFlags)

bool FNamecSettingsMissingTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;

    UNamecSettingsService* Service = StartSettingsService();
    TestEqual(TEXT("Persistence"), Service->GetPersistence(), ENamecSettingsPersistence::Writable);
    TestFalse(TEXT("Loading writes nothing"), IFileManager::Get().FileExists(*SettingsFilePath()));

    const TOptional<FNamecScalabilityTierRow> HighRow = UNamecScalabilitySubsystem::FindTierRow(UNamecScalabilitySubsystem::HighTierRowName);
    if (TestTrue(TEXT("High tier row"), HighRow.IsSet()))
    {
        TestEqual(TEXT("Default GIQuality is the High row's"), Service->GetSettings().Machine.GIQuality, HighRow->GIQuality);
        TestEqual(TEXT("Default ShadowQuality is the High row's"), Service->GetSettings().Machine.ShadowQuality, HighRow->ShadowQuality);
    }

    Service->Apply(Service->GetSettings());
    TestTrue(TEXT("The first Apply writes the file"), IFileManager::Get().FileExists(*SettingsFilePath()));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSettingsCorruptTest, "Namec.Foundation.Settings.CorruptFileBecomesBakAndDefaultsLoad", NamecFoundationTestFlags)

bool FNamecSettingsCorruptTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;

    const FString Garbage = TEXT("not a settings file");
    FFileHelper::SaveStringToFile(Garbage, *SettingsFilePath());
    const TArray<uint8> GarbageBytes = ReadFileBytes(SettingsFilePath());

    UNamecSettingsService* Service = StartSettingsService();
    TestFalse(TEXT("The corrupt file is no longer under its own name"), IFileManager::Get().FileExists(*SettingsFilePath()));

    TestTrue(TEXT("The .bak holds the corrupt bytes"), ReadFileBytes(SettingsBakPath()) == GarbageBytes);

    TestTrue(TEXT("Defaults load"), Service->GetSettings().GetSlot(1).bNightEyes);
    TestEqual(TEXT("Persistence"), Service->GetPersistence(), ENamecSettingsPersistence::Writable);

    Service->Apply(Service->GetSettings());
    TestTrue(TEXT("The next Apply writes a fresh file"), IFileManager::Get().FileExists(*SettingsFilePath()));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSettingsNewerVersionTest, "Namec.Foundation.Settings.NewerVersionFileIsNeverWritten", NamecFoundationTestFlags)

bool FNamecSettingsNewerVersionTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;

    UNamecSettingsSave* Newer = NewObject<UNamecSettingsSave>();
    Newer->Settings.GetSlot(1).bNightEyes = false;
    WriteSaveFileAtVersion(*Newer, Newer->GetCurrentSaveVersion() + 1, UNamecSettingsService::SettingsFileName);
    const TArray<uint8> Before = ReadFileBytes(SettingsFilePath());

    UNamecSettingsService* Service = StartSettingsService();
    TestEqual(TEXT("Persistence"), Service->GetPersistence(), ENamecSettingsPersistence::SessionOnly);
    TestTrue(TEXT("The session uses defaults, not the newer file's values"), Service->GetSettings().GetSlot(1).bNightEyes);

    FNamecSettings Edited = Service->GetSettings();
    Edited.GetSlot(1).bExecutionPrompts = false;
    Service->Apply(Edited);
    TestFalse(TEXT("Apply still changes the session"), Service->GetSettings().GetSlot(1).bExecutionPrompts);
    TestTrue(TEXT("File bytes are identical after an Apply"), ReadFileBytes(SettingsFilePath()) == Before);
    TestFalse(TEXT("No .bak appears"), IFileManager::Get().FileExists(*SettingsBakPath()));
    return true;
}

#endif
