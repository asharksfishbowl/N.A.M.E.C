#include "AI/NamecEnemyEquipComponent.h"
#include "MuCO/CustomizableSkeletalComponent.h"
#include "MuCO/CustomizableObjectInstance.h"
#include "Engine/DataTable.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"

UNamecEnemyEquipComponent::UNamecEnemyEquipComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.5f; // check queue at 0.5 s intervals
}

void UNamecEnemyEquipComponent::BeginPlay()
{
    Super::BeginPlay();
    if (UDataTable* DT = RulesTable.LoadSynchronous())
    {
        if (const FNamecEnemyAIRulesRow* Row = DT->FindRow<FNamecEnemyAIRulesRow>(
                FName("Default"), TEXT(""), false))
        {
            MaxConcurrent    = Row->MaxConcurrentMutableUpdates;
            DeferDistanceCm  = Row->MutableDeferDistanceMetres * 100.f;
        }
    }
}

void UNamecEnemyEquipComponent::EquipItem(FName WearableRow)
{
    PendingQueue.AddUnique(WearableRow);
    OnEquipQueued.Broadcast(WearableRow);
}

void UNamecEnemyEquipComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    DrainQueue();
}

void UNamecEnemyEquipComponent::DrainQueue()
{
    if (PendingQueue.IsEmpty() || ActiveUpdates >= MaxConcurrent) return;

    // Defer if beyond distance threshold
    if (UWorld* World = GetWorld())
    {
        const FVector OwnerLoc = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
        bool bDefer = true;
        for (FLocalPlayerIterator It(GEngine, World); It; ++It)
        {
            if (APlayerController* PC = It->GetPlayerController(World))
            {
                FVector Loc; FRotator Rot;
                PC->GetPlayerViewPoint(Loc, Rot);
                if (FVector::DistSquared(OwnerLoc, Loc) <= DeferDistanceCm * DeferDistanceCm)
                {
                    bDefer = false;
                    break;
                }
            }
        }
        if (bDefer) return;
    }

    // Sort nearest to camera (approximated by actor-camera distance to first local player)
    // For Phase 1 single-item queue; multi-item sort is wired for future work.
    FName WearableRow = PendingQueue[0];
    PendingQueue.RemoveAt(0);
    ++ActiveUpdates;

    // Locate the Mutable component on the owner
    UCustomizableSkeletalComponent* MutableComp = nullptr;
    if (AActor* Owner = GetOwner())
    {
        MutableComp = Owner->FindComponentByClass<UCustomizableSkeletalComponent>();
    }
    UCustomizableObjectInstance* Instance = MutableComp->GetCustomizableObjectInstance();
    if (!Instance) { --ActiveUpdates; return; }

    // Set wearable parameter on the instance (CO must expose an int param "WearableRow")
    Instance->SetIntParameterSelectedOption(TEXT("WearableRow"), WearableRow.ToString());

    // Kick async rebuild with native callback
    FInstanceUpdateNativeDelegate Delegate;
    Delegate.AddUObject(this, &UNamecEnemyEquipComponent::OnMutableUpdateComplete, WearableRow);
    Instance->UpdateSkeletalMeshAsyncResult(Delegate);
}

void UNamecEnemyEquipComponent::OnMutableUpdateComplete(const FUpdateContext& Result, FName WearableRow)
{
    --ActiveUpdates;
    OnEquipCompleted.Broadcast(WearableRow);
}
