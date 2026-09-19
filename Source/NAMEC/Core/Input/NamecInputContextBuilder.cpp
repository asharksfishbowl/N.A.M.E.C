#include "Core/Input/NamecInputContextBuilder.h"
#include "Core/Input/NamecCoreInputRow.h"
#include "EnhancedActionKeyMapping.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputTriggers.h"
#include "PlayerMappableKeySettings.h"

namespace
{
    // The mapping's name is what ties it back to its FNamecBaseBinding. Both properties are
    // protected EditAnywhere fields with no C++ setter, so they are written as the editor writes them.
    void SetMappingName(FEnhancedActionKeyMapping& Mapping, UObject* Outer, FName BindingName)
    {
        UPlayerMappableKeySettings* Settings = NewObject<UPlayerMappableKeySettings>(Outer);
        Settings->Name = BindingName;

        const UScriptStruct* MappingStruct = FEnhancedActionKeyMapping::StaticStruct();
        const FObjectProperty* SettingsProperty = CastFieldChecked<FObjectProperty>(MappingStruct->FindPropertyByName(TEXT("PlayerMappableKeySettings")));
        SettingsProperty->SetObjectPropertyValue_InContainer(&Mapping, Settings);

        const FEnumProperty* BehaviorProperty = CastFieldChecked<FEnumProperty>(MappingStruct->FindPropertyByName(TEXT("SettingBehavior")));
        BehaviorProperty->GetUnderlyingProperty()->SetIntPropertyValue(BehaviorProperty->ContainerPtrToValuePtr<void>(&Mapping), static_cast<int64>(EPlayerMappableKeySettingBehaviors::OverrideSettings));
    }

    void AddAxisModifiers(FEnhancedActionKeyMapping& Mapping, UObject* Outer, ENamecAxisModifier AxisModifier)
    {
        if (AxisModifier == ENamecAxisModifier::Swizzle || AxisModifier == ENamecAxisModifier::SwizzleNegate)
        {
            Mapping.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(Outer));
        }
        if (AxisModifier == ENamecAxisModifier::Negate || AxisModifier == ENamecAxisModifier::SwizzleNegate)
        {
            Mapping.Modifiers.Add(NewObject<UInputModifierNegate>(Outer));
        }
    }

    void AddTrigger(FEnhancedActionKeyMapping& Mapping, UObject* Outer, ENamecBindingTrigger Trigger)
    {
        switch (Trigger)
        {
        case ENamecBindingTrigger::Pressed:
        case ENamecBindingTrigger::StickFlick:
            Mapping.Triggers.Add(NewObject<UInputTriggerPressed>(Outer));
            break;
        case ENamecBindingTrigger::DodgeTap:
        case ENamecBindingTrigger::InventoryTap:
            Mapping.Triggers.Add(NewObject<UInputTriggerTap>(Outer));
            break;
        case ENamecBindingTrigger::SprintHold:
            Mapping.Triggers.Add(NewObject<UInputTriggerHold>(Outer));
            break;
        case ENamecBindingTrigger::CameraToggleHold:
        {
            UInputTriggerHold* Hold = NewObject<UInputTriggerHold>(Outer);
            Hold->bIsOneShot = true;
            Mapping.Triggers.Add(Hold);
            break;
        }
        case ENamecBindingTrigger::Down:
        case ENamecBindingTrigger::ChordModifier:
            break;
        }
    }

    // View is one button with two actions: released before the camera hold completes is the
    // inventory tap. B is the same for dodge and sprint. So each pair shares one boundary.
    TOptional<float> GetThreshold(ENamecBindingTrigger Trigger, const FNamecCoreInputRow& Tuning)
    {
        switch (Trigger)
        {
        case ENamecBindingTrigger::DodgeTap:
        case ENamecBindingTrigger::SprintHold:
            return Tuning.DodgeTapSeconds;
        case ENamecBindingTrigger::InventoryTap:
        case ENamecBindingTrigger::CameraToggleHold:
            return Tuning.CameraToggleHoldSeconds;
        case ENamecBindingTrigger::StickFlick:
            return Tuning.StickFlickThreshold;
        case ENamecBindingTrigger::Down:
        case ENamecBindingTrigger::Pressed:
        case ENamecBindingTrigger::ChordModifier:
            break;
        }
        return TOptional<float>();
    }

    void ApplyThreshold(FEnhancedActionKeyMapping& Mapping, float Threshold)
    {
        for (UInputTrigger* InputTrigger : Mapping.Triggers)
        {
            if (UInputTriggerHold* Hold = Cast<UInputTriggerHold>(InputTrigger))
            {
                Hold->HoldTimeThreshold = Threshold;
            }
            else if (UInputTriggerTap* Tap = Cast<UInputTriggerTap>(InputTrigger))
            {
                Tap->TapReleaseTimeThreshold = Threshold;
            }
            else if (InputTrigger->IsA<UInputTriggerPressed>())
            {
                InputTrigger->ActuationThreshold = Threshold;
            }
        }
    }
}

bool NamecInputContextBuilder::AddBaseBinding(UInputMappingContext& Context, const FNamecBaseBinding& Binding)
{
    const UInputAction* Action = NamecBaseInput::LoadAction(Binding.ActionName);
    const UInputAction* ChordModifier = Binding.bChorded ? NamecBaseInput::LoadAction(NamecBaseInput::ChordModifierActionName) : nullptr;
    if (!Action || (Binding.bChorded && !ChordModifier))
    {
        return false;
    }

    FEnhancedActionKeyMapping& Mapping = Context.MapKey(Action, Binding.DefaultKey);
    SetMappingName(Mapping, &Context, Binding.BindingName);
    AddAxisModifiers(Mapping, &Context, Binding.AxisModifier);
    AddTrigger(Mapping, &Context, Binding.Trigger);
    if (ChordModifier)
    {
        UInputTriggerChordAction* Chord = NewObject<UInputTriggerChordAction>(&Context);
        Chord->ChordAction = ChordModifier;
        Mapping.Triggers.Add(Chord);
    }
    return true;
}

UInputMappingContext* NamecInputContextBuilder::BuildPlayerContext(UObject* Outer, const UInputMappingContext& BaseContext, ENamecInputDevice Device, const TMap<FName, FKey>& Remaps, const FNamecCoreInputRow& Tuning)
{
    UInputMappingContext* PlayerContext = DuplicateObject<UInputMappingContext>(&BaseContext, Outer);

    TMap<FName, ENamecBindingTrigger> TriggerByBindingName;
    for (const FNamecBaseBinding& Binding : NamecBaseInput::GetBindings(Device))
    {
        TriggerByBindingName.Add(Binding.BindingName, Binding.Trigger);
    }

    for (int32 Index = 0; Index < PlayerContext->GetMappings().Num(); ++Index)
    {
        FEnhancedActionKeyMapping& Mapping = PlayerContext->GetMapping(Index);
        const FName BindingName = Mapping.GetMappingName();
        if (const FKey* RemappedKey = Remaps.Find(BindingName))
        {
            Mapping.Key = *RemappedKey;
        }
        const ENamecBindingTrigger* Trigger = TriggerByBindingName.Find(BindingName);
        const TOptional<float> Threshold = Trigger ? GetThreshold(*Trigger, Tuning) : TOptional<float>();
        if (Threshold.IsSet())
        {
            ApplyThreshold(Mapping, Threshold.GetValue());
        }
    }
    return PlayerContext;
}
