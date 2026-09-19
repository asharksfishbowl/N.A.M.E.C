#include "World/NamecTreeActor.h"
#include "World/NamecItemDropActor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ANamecTreeActor::ANamecTreeActor()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    UprightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UprightMesh"));
    SetRootComponent(UprightMesh);

    FallenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallenMesh"));
    FallenMesh->SetupAttachment(RootComponent);
    FallenMesh->SetVisibility(false);
    FallenMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ANamecTreeActor::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
        Health = MaxHealth;
}

void ANamecTreeActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANamecTreeActor, Health);
}

float ANamecTreeActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (!HasAuthority()) return 0.f;
    Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Health = FMath::Max(0.f, Health - DamageAmount);
    if (Health <= 0.f)
        ApplyFelledState();
    return DamageAmount;
}

void ANamecTreeActor::OnRep_Health()
{
    if (Health <= 0.f)
        ApplyFelledState();
}

void ANamecTreeActor::ApplyFelledState()
{
    if (bFelled) return;
    bFelled = true;

    UprightMesh->SetVisibility(false);
    UprightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    FallenMesh->SetVisibility(true);
    FallenMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // Spawn a log item drop at this actor's base position (server-only).
    if (HasAuthority() && ItemDropClass)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        GetWorld()->SpawnActor<ANamecItemDropActor>(ItemDropClass, GetActorLocation(), FRotator::ZeroRotator, Params);
    }
}
