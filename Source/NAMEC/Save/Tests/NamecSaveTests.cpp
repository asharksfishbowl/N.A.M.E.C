#include "Save/NamecSaveFileService.h"
#include "Save/NamecCharacterSave.h"
#include "Save/NamecWorldSave.h"
#include "Save/Tests/NamecSaveTestHelpers.h"
#include "Save/Tests/NamecSaveTestTypes.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

namespace
{
    TArray<uint8> ReadSaveBytes(const FString& FileName)
    {
        return ReadFileBytes(UNamecSaveFileService::GetSaveFilePath(FileName));
    }

    void WriteSaveBytes(const TArray<uint8>& Bytes, const FString& FileName)
    {
        FFileHelper::SaveArrayToFile(Bytes, *UNamecSaveFileService::GetSaveFilePath(FileName));
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSaveCharacterRoundTripTest, "Namec.Foundation.Save.CharacterRoundTrip", NamecFoundationTestFlags)

bool FNamecSaveCharacterRoundTripTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecSaveFileService* Service = NewSaveFileService();

    // The first save on a machine finds no save directory.
    IFileManager::Get().DeleteDirectory(*Platform->SaveDirectory, false, true);

    UNamecCharacterSave* Written = NewObject<UNamecCharacterSave>();
    Written->CharacterGuid = FGuid::NewGuid();
    Written->CharacterName = TEXT("Aldric");
    TestEqual(TEXT("Write result"), Service->Write(*Written, TEXT("Character.sav")), ENamecSaveWriteResult::Ok);
    TestFalse(TEXT("No temp file is left behind"), IFileManager::Get().FileExists(*UNamecSaveFileService::GetTempFilePath(TEXT("Character.sav"))));

    const FNamecSaveLoadOutcome Outcome = Service->Load(TEXT("Character.sav"), UNamecCharacterSave::StaticClass());
    TestEqual(TEXT("Load result"), Outcome.Result, ENamecSaveLoadResult::Ok);
    const UNamecCharacterSave* Loaded = Cast<UNamecCharacterSave>(Outcome.Save);
    if (TestNotNull(TEXT("Loaded character save"), Loaded))
    {
        TestEqual(TEXT("Guid"), Loaded->CharacterGuid, Written->CharacterGuid);
        TestEqual(TEXT("Name"), Loaded->CharacterName, Written->CharacterName);
        TestEqual(TEXT("SaveVersion"), Loaded->SaveVersion, Loaded->GetCurrentSaveVersion());
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSaveWorldRoundTripTest, "Namec.Foundation.Save.WorldRoundTrip", NamecFoundationTestFlags)

bool FNamecSaveWorldRoundTripTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecSaveFileService* Service = NewSaveFileService();

    UNamecWorldSave* Written = NewObject<UNamecWorldSave>();
    Written->WorldName = TEXT("Eastmarch");
    Written->VoxelResolutionCm = 50.f;
    Written->Settings.bFriendlyFire = true;
    Written->Settings.bLanHosting = false;
    Written->Settings.Password = TEXT("hunter2");
    Written->Settings.bRaids = false;
    TestEqual(TEXT("Write result"), Service->Write(*Written, TEXT("World.sav")), ENamecSaveWriteResult::Ok);

    const FNamecSaveLoadOutcome Outcome = Service->Load(TEXT("World.sav"), UNamecWorldSave::StaticClass());
    TestEqual(TEXT("Load result"), Outcome.Result, ENamecSaveLoadResult::Ok);
    const UNamecWorldSave* Loaded = Cast<UNamecWorldSave>(Outcome.Save);
    if (TestNotNull(TEXT("Loaded world save"), Loaded))
    {
        TestEqual(TEXT("WorldName"), Loaded->WorldName, Written->WorldName);
        TestEqual(TEXT("VoxelResolutionCm"), Loaded->VoxelResolutionCm, 50.f);
        TestTrue(TEXT("bFriendlyFire"), Loaded->Settings.bFriendlyFire);
        TestFalse(TEXT("bLanHosting"), Loaded->Settings.bLanHosting);
        TestEqual(TEXT("Password"), Loaded->Settings.Password, FString(TEXT("hunter2")));
        TestFalse(TEXT("bRaids"), Loaded->Settings.bRaids);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSaveMissingTest, "Namec.Foundation.Save.MissingFile", NamecFoundationTestFlags)

bool FNamecSaveMissingTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecSaveFileService* Service = NewSaveFileService();

    AddExpectedError(TEXT("is not a bare save file name"), EAutomationExpectedErrorFlags::Contains, 2);

    TestEqual(TEXT("A file that is not there"), Service->Load(TEXT("Nothing.sav"), UNamecCharacterSave::StaticClass()).Result, ENamecSaveLoadResult::Missing);
    TestEqual(TEXT("A name that leaves the save directory"), Service->Load(TEXT("../Nothing.sav"), UNamecCharacterSave::StaticClass()).Result, ENamecSaveLoadResult::Missing);

    UNamecCharacterSave* Save = NewObject<UNamecCharacterSave>();
    TestEqual(TEXT("Writing to a name that leaves the save directory"), Service->Write(*Save, TEXT("../Escape.sav")), ENamecSaveWriteResult::WriteFailed);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSaveCorruptTest, "Namec.Foundation.Save.GarbageAndTruncatedAreCorruptAndUntouched", NamecFoundationTestFlags)

bool FNamecSaveCorruptTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecSaveFileService* Service = NewSaveFileService();

    TArray<uint8> Garbage;
    for (int32 Index = 0; Index < 512; ++Index)
    {
        Garbage.Add(static_cast<uint8>((Index * 37 + 11) & 0xFF));
    }
    WriteSaveBytes(Garbage, TEXT("Garbage.sav"));
    TestEqual(TEXT("Garbage bytes"), Service->Load(TEXT("Garbage.sav"), UNamecCharacterSave::StaticClass()).Result, ENamecSaveLoadResult::Corrupt);
    TestTrue(TEXT("Garbage file bytes are identical afterwards"), ReadSaveBytes(TEXT("Garbage.sav")) == Garbage);

    UNamecCharacterSave* Save = NewObject<UNamecCharacterSave>();
    Save->CharacterName = TEXT("Aldric");
    Service->Write(*Save, TEXT("Truncated.sav"));
    TArray<uint8> Truncated = ReadSaveBytes(TEXT("Truncated.sav"));
    Truncated.SetNum(Truncated.Num() - 9);
    WriteSaveBytes(Truncated, TEXT("Truncated.sav"));
    TestEqual(TEXT("A truncated save"), Service->Load(TEXT("Truncated.sav"), UNamecCharacterSave::StaticClass()).Result, ENamecSaveLoadResult::Corrupt);
    TestTrue(TEXT("Truncated file bytes are identical afterwards"), ReadSaveBytes(TEXT("Truncated.sav")) == Truncated);

    WriteSaveFileAtVersion(*Save, 0, TEXT("VersionZero.sav"));
    TestEqual(TEXT("A save claiming version 0"), Service->Load(TEXT("VersionZero.sav"), UNamecCharacterSave::StaticClass()).Result, ENamecSaveLoadResult::Corrupt);

    UNamecWorldSave* World = NewObject<UNamecWorldSave>();
    Service->Write(*World, TEXT("WrongType.sav"));
    TestEqual(TEXT("A world save asked for as a character save"), Service->Load(TEXT("WrongType.sav"), UNamecCharacterSave::StaticClass()).Result, ENamecSaveLoadResult::Corrupt);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSaveNewerVersionTest, "Namec.Foundation.Save.NewerVersionIsRefusedAndUntouched", NamecFoundationTestFlags)

bool FNamecSaveNewerVersionTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecSaveFileService* Service = NewSaveFileService();

    UNamecCharacterSave* Save = NewObject<UNamecCharacterSave>();
    WriteSaveFileAtVersion(*Save, Save->GetCurrentSaveVersion() + 1, TEXT("Newer.sav"));
    const TArray<uint8> Before = ReadSaveBytes(TEXT("Newer.sav"));

    const FNamecSaveLoadOutcome Outcome = Service->Load(TEXT("Newer.sav"), UNamecCharacterSave::StaticClass());
    TestEqual(TEXT("Load result"), Outcome.Result, ENamecSaveLoadResult::NewerVersion);
    TestNull(TEXT("No object is handed out"), Outcome.Save);
    TestTrue(TEXT("File bytes are identical afterwards"), ReadSaveBytes(TEXT("Newer.sav")) == Before);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSaveMigrationTest, "Namec.Foundation.Save.OlderVersionMigratesAndFillsDefaults", NamecFoundationTestFlags)

bool FNamecSaveMigrationTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecSaveFileService* Service = NewSaveFileService();

    UNamecMigrationTestSave* Old = NewObject<UNamecMigrationTestSave>();
    Old->PresentSinceVersion1 = TEXT("Kept");
    WriteSaveFileAtVersion(*Old, 1, TEXT("Old.sav"));

    const FNamecSaveLoadOutcome Outcome = Service->Load(TEXT("Old.sav"), UNamecMigrationTestSave::StaticClass());
    TestEqual(TEXT("Load result"), Outcome.Result, ENamecSaveLoadResult::Ok);
    const UNamecMigrationTestSave* Loaded = Cast<UNamecMigrationTestSave>(Outcome.Save);
    if (TestNotNull(TEXT("Loaded save"), Loaded))
    {
        TestEqual(TEXT("SaveVersion is current"), Loaded->SaveVersion, 3);
        TestEqual(TEXT("Version 1 field is kept"), Loaded->PresentSinceVersion1, FString(TEXT("Kept")));
        TestEqual(TEXT("Version 2 default"), Loaded->AddedInVersion2, 7);
        TestEqual(TEXT("Version 3 default"), Loaded->AddedInVersion3, FString(TEXT("FilledByMigration")));
    }

    UNamecMigrationTestSave* Middle = NewObject<UNamecMigrationTestSave>();
    Middle->AddedInVersion2 = 99;
    WriteSaveFileAtVersion(*Middle, 2, TEXT("Middle.sav"));
    const UNamecMigrationTestSave* LoadedMiddle = Cast<UNamecMigrationTestSave>(Service->Load(TEXT("Middle.sav"), UNamecMigrationTestSave::StaticClass()).Save);
    if (TestNotNull(TEXT("Loaded version 2 save"), LoadedMiddle))
    {
        TestEqual(TEXT("A version 2 value is not overwritten by the 1 to 2 migration"), LoadedMiddle->AddedInVersion2, 99);
        TestEqual(TEXT("Version 3 default"), LoadedMiddle->AddedInVersion3, FString(TEXT("FilledByMigration")));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSaveStaleTempTest, "Namec.Foundation.Save.StaleTempFileIsIgnoredAndReplaced", NamecFoundationTestFlags)

bool FNamecSaveStaleTempTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecSaveFileService* Service = NewSaveFileService();

    UNamecCharacterSave* Save = NewObject<UNamecCharacterSave>();
    Save->CharacterName = TEXT("First");
    Service->Write(*Save, TEXT("Character.sav"));

    const FString TempPath = UNamecSaveFileService::GetTempFilePath(TEXT("Character.sav"));
    FFileHelper::SaveStringToFile(TEXT("left behind by a crash mid-save"), *TempPath);

    const UNamecCharacterSave* Loaded = Cast<UNamecCharacterSave>(Service->Load(TEXT("Character.sav"), UNamecCharacterSave::StaticClass()).Save);
    if (TestNotNull(TEXT("The valid file loads beside a stale temp file"), Loaded))
    {
        TestEqual(TEXT("Name"), Loaded->CharacterName, FString(TEXT("First")));
    }

    Save->CharacterName = TEXT("Second");
    TestEqual(TEXT("The next write succeeds"), Service->Write(*Save, TEXT("Character.sav")), ENamecSaveWriteResult::Ok);
    TestFalse(TEXT("The stale temp file is gone"), IFileManager::Get().FileExists(*TempPath));
    const UNamecCharacterSave* Reloaded = Cast<UNamecCharacterSave>(Service->Load(TEXT("Character.sav"), UNamecCharacterSave::StaticClass()).Save);
    if (TestNotNull(TEXT("Reloaded save"), Reloaded))
    {
        TestEqual(TEXT("Name after the second write"), Reloaded->CharacterName, FString(TEXT("Second")));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSaveFailedWriteTest, "Namec.Foundation.Save.FailedWriteLeavesPreviousFileLoadable", NamecFoundationTestFlags)

bool FNamecSaveFailedWriteTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecSaveFileService* Service = NewSaveFileService();

    UNamecCharacterSave* Save = NewObject<UNamecCharacterSave>();
    Save->CharacterName = TEXT("Previous");
    Service->Write(*Save, TEXT("Character.sav"));
    const TArray<uint8> Before = ReadSaveBytes(TEXT("Character.sav"));

    Save->CharacterName = TEXT("NeverWritten");
    Platform->FreeDiskSpaceBytes = 0;
    TestEqual(TEXT("No free space"), Service->Write(*Save, TEXT("Character.sav")), ENamecSaveWriteResult::DiskFull);
    TestTrue(TEXT("File bytes are identical after DiskFull"), ReadSaveBytes(TEXT("Character.sav")) == Before);

    // A directory squatting on the temp path makes the temp write itself fail.
    Platform->FreeDiskSpaceBytes.Reset();
    const FString TempPath = UNamecSaveFileService::GetTempFilePath(TEXT("Character.sav"));
    IFileManager::Get().MakeDirectory(*TempPath, true);
    TestEqual(TEXT("Temp file cannot be written"), Service->Write(*Save, TEXT("Character.sav")), ENamecSaveWriteResult::WriteFailed);
    TestTrue(TEXT("File bytes are identical after WriteFailed"), ReadSaveBytes(TEXT("Character.sav")) == Before);

    const UNamecCharacterSave* Loaded = Cast<UNamecCharacterSave>(Service->Load(TEXT("Character.sav"), UNamecCharacterSave::StaticClass()).Save);
    if (TestNotNull(TEXT("The previous file still loads"), Loaded))
    {
        TestEqual(TEXT("Name"), Loaded->CharacterName, FString(TEXT("Previous")));
    }
    return true;
}

#endif
