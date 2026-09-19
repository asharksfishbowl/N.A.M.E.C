#include "Save/NamecWorldLoader.h"
#include "Save/NamecWorldSave.h"
#include "Save/Tests/NamecSaveTestHelpers.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Core/Tests/NamecTestWorld.h"
#include "World/Map/NamecVoxelMapAsset.h"
#include "World/NamecVoxelTypes.h"
#include "World/Structures/NamecTerrainUpdatedMarker.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

namespace
{
    UNamecVoxelMapAsset* NewLoaderTestMapAsset()
    {
        UNamecVoxelMapAsset* MapAsset = NewObject<UNamecVoxelMapAsset>();
        MapAsset->MapId = FGuid(9, 8, 7, 6);
        MapAsset->MapRevision = 12;
        MapAsset->MapHash = TEXT("hash-12");
        MapAsset->BaseChunkHashes.Add(FIntPoint(0, 0), TEXT("a"));
        MapAsset->BaseChunkHashes.Add(FIntPoint(1, 0), TEXT("b-changed"));
        MapAsset->BaseChunkHashes.Add(FIntPoint(2, 3), TEXT("c-changed"));
        return MapAsset;
    }

    // A save made against MapAsset as it is now, so every rule starts from "otherwise: load".
    UNamecWorldSave* NewMatchingWorldSave(const UNamecVoxelMapAsset& MapAsset)
    {
        UNamecWorldSave* Save = NewObject<UNamecWorldSave>();
        Save->WorldName = TEXT("Eastmarch");
        Save->StampMapIdentity(MapAsset);
        return Save;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecWorldLoaderOutcomesTest, "Namec.Foundation.WorldLoader.EveryOutcomeAndItsExactMessage", NamecFoundationTestFlags)

bool FNamecWorldLoaderOutcomesTest::RunTest(const FString& Parameters)
{
    const UNamecVoxelMapAsset* MapAsset = NewLoaderTestMapAsset();

    UNamecWorldSave* Save = NewMatchingWorldSave(*MapAsset);
    FNamecWorldLoadDecision Decision = UNamecWorldLoader::EvaluateLoadRule(*Save, *MapAsset);
    TestEqual(TEXT("Matching save"), Decision.Outcome, ENamecWorldLoadOutcome::Load);
    TestTrue(TEXT("Matching save has no message"), Decision.Message.IsEmpty());

    Save->MapId = FGuid(1, 1, 1, 1);
    Save->MapRevision = 99;
    Decision = UNamecWorldLoader::EvaluateLoadRule(*Save, *MapAsset);
    TestEqual(TEXT("Different MapId wins over every other field"), Decision.Outcome, ENamecWorldLoadOutcome::RefuseDifferentMap);
    TestEqual(TEXT("Different map message"), Decision.Message.ToString(), FString(TEXT("This world belongs to a different map")));
    TestTrue(TEXT("Different map is a refusal"), Decision.IsRefused());

    Save = NewMatchingWorldSave(*MapAsset);
    Save->MapRevision = 13;
    Decision = UNamecWorldLoader::EvaluateLoadRule(*Save, *MapAsset);
    TestEqual(TEXT("Newer revision"), Decision.Outcome, ENamecWorldLoadOutcome::RefuseNewerMapRevision);
    TestEqual(TEXT("Newer revision message"), Decision.Message.ToString(), FString(TEXT("This world was saved with a newer version of the map")));
    TestTrue(TEXT("Newer revision is a refusal"), Decision.IsRefused());

    Save = NewMatchingWorldSave(*MapAsset);
    Save->MapHash = TEXT("hash-from-a-local-rebake");
    Decision = UNamecWorldLoader::EvaluateLoadRule(*Save, *MapAsset);
    TestEqual(TEXT("Same revision, different hash"), Decision.Outcome, ENamecWorldLoadOutcome::LoadWithMapHashMismatch);
    TestTrue(TEXT("Hash mismatch has no message"), Decision.Message.IsEmpty());
    TestEqual(TEXT("Hash mismatch flags no chunks"), Decision.MismatchedChunks.Num(), 0);
    TestFalse(TEXT("Hash mismatch loads"), Decision.IsRefused());
    AddExpectedMessage(TEXT("MapHash differs from the map asset"), ELogVerbosity::Warning, EAutomationExpectedMessageFlags::Contains, 1);
    UNamecWorldLoader::ApplyDecision(Decision, *Save, *MapAsset);
    TestEqual(TEXT("Hash mismatch marks nothing"), Save->PendingTerrainMarkers.Num(), 0);

    Save = NewMatchingWorldSave(*MapAsset);
    Save->MapRevision = 11;
    Decision = UNamecWorldLoader::EvaluateLoadRule(*Save, *MapAsset);
    TestEqual(TEXT("Older revision"), Decision.Outcome, ENamecWorldLoadOutcome::LoadWithRevisionUpdate);
    TestEqual(TEXT("Older revision message"), Decision.Message.ToString(), FString(TEXT("World updated to map revision 12")));
    TestFalse(TEXT("Older revision loads"), Decision.IsRefused());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecWorldLoaderRevisionTest, "Namec.Foundation.WorldLoader.RevisionUpdateMarksExactlyTheMismatchedChunks", NamecFoundationTestFlags)

bool FNamecWorldLoaderRevisionTest::RunTest(const FString& Parameters)
{
    const UNamecVoxelMapAsset* MapAsset = NewLoaderTestMapAsset();
    UNamecWorldSave* Save = NewMatchingWorldSave(*MapAsset);
    Save->MapRevision = 11;
    Save->EditedChunkBaseHashes.Add(FIntPoint(0, 0), TEXT("a"));
    Save->EditedChunkBaseHashes.Add(FIntPoint(1, 0), TEXT("b"));
    Save->EditedChunkBaseHashes.Add(FIntPoint(2, 3), TEXT("c"));

    const FNamecWorldLoadDecision Decision = UNamecWorldLoader::EvaluateLoadRule(*Save, *MapAsset);
    TestEqual(TEXT("Evaluating changes nothing in the save"), Save->PendingTerrainMarkers.Num(), 0);
    TestEqual(TEXT("Two of the three edited chunks changed"), Decision.MismatchedChunks.Num(), 2);

    UNamecWorldLoader::ApplyDecision(Decision, *Save, *MapAsset);
    TestEqual(TEXT("Exactly the mismatched chunks are pending"), Save->PendingTerrainMarkers.Num(), 2);
    TestTrue(TEXT("(1, 0) is pending"), Save->PendingTerrainMarkers.Contains(FIntPoint(1, 0)));
    TestTrue(TEXT("(2, 3) is pending"), Save->PendingTerrainMarkers.Contains(FIntPoint(2, 3)));
    TestFalse(TEXT("The unchanged chunk is not"), Save->PendingTerrainMarkers.Contains(FIntPoint(0, 0)));
    TestEqual(TEXT("A marked chunk's recorded hash becomes the asset's"), Save->EditedChunkBaseHashes.FindRef(FIntPoint(1, 0)), FString(TEXT("b-changed")));
    TestEqual(TEXT("No edited chunk loses its entry"), Save->EditedChunkBaseHashes.Num(), 3);

    UNamecWorldLoader::ApplyDecision(Decision, *Save, *MapAsset);
    TestEqual(TEXT("Applying twice adds no duplicate"), Save->PendingTerrainMarkers.Num(), 2);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecWorldLoaderRefusalTest, "Namec.Foundation.WorldLoader.ARefusedSaveIsUntouchedOnDisk", NamecFoundationTestFlags)

bool FNamecWorldLoaderRefusalTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    const UNamecVoxelMapAsset* MapAsset = NewLoaderTestMapAsset();
    UNamecSaveFileService* SaveFiles = NewSaveFileService();

    UNamecWorldSave* OtherMap = NewMatchingWorldSave(*MapAsset);
    OtherMap->MapId = FGuid(4, 4, 4, 4);
    SaveFiles->Write(*OtherMap, TEXT("World.sav"));
    const TArray<uint8> Before = ReadFileBytes(UNamecSaveFileService::GetSaveFilePath(TEXT("World.sav")));

    UNamecWorldSave* Loaded = Cast<UNamecWorldSave>(SaveFiles->Load(TEXT("World.sav"), UNamecWorldSave::StaticClass()).Save);
    if (!TestNotNull(TEXT("Loaded world save"), Loaded))
    {
        return true;
    }
    const FNamecWorldLoadDecision Decision = UNamecWorldLoader::EvaluateLoadRule(*Loaded, *MapAsset);
    UNamecWorldLoader::ApplyDecision(Decision, *Loaded, *MapAsset);

    TestTrue(TEXT("The load is refused"), Decision.IsRefused());
    TestEqual(TEXT("A refusal marks nothing"), Loaded->PendingTerrainMarkers.Num(), 0);
    TestTrue(TEXT("File bytes are identical afterwards"), ReadFileBytes(UNamecSaveFileService::GetSaveFilePath(TEXT("World.sav"))) == Before);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecWorldLoaderMarkersTest, "Namec.Foundation.WorldLoader.PendingChunksSpawnMarkersAtTheirCentresOnEveryLoad", NamecFoundationTestFlags)

bool FNamecWorldLoaderMarkersTest::RunTest(const FString& Parameters)
{
    UNamecWorldSave* Save = NewObject<UNamecWorldSave>();
    Save->VoxelResolutionCm = 25.f;
    Save->PendingTerrainMarkers = { FIntPoint(0, 0), FIntPoint(2, 3) };
    const double ChunkSizeCm = NamecChunkH * 25.0;

    for (int32 LoadNumber = 1; LoadNumber <= 2; ++LoadNumber)
    {
        FNamecScopedTestWorld World;
        const TArray<ANamecTerrainUpdatedMarker*> Markers = UNamecWorldLoader::SpawnPendingMarkers(*World.Get(), *Save);
        if (!TestEqual(FString::Printf(TEXT("Load %d spawns two markers"), LoadNumber), Markers.Num(), 2))
        {
            continue;
        }
        TestEqual(TEXT("First marker chunk"), Markers[0]->ChunkCoord, FIntPoint(0, 0));
        TestEqual(TEXT("First marker position"), Markers[0]->GetActorLocation(), FVector(0.5 * ChunkSizeCm, 0.5 * ChunkSizeCm, 0.0));
        TestEqual(TEXT("Second marker chunk"), Markers[1]->ChunkCoord, FIntPoint(2, 3));
        TestEqual(TEXT("Second marker position"), Markers[1]->GetActorLocation(), FVector(2.5 * ChunkSizeCm, 3.5 * ChunkSizeCm, 0.0));
        TestTrue(TEXT("A marker replicates to every player"), Markers[0]->GetIsReplicated() && Markers[0]->bAlwaysRelevant);
    }
    TestEqual(TEXT("Spawning clears no entry"), Save->PendingTerrainMarkers.Num(), 2);
    return true;
}

#endif
