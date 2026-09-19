#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NamecTreeActor.generated.h"

class UStaticMeshComponent;
class ANamecItemDropActor;

// Server-authoritative tree. Health replicates; clients react in OnRep_Health.
// On health reaching zero: upright mesh hides, fallen mesh shows, log item drop spawns.
// Spawned by the PCG graph (PCG_TreeScatter). Set UprightMesh and FallenMesh in
// Blueprint defaults using Nanite-enabled meshes from Content/Benchmark/Nature/.
UCLASS()
class NAMEC_API ANamecTreeActor : public AActor
{
    GENERATED_BODY()

public:
    ANamecTreeActor();

    virtual void BeginPlay() override;
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tree")
    float MaxHealth = 100.f;

    // Blueprint-assigned fallen log drop actor class.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Tree")
    TSubclassOf<ANamecItemDropActor> ItemDropClass;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> UprightMesh;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> FallenMesh;

    UPROPERTY(ReplicatedUsing=OnRep_Health)
    float Health = 100.f;

    UFUNCTION()
    void OnRep_Health();

    void ApplyFelledState();

    bool bFelled = false;
};
