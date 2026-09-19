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
    RefuseNoMapIdentity,
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

    // A refusal changes nothing in the save, so its next write cannot stamp the wrong identity on
    // it. An accepted save is bound to the asset here and nowhere earlier. A revision update also
    // logs each mismatched chunk, marks it pending and brings its recorded hash up to the asset's.
    // Evaluates the save exactly as loaded: a save bound to the asset first would carry the asset's
    // identity and pass every comparison.
    static void ApplyDecision(const FNamecWorldLoadDecision& Decision, UNamecWorldSave& Save, const UNamecVoxelMapAsset& MapAsset);

    // Server only. One marker at each pending chunk's centre, on every load while the entry remains.
    static TArray<ANamecTerrainUpdatedMarker*> SpawnPendingMarkers(UWorld& World, const UNamecWorldSave& Save);
};
