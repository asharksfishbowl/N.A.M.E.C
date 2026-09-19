#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "NamecMessageModal.generated.h"

class UNamecMenuButton;
class UTextBlock;

// The user-facing strings of game-foundation Edge Cases 1 and 3, in one place.
namespace NamecSaveMessages
{
    NAMEC_API FText SaveCouldNotBeLoaded();
    NAMEC_API FText RequiresNewerVersion();
}

// The one message surface: a line of text and an OK entry. Back dismisses it too.
UCLASS()
class NAMEC_API UNamecMessageModal : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    void SetMessage(const FText& InMessage);
    FText GetMessage() const;

    // Deactivates it, which returns the stack to the screen underneath.
    void Dismiss();

protected:
    virtual UWidget* NativeGetDesiredFocusTarget() const override;
    virtual bool NativeOnHandleBackAction() override;

private:
    UPROPERTY()
    TObjectPtr<UTextBlock> MessageText;

    UPROPERTY()
    TObjectPtr<UNamecMenuButton> OkButton;
};
