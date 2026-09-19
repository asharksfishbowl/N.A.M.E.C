#include "World/Structures/NamecTerrainUpdatedMarker.h"
#include "Save/NamecWorldSave.h"
#include "World/NamecVoxelWorld.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

ANamecTerrainUpdatedMarker::ANamecTerrainUpdatedMarker()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // Visible to every player, wherever they are on the map.
    bReplicates = true;
    bAlwaysRelevant = true;
}

void ANamecTerrainUpdatedMarker::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANamecTerrainUpdatedMarker, ChunkCoord);
}

void ANamecTerrainUpdatedMarker::OnInteract(AActor* Interactor)
{
    if (IsActorBeingDestroyed())
    {
        return;
    }

    const UNamecVoxelWorld* VoxelWorld = GetWorld()->GetSubsystem<UNamecVoxelWorld>();
    if (VoxelWorld && VoxelWorld->WorldSave)
    {
        VoxelWorld->WorldSave->PendingTerrainMarkers.Remove(ChunkCoord);
    }
    Destroy();
}
