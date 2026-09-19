#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NamecWorldLoader.generated.h"

class ANamecTerrainUpdatedMarker;
class UNamecVoxelMapAsset;
class UNamecWorldSave;

enum class ENamecWorldLoadOutcome : uint8
{
    Load,
    LoadWithRevisionUpdate,
    LoadWithMapHashMismatch,
    RefuseDifferentMap,
    RefuseNewerMapRevision,
};

struct FNamecWorldLoadDecision
{
    ENamecWorldLoadOutcome Outcome = ENamecWorldLoadOutcome::Load;

    // The exact text authored-map Requirement 25 gives for this outcome; empty when it gives none.
    FText Message;

    // Revision update only: edited chunks whose recorded base hash is not the asset's current one.
    TArray<FIntPoint> MismatchedChunks;

    bool IsRefused() const;
};

// authored-map Requirement 25, evaluated before any chunk streams. No outcome discards an edit
// delta or touches voxel data, and a refusal leaves the file untouched.
UCLASS()
class NAMEC_API UNamecWorldLoader : public UObject
{
    GENERATED_BODY()

public:
    static FNamecWorldLoadDecision EvaluateLoadRule(const UNamecWorldSave& Save, const UNamecVoxelMapAsset& MapAsset);

    // Revision update: one log line per mismatched chunk, the chunk joins PendingTerrainMarkers and
    // its recorded base hash becomes the asset's. The identity fields are rewritten by the next save.
    static void ApplyDecision(const FNamecWorldLoadDecision& Decision, UNamecWorldSave& Save, const UNamecVoxelMapAsset& MapAsset);

    // Server only. One marker at each pending chunk's centre, on every load while the entry remains.
    static TArray<ANamecTerrainUpdatedMarker*> SpawnPendingMarkers(UWorld& World, const UNamecWorldSave& Save);
};
