#include "UI/NamecUILayerSubsystem.h"
#include "UI/NamecUIRootLayout.h"
#include "Save/NamecAutosaveSubsystem.h"
#include "Core/Input/NamecInputContextSubsystem.h"
#include "Save/NamecSaveFileService.h"
#include "Save/NamecSettingsService.h"
#include "Save/NamecWorldLoader.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

void UNamecUILayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // A local player exists before its controller does, and a widget needs the controller.
    GetLocalPlayer()->OnPlayerControllerChanged().AddUObject(this, &UNamecUILayerSubsystem::OnPlayerControllerChanged);
}

void UNamecUILayerSubsystem::Deinitialize()
{
    GetLocalPlayer()->OnPlayerControllerChanged().RemoveAll(this);
    RemoveRootLayout();
    Super::Deinitialize();
}

void UNamecUILayerSubsystem::OnPlayerControllerChanged(APlayerController* NewPlayerController)
{
    if (!NewPlayerController || RootLayout)
    {
        return;
    }

    UGameInstance* GameInstance = GetLocalPlayer()->GetGameInstance();
    FNamecUILayerServices Services;
    Services.SaveFiles = GameInstance->GetSubsystem<UNamecSaveFileService>();
    Services.Autosave = GameInstance->GetSubsystem<UNamecAutosaveSubsystem>();
    Services.Settings = GameInstance->GetSubsystem<UNamecSettingsService>();
    Services.InputContexts = GetLocalPlayer()->GetSubsystem<UNamecInputContextSubsystem>();

    CreateRootLayout(*NewPlayerController->GetWorld(), NewPlayerController, GetLocalPlayer()->GetLocalPlayerIndex() + 1, Services)->AddToPlayerScreen();
}

UNamecUIRootLayout* UNamecUILayerSubsystem::CreateRootLayout(UWorld& World, APlayerController* OwningPlayer, int32 InSlotNumber, const FNamecUILayerServices& Services)
{
    check(Services.SaveFiles && Services.Autosave && Services.Settings && Services.InputContexts);
    RemoveRootLayout();
    SlotNumber = InSlotNumber;

    RootLayout = OwningPlayer ? CreateWidget<UNamecUIRootLayout>(OwningPlayer) : CreateWidget<UNamecUIRootLayout>(&World);
    RootLayout->UseSaveFiles(*Services.SaveFiles);
    RootLayout->UseSettings(*Services.Settings, *Services.InputContexts, SlotNumber);

    if (SlotNumber == 1)
    {
        WarningSource = Services.Autosave;
        WarningSource->OnSaveWarning.AddUObject(this, &UNamecUILayerSubsystem::ShowSaveWarning);
    }
    return RootLayout;
}

void UNamecUILayerSubsystem::RemoveRootLayout()
{
    if (WarningSource)
    {
        WarningSource->OnSaveWarning.RemoveAll(this);
        WarningSource = nullptr;
    }
    if (RootLayout)
    {
        RootLayout->RemoveFromParent();
        RootLayout = nullptr;
    }
}

void UNamecUILayerSubsystem::ShowWorldLoadNotice(const FNamecWorldLoadDecision& Decision)
{
    if (SlotNumber == 1 && RootLayout && Decision.Outcome == ENamecWorldLoadOutcome::LoadWithRevisionUpdate)
    {
        RootLayout->ShowHudNotice(Decision.Message);
    }
}

void UNamecUILayerSubsystem::ShowSaveWarning(const FText& Warning)
{
    RootLayout->ShowHudNotice(Warning);
}
