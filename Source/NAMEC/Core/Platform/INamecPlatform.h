#pragma once

#include "CoreMinimal.h"

// Every platform-specific call the game makes (game-foundation Requirement 10).
// A console port supplies one implementation of this and installs it with FNamecPlatform::Set.
class NAMEC_API INamecPlatform
{
public:
    virtual ~INamecPlatform() = default;

    virtual FString GetSaveDirectory() const = 0;

    virtual FString GetLocalUserId() const = 0;

    virtual FString GetLocalUserDisplayName() const = 0;

    virtual FPlatformUserId GetPlatformUserForInputDevice(FInputDeviceId DeviceId) const = 0;

    // Unset when the platform cannot answer for that path; the caller decides what that means.
    virtual TOptional<uint64> GetFreeDiskSpaceBytes(const FString& Path) const = 0;

    // Returns whether the platform showed its own keyboard. When false, OnAccepted is never
    // called and the calling screen supplies its own text entry.
    virtual bool ShowTextEntryKeyboard(const FText& Prompt, const FString& InitialText, TFunction<void(const FString&)> OnAccepted) = 0;
};

class NAMEC_API FNamecPlatform
{
public:
    static INamecPlatform& Get();

    // Returns the implementation that was installed before, so a test can put it back.
    static TSharedRef<INamecPlatform> Set(TSharedRef<INamecPlatform> NewPlatform);
};
