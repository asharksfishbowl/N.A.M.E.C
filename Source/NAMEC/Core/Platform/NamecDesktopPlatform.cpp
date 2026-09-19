#include "Core/Platform/NamecDesktopPlatform.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "HAL/PlatformMisc.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"

FString FNamecDesktopPlatform::GetSaveDirectory() const
{
    return FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("SaveGames"));
}

FString FNamecDesktopPlatform::GetLocalUserId() const
{
    return FPlatformMisc::GetLoginId();
}

FString FNamecDesktopPlatform::GetLocalUserDisplayName() const
{
    return FPlatformProcess::UserName(false);
}

FPlatformUserId FNamecDesktopPlatform::GetPlatformUserForInputDevice(FInputDeviceId DeviceId) const
{
    return IPlatformInputDeviceMapper::Get().GetUserForInputDevice(DeviceId);
}

TOptional<uint64> FNamecDesktopPlatform::GetFreeDiskSpaceBytes(const FString& Path) const
{
    uint64 TotalBytes = 0;
    uint64 FreeBytes = 0;
    if (!FPlatformMisc::GetDiskTotalAndFreeSpace(Path, TotalBytes, FreeBytes))
    {
        return TOptional<uint64>();
    }
    return FreeBytes;
}

bool FNamecDesktopPlatform::ShowTextEntryKeyboard(const FText& Prompt, const FString& InitialText, TFunction<void(const FString&)> OnAccepted)
{
    // A desktop has no system keyboard overlay.
    return false;
}
