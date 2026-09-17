#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "NamecMapBakeCommandlet.generated.h"

// Run as: UnrealEditor-Cmd.exe NAMEC.uproject -run=NamecMapBake -Map=Benchmark
UCLASS()
class NAMECEDITOR_API UNamecMapBakeCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UNamecMapBakeCommandlet();
    virtual int32 Main(const FString& Params) override;
};
