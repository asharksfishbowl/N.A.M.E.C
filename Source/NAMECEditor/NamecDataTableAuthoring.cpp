#include "NamecDataTableAuthoring.h"
#include "NamecAssetAuthoring.h"
#include "Engine/DataTable.h"

bool NamecDataTableAuthoring::CreateAndSaveDataTable(const FString& PackageName, UScriptStruct* RowStruct, TFunctionRef<void(UDataTable*)> PopulateRows)
{
    UDataTable* Table = NamecAssetAuthoring::NewAssetInPackage<UDataTable>(PackageName);
    Table->RowStruct = RowStruct;
    PopulateRows(Table);
    return NamecAssetAuthoring::SaveAsset(*Table);
}
