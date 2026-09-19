#pragma once

#include "CoreMinimal.h"
#include "Character/NamecCharacterBase.h"
#include "MuCO/CustomizableSkeletalComponent.h"
#include "NamecEnemyCharacter.generated.h"

class UStateTreeComponent;

UCLASS()
class NAMEC_API ANamecEnemyCharacter : public ANamecCharacterBase
{
    GENERATED_BODY()

public:
    ANamecEnemyCharacter();

    UPROPERTY(VisibleAnywhere) TObjectPtr<UStateTreeComponent>         StateTree;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UCustomizableSkeletalComponent> MutableBody;

    UPROPERTY(ReplicatedUsing = OnRep_Health)
    float Health = 100.f;

    UFUNCTION()
    void OnRep_Health();
};
