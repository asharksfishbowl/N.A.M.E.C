#pragma once

#include "CoreMinimal.h"

class UDataTable;
class UScriptStruct;

// The one way a commandlet authors a DT_* asset, so tables never need an editor sitting.
namespace NamecDataTableAuthoring
{
    bool CreateAndSaveDataTable(const FString& PackageName, UScriptStruct* RowStruct, TFunctionRef<void(UDataTable*)> PopulateRows);
}
