#include "Save/NamecAutosaveSubsystem.h"
#include "Save/NamecCharacterSave.h"
#include "Save/Tests/NamecSaveTestHelpers.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

namespace
{
    UDataTable* NewCoreSaveTable(float AutosaveIntervalSeconds)
    {
        UDataTable* Table = NewObject<UDataTable>();
        Table->RowStruct = FNamecCoreSaveRow::StaticStruct();
        FNamecCoreSaveRow Row;
        Row.AutosaveIntervalSeconds = AutosaveIntervalSeconds;
        Table->AddRow(UNamecAutosaveSubsystem::CoreSaveRowName, Row);
        return Table;
    }

    UNamecAutosaveSubsystem* StartAutosaveSubsystem(const UDataTable& CoreSaveTable)
    {
        UGameInstance* GameInstance = NewGameInstance();
        UNamecAutosaveSubsystem* Autosave = NewObject<UNamecAutosaveSubsystem>(GameInstance);
        Autosave->UseSaveFiles(*NewObject<UNamecSaveFileService>(GameInstance));
        Autosave->StartAutosaveTimer(CoreSaveTable);
        return Autosave;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecAutosaveTimerRateTest, "Namec.Foundation.Autosave.TimerRateComesFromTheTable", NamecFoundationTestFlags)

bool FNamecAutosaveTimerRateTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;

    const UDataTable* ShippedTable = LoadObject<UDataTable>(nullptr, UNamecAutosaveSubsystem::CoreSaveTablePackageName);
    if (TestNotNull(TEXT("DT_Core_Save"), ShippedTable))
    {
        const FNamecCoreSaveRow* ShippedRow = ShippedTable->FindRow<FNamecCoreSaveRow>(UNamecAutosaveSubsystem::CoreSaveRowName, TEXT("Test"));
        if (TestNotNull(TEXT("DT_Core_Save row"), ShippedRow))
        {
            TestEqual(TEXT("Timer rate equals the shipped table value"), StartAutosaveSubsystem(*ShippedTable)->GetAutosaveTimerRateSeconds(), ShippedRow->AutosaveIntervalSeconds);
        }
    }

    TestEqual(TEXT("A different row value changes the rate with no rebuild"), StartAutosaveSubsystem(*NewCoreSaveTable(42.f))->GetAutosaveTimerRateSeconds(), 42.f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecAutosaveDiskFullTest, "Namec.Foundation.Autosave.DiskFullWarnsKeepsMemoryAndRetriesNextTick", NamecFoundationTestFlags)

bool FNamecAutosaveDiskFullTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecAutosaveSubsystem* Autosave = StartAutosaveSubsystem(*NewCoreSaveTable(300.f));
    const UNamecSaveFileService* SaveFiles = NewSaveFileService();

    int32 WarningCount = 0;
    Autosave->OnSaveWarning.AddLambda([&WarningCount](const FText&) { ++WarningCount; });

    UNamecCharacterSave* Character = NewObject<UNamecCharacterSave>();
    Character->CharacterName = TEXT("OnDisk");
    Autosave->RegisterSave(*Character, TEXT("Character.sav"));
    Autosave->RunAutosave();
    TestEqual(TEXT("No warning while there is space"), WarningCount, 0);

    AddExpectedMessage(TEXT("was not written"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);

    Character->CharacterName = TEXT("InMemoryOnly");
    Platform->FreeDiskSpaceBytes = 0;
    Autosave->RunAutosave();
    TestEqual(TEXT("OnSaveWarning fires once"), WarningCount, 1);
    TestEqual(TEXT("The object in memory is unchanged"), Character->CharacterName, FString(TEXT("InMemoryOnly")));
    const UNamecCharacterSave* StillOnDisk = Cast<UNamecCharacterSave>(SaveFiles->Load(TEXT("Character.sav"), UNamecCharacterSave::StaticClass()).Save);
    if (TestNotNull(TEXT("The old file still loads"), StillOnDisk))
    {
        TestEqual(TEXT("The old file holds the old value"), StillOnDisk->CharacterName, FString(TEXT("OnDisk")));
    }

    Platform->FreeDiskSpaceBytes.Reset();
    Autosave->RunAutosave();
    TestEqual(TEXT("No further warning once space returns"), WarningCount, 1);
    const UNamecCharacterSave* Retried = Cast<UNamecCharacterSave>(SaveFiles->Load(TEXT("Character.sav"), UNamecCharacterSave::StaticClass()).Save);
    if (TestNotNull(TEXT("The next tick wrote the save"), Retried))
    {
        TestEqual(TEXT("The file holds the in-memory value"), Retried->CharacterName, FString(TEXT("InMemoryOnly")));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecAutosaveRegistrationTest, "Namec.Foundation.Autosave.OnlyRegisteredSavesAreWritten", NamecFoundationTestFlags)

bool FNamecAutosaveRegistrationTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecAutosaveSubsystem* Autosave = StartAutosaveSubsystem(*NewCoreSaveTable(300.f));

    UNamecCharacterSave* Loaded = NewObject<UNamecCharacterSave>();
    UNamecCharacterSave* Unloaded = NewObject<UNamecCharacterSave>();
    Autosave->RegisterSave(*Loaded, TEXT("Loaded.sav"));
    Autosave->RegisterSave(*Unloaded, TEXT("Unloaded.sav"));
    Autosave->UnregisterSave(*Unloaded);

    UNamecCharacterSave* Replacement = NewObject<UNamecCharacterSave>();
    Replacement->CharacterName = TEXT("Replacement");
    Autosave->RegisterSave(*Replacement, TEXT("Loaded.sav"));

    Autosave->SaveAllNow(ENamecSaveReason::HostExit);
    TestTrue(TEXT("A registered save is written"), IFileManager::Get().FileExists(*UNamecSaveFileService::GetSaveFilePath(TEXT("Loaded.sav"))));
    const UNamecCharacterSave* Written = Cast<UNamecCharacterSave>(NewSaveFileService()->Load(TEXT("Loaded.sav"), UNamecCharacterSave::StaticClass()).Save);
    if (TestNotNull(TEXT("Loaded.sav"), Written))
    {
        TestEqual(TEXT("Registering a file name again replaces its writer"), Written->CharacterName, FString(TEXT("Replacement")));
    }
    TestFalse(TEXT("An unregistered save is not"), IFileManager::Get().FileExists(*UNamecSaveFileService::GetSaveFilePath(TEXT("Unloaded.sav"))));
    return true;
}

#endif
