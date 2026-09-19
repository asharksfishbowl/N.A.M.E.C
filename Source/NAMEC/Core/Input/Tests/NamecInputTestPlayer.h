#pragma once

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Core/Input/NamecInputContextSubsystem.h"
#include "Core/Input/Tests/NamecInputTestTypes.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"

// One local player as the input system sees it: a controller, its Enhanced Input objects, a
// mocked input subsystem and this project's context subsystem configured for a slot.
struct FNamecTestPlayer
{
    APlayerController* Controller = nullptr;
    UEnhancedInputComponent* InputComponent = nullptr;
    UNamecMockEnhancedInputSubsystem* Input = nullptr;
    UNamecInputContextSubsystem* Contexts = nullptr;

    FNamecTestPlayer(UWorld* World, int32 SlotNumber, UNamecSettingsService& Settings, const FNamecCoreInputRow& Tuning = FNamecCoreInputRow())
    {
        Controller = NewObject<APlayerController>(World->GetCurrentLevel());
        InputComponent = NewObject<UEnhancedInputComponent>(Controller);
        Controller->InputComponent = InputComponent;
        Controller->PlayerInput = NewObject<UEnhancedPlayerInput>(Controller);
        Controller->InitInputSystem();

        Input = NewObject<UNamecMockEnhancedInputSubsystem>(Controller);
        Input->PlayerInput = Cast<UEnhancedPlayerInput>(Controller->PlayerInput);

        Contexts = NewObject<UNamecInputContextSubsystem>(NewObject<ULocalPlayer>(GEngine));
        Contexts->Configure(*Input, SlotNumber, Settings, Tuning);
        Contexts->AddBaseContexts();
        RebuildNow();
    }

    void RebuildNow() const
    {
        FModifyContextOptions Options;
        Options.bForceImmediately = true;
        Input->RequestRebuildControlMappings(Options);
    }

    void Press(const FKey& Key) const { Controller->InputKey(FInputKeyEventArgs::CreateSimulated(Key, IE_Pressed, 1.f)); }
    void Release(const FKey& Key) const { Controller->InputKey(FInputKeyEventArgs::CreateSimulated(Key, IE_Released, 0.f)); }
    void Tick() const { Controller->PlayerTick(1.f / 60.f); }

    // Counts Triggered events of a base action from now on.
    TSharedRef<int32> CountTriggers(FName ActionName) const
    {
        TSharedRef<int32> Count = MakeShared<int32>(0);
        InputComponent->BindActionInstanceLambda(NamecBaseInput::LoadAction(ActionName), ETriggerEvent::Triggered, [Count](const FInputActionInstance&) { ++*Count; });
        return Count;
    }

    // Looks in the context this player was given, which HasBaseContext proves is applied.
    const FEnhancedActionKeyMapping* FindPlayerMapping(FName BindingName) const
    {
        return Contexts->GetBaseContext(ENamecInputDevice::Gamepad)->GetMappings().FindByPredicate([BindingName](const FEnhancedActionKeyMapping& Mapping) { return Mapping.GetMappingName() == BindingName; });
    }
};

#endif
