#include "Core/Input/NamecInputContextSubsystem.h"
#include "Core/Input/NamecInputContextBuilder.h"
#include "Core/Input/NamecInputRemap.h"
#include "Save/NamecSettingsService.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"

DEFINE_LOG_CATEGORY_STATIC(LogNamecInput, Log, All);

namespace
{
    constexpr int32 BaseContextPriority = 0;
    constexpr int32 PushedContextPriority = 1;

    // Keyboard and mouse belong to local player 1 only (multiplayer Requirement 7).
    bool SlotUsesDevice(int32 SlotNumber, ENamecInputDevice Device)
    {
        return Device == ENamecInputDevice::Gamepad || SlotNumber == 1;
    }
}

void UNamecInputContextSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UEnhancedInputLocalPlayerSubsystem* EnhancedInput = Collection.InitializeDependency<UEnhancedInputLocalPlayerSubsystem>();
    UNamecSettingsService* SettingsService = GetLocalPlayer()->GetGameInstance()->GetSubsystem<UNamecSettingsService>();
    check(EnhancedInput && SettingsService);

    TOptional<FNamecCoreInputRow> TuningRow = NamecCoreInput::LoadRow();
    if (!TuningRow.IsSet())
    {
        UE_LOG(LogNamecInput, Error, TEXT("%s is missing; input thresholds are the compiled-in values. Run -run=NamecCreateFoundationTables"), NamecCoreInput::TablePackageName);
    }

    Configure(*EnhancedInput, GetLocalPlayer()->GetLocalPlayerIndex() + 1, *SettingsService, TuningRow.Get(FNamecCoreInputRow()));
    AddBaseContexts();
}

void UNamecInputContextSubsystem::Deinitialize()
{
    if (Settings)
    {
        Settings->OnSettingsApplied.RemoveAll(this);
    }
    Super::Deinitialize();
}

void UNamecInputContextSubsystem::Configure(IEnhancedInputSubsystemInterface& InInput, int32 InSlotNumber, UNamecSettingsService& InSettings, const FNamecCoreInputRow& InTuning)
{
    if (Settings)
    {
        Settings->OnSettingsApplied.RemoveAll(this);
    }

    Input = &InInput;
    SlotNumber = InSlotNumber;
    Settings = &InSettings;
    Tuning = InTuning;
    Settings->OnSettingsApplied.AddUObject(this, &UNamecInputContextSubsystem::OnSettingsApplied);
}

void UNamecInputContextSubsystem::AddBaseContexts()
{
    check(Input.IsValid() && Settings);

    AppliedSlot = Settings->GetSettings().GetSlot(SlotNumber);
    for (const ENamecInputDevice Device : { ENamecInputDevice::Gamepad, ENamecInputDevice::KeyboardMouse })
    {
        if (!SlotUsesDevice(SlotNumber, Device))
        {
            continue;
        }

        const UInputMappingContext* BaseContext = NamecBaseInput::LoadContext(Device);
        if (!BaseContext)
        {
            UE_LOG(LogNamecInput, Error, TEXT("%s is missing. Run -run=NamecCreateInputAssets"), *NamecBaseInput::GetContextPackageName(Device));
            continue;
        }

        UInputMappingContext* PlayerContext = NamecInputContextBuilder::BuildPlayerContext(this, *BaseContext, Device, NamecInputRemap::GetSlotRemaps(AppliedSlot, Device), Tuning);
        BaseContexts.Add(static_cast<uint8>(Device), PlayerContext);
        Input->AddMappingContext(PlayerContext, BaseContextPriority);
    }
}

void UNamecInputContextSubsystem::RemoveBaseContexts()
{
    for (const TPair<uint8, TObjectPtr<UInputMappingContext>>& BaseContext : BaseContexts)
    {
        if (Input.IsValid())
        {
            Input->RemoveMappingContext(BaseContext.Value);
        }
    }
    BaseContexts.Reset();
}

void UNamecInputContextSubsystem::PushContext(FName ContextId, const UInputMappingContext& MappingContext)
{
    PopContext(ContextId);
    PushedContexts.Add(ContextId, &MappingContext);
    Input->AddMappingContext(&MappingContext, PushedContextPriority);
}

void UNamecInputContextSubsystem::PopContext(FName ContextId)
{
    TObjectPtr<const UInputMappingContext> Pushed;
    if (PushedContexts.RemoveAndCopyValue(ContextId, Pushed))
    {
        Input->RemoveMappingContext(Pushed);
    }
}

void UNamecInputContextSubsystem::ApplySlotRemaps()
{
    RemoveBaseContexts();
    AddBaseContexts();
}

void UNamecInputContextSubsystem::RemapBinding(FName BindingName, const FKey& NewKey)
{
    const ENamecInputDevice Device = NamecInputRemap::GetDeviceOfKey(NewKey);

    FNamecSettings Edited = Settings->GetSettings();
    NamecInputRemap::RemapWithSwap(NamecInputRemap::GetSlotRemaps(Edited.GetSlot(SlotNumber), Device), Device, BindingName, NewKey);
    Settings->Apply(Edited);
}

bool UNamecInputContextSubsystem::HasBaseContext(ENamecInputDevice Device) const
{
    const UInputMappingContext* BaseContext = GetBaseContext(Device);
    return BaseContext && Input->HasMappingContext(BaseContext);
}

const UInputMappingContext* UNamecInputContextSubsystem::GetBaseContext(ENamecInputDevice Device) const
{
    return BaseContexts.FindRef(static_cast<uint8>(Device));
}

void UNamecInputContextSubsystem::OnSettingsApplied(const FNamecSettings& AppliedSettings)
{
    const FNamecLocalPlayerSettings& Slot = AppliedSettings.GetSlot(SlotNumber);
    const bool bRemapsChanged = !Slot.GamepadRemaps.OrderIndependentCompareEqual(AppliedSlot.GamepadRemaps)
        || !Slot.KeyboardMouseRemaps.OrderIndependentCompareEqual(AppliedSlot.KeyboardMouseRemaps);
    if (bRemapsChanged)
    {
        ApplySlotRemaps();
    }
}
