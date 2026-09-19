#include "AI/NamecAILODSubsystem.h"
#include "AI/NamecEnemyCharacter.h"
#include "World/NamecVoxelWorld.h"
#include "World/Map/NamecVoxelMapAsset.h"
#include "World/NamecVoxelTypes.h"
#include "Components/StateTreeComponent.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationDirtyArea.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "TimerManager.h"

static constexpr float kLODDistanceCm = 5000.f; // 50 m
static constexpr float kTickInterval  = 0.5f;

void UNamecAILODSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
    Super::OnWorldBeginPlay(InWorld);

    InWorld.GetTimerManager().SetTimer(LODTimer, this, &UNamecAILODSubsystem::UpdateAILOD,
        kTickInterval, /*bLoop=*/true);

    if (UNamecVoxelWorld* VoxelWorld = InWorld.GetSubsystem<UNamecVoxelWorld>())
    {
        VoxelWorld->OnTerrainEdited.AddUObject(this, &UNamecAILODSubsystem::OnChunkEdited);
    }
}

void UNamecAILODSubsystem::OnChunkEdited(FIntPoint ChunkCoord)
{
    UNamecVoxelWorld* VoxelWorld = GetWorld()->GetSubsystem<UNamecVoxelWorld>();
    if (!VoxelWorld || !VoxelWorld->MapAssetOverride) return;

    const float ChunkCm = NamecChunkH * VoxelWorld->MapAssetOverride->VoxelResolutionCm;
    const FVector Min(ChunkCoord.X * ChunkCm, ChunkCoord.Y * ChunkCm, -ChunkCm);
    const FBox ChunkBounds(Min, Min + FVector(ChunkCm, ChunkCm, ChunkCm * 2.f));

    if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    {
        Nav->AddDirtyArea(ChunkBounds, ENavigationDirtyFlag::All, FName("TerrainEdit"));
    }
}

void UNamecAILODSubsystem::UpdateAILOD()
{
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<FVector> CameraLocs;
    for (FLocalPlayerIterator It(GEngine, World); It; ++It)
    {
        if (APlayerController* PC = It->GetPlayerController(World))
        {
            FVector Loc; FRotator Rot;
            PC->GetPlayerViewPoint(Loc, Rot);
            CameraLocs.Add(Loc);
        }
    }

    for (TActorIterator<ANamecEnemyCharacter> It(World); It; ++It)
    {
        ANamecEnemyCharacter* Enemy = *It;
        if (!Enemy || !Enemy->StateTree) continue;

        bool bNear = CameraLocs.IsEmpty(); // no cameras -> keep AI active
        if (!bNear)
        {
            const FVector EnemyLoc = Enemy->GetActorLocation();
            for (const FVector& CamLoc : CameraLocs)
            {
                if (FVector::DistSquared(EnemyLoc, CamLoc) <= kLODDistanceCm * kLODDistanceCm)
                {
                    bNear = true;
                    break;
                }
            }
        }

        if (bNear)
        {
            if (!Enemy->StateTree->IsRunning())
            {
                Enemy->StateTree->StartLogic();
            }
        }
        else
        {
            if (Enemy->StateTree->IsRunning())
            {
                Enemy->StateTree->StopLogic(TEXT("AI LOD distance"));
            }
        }
    }
}
