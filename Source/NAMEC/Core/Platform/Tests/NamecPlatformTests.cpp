#include "Core/Platform/INamecPlatform.h"
#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
    class FNamecFakePlatform : public INamecPlatform
    {
    public:
        virtual FString GetSaveDirectory() const override { return TEXT("FakeSaveDirectory"); }
        virtual FString GetLocalUserId() const override { return TEXT("FakeUserId"); }
        virtual FString GetLocalUserDisplayName() const override { return TEXT("FakeUser"); }
        virtual FPlatformUserId GetPlatformUserForInputDevice(FInputDeviceId DeviceId) const override { return PLATFORMUSERID_NONE; }
        virtual TOptional<uint64> GetFreeDiskSpaceBytes(const FString& Path) const override { return TOptional<uint64>(0); }
        virtual bool ShowTextEntryKeyboard(const FText& Prompt, const FString& InitialText, TFunction<void(const FString&)> OnAccepted) override { return false; }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecPlatformSaveDirectoryTest, "Namec.Foundation.Platform.SaveDirectoryUnderProjectSaved",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FNamecPlatformSaveDirectoryTest::RunTest(const FString& Parameters)
{
    const FString SaveDirectory = FNamecPlatform::Get().GetSaveDirectory();
    const FString ProjectSaved = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());

    TestTrue(FString::Printf(TEXT("'%s' is under '%s'"), *SaveDirectory, *ProjectSaved), FPaths::IsUnderDirectory(SaveDirectory, ProjectSaved));
    TestNotEqual(TEXT("Save directory is not the Saved folder itself"), SaveDirectory, ProjectSaved);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecPlatformUserIdTest, "Namec.Foundation.Platform.UserIdIsNonEmpty",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FNamecPlatformUserIdTest::RunTest(const FString& Parameters)
{
    TestFalse(TEXT("Local user id is empty"), FNamecPlatform::Get().GetLocalUserId().IsEmpty());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecPlatformFreeDiskSpaceTest, "Namec.Foundation.Platform.FreeDiskSpaceOfSavedFolder",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FNamecPlatformFreeDiskSpaceTest::RunTest(const FString& Parameters)
{
    const FString ProjectSaved = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
    const TOptional<uint64> FreeBytes = FNamecPlatform::Get().GetFreeDiskSpaceBytes(ProjectSaved);

    TestTrue(TEXT("The platform answers for the project Saved folder"), FreeBytes.IsSet());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecPlatformSwapTest, "Namec.Foundation.Platform.SetSwapsAndRestores",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FNamecPlatformSwapTest::RunTest(const FString& Parameters)
{
    INamecPlatform* Original = &FNamecPlatform::Get();

    TSharedRef<INamecPlatform> Fake = MakeShared<FNamecFakePlatform>();
    TSharedRef<INamecPlatform> Previous = FNamecPlatform::Set(Fake);

    TestTrue(TEXT("Set returns the implementation that was installed"), &Previous.Get() == Original);
    TestTrue(TEXT("Get returns the fake"), &FNamecPlatform::Get() == &Fake.Get());
    TestEqual(TEXT("Calls reach the fake"), FNamecPlatform::Get().GetLocalUserId(), FString(TEXT("FakeUserId")));

    FNamecPlatform::Set(Previous);
    TestTrue(TEXT("The original is restored"), &FNamecPlatform::Get() == Original);
    return true;
}

#endif
