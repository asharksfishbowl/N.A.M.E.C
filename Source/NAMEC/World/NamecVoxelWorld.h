#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "World/NamecVoxelTypes.h"
#include "NamecVoxelWorld.generated.h"

class UNamecVoxelMapAsset;
class UNamecDigDepthQuery;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTerrainEdited, FIntPoint /*ChunkCoord*/);

UCLASS()
class NAMEC_API UNamecVoxelWorld : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    // Decompress chunk from map asset and apply stored edit deltas.
    void AddOrUpdateChunk(FIntPoint ChunkCoord);

    // Called on the server only. Validates the request, writes the kernel, re-meshes
    // affected chunks, and broadcasts OnTerrainEdited. The actual Server RPC that calls
    // this must live on a replicated Actor or ActorComponent (UWorldSubsystem has no
    // net owner). Task 6 wires this via UNamecVoxelWorld::ServerApplyTerrainEdit called
    // from the character's RPC.
    void ApplyTerrainEdit(FVector CentreMetres, float RadiusMetres,
                          ENamecEditMode Mode, FName MaterialRow, int32 ToolTier);

    // Returns true if Position is within dig depth limit using baked base data.
    // "Generated air" = air in the baked base data; player edits are ignored.
    bool IsWithinDigDepth(FVector PositionMetres) const;

    // Load the given map asset into the world subsystem.
    void Load(UNamecVoxelMapAsset* MapAsset);

    // Fired after any chunk re-mesh completes.
    FOnTerrainEdited OnTerrainEdited;

    UPROPERTY()
    TObjectPtr<UNamecVoxelMapAsset> MapAssetOverride;

private:
    UPROPERTY()
    TObjectPtr<UNamecDigDepthQuery> DigDepthQuery;
};
