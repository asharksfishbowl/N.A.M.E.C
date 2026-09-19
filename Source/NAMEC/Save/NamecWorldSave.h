#pragma once

#include "CoreMinimal.h"
#include "Save/NamecVersionedSave.h"
#include "NamecWorldSave.generated.h"

class UNamecVoxelMapAsset;

USTRUCT()
struct NAMEC_API FNamecWorldSettings
{
    GENERATED_BODY()

    UPROPERTY()
    bool bFriendlyFire = false;

    UPROPERTY()
    bool bLanHosting = true;

    UPROPERTY()
    FString Password;

    UPROPERTY()
    bool bRaids = true;
};

UCLASS()
class NAMEC_API UNamecWorldSave : public UNamecVersionedSave
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FString WorldName;

    UPROPERTY()
    float VoxelResolutionCm = 25.f;

    UPROPERTY()
    FNamecWorldSettings Settings;

    // Map identity (authored-map Requirement 24), copied from the map asset and rewritten on every save.
    UPROPERTY()
    FGuid MapId;

    UPROPERTY()
    int32 MapRevision = 0;

    UPROPERTY()
    FString MapHash;

    // For each chunk with at least one edit delta: the asset's base hash when its first delta was
    // written. A chunk with no delta has no entry.
    UPROPERTY()
    TMap<FIntPoint, FString> EditedChunkBaseHashes;

    UPROPERTY()
    TArray<FIntPoint> PendingTerrainMarkers;

    // Stamps the identity now and again before every write, until another asset is bound.
    void BindMapAsset(const UNamecVoxelMapAsset& MapAsset);

    void StampMapIdentity(const UNamecVoxelMapAsset& MapAsset);

    // Adds the chunk's entry only when it has none: a later delta never changes it.
    void RecordFirstEditOfChunk(FIntPoint ChunkCoord, const UNamecVoxelMapAsset& MapAsset);

    virtual TConstArrayView<FNamecSaveMigration> GetMigrations() const override;
    virtual void OnBeforeWrite() override;

private:
    UPROPERTY(Transient)
    TObjectPtr<const UNamecVoxelMapAsset> BoundMapAsset;
};
