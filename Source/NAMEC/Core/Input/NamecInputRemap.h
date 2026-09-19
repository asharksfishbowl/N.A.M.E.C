#pragma once

#include "CoreMinimal.h"
#include "Core/Input/NamecBaseInputBindings.h"

struct FNamecLocalPlayerSettings;

namespace NamecInputRemap
{
    NAMEC_API ENamecInputDevice GetDeviceOfKey(const FKey& Key);

    NAMEC_API TMap<FName, FKey>& GetSlotRemaps(FNamecLocalPlayerSettings& Slot, ENamecInputDevice Device);
    NAMEC_API const TMap<FName, FKey>& GetSlotRemaps(const FNamecLocalPlayerSettings& Slot, ENamecInputDevice Device);

    // The key a binding is on for a slot: its remap if it has one, else the table default.
    NAMEC_API FKey GetCurrentKey(const TMap<FName, FKey>& Remaps, const FNamecBaseBinding& Binding);

    // Puts BindingName on NewKey. Every other binding of that device already on NewKey, chorded
    // alike, takes the key BindingName leaves, so nothing is left unbound (Requirement 14, SWAP).
    NAMEC_API void RemapWithSwap(TMap<FName, FKey>& Remaps, ENamecInputDevice Device, FName BindingName, const FKey& NewKey);
}
