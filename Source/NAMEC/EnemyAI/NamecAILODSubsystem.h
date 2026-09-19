#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "NamecAILODSubsystem.generated.h"

// Handles two responsibilities:
//  1. Pauses/resumes StateTree for enemies beyond 50 m from any local viewport camera (0.5 s tick).
//  2. Marks the navmesh dirty for terrain-edited chunks (subscribes to UNamecVoxelWorld::OnTerrainEdited).
UCLASS()
class NAMEC_API UNamecAILODSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

protected:
    virtual void OnWorldBeginPlay(UWorld& InWorld) override;

private:
    void UpdateAILOD();
    void OnChunkEdited(FIntPoint ChunkCoord);
    FTimerHandle LODTimer;
};
