#include "NamecCreateFoundationTablesCommandlet.h"
#include "NamecDataTableAuthoring.h"
#include "Save/NamecAutosaveSubsystem.h"
#include "Engine/DataTable.h"

using NamecDataTableAuthoring::CreateAndSaveDataTable;

int32 UNamecCreateFoundationTablesCommandlet::Main(const FString& Params)
{
    const bool bOk = CreateCoreSaveTable();
    if (!bOk)
    {
        UE_LOG(LogTemp, Error, TEXT("NamecCreateFoundationTables: one or more tables failed to save."));
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("NamecCreateFoundationTables: all tables written successfully."));
    return 0;
}

bool UNamecCreateFoundationTablesCommandlet::CreateCoreSaveTable()
{
    return CreateAndSaveDataTable(
        UNamecAutosaveSubsystem::CoreSaveTablePackageName,
        FNamecCoreSaveRow::StaticStruct(),
        [](UDataTable* Table)
        {
            Table->AddRow(UNamecAutosaveSubsystem::CoreSaveRowName, FNamecCoreSaveRow());
        }
    );
}
