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
    // Decompress chunk from map asset and apply stored edit deltas, then notify listeners.
    void AddOrUpdateChunk(FIntPoint ChunkCoord);

    // Validates the edit and computes per-chunk deltas. Pure computation: does NOT write
    // to EditDeltas or broadcast. Caller applies via ApplyDelta on all machines.
    void ApplyTerrainEdit(FVector CentreMetres, float RadiusMetres,
                          ENamecEditMode Mode, FName MaterialRow, int32 ToolTier,
                          TArray<FIntPoint>& OutChunkCoords,
                          TArray<TArray<FNamecVoxelDelta>>& OutPerChunkDeltas);

    // Apply a replicated voxel delta to the local delta store and re-mesh the chunk.
    // Called on clients when they receive the Multicast RPC from the server.
    void ApplyDelta(FIntPoint ChunkCoord, const TArray<FNamecVoxelDelta>& Deltas);

    // Return the current voxel state: baked base with all stored edit deltas applied.
    // Listeners that receive OnTerrainEdited call this to get the data for SetVoxelData.
    FNamecVoxelData GetChunkVoxelData(FIntPoint ChunkCoord) const;

    // Drop all edit deltas for chunks overlapping ArenaCentre within ArenaRadius and
    // re-mesh those chunks from their baked base. Called when a boss fight ends.
    void RevertArenaEdits(FVector ArenaCentreMetres, float ArenaRadiusMetres);

    // Returns true if Position is within dig depth limit using baked base data.
    bool IsWithinDigDepth(FVector PositionMetres) const;

    // Load the given map asset into the world subsystem.
    void Load(UNamecVoxelMapAsset* MapAsset);

    // Fired after any chunk re-mesh completes. Listeners call GetChunkVoxelData to get state.
    FOnTerrainEdited OnTerrainEdited;

    UPROPERTY()
    TObjectPtr<UNamecVoxelMapAsset> MapAssetOverride;

private:
    UPROPERTY()
    TObjectPtr<UNamecDigDepthQuery> DigDepthQuery;

    // Per-chunk accumulated edit deltas applied on top of the baked base data.
    TMap<FIntPoint, TMap<int32, uint8>> EditDeltas;
    float ChunkSizeMetres() const;
};
