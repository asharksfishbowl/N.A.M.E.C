#pragma once

#include "CoreMinimal.h"
#include "Core/Input/NamecBaseInputBindings.h"

struct FEnhancedActionKeyMapping;
struct FNamecCoreInputRow;

namespace NamecInputContextBuilder
{
    // Adds one base binding to Context: key, axis modifiers, its trigger and, when chorded, the
    // chord trigger. False when the binding's action asset does not load.
    NAMEC_API bool AddBaseBinding(UInputMappingContext& Context, const FNamecBaseBinding& Binding);

    // A player's own transient copy of a base context, with that slot's remapped keys and the
    // table's thresholds. Remaps do not use UEnhancedInputUserSettings: it owns a save file of
    // its own, and game-foundation Requirement 6 keeps remaps in UNamecSettingsSave.
    NAMEC_API UInputMappingContext* BuildPlayerContext(UObject* Outer, const UInputMappingContext& BaseContext, ENamecInputDevice Device, const TMap<FName, FKey>& Remaps, const FNamecCoreInputRow& Tuning);
}
