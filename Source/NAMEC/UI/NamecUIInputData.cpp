#include "UI/NamecUIInputData.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"

const TCHAR* const UNamecUIInputData::TablePackageName = TEXT("/Game/Data/DT_UI_InputActions");
const FName UNamecUIInputData::ClickRowName(TEXT("Click"));
const FName UNamecUIInputData::BackRowName(TEXT("Back"));

FNamecUIInputActionRow::FNamecUIInputActionRow(const FText& InDisplayName, const FKey& KeyboardKey, const FKey& GamepadKey)
{
    DisplayName = InDisplayName;
    KeyboardInputTypeInfo.SetKey(KeyboardKey);
    DefaultGamepadInputTypeInfo.SetKey(GamepadKey);
}

UNamecUIInputData::UNamecUIInputData()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> ActionTable(TablePackageName);
    DefaultClickAction.DataTable = ActionTable.Object;
    DefaultClickAction.RowName = ClickRowName;
    DefaultBackAction.DataTable = ActionTable.Object;
    DefaultBackAction.RowName = BackRowName;
}
