#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"

class UInputAction;
class UInputMappingContext;

enum class ENamecInputDevice : uint8
{
    Gamepad,
    KeyboardMouse,
};

// Which DT_Core_Input value, if any, a binding's trigger threshold comes from at runtime.
enum class ENamecBindingTrigger : uint8
{
    Down,
    Pressed,
    DodgeTap,
    SprintHold,
    InventoryTap,
    CameraToggleHold,
    StickFlick,
    ChordModifier,
};

enum class ENamecAxisModifier : uint8
{
    None,
    Negate,
    Swizzle,
    SwizzleNegate,
};

struct FNamecBaseAction
{
    FName ActionName;
    EInputActionValueType ValueType = EInputActionValueType::Boolean;
};

// One default binding of game-foundation Requirement 14's table. BindingName is unique within a
// device and is the key a remap is saved under; several bindings can feed one action (W A S D).
struct FNamecBaseBinding
{
    FName BindingName;
    FName ActionName;
    FKey DefaultKey;
    bool bChorded = false;
    ENamecBindingTrigger Trigger = ENamecBindingTrigger::Down;
    ENamecAxisModifier AxisModifier = ENamecAxisModifier::None;
};

namespace NamecBaseInput
{
    // The action a chorded binding waits on: LB on a gamepad, Left Alt on the keyboard.
    NAMEC_API extern const FName ChordModifierActionName;

    NAMEC_API extern const FName CameraToggleActionName;

    NAMEC_API extern const FName InteractActionName;

    NAMEC_API TConstArrayView<FNamecBaseAction> GetActions();
    NAMEC_API TConstArrayView<FNamecBaseBinding> GetBindings(ENamecInputDevice Device);

    NAMEC_API FString GetActionPackageName(FName ActionName);
    NAMEC_API FString GetContextPackageName(ENamecInputDevice Device);

    NAMEC_API UInputAction* LoadAction(FName ActionName);
    NAMEC_API UInputMappingContext* LoadContext(ENamecInputDevice Device);
}
