#include "Benchmark/NamecBenchmarkDirector.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Components/StaticMeshComponent.h"

static void SpawnLumenTestScene()
{
    UWorld* World = GEngine ? GEngine->GetCurrentPlayWorld() : nullptr;
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("NamecBenchmark.SpawnLumenTest: no game world"));
        return;
    }

    // TestChunk — produced by NamecMeshExport -Chunk=0,0
    UStaticMesh* ChunkMesh = Cast<UStaticMesh>(
        StaticLoadObject(UStaticMesh::StaticClass(), nullptr,
                         TEXT("/Game/Benchmark/TestChunk")));
    if (ChunkMesh)
    {
        AStaticMeshActor* A = World->SpawnActor<AStaticMeshActor>(
            FVector::ZeroVector, FRotator::ZeroRotator);
        if (A) A->GetStaticMeshComponent()->SetStaticMesh(ChunkMesh);
    }
    else
    {
        UE_LOG(LogTemp, Warning,
               TEXT("NamecBenchmark.SpawnLumenTest: /Game/Benchmark/TestChunk not found — "
                    "run NamecMeshExport -Chunk=0,0 first"));
    }

    // White reference cube (engine built-in shape + default material)
    UStaticMesh* CubeMesh = Cast<UStaticMesh>(
        StaticLoadObject(UStaticMesh::StaticClass(), nullptr,
                         TEXT("/Engine/BasicShapes/Cube.Cube")));
    if (CubeMesh)
    {
        AStaticMeshActor* Cube = World->SpawnActor<AStaticMeshActor>(
            FVector(800.f, 0.f, 100.f), FRotator::ZeroRotator);
        if (Cube) Cube->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
    }

    // Directional light — angled to cast shadows across the chunk surface
    World->SpawnActor<ADirectionalLight>(FVector(0.f, 0.f, 500.f), FRotator(-45.f, -45.f, 0.f));

    UE_LOG(LogTemp, Display, TEXT("NamecBenchmark.SpawnLumenTest: scene spawned"));
}

static FAutoConsoleCommand GSpawnLumenTestCmd(
    TEXT("NamecBenchmark.SpawnLumenTest"),
    TEXT("Spawn TestChunk mesh, white cube, and directional light for Lumen sanity check. "
         "No level authoring needed. Run after NamecMeshExport -Chunk=0,0."),
    FConsoleCommandDelegate::CreateStatic(&SpawnLumenTestScene));

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
