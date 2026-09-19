#pragma once

#include "CoreMinimal.h"
#include "UI/NamecBackableScreen.h"
#include "Save/NamecSettingsSave.h"
#include "NamecSettingsScreen.generated.h"

class UNamecMenuButton;
class UNamecSettingsService;
class UVerticalBox;

enum class ENamecSettingsTab : uint8
{
    Gameplay,
    Controls,
    Graphics,
    Audio,
};

// One line of the Settings screen: what it shows for the current settings and what selecting it
// does to them. SlotNumber is the local player slot the screen was opened by.
struct FNamecSettingsRow
{
    ENamecSettingsTab Tab;
    FName RowName;
    TFunction<FText(const FNamecSettings&, int32 SlotNumber)> MakeLabel;
    TFunction<void(FNamecSettings&, int32 SlotNumber)> OnSelected;
};

DECLARE_MULTICAST_DELEGATE(FNamecOnOpenInputRemap);

// Each local player's Settings menu. Gameplay and Controls edit that player's slot section;
// Graphics and Audio edit the machine-wide section (game-foundation Requirements 6 and 9).
// Every change goes through UNamecSettingsService::Apply.
UCLASS()
class NAMEC_API UNamecSettingsScreen : public UNamecBackableScreen
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    void OpenFor(UNamecSettingsService& InSettings, int32 InSlotNumber);

    void ShowTab(ENamecSettingsTab Tab);
    ENamecSettingsTab GetShownTab() const { return ShownTab; }

    // Selecting a row is what clicking its button does.
    void SelectRow(FName RowName);
    FText GetRowLabel(FName RowName) const;

    // What the Controls tab's one entry does: the root layout opens the remap screen.
    void OpenInputRemap();

    FNamecOnOpenInputRemap OnOpenInputRemap;

protected:
    virtual void NativeDestruct() override;
    virtual UWidget* NativeGetDesiredFocusTarget() const override;

private:
    void RefreshRows(const FNamecSettings& AppliedSettings);

    UPROPERTY()
    TObjectPtr<UNamecSettingsService> Settings;

    int32 SlotNumber = 0;
    ENamecSettingsTab ShownTab = ENamecSettingsTab::Gameplay;

    UPROPERTY()
    TArray<TObjectPtr<UNamecMenuButton>> TabButtons;

    UPROPERTY()
    TArray<TObjectPtr<UNamecMenuButton>> RowButtons;

    UPROPERTY()
    TObjectPtr<UNamecMenuButton> RemapButton;

    UPROPERTY()
    TObjectPtr<UNamecMenuButton> BackButton;
};
