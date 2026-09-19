#include "Save/NamecWorldSave.h"
#include "Save/Tests/NamecSaveTestHelpers.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Core/Tests/NamecTestWorld.h"
#include "World/Map/NamecMapSettings.h"
#include "World/Map/NamecVoxelMapAsset.h"
#include "World/NamecVoxelWorld.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

namespace
{
    // An in-memory map asset with a known identity. The baked VMA_Benchmark is a 282 MB file under
    // the gitignored Content/Map/Baked/, so it is in no fresh checkout and cannot pin exact values.
    UNamecVoxelMapAsset* NewTestMapAsset()
    {
        UNamecVoxelMapAsset* MapAsset = NewObject<UNamecVoxelMapAsset>();
        MapAsset->MapId = FGuid(1, 2, 3, 4);
        MapAsset->MapRevision = 7;
        MapAsset->MapHash = TEXT("map-hash");
        MapAsset->BaseChunkHashes.Add(FIntPoint(0, 0), TEXT("chunk-0-0"));
        MapAsset->BaseChunkHashes.Add(FIntPoint(1, 0), TEXT("chunk-1-0"));
        return MapAsset;
    }

    // Points the project's shipped map asset at a test asset for one test body.
    class FNamecScopedShippedMapAsset
    {
    public:
        explicit FNamecScopedShippedMapAsset(UNamecVoxelMapAsset* MapAsset)
            : Previous(GetMutableDefault<UNamecMapSettings>()->ShippedMapAsset)
        {
            GetMutableDefault<UNamecMapSettings>()->ShippedMapAsset = MapAsset;
        }

        ~FNamecScopedShippedMapAsset()
        {
            GetMutableDefault<UNamecMapSettings>()->ShippedMapAsset = Previous;
        }

    private:
        TSoftObjectPtr<UNamecVoxelMapAsset> Previous;
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecWorldSaveMigrationTest, "Namec.Foundation.WorldSave.VersionOneLoadsAsVersionTwoWithTheShippedMapIdentity", NamecFoundationTestFlags)

bool FNamecWorldSaveMigrationTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecVoxelMapAsset* MapAsset = NewTestMapAsset();
    FNamecScopedShippedMapAsset ShippedMapAsset(MapAsset);

    UNamecWorldSave* VersionOne = NewObject<UNamecWorldSave>();
    VersionOne->WorldName = TEXT("Eastmarch");
    WriteSaveFileAtVersion(*VersionOne, 1, TEXT("World.sav"));

    const FNamecSaveLoadOutcome Outcome = NewSaveFileService()->Load(TEXT("World.sav"), UNamecWorldSave::StaticClass());
    TestEqual(TEXT("Load result"), Outcome.Result, ENamecSaveLoadResult::Ok);
    const UNamecWorldSave* Loaded = Cast<UNamecWorldSave>(Outcome.Save);
    if (TestNotNull(TEXT("Loaded world save"), Loaded))
    {
        TestEqual(TEXT("SaveVersion"), Loaded->SaveVersion, 2);
        TestEqual(TEXT("WorldName is kept"), Loaded->WorldName, FString(TEXT("Eastmarch")));
        TestEqual(TEXT("MapId"), Loaded->MapId, MapAsset->MapId);
        TestEqual(TEXT("MapRevision"), Loaded->MapRevision, 7);
        TestEqual(TEXT("MapHash"), Loaded->MapHash, FString(TEXT("map-hash")));
        TestEqual(TEXT("EditedChunkBaseHashes is empty"), Loaded->EditedChunkBaseHashes.Num(), 0);
        TestEqual(TEXT("PendingTerrainMarkers is empty"), Loaded->PendingTerrainMarkers.Num(), 0);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecWorldSaveStampTest, "Namec.Foundation.WorldSave.IdentityIsRewrittenOnEverySave", NamecFoundationTestFlags)

bool FNamecWorldSaveStampTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    UNamecVoxelMapAsset* MapAsset = NewTestMapAsset();
    UNamecSaveFileService* SaveFiles = NewSaveFileService();

    UNamecWorldSave* World = NewObject<UNamecWorldSave>();
    World->BindMapAsset(*MapAsset);
    TestEqual(TEXT("A new world is stamped at creation"), World->MapRevision, 7);

    // The asset moves on and something stale sits in the save: the next write corrects all three.
    MapAsset->MapRevision = 8;
    MapAsset->MapHash = TEXT("map-hash-8");
    World->MapId = FGuid();
    World->EditedChunkBaseHashes.Add(FIntPoint(1, 0), TEXT("chunk-1-0"));
    World->PendingTerrainMarkers.Add(FIntPoint(1, 0));
    SaveFiles->Write(*World, TEXT("World.sav"));

    const UNamecWorldSave* Loaded = Cast<UNamecWorldSave>(SaveFiles->Load(TEXT("World.sav"), UNamecWorldSave::StaticClass()).Save);
    if (TestNotNull(TEXT("Loaded world save"), Loaded))
    {
        TestEqual(TEXT("MapId"), Loaded->MapId, MapAsset->MapId);
        TestEqual(TEXT("MapRevision"), Loaded->MapRevision, 8);
        TestEqual(TEXT("MapHash"), Loaded->MapHash, FString(TEXT("map-hash-8")));
        TestEqual(TEXT("EditedChunkBaseHashes round trips"), Loaded->EditedChunkBaseHashes.FindRef(FIntPoint(1, 0)), FString(TEXT("chunk-1-0")));
        TestTrue(TEXT("PendingTerrainMarkers round trips"), Loaded->PendingTerrainMarkers.Contains(FIntPoint(1, 0)));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecWorldSaveFirstDeltaTest, "Namec.Foundation.WorldSave.FirstDeltaOfAChunkRecordsItsBaseHashOnce", NamecFoundationTestFlags)

bool FNamecWorldSaveFirstDeltaTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestWorld World;
    UNamecVoxelWorld* VoxelWorld = World->GetSubsystem<UNamecVoxelWorld>();
    if (!TestNotNull(TEXT("Voxel world subsystem"), VoxelWorld))
    {
        return true;
    }

    UNamecVoxelMapAsset* MapAsset = NewTestMapAsset();
    UNamecWorldSave* WorldSave = NewObject<UNamecWorldSave>();
    VoxelWorld->MapAssetOverride = MapAsset;
    VoxelWorld->WorldSave = WorldSave;

    FNamecVoxelDelta Delta;
    Delta.LinearIdx = 5;
    Delta.Material = 1;
    VoxelWorld->ApplyDelta(FIntPoint(1, 0), { Delta });
    TestEqual(TEXT("The first delta in a chunk adds one entry"), WorldSave->EditedChunkBaseHashes.Num(), 1);
    TestEqual(TEXT("The entry is that chunk's base hash"), WorldSave->EditedChunkBaseHashes.FindRef(FIntPoint(1, 0)), FString(TEXT("chunk-1-0")));

    // A re-bake changes the asset's hash; the save must keep what the first delta was written against.
    MapAsset->BaseChunkHashes.Add(FIntPoint(1, 0), TEXT("chunk-1-0-rebaked"));
    Delta.LinearIdx = 6;
    VoxelWorld->ApplyDelta(FIntPoint(1, 0), { Delta });
    TestEqual(TEXT("A second delta does not change it"), WorldSave->EditedChunkBaseHashes.FindRef(FIntPoint(1, 0)), FString(TEXT("chunk-1-0")));
    TestFalse(TEXT("An untouched chunk has no entry"), WorldSave->EditedChunkBaseHashes.Contains(FIntPoint(0, 0)));
    return true;
}

#endif
