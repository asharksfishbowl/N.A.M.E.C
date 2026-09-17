#include "AI/NamecEnemyCharacter.h"
#include "Net/UnrealNetwork.h"

ANamecEnemyCharacter::ANamecEnemyCharacter() {}

void ANamecEnemyCharacter::OnRep_Health() {}

void ANamecEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANamecEnemyCharacter, Health);
}
