#include "UI/NamecMessageModal.h"
#include "UI/NamecMenuButton.h"
#include "UI/NamecCppWidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

#define LOCTEXT_NAMESPACE "NamecMessageModal"

FText NamecSaveMessages::SaveCouldNotBeLoaded()
{
    return LOCTEXT("SaveCouldNotBeLoaded", "Save could not be loaded");
}

FText NamecSaveMessages::RequiresNewerVersion()
{
    return LOCTEXT("RequiresNewerVersion", "Requires newer version");
}

bool UNamecMessageModal::Initialize()
{
    if (UWidgetTree* Tree = NamecCppWidgetTree::BeginBuild(*this))
    {
        UVerticalBox* Box = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Box"));
        MessageText = Tree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("Message"));
        OkButton = Tree->ConstructWidget<UNamecMenuButton>(UNamecMenuButton::StaticClass(), TEXT("Ok"));
        OkButton->SetLabel(LOCTEXT("Ok", "OK"));
        OkButton->OnClicked().AddUObject(this, &UNamecMessageModal::Dismiss);
        Box->AddChildToVerticalBox(MessageText);
        Box->AddChildToVerticalBox(OkButton);
        Tree->RootWidget = Box;
    }
    bIsBackHandler = true;
    return Super::Initialize();
}

void UNamecMessageModal::SetMessage(const FText& InMessage)
{
    MessageText->SetText(InMessage);
}

FText UNamecMessageModal::GetMessage() const
{
    return MessageText->GetText();
}

void UNamecMessageModal::Dismiss()
{
    DeactivateWidget();
}

UWidget* UNamecMessageModal::NativeGetDesiredFocusTarget() const
{
    return OkButton;
}

bool UNamecMessageModal::NativeOnHandleBackAction()
{
    Dismiss();
    return true;
}

#undef LOCTEXT_NAMESPACE
