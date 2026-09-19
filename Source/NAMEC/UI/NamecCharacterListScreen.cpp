#include "UI/NamecCharacterListScreen.h"
#include "UI/NamecCppWidgetTree.h"
#include "UI/NamecMenuButton.h"
#include "UI/NamecMessageModal.h"
#include "Save/NamecCharacterSave.h"
#include "Save/NamecSaveFileService.h"
#include "Components/VerticalBox.h"

#define LOCTEXT_NAMESPACE "NamecCharacterList"

FNamecCharacterListing UNamecCharacterListScreen::ReadCharacters(const UNamecSaveFileService& SaveFiles)
{
    FNamecCharacterListing Listing;
    for (const FString& FileName : SaveFiles.FindSaveFiles(UNamecCharacterSave::FileNamePrefix))
    {
        const FNamecSaveLoadOutcome Outcome = SaveFiles.Load(FileName, UNamecCharacterSave::StaticClass());
        switch (Outcome.Result)
        {
        case ENamecSaveLoadResult::Ok:
            Listing.Entries.Add({ FText::FromString(CastChecked<UNamecCharacterSave>(Outcome.Save)->CharacterName), true });
            break;
        case ENamecSaveLoadResult::NewerVersion:
            // The service hands out no object for a newer save, so the file name stands in for the character's.
            Listing.Entries.Add({ FText::Format(LOCTEXT("NewerVersionEntry", "{0} - {1}"), FText::FromString(FPaths::GetBaseFilename(FileName)), NamecSaveMessages::RequiresNewerVersion()), false });
            break;
        case ENamecSaveLoadResult::Corrupt:
            Listing.bAnyCorrupt = true;
            break;
        case ENamecSaveLoadResult::Missing:
            break;
        }
    }
    return Listing;
}

bool UNamecCharacterListScreen::Initialize()
{
    if (UWidgetTree* Tree = NamecCppWidgetTree::BeginBuild(*this))
    {
        UVerticalBox* Box = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Screen"));
        CharacterBox = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Characters"));
        BackButton = Tree->ConstructWidget<UNamecMenuButton>(UNamecMenuButton::StaticClass(), TEXT("Back"));
        BackButton->SetLabel(LOCTEXT("Back", "Back"));
        BackButton->OnClicked().AddUObject(this, &UNamecCharacterListScreen::GoBack);
        Box->AddChildToVerticalBox(CharacterBox);
        Box->AddChildToVerticalBox(BackButton);
        Tree->RootWidget = Box;
    }
    return Super::Initialize();
}

void UNamecCharacterListScreen::ShowCharacters(const FNamecCharacterListing& Listing)
{
    // The stack pools its screens, so this instance may still hold its last visit's entries.
    CharacterBox->ClearChildren();
    CharacterButtons.Reset();

    TArray<UWidget*> FocusableEntries;
    for (const FNamecCharacterListEntry& Entry : Listing.Entries)
    {
        UNamecMenuButton* Button = WidgetTree->ConstructWidget<UNamecMenuButton>(UNamecMenuButton::StaticClass());
        Button->SetLabel(Entry.Label);
        Button->SetIsEnabled(Entry.bSelectable);
        CharacterBox->AddChildToVerticalBox(Button);
        CharacterButtons.Add(Button);
        if (Entry.bSelectable)
        {
            FocusableEntries.Add(Button);
        }
    }
    FocusableEntries.Add(BackButton);
    NamecMenuNavigation::LinkVertically(FocusableEntries);
}

UWidget* UNamecCharacterListScreen::NativeGetDesiredFocusTarget() const
{
    UNamecMenuButton* FirstEnabled = NamecMenuNavigation::FindFirstEnabled(CharacterButtons);
    return FirstEnabled ? FirstEnabled : BackButton.Get();
}

#undef LOCTEXT_NAMESPACE
