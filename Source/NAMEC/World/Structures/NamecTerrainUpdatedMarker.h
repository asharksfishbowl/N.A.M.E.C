#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/NamecInteractable.h"
#include "NamecTerrainUpdatedMarker.generated.h"

// The persisted notice of authored-map Requirement 25.2: one per chunk whose base terrain changed
// under a player's edits. Not FOnTerrainEdited, which reports live edits and persists nothing.
UCLASS()
class NAMEC_API ANamecTerrainUpdatedMarker : public AActor, public INamecInteractable
{
    GENERATED_BODY()

public:
    ANamecTerrainUpdatedMarker();

    // Clears this chunk from the loaded world save's PendingTerrainMarkers and despawns. A second
    // interaction in the same frame finds the marker already going and does nothing (Edge Case 8).
    virtual void OnInteract(AActor* Interactor) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(Replicated)
    FIntPoint ChunkCoord = FIntPoint::ZeroValue;
};
