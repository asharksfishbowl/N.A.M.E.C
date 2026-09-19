#include "UI/NamecUILayerSubsystem.h"
#include "UI/NamecUIRootLayout.h"
#include "Save/NamecAutosaveSubsystem.h"
#include "Save/NamecSaveFileService.h"
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
    UNamecSaveFileService* SaveFiles = GameInstance->GetSubsystem<UNamecSaveFileService>();
    UNamecAutosaveSubsystem* Autosave = GameInstance->GetSubsystem<UNamecAutosaveSubsystem>();
    check(SaveFiles && Autosave);

    CreateRootLayout(*NewPlayerController->GetWorld(), NewPlayerController, GetLocalPlayer()->GetLocalPlayerIndex() + 1, *SaveFiles, *Autosave)->AddToPlayerScreen();
}

UNamecUIRootLayout* UNamecUILayerSubsystem::CreateRootLayout(UWorld& World, APlayerController* OwningPlayer, int32 SlotNumber, UNamecSaveFileService& SaveFiles, UNamecAutosaveSubsystem& Autosave)
{
    RemoveRootLayout();

    RootLayout = OwningPlayer ? CreateWidget<UNamecUIRootLayout>(OwningPlayer) : CreateWidget<UNamecUIRootLayout>(&World);
    RootLayout->UseSaveFiles(SaveFiles);

    if (SlotNumber == 1)
    {
        WarningSource = &Autosave;
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

void UNamecUILayerSubsystem::ShowSaveWarning(const FText& Warning)
{
    RootLayout->ShowHudNotice(Warning);
}
