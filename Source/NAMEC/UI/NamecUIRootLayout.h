#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "UI/NamecMainMenuScreen.h"
#include "NamecUIRootLayout.generated.h"

class UCommonActivatableWidget;
class UCommonActivatableWidgetStack;
class UNamecSaveFileService;
class UOverlay;
class UTextBlock;

// One per local player: a menu stack and a HUD layer. Later phases push their screens on the
// stack and their HUD widgets on the layer.
UCLASS()
class NAMEC_API UNamecUIRootLayout : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    // The service the screens read saves through. A test has no game instance to find it in.
    void UseSaveFiles(UNamecSaveFileService& InSaveFiles);

    UNamecMainMenuScreen* ShowMainMenu();

    UCommonActivatableWidget* GetActiveMenuScreen() const;
    UCommonActivatableWidgetStack* GetMenuStack() const { return MenuStack; }
    UOverlay* GetHudLayer() const { return HudLayer; }

    // One line of text on the HUD layer, replacing the last one. Later phases own its look and timing.
    void ShowHudNotice(const FText& Notice);
    FText GetHudNotice() const;

private:
    void OnMainMenuEntrySelected(ENamecMainMenuEntry Entry);
    void ShowCharacterList();

    UPROPERTY()
    TObjectPtr<UCommonActivatableWidgetStack> MenuStack;

    UPROPERTY()
    TObjectPtr<UOverlay> HudLayer;

    UPROPERTY()
    TObjectPtr<UTextBlock> HudNotice;

    UPROPERTY()
    TObjectPtr<UNamecSaveFileService> SaveFiles;
};
