#include "Benchmark/NamecBenchmarkDirector.h"

ANamecBenchmarkDirector::ANamecBenchmarkDirector() {}

void ANamecBenchmarkDirector::BeginPlay()
{
    Super::BeginPlay();
    RunCmd.Emplace(TEXT("NamecBenchmark.Run"), TEXT("Run the benchmark sequence"),
        FConsoleCommandDelegate::CreateUObject(this, &ANamecBenchmarkDirector::RunBenchmark));
}

void ANamecBenchmarkDirector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    RunCmd.Reset();
    Super::EndPlay(EndPlayReason);
}

void ANamecBenchmarkDirector::RunBenchmark() {}
