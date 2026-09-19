#include "UI/NamecMainMenuScreen.h"
#include "UI/NamecMenuButton.h"
#include "UI/NamecCppWidgetTree.h"
#include "Components/VerticalBox.h"

#define LOCTEXT_NAMESPACE "NamecMainMenu"

namespace
{
    struct FNamecMainMenuEntryInfo
    {
        ENamecMainMenuEntry Entry;
        FText Label;
        bool bEnabled;
    };

    // New World, Load World and Join LAN Game are Phase 3 and Phase 4 flows. They are present and
    // disabled, and no placeholder screen is built for them.
    TArray<FNamecMainMenuEntryInfo> GetEntryInfos()
    {
        return {
            { ENamecMainMenuEntry::NewWorld, LOCTEXT("NewWorld", "New World"), false },
            { ENamecMainMenuEntry::LoadWorld, LOCTEXT("LoadWorld", "Load World"), false },
            { ENamecMainMenuEntry::JoinLanGame, LOCTEXT("JoinLanGame", "Join LAN Game"), false },
            { ENamecMainMenuEntry::Characters, LOCTEXT("Characters", "Characters"), true },
            { ENamecMainMenuEntry::Settings, LOCTEXT("Settings", "Settings"), true },
            { ENamecMainMenuEntry::Quit, LOCTEXT("Quit", "Quit"), true },
        };
    }
}

bool UNamecMainMenuScreen::Initialize()
{
    if (UWidgetTree* Tree = NamecCppWidgetTree::BeginBuild(*this))
    {
        UVerticalBox* Box = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Entries"));
        TArray<UWidget*> FocusableEntries;
        for (const FNamecMainMenuEntryInfo& Info : GetEntryInfos())
        {
            UNamecMenuButton* Button = Tree->ConstructWidget<UNamecMenuButton>(UNamecMenuButton::StaticClass());
            Button->SetLabel(Info.Label);
            Button->SetIsEnabled(Info.bEnabled);
            Button->OnClicked().AddUObject(this, &UNamecMainMenuScreen::SelectEntry, Info.Entry);
            Box->AddChildToVerticalBox(Button);
            EntryButtons.Add(Button);
            if (Info.bEnabled)
            {
                FocusableEntries.Add(Button);
            }
        }
        NamecMenuNavigation::LinkVertically(FocusableEntries);
        Tree->RootWidget = Box;
    }
    return Super::Initialize();
}

void UNamecMainMenuScreen::SelectEntry(ENamecMainMenuEntry Entry)
{
    if (EntryButtons[static_cast<int32>(Entry)]->GetIsEnabled())
    {
        OnEntrySelected.Broadcast(Entry);
    }
}

UWidget* UNamecMainMenuScreen::NativeGetDesiredFocusTarget() const
{
    return NamecMenuNavigation::FindFirstEnabled(EntryButtons);
}

#undef LOCTEXT_NAMESPACE
