#include "Save/NamecWorldSave.h"
#include "World/Map/NamecVoxelMapAsset.h"

void UNamecWorldSave::BindMapAsset(const UNamecVoxelMapAsset& MapAsset)
{
    BoundMapAsset = &MapAsset;
    StampMapIdentity(MapAsset);
}

void UNamecWorldSave::StampMapIdentity(const UNamecVoxelMapAsset& MapAsset)
{
    MapId = MapAsset.MapId;
    MapRevision = MapAsset.MapRevision;
    MapHash = MapAsset.MapHash;
}

void UNamecWorldSave::RecordFirstEditOfChunk(FIntPoint ChunkCoord, const UNamecVoxelMapAsset& MapAsset)
{
    if (!EditedChunkBaseHashes.Contains(ChunkCoord))
    {
        EditedChunkBaseHashes.Add(ChunkCoord, MapAsset.BaseChunkHashes.FindRef(ChunkCoord));
    }
}

void UNamecWorldSave::OnBeforeWrite()
{
    if (BoundMapAsset)
    {
        StampMapIdentity(*BoundMapAsset);
    }
}
