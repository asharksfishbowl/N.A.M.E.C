#include "UI/Settings/NamecSettingsScreen.h"
#include "UI/NamecCppWidgetTree.h"
#include "UI/NamecMenuButton.h"
#include "Save/NamecSettingsService.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"

#define LOCTEXT_NAMESPACE "NamecSettingsScreen"

namespace
{
    FText OnOff(bool bOn)
    {
        return bOn ? LOCTEXT("On", "On") : LOCTEXT("Off", "Off");
    }

    // Steps up and wraps to the low end, so one Select action reaches every value on a gamepad.
    int32 CycleInt(int32 Value, int32 Min, int32 Max)
    {
        return Value >= Max ? Min : Value + 1;
    }

    float CycleTenths(float Value, float Min, float Max, float Step)
    {
        return Value + Step > Max + KINDA_SMALL_NUMBER ? Min : Value + Step;
    }

    const TArray<FNamecSettingsRow>& GetRows()
    {
        static const TArray<FNamecSettingsRow> Rows =
        {
            { ENamecSettingsTab::Gameplay, TEXT("NightEyes"),
                [](const FNamecSettings& S, int32 Slot) { return FText::Format(LOCTEXT("NightEyes", "Night Eyes: {0}"), OnOff(S.GetSlot(Slot).bNightEyes)); },
                [](FNamecSettings& S, int32 Slot) { S.GetSlot(Slot).bNightEyes = !S.GetSlot(Slot).bNightEyes; } },
            { ENamecSettingsTab::Gameplay, TEXT("ExecutionPrompts"),
                [](const FNamecSettings& S, int32 Slot) { return FText::Format(LOCTEXT("ExecutionPrompts", "Execution prompts: {0}"), OnOff(S.GetSlot(Slot).bExecutionPrompts)); },
                [](FNamecSettings& S, int32 Slot) { S.GetSlot(Slot).bExecutionPrompts = !S.GetSlot(Slot).bExecutionPrompts; } },
            { ENamecSettingsTab::Graphics, TEXT("GIQuality"),
                [](const FNamecSettings& S, int32) { return FText::Format(LOCTEXT("GIQuality", "Global illumination: {0}"), FText::AsNumber(S.Machine.GIQuality)); },
                [](FNamecSettings& S, int32) { S.Machine.GIQuality = CycleInt(S.Machine.GIQuality, 0, 3); } },
            { ENamecSettingsTab::Graphics, TEXT("ShadowQuality"),
                [](const FNamecSettings& S, int32) { return FText::Format(LOCTEXT("ShadowQuality", "Shadow quality: {0}"), FText::AsNumber(S.Machine.ShadowQuality)); },
                [](FNamecSettings& S, int32) { S.Machine.ShadowQuality = CycleInt(S.Machine.ShadowQuality, 0, 3); } },
            { ENamecSettingsTab::Graphics, TEXT("FoliageDensity"),
                [](const FNamecSettings& S, int32) { return FText::Format(LOCTEXT("FoliageDensity", "Foliage density: {0}%"), FText::AsNumber(FMath::RoundToInt(S.Machine.FoliageDensityPercent))); },
                [](FNamecSettings& S, int32) { S.Machine.FoliageDensityPercent = CycleTenths(S.Machine.FoliageDensityPercent, 10.f, 100.f, 10.f); } },
            { ENamecSettingsTab::Graphics, TEXT("ViewDistance"),
                [](const FNamecSettings& S, int32) { return FText::Format(LOCTEXT("ViewDistance", "View distance: {0}%"), FText::AsNumber(FMath::RoundToInt(S.Machine.ViewDistancePercent))); },
                [](FNamecSettings& S, int32) { S.Machine.ViewDistancePercent = CycleTenths(S.Machine.ViewDistancePercent, 10.f, 100.f, 10.f); } },
            { ENamecSettingsTab::Audio, TEXT("MasterVolume"),
                [](const FNamecSettings& S, int32) { return FText::Format(LOCTEXT("MasterVolume", "Master volume: {0}%"), FText::AsNumber(FMath::RoundToInt(S.Machine.MasterVolume * 100.f))); },
                [](FNamecSettings& S, int32) { S.Machine.MasterVolume = CycleTenths(S.Machine.MasterVolume, 0.f, 1.f, 0.1f); } },
            { ENamecSettingsTab::Audio, TEXT("MusicVolume"),
                [](const FNamecSettings& S, int32) { return FText::Format(LOCTEXT("MusicVolume", "Music volume: {0}%"), FText::AsNumber(FMath::RoundToInt(S.Machine.MusicVolume * 100.f))); },
                [](FNamecSettings& S, int32) { S.Machine.MusicVolume = CycleTenths(S.Machine.MusicVolume, 0.f, 1.f, 0.1f); } },
            { ENamecSettingsTab::Audio, TEXT("EffectsVolume"),
                [](const FNamecSettings& S, int32) { return FText::Format(LOCTEXT("EffectsVolume", "Effects volume: {0}%"), FText::AsNumber(FMath::RoundToInt(S.Machine.EffectsVolume * 100.f))); },
                [](FNamecSettings& S, int32) { S.Machine.EffectsVolume = CycleTenths(S.Machine.EffectsVolume, 0.f, 1.f, 0.1f); } },
        };
        return Rows;
    }

    FText GetTabLabel(ENamecSettingsTab Tab)
    {
        switch (Tab)
        {
        case ENamecSettingsTab::Gameplay: return LOCTEXT("Gameplay", "Gameplay");
        case ENamecSettingsTab::Controls: return LOCTEXT("Controls", "Controls");
        case ENamecSettingsTab::Graphics: return LOCTEXT("Graphics", "Graphics");
        case ENamecSettingsTab::Audio: return LOCTEXT("Audio", "Audio");
        }
        return FText::GetEmpty();
    }
}

bool UNamecSettingsScreen::Initialize()
{
    if (UWidgetTree* Tree = NamecCppWidgetTree::BeginBuild(*this))
    {
        UVerticalBox* Box = Tree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Screen"));
        UHorizontalBox* Tabs = Tree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("Tabs"));
        Box->AddChildToVerticalBox(Tabs);

        for (const ENamecSettingsTab Tab : { ENamecSettingsTab::Gameplay, ENamecSettingsTab::Controls, ENamecSettingsTab::Graphics, ENamecSettingsTab::Audio })
        {
            UNamecMenuButton* TabButton = Tree->ConstructWidget<UNamecMenuButton>(UNamecMenuButton::StaticClass());
            TabButton->SetLabel(GetTabLabel(Tab));
            TabButton->OnClicked().AddUObject(this, &UNamecSettingsScreen::ShowTab, Tab);
            Tabs->AddChildToHorizontalBox(TabButton);
            TabButtons.Add(TabButton);
        }
        for (const FNamecSettingsRow& Row : GetRows())
        {
            UNamecMenuButton* RowButton = Tree->ConstructWidget<UNamecMenuButton>(UNamecMenuButton::StaticClass());
            RowButton->OnClicked().AddUObject(this, &UNamecSettingsScreen::SelectRow, Row.RowName);
            Box->AddChildToVerticalBox(RowButton);
            RowButtons.Add(RowButton);
        }

        RemapButton = Tree->ConstructWidget<UNamecMenuButton>(UNamecMenuButton::StaticClass(), TEXT("Remap"));
        RemapButton->SetLabel(LOCTEXT("RemapControls", "Remap controls"));
        RemapButton->OnClicked().AddUObject(this, &UNamecSettingsScreen::OpenInputRemap);
        Box->AddChildToVerticalBox(RemapButton);

        BackButton = Tree->ConstructWidget<UNamecMenuButton>(UNamecMenuButton::StaticClass(), TEXT("Back"));
        BackButton->SetLabel(LOCTEXT("Back", "Back"));
        BackButton->OnClicked().AddUObject(this, &UNamecSettingsScreen::GoBack);
        Box->AddChildToVerticalBox(BackButton);
        Tree->RootWidget = Box;
    }
    return Super::Initialize();
}

void UNamecSettingsScreen::OpenFor(UNamecSettingsService& InSettings, int32 InSlotNumber)
{
    // The stack pools its screens: this instance may still be listening to its last visit's service.
    if (Settings)
    {
        Settings->OnSettingsApplied.RemoveAll(this);
    }
    Settings = &InSettings;
    SlotNumber = InSlotNumber;
    Settings->OnSettingsApplied.AddUObject(this, &UNamecSettingsScreen::RefreshRows);
    ShowTab(ENamecSettingsTab::Gameplay);
}

void UNamecSettingsScreen::ShowTab(ENamecSettingsTab Tab)
{
    ShownTab = Tab;
    RefreshRows(Settings->GetSettings());
}

void UNamecSettingsScreen::RefreshRows(const FNamecSettings& AppliedSettings)
{
    TArray<UWidget*> FocusableEntries;
    for (int32 Index = 0; Index < RowButtons.Num(); ++Index)
    {
        const FNamecSettingsRow& Row = GetRows()[Index];
        const bool bShown = Row.Tab == ShownTab;
        RowButtons[Index]->SetLabel(Row.MakeLabel(AppliedSettings, SlotNumber));
        RowButtons[Index]->SetVisibility(bShown ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        if (bShown)
        {
            FocusableEntries.Add(RowButtons[Index]);
        }
    }
    // The Controls tab holds no setting of its own: remapping is a screen, not a row.
    const bool bControlsShown = ShownTab == ENamecSettingsTab::Controls;
    RemapButton->SetVisibility(bControlsShown ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    if (bControlsShown)
    {
        FocusableEntries.Add(RemapButton);
    }
    FocusableEntries.Add(BackButton);
    NamecMenuNavigation::LinkVertically(FocusableEntries);
}

void UNamecSettingsScreen::OpenInputRemap()
{
    OnOpenInputRemap.Broadcast();
}

void UNamecSettingsScreen::SelectRow(FName RowName)
{
    const FNamecSettingsRow* Row = GetRows().FindByPredicate([RowName](const FNamecSettingsRow& Candidate) { return Candidate.RowName == RowName; });
    if (!Row)
    {
        return;
    }

    FNamecSettings Edited = Settings->GetSettings();
    Row->OnSelected(Edited, SlotNumber);
    Settings->Apply(Edited);
}

FText UNamecSettingsScreen::GetRowLabel(FName RowName) const
{
    const int32 Index = GetRows().IndexOfByPredicate([RowName](const FNamecSettingsRow& Candidate) { return Candidate.RowName == RowName; });
    return RowButtons.IsValidIndex(Index) ? RowButtons[Index]->GetLabel() : FText::GetEmpty();
}

void UNamecSettingsScreen::NativeDestruct()
{
    if (Settings)
    {
        Settings->OnSettingsApplied.RemoveAll(this);
    }
    Super::NativeDestruct();
}

UWidget* UNamecSettingsScreen::NativeGetDesiredFocusTarget() const
{
    if (ShownTab == ENamecSettingsTab::Controls)
    {
        return RemapButton;
    }
    const int32 FirstShown = GetRows().IndexOfByPredicate([this](const FNamecSettingsRow& Row) { return Row.Tab == ShownTab; });
    return RowButtons.IsValidIndex(FirstShown) ? RowButtons[FirstShown].Get() : BackButton.Get();
}

#undef LOCTEXT_NAMESPACE
