#include "Core/Input/NamecBaseInputBindings.h"
#include "InputAction.h"
#include "InputMappingContext.h"

// Only the unconditional rows of game-foundation Requirement 14 are here. A row that names a
// context (tool, shovel, Hammer, placement, bow, Fishing Rod, bite window, torch, Left Hand, bed,
// water) is mapped by the phase that owns that context, through PushContext.

const FName NamecBaseInput::ChordModifierActionName(TEXT("ChordModifier"));
const FName NamecBaseInput::CameraToggleActionName(TEXT("CameraToggle"));
const FName NamecBaseInput::InteractActionName(TEXT("Interact"));

namespace
{
    using enum ENamecBindingTrigger;
    using enum ENamecAxisModifier;

    const FNamecBaseAction BaseActions[] =
    {
        { TEXT("ChordModifier") },
        { TEXT("Move"), EInputActionValueType::Axis2D },
        { TEXT("Look"), EInputActionValueType::Axis2D },
        { TEXT("Jump") },
        { TEXT("Interact") },
        { TEXT("Climb") },
        { TEXT("DodgeRoll") },
        { TEXT("Sprint") },
        { TEXT("UseConsumable") },
        { TEXT("ToggleTwoHanding") },
        { TEXT("Execute") },
        { TEXT("LightAttack") },
        { TEXT("HeavyAttack") },
        { TEXT("Block") },
        { TEXT("Parry") },
        { TEXT("Ability1") },
        { TEXT("Ability2") },
        { TEXT("Ability3") },
        { TEXT("Ability4") },
        { TEXT("Ability5") },
        { TEXT("Ability6") },
        { TEXT("LockOnToggle") },
        { TEXT("CycleLockOnTarget"), EInputActionValueType::Axis2D },
        { TEXT("Crouch") },
        { TEXT("CameraToggle") },
        { TEXT("InventoryMenu") },
        { TEXT("QuestLog") },
        { TEXT("FavoritesMenu") },
        { TEXT("CycleRightHandHotkeys") },
        { TEXT("CycleLeftHandHotkeys") },
        { TEXT("CycleConsumableHotkeys") },
        { TEXT("HotkeySlot1") },
        { TEXT("HotkeySlot2") },
        { TEXT("HotkeySlot3") },
        { TEXT("HotkeySlot4") },
        { TEXT("HotkeySlot5") },
        { TEXT("HotkeySlot6") },
        { TEXT("HotkeySlot7") },
        { TEXT("HotkeySlot8") },
        { TEXT("PauseMenu") },
    };

    // A, B and Y each carry more than one action: the game decides between them from what is
    // targeted or shown, which is a rule of the phase that owns the action, not of the binding.
    const FNamecBaseBinding GamepadBindings[] =
    {
        { TEXT("ChordModifier"), TEXT("ChordModifier"), EKeys::Gamepad_LeftShoulder, false, ChordModifier },
        { TEXT("Move"), TEXT("Move"), EKeys::Gamepad_Left2D },
        { TEXT("Look"), TEXT("Look"), EKeys::Gamepad_Right2D },
        { TEXT("Jump"), TEXT("Jump"), EKeys::Gamepad_FaceButton_Bottom, false, Pressed },
        { TEXT("Interact"), TEXT("Interact"), EKeys::Gamepad_FaceButton_Bottom, false, Pressed },
        { TEXT("Climb"), TEXT("Climb"), EKeys::Gamepad_FaceButton_Bottom },
        { TEXT("DodgeRoll"), TEXT("DodgeRoll"), EKeys::Gamepad_FaceButton_Right, false, DodgeTap },
        { TEXT("Sprint"), TEXT("Sprint"), EKeys::Gamepad_FaceButton_Right, false, SprintHold },
        { TEXT("UseConsumable"), TEXT("UseConsumable"), EKeys::Gamepad_FaceButton_Left, false, Pressed },
        { TEXT("ToggleTwoHanding"), TEXT("ToggleTwoHanding"), EKeys::Gamepad_FaceButton_Top, false, Pressed },
        { TEXT("Execute"), TEXT("Execute"), EKeys::Gamepad_FaceButton_Top, false, Pressed },
        { TEXT("LightAttack"), TEXT("LightAttack"), EKeys::Gamepad_RightShoulder, false, Pressed },
        { TEXT("HeavyAttack"), TEXT("HeavyAttack"), EKeys::Gamepad_RightTrigger, false, Pressed },
        { TEXT("Block"), TEXT("Block"), EKeys::Gamepad_LeftTrigger },
        { TEXT("Parry"), TEXT("Parry"), EKeys::Gamepad_LeftTrigger, true, Pressed },
        { TEXT("Ability1"), TEXT("Ability1"), EKeys::Gamepad_FaceButton_Bottom, true, Pressed },
        { TEXT("Ability2"), TEXT("Ability2"), EKeys::Gamepad_FaceButton_Right, true, Pressed },
        { TEXT("Ability3"), TEXT("Ability3"), EKeys::Gamepad_FaceButton_Left, true, Pressed },
        { TEXT("Ability4"), TEXT("Ability4"), EKeys::Gamepad_FaceButton_Top, true, Pressed },
        { TEXT("Ability5"), TEXT("Ability5"), EKeys::Gamepad_RightShoulder, true, Pressed },
        { TEXT("Ability6"), TEXT("Ability6"), EKeys::Gamepad_RightTrigger, true, Pressed },
        { TEXT("LockOnToggle"), TEXT("LockOnToggle"), EKeys::Gamepad_RightThumbstick, false, Pressed },
        { TEXT("CycleLockOnTarget"), TEXT("CycleLockOnTarget"), EKeys::Gamepad_Right2D, false, StickFlick },
        { TEXT("Crouch"), TEXT("Crouch"), EKeys::Gamepad_LeftThumbstick, false, Pressed },
        { TEXT("CameraToggle"), TEXT("CameraToggle"), EKeys::Gamepad_Special_Left, false, CameraToggleHold },
        { TEXT("InventoryMenu"), TEXT("InventoryMenu"), EKeys::Gamepad_Special_Left, false, InventoryTap },
        { TEXT("FavoritesMenu"), TEXT("FavoritesMenu"), EKeys::Gamepad_DPad_Up, false, Pressed },
        { TEXT("CycleRightHandHotkeys"), TEXT("CycleRightHandHotkeys"), EKeys::Gamepad_DPad_Left, false, Pressed },
        { TEXT("CycleLeftHandHotkeys"), TEXT("CycleLeftHandHotkeys"), EKeys::Gamepad_DPad_Right, false, Pressed },
        { TEXT("CycleConsumableHotkeys"), TEXT("CycleConsumableHotkeys"), EKeys::Gamepad_DPad_Down, false, Pressed },
        { TEXT("PauseMenu"), TEXT("PauseMenu"), EKeys::Gamepad_Special_Right, false, Pressed },
    };

    const FNamecBaseBinding KeyboardMouseBindings[] =
    {
        { TEXT("ChordModifier"), TEXT("ChordModifier"), EKeys::LeftAlt, false, ChordModifier },
        { TEXT("MoveForward"), TEXT("Move"), EKeys::W, false, Down, Swizzle },
        { TEXT("MoveBackward"), TEXT("Move"), EKeys::S, false, Down, SwizzleNegate },
        { TEXT("MoveLeft"), TEXT("Move"), EKeys::A, false, Down, Negate },
        { TEXT("MoveRight"), TEXT("Move"), EKeys::D },
        { TEXT("Look"), TEXT("Look"), EKeys::Mouse2D },
        { TEXT("Jump"), TEXT("Jump"), EKeys::SpaceBar, false, Pressed },
        { TEXT("Interact"), TEXT("Interact"), EKeys::E, false, Pressed },
        { TEXT("Climb"), TEXT("Climb"), EKeys::SpaceBar },
        { TEXT("DodgeRoll"), TEXT("DodgeRoll"), EKeys::LeftShift, false, DodgeTap },
        { TEXT("Sprint"), TEXT("Sprint"), EKeys::LeftShift, false, SprintHold },
        { TEXT("UseConsumable"), TEXT("UseConsumable"), EKeys::F, false, Pressed },
        { TEXT("ToggleTwoHanding"), TEXT("ToggleTwoHanding"), EKeys::R, false, Pressed },
        { TEXT("Execute"), TEXT("Execute"), EKeys::R, false, Pressed },
        { TEXT("LightAttack"), TEXT("LightAttack"), EKeys::LeftMouseButton, false, Pressed },
        { TEXT("HeavyAttack"), TEXT("HeavyAttack"), EKeys::MiddleMouseButton, false, Pressed },
        { TEXT("Block"), TEXT("Block"), EKeys::RightMouseButton },
        { TEXT("Parry"), TEXT("Parry"), EKeys::RightMouseButton, true, Pressed },
        { TEXT("Ability1"), TEXT("Ability1"), EKeys::Z, false, Pressed },
        { TEXT("Ability2"), TEXT("Ability2"), EKeys::X, false, Pressed },
        { TEXT("Ability3"), TEXT("Ability3"), EKeys::C, false, Pressed },
        { TEXT("Ability4"), TEXT("Ability4"), EKeys::V, false, Pressed },
        { TEXT("Ability5"), TEXT("Ability5"), EKeys::B, false, Pressed },
        { TEXT("Ability6"), TEXT("Ability6"), EKeys::N, false, Pressed },
        { TEXT("LockOnToggle"), TEXT("LockOnToggle"), EKeys::Tab, false, Pressed },
        { TEXT("CycleLockOnTarget"), TEXT("CycleLockOnTarget"), EKeys::Mouse2D, false, StickFlick },
        { TEXT("Crouch"), TEXT("Crouch"), EKeys::LeftControl, false, Pressed },
        { TEXT("CameraToggle"), TEXT("CameraToggle"), EKeys::H, false, Pressed },
        { TEXT("InventoryMenu"), TEXT("InventoryMenu"), EKeys::I, false, Pressed },
        { TEXT("QuestLog"), TEXT("QuestLog"), EKeys::J, false, Pressed },
        { TEXT("FavoritesMenu"), TEXT("FavoritesMenu"), EKeys::Q, false, Pressed },
        { TEXT("HotkeySlot1"), TEXT("HotkeySlot1"), EKeys::One, false, Pressed },
        { TEXT("HotkeySlot2"), TEXT("HotkeySlot2"), EKeys::Two, false, Pressed },
        { TEXT("HotkeySlot3"), TEXT("HotkeySlot3"), EKeys::Three, false, Pressed },
        { TEXT("HotkeySlot4"), TEXT("HotkeySlot4"), EKeys::Four, false, Pressed },
        { TEXT("HotkeySlot5"), TEXT("HotkeySlot5"), EKeys::Five, false, Pressed },
        { TEXT("HotkeySlot6"), TEXT("HotkeySlot6"), EKeys::Six, false, Pressed },
        { TEXT("HotkeySlot7"), TEXT("HotkeySlot7"), EKeys::Seven, false, Pressed },
        { TEXT("HotkeySlot8"), TEXT("HotkeySlot8"), EKeys::Eight, false, Pressed },
        { TEXT("PauseMenu"), TEXT("PauseMenu"), EKeys::Escape, false, Pressed },
    };

}

TConstArrayView<FNamecBaseAction> NamecBaseInput::GetActions()
{
    return BaseActions;
}

TConstArrayView<FNamecBaseBinding> NamecBaseInput::GetBindings(ENamecInputDevice Device)
{
    return Device == ENamecInputDevice::Gamepad ? TConstArrayView<FNamecBaseBinding>(GamepadBindings) : TConstArrayView<FNamecBaseBinding>(KeyboardMouseBindings);
}

FString NamecBaseInput::GetActionPackageName(FName ActionName)
{
    return FString::Printf(TEXT("/Game/Input/Actions/IA_%s"), *ActionName.ToString());
}

FString NamecBaseInput::GetContextPackageName(ENamecInputDevice Device)
{
    return Device == ENamecInputDevice::Gamepad ? TEXT("/Game/Input/IMC_Gamepad") : TEXT("/Game/Input/IMC_KeyboardMouse");
}

UInputAction* NamecBaseInput::LoadAction(FName ActionName)
{
    return LoadObject<UInputAction>(nullptr, *GetActionPackageName(ActionName));
}

UInputMappingContext* NamecBaseInput::LoadContext(ENamecInputDevice Device)
{
    return LoadObject<UInputMappingContext>(nullptr, *GetContextPackageName(Device));
}
