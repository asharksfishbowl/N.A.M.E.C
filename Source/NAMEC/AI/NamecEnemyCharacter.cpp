#include "AI/NamecEnemyCharacter.h"
#include "AI/NamecAIController.h"
#include "Components/StateTreeComponent.h"
#include "Net/UnrealNetwork.h"

ANamecEnemyCharacter::ANamecEnemyCharacter()
{
    StateTree = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTree"));
    // bStartLogicAutomatically defaults to true; AIController calls StartLogic explicitly on possess.
    StateTree->SetStartLogicAutomatically(false);

    MutableBody = CreateDefaultSubobject<UCustomizableSkeletalComponent>(TEXT("MutableBody"));

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AIControllerClass = ANamecAIController::StaticClass();
}

void ANamecEnemyCharacter::OnRep_Health() {}

void ANamecEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ANamecEnemyCharacter, Health);
}
