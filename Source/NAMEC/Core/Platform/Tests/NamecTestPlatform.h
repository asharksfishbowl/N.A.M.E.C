#pragma once

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Core/Platform/INamecPlatform.h"
#include "HAL/FileManager.h"
#include "Misc/Guid.h"
#include "Misc/Paths.h"

// A platform whose save directory and free-space answer the test controls.
class FNamecTestPlatform : public INamecPlatform
{
public:
    FString SaveDirectory = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("AutomationSaves") / FGuid::NewGuid().ToString());
    TOptional<uint64> FreeDiskSpaceBytes;

    virtual FString GetSaveDirectory() const override { return SaveDirectory; }
    virtual FString GetLocalUserId() const override { return TEXT("TestUserId"); }
    virtual FString GetLocalUserDisplayName() const override { return TEXT("TestUser"); }
    virtual FPlatformUserId GetPlatformUserForInputDevice(FInputDeviceId DeviceId) const override { return PLATFORMUSERID_NONE; }
    virtual TOptional<uint64> GetFreeDiskSpaceBytes(const FString& Path) const override { return FreeDiskSpaceBytes; }
    virtual bool ShowTextEntryKeyboard(const FText& Prompt, const FString& InitialText, TFunction<void(const FString&)> OnAccepted) override { return false; }
};

class FNamecScopedTestPlatform
{
public:
    FNamecScopedTestPlatform()
        : Platform(MakeShared<FNamecTestPlatform>())
        , Previous(FNamecPlatform::Set(Platform))
    {
        IFileManager::Get().MakeDirectory(*Platform->SaveDirectory, true);
    }

    ~FNamecScopedTestPlatform()
    {
        FNamecPlatform::Set(Previous);
        IFileManager::Get().DeleteDirectory(*Platform->SaveDirectory, false, true);
    }

    FNamecTestPlatform* operator->() const { return &Platform.Get(); }

private:
    TSharedRef<FNamecTestPlatform> Platform;
    TSharedRef<INamecPlatform> Previous;
};

#endif
