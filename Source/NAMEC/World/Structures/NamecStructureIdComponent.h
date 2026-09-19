#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NamecStructureIdComponent.generated.h"

UCLASS(ClassGroup=(NAMEC), meta=(BlueprintSpawnableComponent))
class NAMEC_API UNamecStructureIdComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGuid StructureId;
};
