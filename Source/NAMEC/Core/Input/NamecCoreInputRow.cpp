#include "Core/Input/NamecCoreInputRow.h"
#include "Core/NamecDataTableRows.h"

const TCHAR* const NamecCoreInput::TablePackageName = TEXT("/Game/Data/DT_Core_Input");
const FName NamecCoreInput::RowName(TEXT("Default"));

TOptional<FNamecCoreInputRow> NamecCoreInput::LoadRow()
{
    return NamecDataTableRows::LoadRow<FNamecCoreInputRow>(TablePackageName, RowName);
}
