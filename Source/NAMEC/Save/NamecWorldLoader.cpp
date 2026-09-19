#include "Save/NamecWorldLoader.h"
#include "Save/NamecWorldSave.h"
#include "World/Map/NamecVoxelMapAsset.h"
#include "World/NamecVoxelTypes.h"
#include "World/Structures/NamecTerrainUpdatedMarker.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "NamecWorldLoader"

DEFINE_LOG_CATEGORY_STATIC(LogNamecWorldLoader, Log, All);

bool FNamecWorldLoadDecision::IsRefused() const
{
    return Outcome == ENamecWorldLoadOutcome::RefuseNoMapIdentity
        || Outcome == ENamecWorldLoadOutcome::RefuseDifferentMap
        || Outcome == ENamecWorldLoadOutcome::RefuseNewerMapRevision;
}

FNamecWorldLoadDecision UNamecWorldLoader::EvaluateLoadRule(const UNamecWorldSave& Save, const UNamecVoxelMapAsset& MapAsset)
{
    FNamecWorldLoadDecision Decision;

    // Its own outcome: "a different map" is false for a damaged or hand-edited file.
    if (!Save.MapId.IsValid())
    {
        Decision.Outcome = ENamecWorldLoadOutcome::RefuseNoMapIdentity;
        Decision.Message = LOCTEXT("NoMapIdentity", "This world file has no map identity and cannot be loaded. The file may be damaged.");
    }
    else if (Save.MapId != MapAsset.MapId)
    {
        Decision.Outcome = ENamecWorldLoadOutcome::RefuseDifferentMap;
        Decision.Message = LOCTEXT("DifferentMap", "This world belongs to a different map");
    }
    else if (Save.MapRevision > MapAsset.MapRevision)
    {
        Decision.Outcome = ENamecWorldLoadOutcome::RefuseNewerMapRevision;
        Decision.Message = LOCTEXT("NewerMap", "This world was saved with a newer version of the map");
    }
    else if (Save.MapRevision < MapAsset.MapRevision)
    {
        Decision.Outcome = ENamecWorldLoadOutcome::LoadWithRevisionUpdate;
        Decision.Message = FText::Format(LOCTEXT("RevisionUpdate", "World updated to map revision {0}"), FText::AsNumber(MapAsset.MapRevision, &FNumberFormattingOptions::DefaultNoGrouping()));
        for (const TPair<FIntPoint, FString>& EditedChunk : Save.EditedChunkBaseHashes)
        {
            if (MapAsset.BaseChunkHashes.FindRef(EditedChunk.Key) != EditedChunk.Value)
            {
                Decision.MismatchedChunks.Add(EditedChunk.Key);
            }
        }
    }
    else if (Save.MapHash != MapAsset.MapHash)
    {
        Decision.Outcome = ENamecWorldLoadOutcome::LoadWithMapHashMismatch;
    }
    return Decision;
}

void UNamecWorldLoader::ApplyDecision(const FNamecWorldLoadDecision& Decision, UNamecWorldSave& Save, const UNamecVoxelMapAsset& MapAsset)
{
    if (Decision.Outcome == ENamecWorldLoadOutcome::RefuseNoMapIdentity)
    {
        UE_LOG(LogNamecWorldLoader, Warning, TEXT("World '%s': the save has no MapId and was refused"), *Save.WorldName);
    }
    if (Decision.IsRefused())
    {
        return;
    }

    if (Decision.Outcome == ENamecWorldLoadOutcome::LoadWithMapHashMismatch)
    {
        UE_LOG(LogNamecWorldLoader, Warning, TEXT("World '%s': MapHash differs from the map asset at the same MapRevision %d"), *Save.WorldName, MapAsset.MapRevision);
    }

    for (const FIntPoint& Chunk : Decision.MismatchedChunks)
    {
        UE_LOG(LogNamecWorldLoader, Log, TEXT("World '%s': base terrain of edited chunk (%d, %d) changed in map revision %d"), *Save.WorldName, Chunk.X, Chunk.Y, MapAsset.MapRevision);
        Save.PendingTerrainMarkers.AddUnique(Chunk);
        Save.EditedChunkBaseHashes.Add(Chunk, MapAsset.BaseChunkHashes.FindRef(Chunk));
    }

    Save.BindMapAsset(MapAsset);
}

TArray<ANamecTerrainUpdatedMarker*> UNamecWorldLoader::SpawnPendingMarkers(UWorld& World, const UNamecWorldSave& Save)
{
    TArray<ANamecTerrainUpdatedMarker*> Markers;
    if (World.GetNetMode() == NM_Client)
    {
        return Markers;
    }

    const double ChunkSizeCm = NamecChunkH * static_cast<double>(Save.VoxelResolutionCm);
    for (const FIntPoint& Chunk : Save.PendingTerrainMarkers)
    {
        const FVector ChunkCentre((Chunk.X + 0.5) * ChunkSizeCm, (Chunk.Y + 0.5) * ChunkSizeCm, 0.0);
        ANamecTerrainUpdatedMarker* Marker = World.SpawnActor<ANamecTerrainUpdatedMarker>(ChunkCentre, FRotator::ZeroRotator);
        Marker->ChunkCoord = Chunk;
        Markers.Add(Marker);
    }
    return Markers;
}

#undef LOCTEXT_NAMESPACE
