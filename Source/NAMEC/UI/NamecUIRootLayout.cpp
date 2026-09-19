#include "UI/NamecUIRootLayout.h"
#include "UI/NamecCharacterListScreen.h"
#include "UI/NamecMessageModal.h"
#include "UI/Settings/InputRemapScreen/NamecInputRemapScreen.h"
#include "UI/Settings/NamecSettingsScreen.h"
#include "UI/NamecCppWidgetTree.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

bool UNamecUIRootLayout::Initialize()
{
    if (UWidgetTree* Tree = NamecCppWidgetTree::BeginBuild(*this))
    {
        UOverlay* Root = Tree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("Root"));
        HudLayer = Tree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("HudLayer"));
        HudNotice = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HudNotice"));
        HudLayer->AddChildToOverlay(HudNotice);
        MenuStack = Tree->ConstructWidget<UCommonActivatableWidgetStack>(UCommonActivatableWidgetStack::StaticClass(), TEXT("MenuStack"));
        // No transition: a screen is active the moment it is pushed, not some frames later.
        MenuStack->SetTransitionDuration(0.f);
        Root->AddChildToOverlay(HudLayer);
        Root->AddChildToOverlay(MenuStack);
        Tree->RootWidget = Root;
    }
    return Super::Initialize();
}

void UNamecUIRootLayout::UseSaveFiles(UNamecSaveFileService& InSaveFiles)
{
    SaveFiles = &InSaveFiles;
}

void UNamecUIRootLayout::ShowHudNotice(const FText& Notice)
{
    HudNotice->SetText(Notice);
}

FText UNamecUIRootLayout::GetHudNotice() const
{
    return HudNotice->GetText();
}

void UNamecUIRootLayout::UseSettings(UNamecSettingsService& InSettings, UNamecInputContextSubsystem& InInputContexts, int32 InSlotNumber)
{
    Settings = &InSettings;
    InputContexts = &InInputContexts;
    SlotNumber = InSlotNumber;
}

UNamecMainMenuScreen* UNamecUIRootLayout::ShowMainMenu()
{
    // The stack pools its screens: this may be an instance this layout already bound to.
    UNamecMainMenuScreen* MainMenu = MenuStack->AddWidget<UNamecMainMenuScreen>(UNamecMainMenuScreen::StaticClass());
    MainMenu->OnEntrySelected.RemoveAll(this);
    MainMenu->OnEntrySelected.AddUObject(this, &UNamecUIRootLayout::OnMainMenuEntrySelected);
    return MainMenu;
}

UCommonActivatableWidget* UNamecUIRootLayout::GetActiveMenuScreen() const
{
    return MenuStack->GetActiveWidget();
}

void UNamecUIRootLayout::OnMainMenuEntrySelected(ENamecMainMenuEntry Entry)
{
    switch (Entry)
    {
    case ENamecMainMenuEntry::Characters:
        ShowCharacterList();
        break;
    case ENamecMainMenuEntry::Quit:
        UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
        break;
    case ENamecMainMenuEntry::Settings:
        ShowSettings();
        break;
    case ENamecMainMenuEntry::NewWorld:
    case ENamecMainMenuEntry::LoadWorld:
    case ENamecMainMenuEntry::JoinLanGame:
        break;
    }
}

void UNamecUIRootLayout::ShowCharacterList()
{
    check(SaveFiles);
    const FNamecCharacterListing Listing = UNamecCharacterListScreen::ReadCharacters(*SaveFiles);

    // Edge Case 1: the message, then back to the menu. Dismissing the modal leaves the main menu
    // on top, so no list is pushed at all.
    if (Listing.bAnyCorrupt)
    {
        MenuStack->AddWidget<UNamecMessageModal>(UNamecMessageModal::StaticClass())->SetMessage(NamecSaveMessages::SaveCouldNotBeLoaded());
        return;
    }
    MenuStack->AddWidget<UNamecCharacterListScreen>(UNamecCharacterListScreen::StaticClass())->ShowCharacters(Listing);
}

void UNamecUIRootLayout::ShowSettings()
{
    check(Settings);
    // The stack pools its screens: this may be an instance this layout already bound to.
    UNamecSettingsScreen* SettingsScreen = MenuStack->AddWidget<UNamecSettingsScreen>(UNamecSettingsScreen::StaticClass());
    SettingsScreen->OnOpenInputRemap.RemoveAll(this);
    SettingsScreen->OnOpenInputRemap.AddUObject(this, &UNamecUIRootLayout::ShowInputRemap);
    SettingsScreen->OpenFor(*Settings, SlotNumber);
}

void UNamecUIRootLayout::ShowInputRemap()
{
    check(Settings && InputContexts);
    MenuStack->AddWidget<UNamecInputRemapScreen>(UNamecInputRemapScreen::StaticClass())->OpenFor(*InputContexts, *Settings, SlotNumber);
}
