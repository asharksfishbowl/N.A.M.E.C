#include "EnemyAI/NamecAIController.h"
#include "EnemyAI/NamecEnemyCharacter.h"
#include "Components/StateTreeComponent.h"

ANamecAIController::ANamecAIController() {}

void ANamecAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    if (ANamecEnemyCharacter* Enemy = Cast<ANamecEnemyCharacter>(InPawn))
    {
        if (Enemy->StateTree)
        {
            Enemy->StateTree->StartLogic();
        }
    }
}
