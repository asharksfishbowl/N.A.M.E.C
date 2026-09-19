#include "Core/Input/NamecInputRemap.h"
#include "Save/NamecSettingsSave.h"

ENamecInputDevice NamecInputRemap::GetDeviceOfKey(const FKey& Key)
{
    return Key.IsGamepadKey() ? ENamecInputDevice::Gamepad : ENamecInputDevice::KeyboardMouse;
}

TMap<FName, FKey>& NamecInputRemap::GetSlotRemaps(FNamecLocalPlayerSettings& Slot, ENamecInputDevice Device)
{
    return Device == ENamecInputDevice::Gamepad ? Slot.GamepadRemaps : Slot.KeyboardMouseRemaps;
}

const TMap<FName, FKey>& NamecInputRemap::GetSlotRemaps(const FNamecLocalPlayerSettings& Slot, ENamecInputDevice Device)
{
    return GetSlotRemaps(const_cast<FNamecLocalPlayerSettings&>(Slot), Device);
}

FKey NamecInputRemap::GetCurrentKey(const TMap<FName, FKey>& Remaps, const FNamecBaseBinding& Binding)
{
    const FKey* Remapped = Remaps.Find(Binding.BindingName);
    return Remapped ? *Remapped : Binding.DefaultKey;
}

void NamecInputRemap::RemapWithSwap(TMap<FName, FKey>& Remaps, ENamecInputDevice Device, FName BindingName, const FKey& NewKey)
{
    const TConstArrayView<FNamecBaseBinding> Bindings = NamecBaseInput::GetBindings(Device);
    const FNamecBaseBinding* Remapped = Bindings.FindByPredicate([BindingName](const FNamecBaseBinding& Binding) { return Binding.BindingName == BindingName; });
    if (!Remapped)
    {
        return;
    }

    // A binding back on its default key has no entry, so an empty map always means all defaults.
    auto SetKey = [&Remaps](const FNamecBaseBinding& Binding, const FKey& Key)
    {
        if (Key == Binding.DefaultKey)
        {
            Remaps.Remove(Binding.BindingName);
        }
        else
        {
            Remaps.Add(Binding.BindingName, Key);
        }
    };

    const FKey PreviousKey = GetCurrentKey(Remaps, *Remapped);
    for (const FNamecBaseBinding& Other : Bindings)
    {
        if (Other.BindingName != BindingName && Other.bChorded == Remapped->bChorded && GetCurrentKey(Remaps, Other) == NewKey)
        {
            SetKey(Other, PreviousKey);
        }
    }
    SetKey(*Remapped, NewKey);
}
