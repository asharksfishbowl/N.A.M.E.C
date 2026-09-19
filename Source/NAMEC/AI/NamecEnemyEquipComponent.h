#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "NamecEnemyEquipComponent.generated.h"

class UCustomizableSkeletalComponent;
class UCustomizableObjectInstance;
struct FUpdateContext;

USTRUCT(BlueprintType)
struct FNamecEnemyAIRulesRow : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) int32 MaxConcurrentMutableUpdates   = 2;
    UPROPERTY(EditAnywhere) float MutableDeferDistanceMetres    = 50.f;
};

USTRUCT(BlueprintType)
struct FNamecBenchmarkWearableRow : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere) int32                       BodyVariant  = 0; // 0=HumanMale, 1=HumanFemale
    UPROPERTY(EditAnywhere) TSoftObjectPtr<USkeletalMesh> MeshVariant;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipQueued,    FName /*WearableRow*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquipCompleted, FName /*WearableRow*/);

// Manages asynchronous Mutable rebuild for wearable gear equip.
// - Queues are drained at most MaxConcurrentMutableUpdates rebuilds at once.
// - Enemies beyond MutableDeferDistanceMetres from the nearest local camera are deferred.
// - Queue is sorted nearest-camera-first; player rebuilds always precede enemy rebuilds
//   (player component calls UpdateSkeletalMeshAsync directly with bForceHighPriority=true).
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NAMEC_API UNamecEnemyEquipComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UNamecEnemyEquipComponent();

    // Rules DataTable; assigned from ANamecEnemyCharacter constructor or Blueprint.
    UPROPERTY(EditAnywhere) TSoftObjectPtr<UDataTable> RulesTable;

    FOnEquipQueued    OnEquipQueued;
    FOnEquipCompleted OnEquipCompleted;

    // Queue a wearable row equip. Silently replaces any pending request for the same slot.
    void EquipItem(FName WearableRow);

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

private:
    void DrainQueue();
    void OnMutableUpdateComplete(const FUpdateContext& Result, FName WearableRow);

    TArray<FName> PendingQueue;
    int32         ActiveUpdates = 0;
    int32         MaxConcurrent = 2;      // refreshed from DT_EnemyAI_Rules on BeginPlay
    float         DeferDistanceCm = 5000.f; // 50 m
};
