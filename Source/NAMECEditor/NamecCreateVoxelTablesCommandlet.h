#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "NamecCreateVoxelTablesCommandlet.generated.h"

// Run: UnrealEditor-Cmd.exe NAMEC.uproject -run=NamecCreateVoxelTables
UCLASS()
class UNamecCreateVoxelTablesCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    virtual int32 Main(const FString& Params) override;
private:
    bool CreateAndSaveDataTable(const FString& PackageName, UScriptStruct* RowStruct,
        TFunctionRef<void(UDataTable*)> PopulateRows);
    bool CreateVoxelMaterialsTable();
    bool CreateClimatesTable();
    bool CreateOreVeinsTable();
};
