#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Commandlets/Commandlet.h"
#include "NamecCreateBenchmarkTablesCommandlet.generated.h"


USTRUCT(BlueprintType)
struct FNamecCharacterRaceRow : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) FName RaceId;
    UPROPERTY(EditAnywhere) FText DisplayName;
    UPROPERTY(EditAnywhere) TSoftObjectPtr<UObject> BaseCO;
    UPROPERTY(EditAnywhere) FName AnimSetRow;
};

// Commandlet that programmatically creates and saves benchmark DataTable assets.
// Run via: UnrealEditor-Cmd.exe NAMEC.uproject -run=NamecCreateBenchmarkTables
UCLASS()
class UNamecCreateBenchmarkTablesCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    virtual int32 Main(const FString& Params) override;
private:
    bool CreateAndSaveDataTable(const FString& PackageName, UScriptStruct* RowStruct,
        TFunctionRef<void(UDataTable*)> PopulateRows);
    bool CreateScalabilityTable();
    bool CreateCharacterRacesTable();
    bool CreateBuildingPiecesTable();
    bool CreateEnemyAIRulesTable();
    bool CreateBenchmarkWearablesTable();
};
