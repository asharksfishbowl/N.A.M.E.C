#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "NamecCreateFoundationTablesCommandlet.generated.h"

// Authors the Phase 2 tuning tables. Each Phase 2 task that needs a table adds it here.
// Run: UnrealEditor-Cmd.exe NAMEC.uproject -run=NamecCreateFoundationTables
UCLASS()
class UNamecCreateFoundationTablesCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    virtual int32 Main(const FString& Params) override;
private:
    bool CreateCoreSaveTable();
    bool CreateCoreInputTable();
};
