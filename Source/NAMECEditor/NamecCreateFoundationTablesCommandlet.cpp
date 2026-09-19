#include "NamecCreateFoundationTablesCommandlet.h"
#include "NamecDataTableAuthoring.h"
#include "Core/Input/NamecCoreInputRow.h"
#include "Multiplayer/NamecSessionRules.h"
#include "Save/NamecAutosaveSubsystem.h"
#include "UI/NamecUIInputData.h"
#include "Engine/DataTable.h"

using NamecDataTableAuthoring::CreateAndSaveDataTable;

int32 UNamecCreateFoundationTablesCommandlet::Main(const FString& Params)
{
    bool bOk = CreateCoreSaveTable();
    bOk &= CreateCoreInputTable();
    bOk &= CreateSessionTable();
    bOk &= CreateUIInputActionsTable();
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

bool UNamecCreateFoundationTablesCommandlet::CreateCoreInputTable()
{
    return CreateAndSaveDataTable(
        NamecCoreInput::TablePackageName,
        FNamecCoreInputRow::StaticStruct(),
        [](UDataTable* Table)
        {
            Table->AddRow(NamecCoreInput::RowName, FNamecCoreInputRow());
        }
    );
}

bool UNamecCreateFoundationTablesCommandlet::CreateSessionTable()
{
    return CreateAndSaveDataTable(
        NamecSessionRules::TablePackageName,
        FNamecSessionRow::StaticStruct(),
        [](UDataTable* Table)
        {
            Table->AddRow(NamecSessionRules::RowName, FNamecSessionRow());
        }
    );
}

#define LOCTEXT_NAMESPACE "NamecUIInputActions"

bool UNamecCreateFoundationTablesCommandlet::CreateUIInputActionsTable()
{
    return CreateAndSaveDataTable(
        UNamecUIInputData::TablePackageName,
        FNamecUIInputActionRow::StaticStruct(),
        [](UDataTable* Table)
        {
            Table->AddRow(UNamecUIInputData::ClickRowName, FNamecUIInputActionRow(LOCTEXT("Click", "Select"), EKeys::Enter, EKeys::Gamepad_FaceButton_Bottom));
            Table->AddRow(UNamecUIInputData::BackRowName, FNamecUIInputActionRow(LOCTEXT("Back", "Back"), EKeys::Escape, EKeys::Gamepad_FaceButton_Right));
        }
    );
}

#undef LOCTEXT_NAMESPACE
