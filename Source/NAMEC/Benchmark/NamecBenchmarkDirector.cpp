#include "Benchmark/NamecBenchmarkDirector.h"
#include "HAL/IConsoleManager.h"

ANamecBenchmarkDirector::ANamecBenchmarkDirector() {}

void ANamecBenchmarkDirector::BeginPlay()
{
    Super::BeginPlay();
    RunCmd = TUniquePtr<IConsoleObject>(IConsoleManager::Get().RegisterConsoleCommand(
        TEXT("NamecBenchmark.Run"),
        TEXT("Run the benchmark sequence"),
        FConsoleCommandDelegate::CreateUObject(this, &ANamecBenchmarkDirector::RunBenchmark)));
}

void ANamecBenchmarkDirector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    RunCmd.Reset();
    Super::EndPlay(EndPlayReason);
}

void ANamecBenchmarkDirector::RunBenchmark() {}
