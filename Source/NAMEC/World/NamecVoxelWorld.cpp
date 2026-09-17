#include "World/NamecVoxelWorld.h"
#include "World/Map/NamecVoxelMapAsset.h"

void UNamecVoxelWorld::AddOrUpdateChunk(FIntPoint ChunkCoord) {}

void UNamecVoxelWorld::ApplyTerrainEdit(FVector CentreMetres, float RadiusMetres,
    ENamecEditMode Mode, FName MaterialRow, int32 ToolTier) {}

bool UNamecVoxelWorld::IsWithinDigDepth(FVector PositionMetres) const
{
    return false;
}

void UNamecVoxelWorld::Load(UNamecVoxelMapAsset* MapAsset)
{
    MapAssetOverride = MapAsset;
}
