#include "Core/Tests/NamecTestFlags.h"
#include "Engine/DataTable.h"
#include "PCGGraph.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecBenchmarkGeneratedAssetsLoadTest, "Namec.Benchmark.GeneratedAssetsLoad", NamecFoundationTestFlags)

bool FNamecBenchmarkGeneratedAssetsLoadTest::RunTest(const FString& Parameters)
{
    // The commandlet-generated assets that are committed, so a fresh checkout needs no commandlet run.
    const TCHAR* const TablePackages[] =
    {
        TEXT("/Game/Data/DT_Character_Races"),
        TEXT("/Game/Data/DT_Benchmark_BuildingPieces"),
        TEXT("/Game/Data/DT_EnemyAI_Rules"),
        TEXT("/Game/Data/DT_EnemyAI_BenchmarkWearables"),
        TEXT("/Game/Data/DT_PCG_Trees"),
        TEXT("/Game/Data/DT_PCG_Rocks"),
    };
    for (const TCHAR* PackageName : TablePackages)
    {
        const UDataTable* Table = LoadObject<UDataTable>(nullptr, PackageName);
        if (TestNotNull(PackageName, Table))
        {
            TestNotNull(FString::Printf(TEXT("%s row struct"), PackageName), Table->GetRowStruct());
        }
    }

    for (const TCHAR* PackageName : { TEXT("/Game/World/PCG/PCG_BenchmarkTrees"), TEXT("/Game/World/PCG/PCG_BenchmarkRocks") })
    {
        TestNotNull(PackageName, LoadObject<UPCGGraph>(nullptr, PackageName));
    }
    return true;
}

#endif
