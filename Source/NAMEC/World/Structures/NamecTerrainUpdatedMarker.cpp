#include "World/Structures/NamecTerrainUpdatedMarker.h"
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
