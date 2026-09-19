#pragma once

#include "CoreMinimal.h"
#include "UI/NamecBackableScreen.h"
#include "Core/Input/NamecBaseInputBindings.h"
#include "NamecInputRemapScreen.generated.h"

class UNamecInputContextSubsystem;
class UNamecMenuButton;
class UNamecSettingsService;
class UVerticalBox;

// Lists the base bindings of this local player's devices, captures the next key for the one the
// player picks, and saves it through that player's UNamecInputContextSubsystem, which applies the
// swap rule and touches only that slot's section (game-foundation Requirement 14).
UCLASS()
class NAMEC_API UNamecInputRemapScreen : public UNamecBackableScreen
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    void OpenFor(UNamecInputContextSubsystem& InContexts, UNamecSettingsService& InSettings, int32 InSlotNumber);

    // Picking a binding is what clicking its button does: the next key of that device is captured.
    void BeginCapture(ENamecInputDevice Device, FName BindingName);
    bool IsCapturing() const { return !CapturingBinding.IsNone(); }

    // False, still capturing, when Key belongs to the other device.
    bool CaptureKey(const FKey& Key);

    FText GetBindingLabel(ENamecInputDevice Device, FName BindingName) const;

    // Leaving also abandons a capture in progress.
    virtual void GoBack() override;

protected:
    virtual void NativeDestruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual UWidget* NativeGetDesiredFocusTarget() const override;

private:
    struct FListedBinding
    {
        ENamecInputDevice Device;
        FName BindingName;
    };

    void RebuildBindingList();
    void RefreshLabels(const struct FNamecSettings& AppliedSettings);

    UPROPERTY()
    TObjectPtr<UNamecInputContextSubsystem> Contexts;

    UPROPERTY()
    TObjectPtr<UNamecSettingsService> Settings;

    int32 SlotNumber = 0;

    ENamecInputDevice CapturingDevice = ENamecInputDevice::Gamepad;
    FName CapturingBinding;

    TArray<FListedBinding> ListedBindings;

    UPROPERTY()
    TObjectPtr<UVerticalBox> BindingBox;

    UPROPERTY()
    TArray<TObjectPtr<UNamecMenuButton>> BindingButtons;

    UPROPERTY()
    TObjectPtr<UNamecMenuButton> BackButton;
};
