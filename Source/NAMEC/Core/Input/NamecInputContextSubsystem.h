#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Core/Input/NamecBaseInputBindings.h"
#include "Core/Input/NamecCoreInputRow.h"
#include "Save/NamecSettingsSave.h"
#include "UObject/WeakInterfacePtr.h"
#include "NamecInputContextSubsystem.generated.h"

class IEnhancedInputSubsystemInterface;
class UInputMappingContext;
class UNamecSettingsService;

UCLASS()
class NAMEC_API UNamecInputContextSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Public because a test has no local player to run Initialize for it. SlotNumber is 1 to 4.
    void Configure(IEnhancedInputSubsystemInterface& InInput, int32 InSlotNumber, UNamecSettingsService& InSettings, const FNamecCoreInputRow& InTuning);

    // Gamepad for every slot; keyboard and mouse for slot 1 only (multiplayer Requirement 7).
    void AddBaseContexts();

    // A pushed context sits above the base, so it replaces the unchorded default of the same button.
    void PushContext(FName ContextId, const UInputMappingContext& MappingContext);
    void PopContext(FName ContextId);

    // Rebuilds this player's base contexts from its slot's section of the settings save.
    void ApplySlotRemaps();

    // Saves the remap, with the swap rule, to this player's slot only. The device is the key's.
    void RemapBinding(FName BindingName, const FKey& NewKey);

    bool HasBaseContext(ENamecInputDevice Device) const;

    // Null for a device this player has no base context for.
    const UInputMappingContext* GetBaseContext(ENamecInputDevice Device) const;

private:
    void RemoveBaseContexts();
    void OnSettingsApplied(const FNamecSettings& Settings);

    // Weak: the Enhanced Input subsystem can be torn down before this one.
    TWeakInterfacePtr<IEnhancedInputSubsystemInterface> Input;

    int32 SlotNumber = 0;

    UPROPERTY()
    TObjectPtr<UNamecSettingsService> Settings;

    FNamecCoreInputRow Tuning;

    // What the base contexts were last built from. Another slot's change, or a volume change,
    // must not rebuild this player's mappings: a rebuild resets every hold in progress.
    FNamecLocalPlayerSettings AppliedSlot;

    UPROPERTY()
    TMap<uint8, TObjectPtr<UInputMappingContext>> BaseContexts;

    UPROPERTY()
    TMap<FName, TObjectPtr<const UInputMappingContext>> PushedContexts;
};
