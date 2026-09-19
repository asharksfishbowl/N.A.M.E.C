#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

namespace NamecDataTableRows
{
    // The row by value, unset when the table or the row is missing. By value because nothing
    // keeps the table loaded for a caller that holds a pointer into it.
    template<typename RowType>
    TOptional<RowType> LoadRow(const TCHAR* TablePackageName, FName RowName)
    {
        const UDataTable* Table = LoadObject<UDataTable>(nullptr, TablePackageName);
        const RowType* Row = Table ? Table->FindRow<RowType>(RowName, TEXT("NamecDataTableRows::LoadRow"), false) : nullptr;
        return Row ? TOptional<RowType>(*Row) : TOptional<RowType>();
    }
}
