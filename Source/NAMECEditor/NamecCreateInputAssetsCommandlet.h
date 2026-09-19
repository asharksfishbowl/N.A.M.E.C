#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "NamecCreateInputAssetsCommandlet.generated.h"

enum class ENamecInputDevice : uint8;

// Authors Content/Input/ from NamecBaseInput's tables: one IA_ per base action and the two base
// mapping contexts. Run: UnrealEditor-Cmd.exe NAMEC.uproject -run=NamecCreateInputAssets
UCLASS()
class UNamecCreateInputAssetsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    virtual int32 Main(const FString& Params) override;
private:
    bool CreateActions();
    bool CreateContext(ENamecInputDevice Device);
};
