#include "UI/NamecUIRootLayout.h"
#include "UI/NamecCharacterListScreen.h"
#include "UI/NamecMenuButton.h"
#include "UI/NamecMessageModal.h"
#include "Save/NamecCharacterSave.h"
#include "Save/Tests/NamecSaveTestHelpers.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Core/Tests/NamecTestWorld.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetNavigation.h"
#include "UI/Tests/NamecUITestHelpers.h"
#include "CommonUITypes.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

namespace
{
    // A root layout with its Slate widgets built, which is what makes the menu stack live.
    struct FNamecTestRootLayout
    {
        UNamecUIRootLayout* Root;
        TSharedRef<SWidget> SlateRoot;

        FNamecTestRootLayout(UWorld* World, UNamecSaveFileService& SaveFiles)
            : Root(CreateWidget<UNamecUIRootLayout>(World))
            , SlateRoot(NamecUITestHelpers::BuildSlate(*Root))
        {
            Root->UseSaveFiles(SaveFiles);
        }

        UNamecUIRootLayout* operator->() const { return Root; }
    };

    UNamecMenuButton* ButtonOf(const UNamecMainMenuScreen& MainMenu, ENamecMainMenuEntry Entry)
    {
        return MainMenu.GetEntryButtons()[static_cast<int32>(Entry)];
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecUIMainMenuEntriesTest, "Namec.Foundation.UI.MainMenuHasTheSixEntriesInOrder", NamecFoundationTestFlags)

bool FNamecUIMainMenuEntriesTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    const FNamecTestRootLayout Root(World.Get(), *NewSaveFileService());
    const UNamecMainMenuScreen* MainMenu = Root->ShowMainMenu();

    const TCHAR* const ExpectedLabels[] = { TEXT("New World"), TEXT("Load World"), TEXT("Join LAN Game"), TEXT("Characters"), TEXT("Settings"), TEXT("Quit") };
    const bool ExpectedEnabled[] = { false, false, false, true, true, true };
    if (!TestEqual(TEXT("Entry count"), MainMenu->GetEntryButtons().Num(), 6))
    {
        return true;
    }
    for (int32 Index = 0; Index < 6; ++Index)
    {
        TestEqual(FString::Printf(TEXT("Entry %d label"), Index), MainMenu->GetEntryButtons()[Index]->GetLabel().ToString(), FString(ExpectedLabels[Index]));
        TestEqual(FString::Printf(TEXT("Entry %d enabled"), Index), MainMenu->GetEntryButtons()[Index]->GetIsEnabled(), ExpectedEnabled[Index]);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecUIMainMenuFocusTest, "Namec.Foundation.UI.FirstEnabledEntryHasFocusAndDownReachesEveryEnabledEntry", NamecFoundationTestFlags)

bool FNamecUIMainMenuFocusTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    const FNamecTestRootLayout Root(World.Get(), *NewSaveFileService());
    const UNamecMainMenuScreen* MainMenu = Root->ShowMainMenu();

    TestTrue(TEXT("The main menu is the active screen"), Root->GetActiveMenuScreen() == MainMenu);
    TestTrue(TEXT("The main menu is activated"), MainMenu->IsActivated());

    UWidget* Focused = MainMenu->GetDesiredFocusTarget();
    TestTrue(TEXT("The first enabled entry, Characters, has focus on activation"), Focused == ButtonOf(*MainMenu, ENamecMainMenuEntry::Characters));

    // Simulated down navigation: follow each entry's explicit Down rule, as Slate would on D-pad Down.
    TArray<UWidget*> Visited;
    while (Focused && !Visited.Contains(Focused))
    {
        Visited.Add(Focused);
        TestTrue(TEXT("Navigation never stops on a disabled entry"), Focused->GetIsEnabled());
        Focused = Focused->Navigation ? Focused->Navigation->Down.Widget.Get() : nullptr;
    }
    TestEqual(TEXT("Down reaches every enabled entry"), Visited.Num(), 3);
    TestTrue(TEXT("Down wraps back to the first enabled entry"), Focused == ButtonOf(*MainMenu, ENamecMainMenuEntry::Characters));
    TestTrue(TEXT("Settings is reached"), Visited.Contains(ButtonOf(*MainMenu, ENamecMainMenuEntry::Settings)));
    TestTrue(TEXT("Quit is reached"), Visited.Contains(ButtonOf(*MainMenu, ENamecMainMenuEntry::Quit)));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecUIBackTest, "Namec.Foundation.UI.BackReturnsFromCharactersToTheMenu", NamecFoundationTestFlags)

bool FNamecUIBackTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    UNamecSaveFileService* SaveFiles = NewSaveFileService();

    UNamecCharacterSave* Character = NewObject<UNamecCharacterSave>();
    Character->CharacterGuid = FGuid::NewGuid();
    Character->CharacterName = TEXT("Aldric");
    SaveFiles->Write(*Character, UNamecCharacterSave::MakeFileName(Character->CharacterGuid));

    const FNamecTestRootLayout Root(World.Get(), *SaveFiles);
    UNamecMainMenuScreen* MainMenu = Root->ShowMainMenu();

    MainMenu->SelectEntry(ENamecMainMenuEntry::NewWorld);
    TestTrue(TEXT("A disabled entry opens nothing"), Root->GetActiveMenuScreen() == MainMenu);

    MainMenu->SelectEntry(ENamecMainMenuEntry::Characters);
    UNamecCharacterListScreen* CharacterList = Cast<UNamecCharacterListScreen>(Root->GetActiveMenuScreen());
    if (!TestNotNull(TEXT("Characters opens the character list"), CharacterList))
    {
        return true;
    }
    if (TestEqual(TEXT("One character is listed"), CharacterList->GetCharacterButtons().Num(), 1))
    {
        TestEqual(TEXT("It is listed by name"), CharacterList->GetCharacterButtons()[0]->GetLabel().ToString(), FString(TEXT("Aldric")));
        TestTrue(TEXT("The first entry has focus"), CharacterList->GetDesiredFocusTarget() == CharacterList->GetCharacterButtons()[0]);
    }

    CharacterList->GoBack();
    TestTrue(TEXT("Back returns to the main menu"), Root->GetActiveMenuScreen() == MainMenu);

    // The stack hands the same pooled screen back on a second visit.
    MainMenu->SelectEntry(ENamecMainMenuEntry::Characters);
    const UNamecCharacterListScreen* SecondVisit = Cast<UNamecCharacterListScreen>(Root->GetActiveMenuScreen());
    if (TestNotNull(TEXT("Characters opens again"), SecondVisit))
    {
        TestEqual(TEXT("A second visit lists the character once, not twice"), SecondVisit->GetCharacterButtons().Num(), 1);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecUINewerVersionTest, "Namec.Foundation.UI.NewerVersionCharacterIsGreyedWithTheExactText", NamecFoundationTestFlags)

bool FNamecUINewerVersionTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    UNamecSaveFileService* SaveFiles = NewSaveFileService();

    UNamecCharacterSave* Newer = NewObject<UNamecCharacterSave>();
    const FString FileName = UNamecCharacterSave::MakeFileName(FGuid::NewGuid());
    WriteSaveFileAtVersion(*Newer, Newer->GetCurrentSaveVersion() + 1, FileName);

    const FNamecTestRootLayout Root(World.Get(), *SaveFiles);
    Root->ShowMainMenu()->SelectEntry(ENamecMainMenuEntry::Characters);
    const UNamecCharacterListScreen* CharacterList = Cast<UNamecCharacterListScreen>(Root->GetActiveMenuScreen());
    if (TestNotNull(TEXT("Character list"), CharacterList) && TestEqual(TEXT("The newer save is listed"), CharacterList->GetCharacterButtons().Num(), 1))
    {
        const UNamecMenuButton* Entry = CharacterList->GetCharacterButtons()[0];
        TestFalse(TEXT("It is greyed out"), Entry->GetIsEnabled());
        TestTrue(TEXT("It carries the exact text"), Entry->GetLabel().ToString().EndsWith(TEXT("Requires newer version")));
        TestTrue(TEXT("Focus goes to Back, not to the greyed entry"), CharacterList->GetDesiredFocusTarget() != Entry);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecUICorruptTest, "Namec.Foundation.UI.CorruptCharacterShowsTheExactTextAndIsUntouched", NamecFoundationTestFlags)

bool FNamecUICorruptTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    UNamecSaveFileService* SaveFiles = NewSaveFileService();

    const FString FilePath = UNamecSaveFileService::GetSaveFilePath(UNamecCharacterSave::MakeFileName(FGuid::NewGuid()));
    FFileHelper::SaveStringToFile(TEXT("not a character save"), *FilePath);
    const TArray<uint8> Before = ReadFileBytes(FilePath);

    const FNamecTestRootLayout Root(World.Get(), *SaveFiles);
    UNamecMainMenuScreen* MainMenu = Root->ShowMainMenu();
    MainMenu->SelectEntry(ENamecMainMenuEntry::Characters);

    UNamecMessageModal* Modal = Cast<UNamecMessageModal>(Root->GetActiveMenuScreen());
    if (TestNotNull(TEXT("A message is shown"), Modal))
    {
        TestEqual(TEXT("The exact text"), Modal->GetMessage().ToString(), FString(TEXT("Save could not be loaded")));
        Modal->Dismiss();
        TestTrue(TEXT("Dismissing it returns to the main menu"), Root->GetActiveMenuScreen() == MainMenu);

        MainMenu->SelectEntry(ENamecMainMenuEntry::Characters);
        TestNotNull(TEXT("Trying again shows one message again"), Cast<UNamecMessageModal>(Root->GetActiveMenuScreen()));
        Cast<UNamecMessageModal>(Root->GetActiveMenuScreen())->Dismiss();
        TestTrue(TEXT("And one dismissal still lands on the main menu"), Root->GetActiveMenuScreen() == MainMenu);
    }
    TestTrue(TEXT("File bytes are identical afterwards"), ReadFileBytes(FilePath) == Before);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecUIInputSettingsTest, "Namec.Foundation.UI.CommonUIUsesItsOwnActionRouterAndTheProjectInputData", NamecFoundationTestFlags)

bool FNamecUIInputSettingsTest::RunTest(const FString& Parameters)
{
    NamecUITestHelpers::LoadCommonInputData();
    const UCommonInputSettings& InputSettings = ICommonInputModule::GetSettings();
    TestFalse(TEXT("The Experimental Enhanced Input bridge is off under the project config"), InputSettings.GetEnableEnhancedInputSupport());

    const FCommonInputActionDataBase* Click = InputSettings.GetDefaultClickAction().GetRow<FCommonInputActionDataBase>(TEXT("Test"));
    const FCommonInputActionDataBase* Back = InputSettings.GetDefaultBackAction().GetRow<FCommonInputActionDataBase>(TEXT("Test"));
    TestNotNull(TEXT("The project input data resolves the Click row of DT_UI_InputActions"), Click);
    TestNotNull(TEXT("The project input data resolves the Back row of DT_UI_InputActions"), Back);
    return true;
}

#endif
