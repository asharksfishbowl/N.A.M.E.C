#include "UI/Settings/InputRemapScreen/NamecInputRemapScreen.h"
#include "UI/NamecCppWidgetTree.h"
#include "UI/NamecMenuButton.h"
#include "Core/Input/NamecInputContextSubsystem.h"
#include "Core/Input/NamecInputRemap.h"
#include "Save/NamecSettingsService.h"
#include "Components/VerticalBox.h"

#define LOCTEXT_NAMESPACE "NamecInputRemapScreen"

bool UNamecInputRemapScreen::Initialize()
{
    if (UWidgetTree* Tree = NamecCppWidgetTree::BeginBuild(*this))
    {
        UVerticalBox* Box = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Screen"));
        BindingBox = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Bindings"));
        BackButton = Tree->ConstructWidget<UNamecMenuButton>(UNamecMenuButton::StaticClass(), TEXT("Back"));
        BackButton->SetLabel(LOCTEXT("Back", "Back"));
        BackButton->OnClicked().AddUObject(this, &UNamecInputRemapScreen::GoBack);
        Box->AddChildToVerticalBox(BindingBox);
        Box->AddChildToVerticalBox(BackButton);
        Tree->RootWidget = Box;
    }
    // Key capture arrives through NativeOnKeyDown, which only a focusable widget receives.
    SetIsFocusable(true);
    return Super::Initialize();
}

void UNamecInputRemapScreen::OpenFor(UNamecInputContextSubsystem& InContexts, UNamecSettingsService& InSettings, int32 InSlotNumber)
{
    // The stack pools its screens: this instance may still be listening to its last visit's service.
    if (Settings)
    {
        Settings->OnSettingsApplied.RemoveAll(this);
    }
    Contexts = &InContexts;
    Settings = &InSettings;
    SlotNumber = InSlotNumber;
    CapturingBinding = NAME_None;
    Settings->OnSettingsApplied.AddUObject(this, &UNamecInputRemapScreen::RefreshLabels);

    RebuildBindingList();
    RefreshLabels(Settings->GetSettings());
}

void UNamecInputRemapScreen::RebuildBindingList()
{
    BindingBox->ClearChildren();
    BindingButtons.Reset();
    ListedBindings.Reset();

    TArray<UWidget*> FocusableEntries;
    for (const ENamecInputDevice Device : { ENamecInputDevice::Gamepad, ENamecInputDevice::KeyboardMouse })
    {
        // Only the devices this player has a base context for: keyboard and mouse is slot 1 only.
        if (!Contexts->GetBaseContext(Device))
        {
            continue;
        }
        for (const FNamecBaseBinding& Binding : NamecBaseInput::GetBindings(Device))
        {
            // A stick or the mouse is an axis, not a key a press can be captured for.
            if (Binding.DefaultKey.IsAxis2D())
            {
                continue;
            }
            UNamecMenuButton* Button = WidgetTree->ConstructWidget<UNamecMenuButton>(UNamecMenuButton::StaticClass());
            Button->OnClicked().AddUObject(this, &UNamecInputRemapScreen::BeginCapture, Device, Binding.BindingName);
            BindingBox->AddChildToVerticalBox(Button);
            BindingButtons.Add(Button);
            ListedBindings.Add({ Device, Binding.BindingName });
            FocusableEntries.Add(Button);
        }
    }
    FocusableEntries.Add(BackButton);
    NamecMenuNavigation::LinkVertically(FocusableEntries);
}

void UNamecInputRemapScreen::RefreshLabels(const FNamecSettings& AppliedSettings)
{
    const FNamecLocalPlayerSettings& SlotSettings = AppliedSettings.GetSlot(SlotNumber);
    for (int32 Index = 0; Index < ListedBindings.Num(); ++Index)
    {
        const FListedBinding& Listed = ListedBindings[Index];
        const FNamecBaseBinding* Binding = NamecBaseInput::GetBindings(Listed.Device).FindByPredicate([&Listed](const FNamecBaseBinding& Candidate) { return Candidate.BindingName == Listed.BindingName; });
        const bool bCapturingThis = Listed.BindingName == CapturingBinding && Listed.Device == CapturingDevice;
        const FText KeyText = bCapturingThis ? LOCTEXT("PressAKey", "press a key...") : NamecInputRemap::GetCurrentKey(NamecInputRemap::GetSlotRemaps(SlotSettings, Listed.Device), *Binding).GetDisplayName();
        BindingButtons[Index]->SetLabel(FText::Format(LOCTEXT("BindingLabel", "{0}: {1}"), FText::FromName(Listed.BindingName), KeyText));
    }
}

void UNamecInputRemapScreen::BeginCapture(ENamecInputDevice Device, FName BindingName)
{
    CapturingDevice = Device;
    CapturingBinding = BindingName;
    RefreshLabels(Settings->GetSettings());
}

bool UNamecInputRemapScreen::CaptureKey(const FKey& Key)
{
    if (!IsCapturing() || NamecInputRemap::GetDeviceOfKey(Key) != CapturingDevice)
    {
        return false;
    }

    const FName BindingName = CapturingBinding;
    CapturingBinding = NAME_None;
    // Applying the settings is what refreshes the labels, through OnSettingsApplied.
    Contexts->RemapBinding(BindingName, Key);
    return true;
}

FText UNamecInputRemapScreen::GetBindingLabel(ENamecInputDevice Device, FName BindingName) const
{
    const int32 Index = ListedBindings.IndexOfByPredicate([Device, BindingName](const FListedBinding& Listed) { return Listed.Device == Device && Listed.BindingName == BindingName; });
    return BindingButtons.IsValidIndex(Index) ? BindingButtons[Index]->GetLabel() : FText::GetEmpty();
}

void UNamecInputRemapScreen::GoBack()
{
    CapturingBinding = NAME_None;
    Super::GoBack();
}

void UNamecInputRemapScreen::NativeDestruct()
{
    if (Settings)
    {
        Settings->OnSettingsApplied.RemoveAll(this);
    }
    Super::NativeDestruct();
}

FReply UNamecInputRemapScreen::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    return CaptureKey(InKeyEvent.GetKey()) ? FReply::Handled() : Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

UWidget* UNamecInputRemapScreen::NativeGetDesiredFocusTarget() const
{
    return BindingButtons.Num() > 0 ? BindingButtons[0].Get() : BackButton.Get();
}

#undef LOCTEXT_NAMESPACE
