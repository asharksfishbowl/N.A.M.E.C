#pragma once

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Core/Input/Tests/NamecInputTestPlayer.h"
#include "Save/NamecAutosaveSubsystem.h"
#include "Save/Tests/NamecSaveTestHelpers.h"
#include "UI/NamecUILayerSubsystem.h"
#include "UI/NamecUIRootLayout.h"
#include "UI/Tests/NamecUITestHelpers.h"

// The game-instance services one machine's local players share, as a test builds them.
struct FNamecTestMachine
{
    UGameInstance* GameInstance = NamecSaveTestHelpers::NewGameInstance();
    UNamecSaveFileService* SaveFiles = NewObject<UNamecSaveFileService>(GameInstance);
    UNamecAutosaveSubsystem* Autosave = NewObject<UNamecAutosaveSubsystem>(GameInstance);
    UNamecSettingsService* Settings = NewObject<UNamecSettingsService>(GameInstance);

    FNamecTestMachine()
    {
        Autosave->UseSaveFiles(*SaveFiles);
        Settings->LoadSettings(*SaveFiles);
    }
};

// One local player end to end: its input contexts, its UI layer and its root layout with the
// Slate widgets held alive, as its viewport would.
struct FNamecTestLocalPlayerUI
{
    FNamecTestPlayer Input;
    UNamecUILayerSubsystem* Layer;
    UNamecUIRootLayout* Root;
    TSharedRef<SWidget> SlateRoot;

    FNamecTestLocalPlayerUI(UWorld& World, int32 SlotNumber, const FNamecTestMachine& Machine)
        : Input(&World, SlotNumber, *Machine.Settings)
        , Layer(NewObject<UNamecUILayerSubsystem>(NewObject<ULocalPlayer>(GEngine)))
        , Root(Layer->CreateRootLayout(World, nullptr, SlotNumber, { Machine.SaveFiles, Machine.Autosave, Machine.Settings, Input.Contexts }))
        , SlateRoot(NamecUITestHelpers::BuildSlate(*Root))
    {
    }
};

#endif
