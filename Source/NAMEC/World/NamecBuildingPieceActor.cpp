#include "World/NamecBuildingPieceActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ANamecBuildingPieceActor::ANamecBuildingPieceActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    GeoCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeoCollection"));
    SetRootComponent(GeoCollection);

    DestroyedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestroyedMesh"));
    DestroyedMesh->SetupAttachment(RootComponent);
    DestroyedMesh->SetVisibility(false);
    DestroyedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ANamecBuildingPieceActor::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
        Health = MaxHealth;
}

void ANamecBuildingPieceActor::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANamecBuildingPieceActor, Health);
}

float ANamecBuildingPieceActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (!HasAuthority()) return 0.f;
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Health = FMath::Max(0.f, Health - DamageAmount);
    if (Health <= 0.f)
        ApplyDestroyedState();
    return DamageAmount;
}

void ANamecBuildingPieceActor::OnRep_Health()
{
    if (Health <= 0.f)
        ApplyDestroyedState();
}

void ANamecBuildingPieceActor::ApplyDestroyedState()
{
    if (bDestroyed) return;
    bDestroyed = true;
    GeoCollection->SetVisibility(false);
    GeoCollection->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DestroyedMesh->SetVisibility(true);
    DestroyedMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
