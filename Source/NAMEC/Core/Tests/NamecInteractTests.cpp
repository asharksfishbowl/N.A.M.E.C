#include "Core/NamecInteractable.h"
#include "Core/Platform/Tests/NamecTestPlatform.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Core/Tests/NamecTestWorld.h"
#include "Character/NamecPlayerCharacter.h"
#include "Save/NamecAutosaveSubsystem.h"
#include "Save/NamecWorldLoader.h"
#include "Save/NamecWorldSave.h"
#include "Save/Tests/NamecSaveTestHelpers.h"
#include "UI/NamecUILayerSubsystem.h"
#include "UI/NamecUIRootLayout.h"
#include "UI/Tests/NamecUITestHelpers.h"
#include "World/NamecVoxelWorld.h"
#include "World/Structures/NamecTerrainUpdatedMarker.h"
#include "Engine/LocalPlayer.h"

#if WITH_DEV_AUTOMATION_TESTS

using namespace NamecSaveTestHelpers;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecInteractMarkerTest, "Namec.Foundation.Interact.MarkerClearsItsChunkAndDespawnsOnce", NamecFoundationTestFlags)

bool FNamecInteractMarkerTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestWorld World;
    UNamecVoxelWorld* VoxelWorld = World->GetSubsystem<UNamecVoxelWorld>();
    ANamecPlayerCharacter* Player = World->SpawnActor<ANamecPlayerCharacter>();
    ANamecPlayerCharacter* OtherPlayer = World->SpawnActor<ANamecPlayerCharacter>();
    if (!TestNotNull(TEXT("Voxel world"), VoxelWorld) || !TestNotNull(TEXT("Player"), Player) || !TestNotNull(TEXT("Other player"), OtherPlayer))
    {
        return true;
    }

    UNamecWorldSave* WorldSave = NewObject<UNamecWorldSave>();
    WorldSave->PendingTerrainMarkers = { FIntPoint(0, 0), FIntPoint(2, 3) };
    VoxelWorld->WorldSave = WorldSave;
    const TArray<ANamecTerrainUpdatedMarker*> Markers = UNamecWorldLoader::SpawnPendingMarkers(*World.Get(), *WorldSave);
    if (!TestEqual(TEXT("Two markers"), Markers.Num(), 2))
    {
        return true;
    }

    // The server-side handler is called directly: a test world never begins play, so the engine
    // absorbs a server RPC instead of running it. The RPC transport itself is the engine's.
    Player->ServerInteract_Implementation(OtherPlayer);
    TestEqual(TEXT("A target without the interface is ignored"), WorldSave->PendingTerrainMarkers.Num(), 2);

    // Two players, the same marker, the same frame (Edge Case 8).
    Player->ServerInteract_Implementation(Markers[1]);
    OtherPlayer->ServerInteract_Implementation(Markers[1]);
    TestTrue(TEXT("The marker is despawned"), Markers[1]->IsActorBeingDestroyed());
    TestFalse(TEXT("Its chunk is no longer pending"), WorldSave->PendingTerrainMarkers.Contains(FIntPoint(2, 3)));
    TestTrue(TEXT("The other chunk still is"), WorldSave->PendingTerrainMarkers.Contains(FIntPoint(0, 0)));
    TestEqual(TEXT("The second interaction removed nothing more"), WorldSave->PendingTerrainMarkers.Num(), 1);
    TestFalse(TEXT("The other marker is untouched"), Markers[0]->IsActorBeingDestroyed());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecInteractLoadNoticeTest, "Namec.Foundation.Interact.RevisionNoticeShowsOnPlayerOneOnly", NamecFoundationTestFlags)

bool FNamecInteractLoadNoticeTest::RunTest(const FString& Parameters)
{
    FNamecScopedTestPlatform Platform;
    FNamecScopedTestWorld World;
    UGameInstance* GameInstance = NewGameInstance();
    UNamecSaveFileService* SaveFiles = NewObject<UNamecSaveFileService>(GameInstance);
    UNamecAutosaveSubsystem* Autosave = NewObject<UNamecAutosaveSubsystem>(GameInstance);

    UNamecUILayerSubsystem* PlayerOneLayer = NewObject<UNamecUILayerSubsystem>(NewObject<ULocalPlayer>(GEngine));
    UNamecUILayerSubsystem* PlayerTwoLayer = NewObject<UNamecUILayerSubsystem>(NewObject<ULocalPlayer>(GEngine));
    UNamecUIRootLayout* PlayerOneRoot = PlayerOneLayer->CreateRootLayout(*World.Get(), nullptr, 1, *SaveFiles, *Autosave);
    UNamecUIRootLayout* PlayerTwoRoot = PlayerTwoLayer->CreateRootLayout(*World.Get(), nullptr, 2, *SaveFiles, *Autosave);
    const TSharedRef<SWidget> PlayerOneSlate = NamecUITestHelpers::BuildSlate(*PlayerOneRoot);
    const TSharedRef<SWidget> PlayerTwoSlate = NamecUITestHelpers::BuildSlate(*PlayerTwoRoot);

    FNamecWorldLoadDecision HashMismatch;
    HashMismatch.Outcome = ENamecWorldLoadOutcome::LoadWithMapHashMismatch;
    PlayerOneLayer->ShowWorldLoadNotice(HashMismatch);
    TestTrue(TEXT("An outcome with no message shows nothing"), PlayerOneRoot->GetHudNotice().IsEmpty());

    FNamecWorldLoadDecision RevisionUpdate;
    RevisionUpdate.Outcome = ENamecWorldLoadOutcome::LoadWithRevisionUpdate;
    RevisionUpdate.Message = FText::FromString(TEXT("World updated to map revision 12"));
    for (UNamecUILayerSubsystem* Layer : { PlayerOneLayer, PlayerTwoLayer })
    {
        Layer->ShowWorldLoadNotice(RevisionUpdate);
    }
    TestEqual(TEXT("The notice appears on player 1's HUD layer"), PlayerOneRoot->GetHudNotice().ToString(), FString(TEXT("World updated to map revision 12")));
    TestTrue(TEXT("And not on player 2's"), PlayerTwoRoot->GetHudNotice().IsEmpty());
    return true;
}

#endif
