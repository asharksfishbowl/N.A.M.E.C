#include "NamecCreateBenchmarkPCGCommandlet.h"
#include "PCGGraph.h"
#include "Elements/PCGSurfaceSampler.h"
#include "Elements/PCGStaticMeshSpawner.h"
#include "MeshSelectors/PCGMeshSelectorWeighted.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataTable.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"

int32 UNamecCreateBenchmarkPCGCommandlet::Main(const FString& Params)
{
    bool bOk = CreatePCGMeshTable(
        TEXT("/Game/Data/DT_PCG_Trees"),
        TEXT("/Game/Placeholder/Trees/SM_Tree_Pine_01.SM_Tree_Pine_01"), 1.0f);

    bOk &= CreatePCGMeshTable(
        TEXT("/Game/Data/DT_PCG_Rocks"),
        TEXT("/Game/Placeholder/Rocks/SM_Rock_01.SM_Rock_01"), 2.0f);

    bOk &= CreatePCGGraph(
        TEXT("/Game/World/PCG/PCG_BenchmarkTrees"),
        TEXT("/Game/Placeholder/Trees/SM_Tree_Pine_01.SM_Tree_Pine_01"));

    bOk &= CreatePCGGraph(
        TEXT("/Game/World/PCG/PCG_BenchmarkRocks"),
        TEXT("/Game/Placeholder/Rocks/SM_Rock_01.SM_Rock_01"));

    return bOk ? 0 : 1;
}

bool UNamecCreateBenchmarkPCGCommandlet::CreatePCGMeshTable(
    const FString& PackageName, const FString& MeshPath, float Density)
{
    UPackage* Pkg = CreatePackage(*PackageName);
    Pkg->FullyLoad();
    FString AssetName = FPackageName::GetLongPackageAssetName(PackageName);
    UDataTable* T = NewObject<UDataTable>(Pkg, *AssetName, RF_Public | RF_Standalone);
    T->RowStruct = FNamecPCGMeshRow::StaticStruct();

    FNamecPCGMeshRow Row;
    Row.Mesh    = FSoftObjectPath(MeshPath);
    Row.Density = Density;
    T->AddRow(FName("Default"), Row);

    Pkg->MarkPackageDirty();
    FString FilePath = FPackageName::LongPackageNameToFilename(
        PackageName, FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), true);
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    bool bSaved = UPackage::SavePackage(Pkg, T, *FilePath, SaveArgs);
    UE_LOG(LogTemp, Display, TEXT("NamecCreateBenchmarkPCG: %s -> %s"),
        *PackageName, bSaved ? TEXT("OK") : TEXT("FAIL"));

    if (bSaved)
    {
        FAssetRegistryModule& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        AR.Get().AssetCreated(T);
    }
    return bSaved;
}

bool UNamecCreateBenchmarkPCGCommandlet::CreatePCGGraph(
    const FString& PackageName, const FString& MeshPath)
{
    UPackage* Pkg = CreatePackage(*PackageName);
    Pkg->FullyLoad();
    FString AssetName = FPackageName::GetLongPackageAssetName(PackageName);
    UPCGGraph* Graph = NewObject<UPCGGraph>(Pkg, *AssetName, RF_Public | RF_Standalone);

    // Surface sampler: generates random points on the landscape surface
    UPCGSurfaceSamplerSettings* SamplerSettings = nullptr;
    UPCGNode* SamplerNode = Graph->AddNodeOfType<UPCGSurfaceSamplerSettings>(SamplerSettings);
    SamplerSettings->PointsPerSquaredMeter = 0.1f;

    // Spawner: places static mesh instances at sampled points
    UPCGStaticMeshSpawnerSettings* SpawnerSettings = nullptr;
    UPCGNode* SpawnerNode = Graph->AddNodeOfType<UPCGStaticMeshSpawnerSettings>(SpawnerSettings);
    SpawnerSettings->SetMeshSelectorType(UPCGMeshSelectorWeighted::StaticClass());
    UPCGMeshSelectorWeighted* Selector = Cast<UPCGMeshSelectorWeighted>(SpawnerSettings->MeshSelectorParameters);
    if (Selector)
    {
        // Constructor(SoftMesh, Weight) sets Descriptor.StaticMesh and Weight
        Selector->MeshEntries.Add(FPCGMeshSelectorWeightedEntry(
            TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(MeshPath)), 1));
    }

    // Wire: sampler -> spawner -> graph output
    Graph->AddEdge(SamplerNode, NAME_None, SpawnerNode,           NAME_None);
    Graph->AddEdge(SpawnerNode, NAME_None, Graph->GetOutputNode(), NAME_None);

    Pkg->MarkPackageDirty();
    FString FilePath = FPackageName::LongPackageNameToFilename(
        PackageName, FPackageName::GetAssetPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), true);
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    bool bSaved = UPackage::SavePackage(Pkg, Graph, *FilePath, SaveArgs);
    UE_LOG(LogTemp, Display, TEXT("NamecCreateBenchmarkPCG: %s -> %s"),
        *PackageName, bSaved ? TEXT("OK") : TEXT("FAIL"));

    // HANDOVER: PCG 5.8 API verified: UPCGSurfaceSamplerSettings::PointsPerSquaredMeter for density;
    // UPCGStaticMeshSpawnerSettings + UPCGMeshSelectorWeighted::MeshEntries for mesh; no DataTable
    // pin available on UPCGStaticMeshSpawnerSettings in 5.8 (Table pin requires PCGAttributeGraph node
    // wired separately at edit time). Placeholder mesh replaces at edit time.

    if (bSaved)
    {
        FAssetRegistryModule& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        AR.Get().AssetCreated(Graph);
    }
    return bSaved;
}
