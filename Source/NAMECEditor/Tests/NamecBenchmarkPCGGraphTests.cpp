#include "Core/Tests/NamecTestFlags.h"
#include "NamecPCGWiring.h"
#include "Elements/PCGStaticMeshSpawner.h"
#include "Elements/PCGSurfaceSampler.h"
#include "PCGCommon.h"
#include "PCGGraph.h"
#include "PCGNode.h"
#include "PCGPin.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
    template<typename SettingsType>
    const UPCGNode* FindNodeWithSettings(const UPCGGraph& Graph)
    {
        UPCGNode* const* Found = Graph.GetNodes().FindByPredicate([](const UPCGNode* Node) { return Node->GetSettings() && Node->GetSettings()->IsA<SettingsType>(); });
        return Found ? *Found : nullptr;
    }

}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecBenchmarkPCGGraphsWiredTest, "Namec.Benchmark.PCGGraphsWired", NamecFoundationTestFlags)

bool FNamecBenchmarkPCGGraphsWiredTest::RunTest(const FString& Parameters)
{
    for (const TCHAR* GraphPath : { TEXT("/Game/World/PCG/PCG_BenchmarkTrees"), TEXT("/Game/World/PCG/PCG_BenchmarkRocks") })
    {
        const UPCGGraph* Graph = LoadObject<UPCGGraph>(nullptr, GraphPath);
        if (!TestNotNull(GraphPath, Graph))
        {
            continue;
        }

        const UPCGNode* Sampler = FindNodeWithSettings<UPCGSurfaceSamplerSettings>(*Graph);
        const UPCGNode* Spawner = FindNodeWithSettings<UPCGStaticMeshSpawnerSettings>(*Graph);
        if (!TestNotNull(FString::Printf(TEXT("%s surface sampler node"), GraphPath), Sampler) || !TestNotNull(FString::Printf(TEXT("%s static mesh spawner node"), GraphPath), Spawner))
        {
            continue;
        }

        TestTrue(FString::Printf(TEXT("%s: the sampler's output feeds the spawner's input"), GraphPath),
            NamecPCGWiring::PinsShareAnEdge(Sampler->GetOutputPin(PCGPinConstants::DefaultOutputLabel), Spawner->GetInputPin(PCGPinConstants::DefaultInputLabel)));
        TestTrue(FString::Printf(TEXT("%s: the spawner's output feeds the graph output node"), GraphPath),
            NamecPCGWiring::PinsShareAnEdge(Spawner->GetOutputPin(PCGPinConstants::DefaultOutputLabel), Graph->GetOutputNode()->GetInputPin(PCGPinConstants::DefaultOutputLabel)));
    }
    return true;
}

#endif
