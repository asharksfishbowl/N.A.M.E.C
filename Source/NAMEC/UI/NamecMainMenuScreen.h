#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "NamecMainMenuScreen.generated.h"

class UNamecMenuButton;

// game-foundation Requirement 11's entries, in its order.
enum class ENamecMainMenuEntry : uint8
{
    NewWorld,
    LoadWorld,
    JoinLanGame,
    Characters,
    Settings,
    Quit,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FNamecOnMainMenuEntrySelected, ENamecMainMenuEntry);

UCLASS()
class NAMEC_API UNamecMainMenuScreen : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    // One button per ENamecMainMenuEntry, indexed by it.
    const TArray<UNamecMenuButton*>& GetEntryButtons() const { return EntryButtons; }

    void SelectEntry(ENamecMainMenuEntry Entry);

    FNamecOnMainMenuEntrySelected OnEntrySelected;

protected:
    virtual UWidget* NativeGetDesiredFocusTarget() const override;

private:
    UPROPERTY()
    TArray<TObjectPtr<UNamecMenuButton>> EntryButtons;
};
