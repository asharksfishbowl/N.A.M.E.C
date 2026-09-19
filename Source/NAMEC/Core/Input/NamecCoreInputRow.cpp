#include "Core/Input/NamecCoreInputRow.h"

const TCHAR* const NamecCoreInput::TablePackageName = TEXT("/Game/Data/DT_Core_Input");
const FName NamecCoreInput::RowName(TEXT("Default"));

TOptional<FNamecCoreInputRow> NamecCoreInput::LoadRow()
{
    const UDataTable* Table = LoadObject<UDataTable>(nullptr, TablePackageName);
    const FNamecCoreInputRow* Row = Table ? Table->FindRow<FNamecCoreInputRow>(RowName, TEXT("NamecCoreInput::LoadRow"), false) : nullptr;
    return Row ? TOptional<FNamecCoreInputRow>(*Row) : TOptional<FNamecCoreInputRow>();
}
