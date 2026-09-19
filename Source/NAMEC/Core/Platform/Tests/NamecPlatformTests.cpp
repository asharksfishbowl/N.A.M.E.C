#include "Core/Platform/INamecPlatform.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Misc/Paths.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecPlatformSaveDirectoryTest, "Namec.Foundation.Platform.SaveDirectoryUnderProjectSaved", NamecFoundationTestFlags)

bool FNamecPlatformSaveDirectoryTest::RunTest(const FString& Parameters)
{
    const FString SaveDirectory = FNamecPlatform::Get().GetSaveDirectory();
    const FString ProjectSaved = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());

    TestTrue(FString::Printf(TEXT("'%s' is under '%s'"), *SaveDirectory, *ProjectSaved), FPaths::IsUnderDirectory(SaveDirectory, ProjectSaved));
    TestNotEqual(TEXT("Save directory is not the Saved folder itself"), SaveDirectory, ProjectSaved);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecPlatformUserIdTest, "Namec.Foundation.Platform.UserIdIsNonEmpty", NamecFoundationTestFlags)

bool FNamecPlatformUserIdTest::RunTest(const FString& Parameters)
{
    TestFalse(TEXT("Local user id is empty"), FNamecPlatform::Get().GetLocalUserId().IsEmpty());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecPlatformFreeDiskSpaceTest, "Namec.Foundation.Platform.FreeDiskSpaceOfSavedFolder", NamecFoundationTestFlags)

bool FNamecPlatformFreeDiskSpaceTest::RunTest(const FString& Parameters)
{
    const FString ProjectSaved = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir());
    const TOptional<uint64> FreeBytes = FNamecPlatform::Get().GetFreeDiskSpaceBytes(ProjectSaved);

    TestTrue(TEXT("The platform answers for the project Saved folder"), FreeBytes.IsSet());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecPlatformSwapTest, "Namec.Foundation.Platform.SetSwapsAndRestores", NamecFoundationTestFlags)

bool FNamecPlatformSwapTest::RunTest(const FString& Parameters)
{
    INamecPlatform* Original = &FNamecPlatform::Get();

    TSharedRef<INamecPlatform> Fake = MakeShared<FNamecTestPlatform>();
    TSharedRef<INamecPlatform> Previous = FNamecPlatform::Set(Fake);

    TestTrue(TEXT("Set returns the implementation that was installed"), &Previous.Get() == Original);
    TestTrue(TEXT("Get returns the fake"), &FNamecPlatform::Get() == &Fake.Get());
    TestEqual(TEXT("Calls reach the fake"), FNamecPlatform::Get().GetLocalUserId(), FString(TEXT("TestUserId")));

    FNamecPlatform::Set(Previous);
    TestTrue(TEXT("The original is restored"), &FNamecPlatform::Get() == Original);
    return true;
}

#endif
