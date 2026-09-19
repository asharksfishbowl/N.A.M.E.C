#include "Core/Input/NamecInputContextSubsystem.h"
#include "Core/Input/Tests/NamecInputTestPlayer.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Core/Tests/NamecTestWorld.h"
#include "Character/NamecPlayerCharacter.h"
#include "Save/Tests/NamecSaveTestHelpers.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecInputAssetsTest, "Namec.Foundation.Input.EveryBaseBindingHasItsDefaultInBothContexts", NamecFoundationTestFlags)

bool FNamecInputAssetsTest::RunTest(const FString& Parameters)
{
    for (const FNamecBaseAction& BaseAction : NamecBaseInput::GetActions())
    {
        const UInputAction* Action = NamecBaseInput::LoadAction(BaseAction.ActionName);
        if (TestNotNull(FString::Printf(TEXT("IA_%s"), *BaseAction.ActionName.ToString()), Action))
        {
            TestEqual(FString::Printf(TEXT("IA_%s value type"), *BaseAction.ActionName.ToString()), Action->ValueType, BaseAction.ValueType);
        }
    }

    for (const ENamecInputDevice Device : { ENamecInputDevice::Gamepad, ENamecInputDevice::KeyboardMouse })
    {
        const UInputMappingContext* Context = NamecBaseInput::LoadContext(Device);
        if (!TestNotNull(NamecBaseInput::GetContextPackageName(Device), Context))
        {
            continue;
        }

        const TConstArrayView<FNamecBaseBinding> Bindings = NamecBaseInput::GetBindings(Device);
        TestEqual(TEXT("The context holds the table's bindings and no others"), Context->GetMappings().Num(), Bindings.Num());
        for (const FNamecBaseBinding& Binding : Bindings)
        {
            const FString What = FString::Printf(TEXT("%s: %s"), *Context->GetName(), *Binding.BindingName.ToString());
            const FEnhancedActionKeyMapping* Mapping = Context->GetMappings().FindByPredicate([&Binding](const FEnhancedActionKeyMapping& Candidate) { return Candidate.GetMappingName() == Binding.BindingName; });
            if (!TestNotNull(What, Mapping))
            {
                continue;
            }
            TestEqual(What + TEXT(" action"), Mapping->Action->GetName(), FString::Printf(TEXT("IA_%s"), *Binding.ActionName.ToString()));
            TestEqual(What + TEXT(" key"), Mapping->Key, Binding.DefaultKey);
            const bool bHasChord = Mapping->Triggers.ContainsByPredicate([](const UInputTrigger* Trigger) { return Trigger->IsA<UInputTriggerChordAction>(); });
            TestEqual(What + TEXT(" chorded"), bHasChord, Binding.bChorded);
        }
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecInputChordTest, "Namec.Foundation.Input.ChordFiresItsActionAndNotTheUnchordedOne", NamecFoundationTestFlags)

bool FNamecInputChordTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    const FNamecTestPlayer Player(World.Get(), 1, *StartSettingsService());

    const TSharedRef<int32> Ability1 = Player.CountTriggers(TEXT("Ability1"));
    const TSharedRef<int32> Jump = Player.CountTriggers(TEXT("Jump"));

    Player.Press(EKeys::Gamepad_LeftShoulder);
    Player.Tick();
    Player.Press(EKeys::Gamepad_FaceButton_Bottom);
    Player.Tick();
    TestEqual(TEXT("LB + A fires Ability 1"), *Ability1, 1);
    TestEqual(TEXT("LB + A does not fire Jump"), *Jump, 0);

    Player.Release(EKeys::Gamepad_FaceButton_Bottom);
    Player.Release(EKeys::Gamepad_LeftShoulder);
    Player.Tick();
    Player.Press(EKeys::Gamepad_FaceButton_Bottom);
    Player.Tick();
    TestEqual(TEXT("A alone fires Jump"), *Jump, 1);
    TestEqual(TEXT("A alone does not fire Ability 1"), *Ability1, 1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecInputDeviceRuleTest, "Namec.Foundation.Input.OnlySlotOneGetsKeyboardAndMouse", NamecFoundationTestFlags)

bool FNamecInputDeviceRuleTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    UNamecSettingsService* Settings = StartSettingsService();
    const FNamecTestPlayer PlayerOne(World.Get(), 1, *Settings);
    const FNamecTestPlayer PlayerTwo(World.Get(), 2, *Settings);

    TestTrue(TEXT("Slot 1 gamepad"), PlayerOne.Contexts->HasBaseContext(ENamecInputDevice::Gamepad));
    TestTrue(TEXT("Slot 1 keyboard and mouse"), PlayerOne.Contexts->HasBaseContext(ENamecInputDevice::KeyboardMouse));
    TestTrue(TEXT("Slot 2 gamepad"), PlayerTwo.Contexts->HasBaseContext(ENamecInputDevice::Gamepad));
    TestFalse(TEXT("Slot 2 has no keyboard-and-mouse context"), PlayerTwo.Contexts->HasBaseContext(ENamecInputDevice::KeyboardMouse));
    TestNull(TEXT("Slot 2 was built no keyboard-and-mouse context"), PlayerTwo.Contexts->GetBaseContext(ENamecInputDevice::KeyboardMouse));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecInputRemapTest, "Namec.Foundation.Input.RemapSwapsPersistsAndTouchesOneSlot", NamecFoundationTestFlags)

bool FNamecInputRemapTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    UNamecSettingsService* Settings = StartSettingsService();
    const FNamecTestPlayer PlayerOne(World.Get(), 1, *Settings);
    const FNamecTestPlayer PlayerTwo(World.Get(), 2, *Settings);

    const UInputMappingContext* PlayerOneContextBefore = PlayerOne.Contexts->GetBaseContext(ENamecInputDevice::Gamepad);
    const UInputMappingContext* PlayerTwoContextBefore = PlayerTwo.Contexts->GetBaseContext(ENamecInputDevice::Gamepad);

    // X is UseConsumable and D-pad Up is FavoritesMenu, so this remap must swap them.
    PlayerTwo.Contexts->RemapBinding(TEXT("UseConsumable"), EKeys::Gamepad_DPad_Up);
    TestTrue(TEXT("Another slot's remap does not rebuild player 1's mappings"), PlayerOne.Contexts->GetBaseContext(ENamecInputDevice::Gamepad) == PlayerOneContextBefore);
    TestTrue(TEXT("Player 2's mappings are rebuilt"), PlayerTwo.Contexts->GetBaseContext(ENamecInputDevice::Gamepad) != PlayerTwoContextBefore);
    PlayerOne.RebuildNow();
    PlayerTwo.RebuildNow();

    const FEnhancedActionKeyMapping* Remapped = PlayerTwo.FindPlayerMapping(TEXT("UseConsumable"));
    const FEnhancedActionKeyMapping* Displaced = PlayerTwo.FindPlayerMapping(TEXT("FavoritesMenu"));
    const FEnhancedActionKeyMapping* OtherPlayer = PlayerOne.FindPlayerMapping(TEXT("UseConsumable"));
    if (TestNotNull(TEXT("Slot 2 UseConsumable"), Remapped) && TestNotNull(TEXT("Slot 2 FavoritesMenu"), Displaced) && TestNotNull(TEXT("Slot 1 UseConsumable"), OtherPlayer))
    {
        TestEqual(TEXT("The remapped action is on the new key"), Remapped->Key, EKeys::Gamepad_DPad_Up);
        TestEqual(TEXT("The displaced action took the old key"), Displaced->Key, EKeys::Gamepad_FaceButton_Left);
        TestEqual(TEXT("Slot 1 keeps the default"), OtherPlayer->Key, EKeys::Gamepad_FaceButton_Left);
    }

    PlayerTwo.Contexts->RemapBinding(TEXT("Crouch"), EKeys::Gamepad_LeftThumbstick);
    TestFalse(TEXT("A binding put on its default key has no saved entry"), Settings->GetSettings().GetSlot(2).GamepadRemaps.Contains(TEXT("Crouch")));

    const FNamecSettings& Reloaded = StartSettingsService()->GetSettings();
    TestEqual(TEXT("Slot 2 remap survives a reload"), Reloaded.GetSlot(2).GamepadRemaps.FindRef(TEXT("UseConsumable")), EKeys::Gamepad_DPad_Up);
    TestEqual(TEXT("Slot 2 swap survives a reload"), Reloaded.GetSlot(2).GamepadRemaps.FindRef(TEXT("FavoritesMenu")), EKeys::Gamepad_FaceButton_Left);
    for (const int32 OtherSlot : { 1, 3, 4 })
    {
        TestEqual(FString::Printf(TEXT("Slot %d gamepad remaps"), OtherSlot), Reloaded.GetSlot(OtherSlot).GamepadRemaps.Num(), 0);
        TestEqual(FString::Printf(TEXT("Slot %d keyboard remaps"), OtherSlot), Reloaded.GetSlot(OtherSlot).KeyboardMouseRemaps.Num(), 0);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecInputThresholdTest, "Namec.Foundation.Input.ThresholdsComeFromTheTableNotTheAsset", NamecFoundationTestFlags)

bool FNamecInputThresholdTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;

    FNamecCoreInputRow Tuning;
    Tuning.CameraToggleHoldSeconds = 2.5f;
    const FNamecTestPlayer Player(World.Get(), 1, *StartSettingsService(), Tuning);

    const FEnhancedActionKeyMapping* CameraToggle = Player.FindPlayerMapping(TEXT("CameraToggle"));
    if (TestNotNull(TEXT("Live gamepad CameraToggle mapping"), CameraToggle))
    {
        const UInputTriggerHold* Hold = nullptr;
        for (const UInputTrigger* Trigger : CameraToggle->Triggers)
        {
            Hold = Hold ? Hold : Cast<UInputTriggerHold>(Trigger);
        }
        if (TestNotNull(TEXT("Hold trigger"), Hold))
        {
            TestEqual(TEXT("The player's hold threshold is the row's value"), Hold->HoldTimeThreshold, 2.5f);
        }
    }

    const TOptional<FNamecCoreInputRow> ShippedRow = NamecCoreInput::LoadRow();
    if (TestTrue(TEXT("DT_Core_Input row"), ShippedRow.IsSet()))
    {
        TestEqual(TEXT("Shipped CameraToggleHoldSeconds"), ShippedRow->CameraToggleHoldSeconds, 0.5f);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecInputPushedContextTest, "Namec.Foundation.Input.PushedContextReplacesTheBaseBindingOfItsButton", NamecFoundationTestFlags)

bool FNamecInputPushedContextTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    const FNamecTestPlayer Player(World.Get(), 1, *StartSettingsService());

    // A stand-in for a later phase's context: the tool swing takes RB from the light attack.
    UInputAction* SwingTool = NewObject<UInputAction>();
    UInputMappingContext* ToolContext = NewObject<UInputMappingContext>();
    ToolContext->MapKey(SwingTool, EKeys::Gamepad_RightShoulder);

    int32 SwingCount = 0;
    Player.InputComponent->BindActionInstanceLambda(SwingTool, ETriggerEvent::Triggered, [&SwingCount](const FInputActionInstance&) { ++SwingCount; });
    const TSharedRef<int32> LightAttack = Player.CountTriggers(TEXT("LightAttack"));

    Player.Contexts->PushContext(TEXT("Tool"), *ToolContext);
    Player.RebuildNow();
    Player.Press(EKeys::Gamepad_RightShoulder);
    Player.Tick();
    TestEqual(TEXT("The pushed context's action fires"), SwingCount, 1);
    TestEqual(TEXT("The base binding of the same button does not"), *LightAttack, 0);

    Player.Release(EKeys::Gamepad_RightShoulder);
    Player.Tick();
    Player.Contexts->PopContext(TEXT("Tool"));
    Player.RebuildNow();
    Player.Press(EKeys::Gamepad_RightShoulder);
    Player.Tick();
    TestEqual(TEXT("After the pop the base binding fires again"), *LightAttack, 1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecInputCameraToggleTest, "Namec.Foundation.Input.CameraToggleFlipsOnePlayerAndNotAnother", NamecFoundationTestFlags)

bool FNamecInputCameraToggleTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestWorld World;
    ANamecPlayerCharacter* First = World->SpawnActor<ANamecPlayerCharacter>();
    ANamecPlayerCharacter* Second = World->SpawnActor<ANamecPlayerCharacter>();
    if (!TestNotNull(TEXT("First character"), First) || !TestNotNull(TEXT("Second character"), Second))
    {
        return true;
    }

    TestFalse(TEXT("Third person is the default"), First->IsFirstPerson());
    First->ToggleCamera();
    TestTrue(TEXT("The toggled player is in first person"), First->IsFirstPerson());
    TestFalse(TEXT("The other player is unchanged"), Second->IsFirstPerson());
    First->ToggleCamera();
    TestFalse(TEXT("A second toggle returns to third person"), First->IsFirstPerson());
    return true;
}

#endif
