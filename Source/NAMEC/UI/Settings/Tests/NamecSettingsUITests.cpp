#include "UI/Settings/NamecSettingsScreen.h"
#include "UI/Settings/InputRemapScreen/NamecInputRemapScreen.h"
#include "UI/Tests/NamecTestLocalPlayerUI.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Core/Tests/NamecTestWorld.h"
#include "CommonInputSettings.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

namespace
{
    UNamecSettingsScreen* OpenSettings(const FNamecTestLocalPlayerUI& Player)
    {
        Player.Root->ShowMainMenu()->SelectEntry(ENamecMainMenuEntry::Settings);
        return Cast<UNamecSettingsScreen>(Player.Root->GetActiveMenuScreen());
    }

    UNamecInputRemapScreen* OpenInputRemap(const FNamecTestLocalPlayerUI& Player)
    {
        UNamecSettingsScreen* SettingsScreen = OpenSettings(Player);
        if (SettingsScreen)
        {
            SettingsScreen->ShowTab(ENamecSettingsTab::Controls);
            SettingsScreen->OpenInputRemap();
        }
        return Cast<UNamecInputRemapScreen>(Player.Root->GetActiveMenuScreen());
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSettingsUISlotTest, "Namec.Foundation.SettingsUI.PlayerTwoTogglingNightEyesChangesSlotTwoOnly", NamecFoundationTestFlags)

bool FNamecSettingsUISlotTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    const FNamecTestMachine Machine;
    const FNamecTestLocalPlayerUI PlayerTwo(*World.Get(), 2, Machine);

    UNamecSettingsScreen* SettingsScreen = OpenSettings(PlayerTwo);
    if (!TestNotNull(TEXT("Settings opens from the main menu"), SettingsScreen))
    {
        return true;
    }
    TestEqual(TEXT("It opens on the Gameplay tab"), SettingsScreen->GetShownTab(), ENamecSettingsTab::Gameplay);
    TestEqual(TEXT("Label before"), SettingsScreen->GetRowLabel(TEXT("NightEyes")).ToString(), FString(TEXT("Night Eyes: On")));

    SettingsScreen->SelectRow(TEXT("NightEyes"));
    TestEqual(TEXT("The label follows the applied setting"), SettingsScreen->GetRowLabel(TEXT("NightEyes")).ToString(), FString(TEXT("Night Eyes: Off")));

    const FNamecSettings& Reloaded = StartSettingsService()->GetSettings();
    TestFalse(TEXT("Slot 2 Night Eyes is off after a reload"), Reloaded.GetSlot(2).bNightEyes);
    for (const int32 OtherSlot : { 1, 3, 4 })
    {
        TestTrue(FString::Printf(TEXT("Slot %d Night Eyes is unchanged"), OtherSlot), Reloaded.GetSlot(OtherSlot).bNightEyes);
    }

    SettingsScreen->GoBack();
    TestNotNull(TEXT("Back returns to the main menu"), Cast<UNamecMainMenuScreen>(PlayerTwo.Root->GetActiveMenuScreen()));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSettingsUIMachineTest, "Namec.Foundation.SettingsUI.GraphicsChangeLandsInTheMachineSectionWithOneBroadcast", NamecFoundationTestFlags)

bool FNamecSettingsUIMachineTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    const FNamecTestMachine Machine;
    const FNamecTestLocalPlayerUI PlayerTwo(*World.Get(), 2, Machine);
    UNamecSettingsScreen* SettingsScreen = OpenSettings(PlayerTwo);
    if (!TestNotNull(TEXT("Settings screen"), SettingsScreen))
    {
        return true;
    }

    int32 BroadcastCount = 0;
    Machine.Settings->OnSettingsApplied.AddLambda([&BroadcastCount](const FNamecSettings&) { ++BroadcastCount; });
    const int32 ShadowBefore = Machine.Settings->GetSettings().Machine.ShadowQuality;

    SettingsScreen->ShowTab(ENamecSettingsTab::Graphics);
    SettingsScreen->SelectRow(TEXT("ShadowQuality"));
    TestEqual(TEXT("OnSettingsApplied fires once"), BroadcastCount, 1);

    const FNamecSettings& Reloaded = StartSettingsService()->GetSettings();
    TestNotEqual(TEXT("The machine-wide shadow quality changed"), Reloaded.Machine.ShadowQuality, ShadowBefore);
    TestTrue(TEXT("No slot section changed"), Reloaded.GetSlot(2).bNightEyes && Reloaded.GetSlot(2).bExecutionPrompts && Reloaded.GetSlot(2).GamepadRemaps.Num() == 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSettingsUIRemapTest, "Namec.Foundation.SettingsUI.RemapOnTheScreenSwapsAndPersistsInSlotTwoOnly", NamecFoundationTestFlags)

bool FNamecSettingsUIRemapTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    const FNamecTestMachine Machine;
    const FNamecTestLocalPlayerUI PlayerTwo(*World.Get(), 2, Machine);
    UNamecInputRemapScreen* RemapScreen = OpenInputRemap(PlayerTwo);
    if (!TestNotNull(TEXT("Remap screen opens from the Controls tab"), RemapScreen))
    {
        return true;
    }

    TestTrue(TEXT("Player 2 is offered no keyboard binding"), RemapScreen->GetBindingLabel(ENamecInputDevice::KeyboardMouse, TEXT("Crouch")).IsEmpty());
    TestTrue(TEXT("A stick is not offered for capture"), RemapScreen->GetBindingLabel(ENamecInputDevice::Gamepad, TEXT("Move")).IsEmpty());

    // Action A = UseConsumable (X). Action B = FavoritesMenu (D-pad Up).
    RemapScreen->BeginCapture(ENamecInputDevice::Gamepad, TEXT("UseConsumable"));
    TestTrue(TEXT("The screen is capturing"), RemapScreen->IsCapturing());
    TestFalse(TEXT("A keyboard key is not taken for a gamepad binding"), RemapScreen->CaptureKey(EKeys::K));
    TestTrue(TEXT("It is still capturing"), RemapScreen->IsCapturing());
    TestTrue(TEXT("The next gamepad key is captured"), RemapScreen->CaptureKey(EKeys::Gamepad_DPad_Up));
    TestFalse(TEXT("Capture ends"), RemapScreen->IsCapturing());

    TestTrue(TEXT("A shows B's key"), RemapScreen->GetBindingLabel(ENamecInputDevice::Gamepad, TEXT("UseConsumable")).ToString().EndsWith(EKeys::Gamepad_DPad_Up.GetDisplayName().ToString()));
    TestTrue(TEXT("B shows A's previous key"), RemapScreen->GetBindingLabel(ENamecInputDevice::Gamepad, TEXT("FavoritesMenu")).ToString().EndsWith(EKeys::Gamepad_FaceButton_Left.GetDisplayName().ToString()));

    const FNamecSettings& Reloaded = StartSettingsService()->GetSettings();
    TestEqual(TEXT("A is on B's key after a reload"), Reloaded.GetSlot(2).GamepadRemaps.FindRef(TEXT("UseConsumable")), EKeys::Gamepad_DPad_Up);
    TestEqual(TEXT("B is on A's previous key after a reload"), Reloaded.GetSlot(2).GamepadRemaps.FindRef(TEXT("FavoritesMenu")), EKeys::Gamepad_FaceButton_Left);

    // No action unbound: every gamepad binding still resolves to a valid key.
    for (const FNamecBaseBinding& Binding : NamecBaseInput::GetBindings(ENamecInputDevice::Gamepad))
    {
        TestTrue(FString::Printf(TEXT("%s is still bound"), *Binding.BindingName.ToString()), NamecInputRemap::GetCurrentKey(Reloaded.GetSlot(2).GamepadRemaps, Binding).IsValid());
    }
    for (const int32 OtherSlot : { 1, 3, 4 })
    {
        TestEqual(FString::Printf(TEXT("Slot %d gamepad remaps"), OtherSlot), Reloaded.GetSlot(OtherSlot).GamepadRemaps.Num(), 0);
        TestEqual(FString::Printf(TEXT("Slot %d keyboard remaps"), OtherSlot), Reloaded.GetSlot(OtherSlot).KeyboardMouseRemaps.Num(), 0);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecSettingsUIEvaluationTest, "Namec.Foundation.SettingsUI.MenusAndGameplayInputAreSeparateSystems", NamecFoundationTestFlags)

bool FNamecSettingsUIEvaluationTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    const FNamecTestMachine Machine;
    const FNamecTestLocalPlayerUI Player(*World.Get(), 1, Machine);

    TestFalse(TEXT("UCommonInputSettings reports the Enhanced Input bridge off under the project config"), GetDefault<UCommonInputSettings>()->GetEnableEnhancedInputSupport());

    UNamecSettingsScreen* SettingsScreen = OpenSettings(Player);
    if (!TestNotNull(TEXT("Settings screen"), SettingsScreen))
    {
        return true;
    }

    // With the screen open, the player's gameplay mapping contexts are still applied and still fire.
    TestTrue(TEXT("Gamepad context stays applied"), Player.Input.Contexts->HasBaseContext(ENamecInputDevice::Gamepad));
    TestTrue(TEXT("Keyboard-and-mouse context stays applied"), Player.Input.Contexts->HasBaseContext(ENamecInputDevice::KeyboardMouse));
    const TSharedRef<int32> Jump = Player.Input.CountTriggers(TEXT("Jump"));
    Player.Input.Press(EKeys::Gamepad_FaceButton_Bottom);
    Player.Input.Tick();
    TestEqual(TEXT("A gameplay action still fires through Enhanced Input"), *Jump, 1);

    // Back is Common UI's: the screen is a back handler, and handling Back closes it.
    TestTrue(TEXT("The screen is activated"), SettingsScreen->IsActivated());
    SettingsScreen->GoBack();
    TestFalse(TEXT("Handling Back deactivates it"), SettingsScreen->IsActivated());
    TestNotNull(TEXT("And the main menu is on top again"), Cast<UNamecMainMenuScreen>(Player.Root->GetActiveMenuScreen()));
    return true;
}

#endif
