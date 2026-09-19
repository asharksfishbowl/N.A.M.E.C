#include "UI/NamecUILayerSubsystem.h"
#include "UI/NamecUIRootLayout.h"
#include "Core/Input/Tests/NamecInputTestPlayer.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Core/Tests/NamecTestWorld.h"
#include "Save/NamecAutosaveSubsystem.h"
#include "Save/NamecCharacterSave.h"
#include "Save/Tests/NamecSaveTestHelpers.h"
#include "UI/Tests/NamecTestLocalPlayerUI.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecUILayerTest, "Namec.Foundation.UILayer.EachLocalPlayerHasItsOwnMenuStackAndHudLayer", NamecFoundationTestFlags)

bool FNamecUILayerTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    const FNamecTestMachine Machine;
    UNamecAutosaveSubsystem* Autosave = Machine.Autosave;
    const FNamecTestLocalPlayerUI PlayerOne(*World.Get(), 1, Machine);
    const FNamecTestLocalPlayerUI PlayerTwo(*World.Get(), 2, Machine);

    TestTrue(TEXT("Each local player has its own root layout"), PlayerOne.Root && PlayerTwo.Root && PlayerOne.Root != PlayerTwo.Root);
    TestTrue(TEXT("And its own menu stack"), PlayerOne.Root->GetMenuStack() != PlayerTwo.Root->GetMenuStack());
    TestTrue(TEXT("And its own HUD layer"), PlayerOne.Root->GetHudLayer() != PlayerTwo.Root->GetHudLayer());

    const UNamecMainMenuScreen* PlayerTwoMenu = PlayerTwo.Root->ShowMainMenu();
    TestTrue(TEXT("Opening the menu for player 2 activates it in player 2's stack"), PlayerTwo.Root->GetActiveMenuScreen() == PlayerTwoMenu);
    TestNull(TEXT("And activates nothing in player 1's stack"), PlayerOne.Root->GetActiveMenuScreen());

    // A disk-full autosave on this machine: the warning belongs on local player 1's HUD only.
    UNamecCharacterSave* Character = NewObject<UNamecCharacterSave>();
    Autosave->RegisterSave(*Character, TEXT("Character.sav"));
    Platform->FreeDiskSpaceBytes = 0;
    AddExpectedMessage(TEXT("was not written"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);
    Autosave->RunAutosave();
    TestEqual(TEXT("OnSaveWarning puts its text on player 1's HUD layer"), PlayerOne.Root->GetHudNotice().ToString(), FString(TEXT("Not enough disk space to save.")));
    TestTrue(TEXT("And not on player 2's"), PlayerTwo.Root->GetHudNotice().IsEmpty());

    PlayerTwo.Layer->RemoveRootLayout();
    TestNull(TEXT("Removing player 2 removes its root"), PlayerTwo.Layer->GetRootLayout());
    TestTrue(TEXT("And only its root"), PlayerOne.Layer->GetRootLayout() == PlayerOne.Root);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecUILayerDeviceRuleTest, "Namec.Foundation.UILayer.PlayerTwoHasNoKeyboardAndMouseEndToEnd", NamecFoundationTestFlags)

bool FNamecUILayerDeviceRuleTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    UNamecSettingsService* Settings = StartSettingsService();
    const FNamecTestPlayer PlayerOne(World.Get(), 1, *Settings);
    const FNamecTestPlayer PlayerTwo(World.Get(), 2, *Settings);

    // The same keyboard key reaches both controllers; only player 1 has a context that maps it.
    const TSharedRef<int32> PlayerOneCrouch = PlayerOne.CountTriggers(TEXT("Crouch"));
    const TSharedRef<int32> PlayerTwoCrouch = PlayerTwo.CountTriggers(TEXT("Crouch"));
    for (const FNamecTestPlayer* Player : { &PlayerOne, &PlayerTwo })
    {
        Player->Press(EKeys::LeftControl);
        Player->Tick();
    }
    TestEqual(TEXT("Left Ctrl crouches local player 1"), *PlayerOneCrouch, 1);
    TestEqual(TEXT("Left Ctrl does nothing for local player 2"), *PlayerTwoCrouch, 0);

    PlayerTwo.Press(EKeys::Gamepad_LeftThumbstick);
    PlayerTwo.Tick();
    TestEqual(TEXT("Player 2's gamepad still works"), *PlayerTwoCrouch, 1);
    return true;
}

#endif
