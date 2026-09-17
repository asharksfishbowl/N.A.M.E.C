#pragma once

#include "CoreMinimal.h"
#include "Character/NamecCharacterBase.h"
#include "NamecEnemyCharacter.generated.h"

UCLASS()
class NAMEC_API ANamecEnemyCharacter : public ANamecCharacterBase
{
    GENERATED_BODY()

public:
    ANamecEnemyCharacter();

    UPROPERTY(ReplicatedUsing = OnRep_Health)
    float Health = 100.f;

    UFUNCTION()
    void OnRep_Health();
};
