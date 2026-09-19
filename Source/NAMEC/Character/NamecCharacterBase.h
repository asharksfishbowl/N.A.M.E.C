#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NamecCharacterBase.generated.h"

UCLASS(Abstract)
class NAMEC_API ANamecCharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    ANamecCharacterBase();
};
