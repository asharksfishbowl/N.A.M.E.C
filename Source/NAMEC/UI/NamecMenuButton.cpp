#include "UI/NamecMenuButton.h"
#include "UI/NamecCppWidgetTree.h"
#include "Components/TextBlock.h"

bool UNamecMenuButton::Initialize()
{
    if (UWidgetTree* Tree = NamecCppWidgetTree::BeginBuild(*this))
    {
        Label = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Label"));
        Tree->RootWidget = Label;
    }
    return Super::Initialize();
}

void UNamecMenuButton::SetLabel(const FText& InLabel)
{
    Label->SetText(InLabel);
}

FText UNamecMenuButton::GetLabel() const
{
    return Label->GetText();
}

void NamecMenuNavigation::LinkVertically(TConstArrayView<UWidget*> Entries)
{
    for (int32 Index = 0; Index < Entries.Num(); ++Index)
    {
        Entries[Index]->SetNavigationRuleExplicit(EUINavigation::Down, Entries[(Index + 1) % Entries.Num()]);
        Entries[Index]->SetNavigationRuleExplicit(EUINavigation::Up, Entries[(Index + Entries.Num() - 1) % Entries.Num()]);
    }
}

UNamecMenuButton* NamecMenuNavigation::FindFirstEnabled(TConstArrayView<TObjectPtr<UNamecMenuButton>> Buttons)
{
    const TObjectPtr<UNamecMenuButton>* FirstEnabled = Buttons.FindByPredicate([](const UNamecMenuButton* Button) { return Button->GetIsEnabled(); });
    return FirstEnabled ? FirstEnabled->Get() : nullptr;
}
