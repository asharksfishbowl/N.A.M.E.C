#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NamecTerrainUpdatedMarker.generated.h"

// The persisted notice of authored-map Requirement 25.2: one per chunk whose base terrain changed
// under a player's edits. Not FOnTerrainEdited, which reports live edits and persists nothing.
UCLASS()
class NAMEC_API ANamecTerrainUpdatedMarker : public AActor
{
    GENERATED_BODY()

public:
    ANamecTerrainUpdatedMarker();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(Replicated)
    FIntPoint ChunkCoord = FIntPoint::ZeroValue;
};
