#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "NamecBuildingPieceActor.generated.h"

class UGeometryCollectionComponent;
class UStaticMeshComponent;
class UGeometryCollection;

// Row struct for DT_Benchmark_BuildingPieces.
USTRUCT(BlueprintType)
struct NAMEC_API FNamecBuildingPieceRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UGeometryCollection> GeometryCollection;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UStaticMesh>          DestroyedMesh;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float                                MaxHealth = 100.f;
};

// Server-authoritative building piece. Health replicates; clients react in OnRep_Health.
// Fracture visuals come from the pre-authored Chaos Geometry Collection (not runtime).
// On destroy: hides the GeoCollection and shows the pre-built rubble static mesh.
UCLASS()
class NAMEC_API ANamecBuildingPieceActor : public AActor
{
    GENERATED_BODY()

public:
    ANamecBuildingPieceActor();

    virtual void BeginPlay() override;
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;
    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Set in Blueprint default properties to match the DataTable row value.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Building")
    float MaxHealth = 100.f;

private:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UGeometryCollectionComponent> GeoCollection;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> DestroyedMesh;

    UPROPERTY(ReplicatedUsing=OnRep_Health)
    float Health = 100.f;

    UFUNCTION()
    void OnRep_Health();

    void ApplyDestroyedState();
};
