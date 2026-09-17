#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NamecBenchmarkDirector.generated.h"

// UFUNCTION(Exec) does not reach plain AActor. NamecBenchmark.Run is registered via
// FAutoConsoleCommand in BeginPlay.
UCLASS()
class NAMEC_API ANamecBenchmarkDirector : public AActor
{
    GENERATED_BODY()

public:
    ANamecBenchmarkDirector();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void RunBenchmark();

    TUniquePtr<class IConsoleObject> RunCmd;
};
