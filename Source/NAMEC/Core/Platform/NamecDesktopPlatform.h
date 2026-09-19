#pragma once

#include "CoreMinimal.h"
#include "Core/Platform/INamecPlatform.h"

class NAMEC_API FNamecDesktopPlatform : public INamecPlatform
{
public:
    virtual FString GetSaveDirectory() const override;
    virtual FString GetLocalUserId() const override;
    virtual FString GetLocalUserDisplayName() const override;
    virtual FPlatformUserId GetPlatformUserForInputDevice(FInputDeviceId DeviceId) const override;
    virtual TOptional<uint64> GetFreeDiskSpaceBytes(const FString& Path) const override;
    virtual bool ShowTextEntryKeyboard(const FText& Prompt, const FString& InitialText, TFunction<void(const FString&)> OnAccepted) override;
};
