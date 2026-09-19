#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "NamecCharacterListScreen.generated.h"

class UNamecMenuButton;
class UNamecSaveFileService;
class UVerticalBox;

struct FNamecCharacterListEntry
{
    FText Label;

    // False for a save from a newer build: shown greyed out, never selectable (Edge Case 3).
    bool bSelectable = true;
};

// What this machine's character files hold. Reading it writes nothing.
struct FNamecCharacterListing
{
    TArray<FNamecCharacterListEntry> Entries;

    // A corrupt file is no entry. Edge Case 1 sends the player back to the menu with a message.
    bool bAnyCorrupt = false;
};

// Lists this machine's character saves. It creates and deletes nothing yet: character creation
// is Phase 3.
UCLASS()
class NAMEC_API UNamecCharacterListScreen : public UCommonActivatableWidget
{
    GENERATED_BODY()

public:
    static FNamecCharacterListing ReadCharacters(const UNamecSaveFileService& SaveFiles);

    virtual bool Initialize() override;

    void ShowCharacters(const FNamecCharacterListing& Listing);

    const TArray<TObjectPtr<UNamecMenuButton>>& GetCharacterButtons() const { return CharacterButtons; }

    void GoBack();

protected:
    virtual UWidget* NativeGetDesiredFocusTarget() const override;
    virtual bool NativeOnHandleBackAction() override;

private:
    UPROPERTY()
    TObjectPtr<UVerticalBox> CharacterBox;

    UPROPERTY()
    TArray<TObjectPtr<UNamecMenuButton>> CharacterButtons;

    UPROPERTY()
    TObjectPtr<UNamecMenuButton> BackButton;
};
