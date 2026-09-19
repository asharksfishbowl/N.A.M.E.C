#pragma once

#include "CoreMinimal.h"
#include "Save/NamecVersionedSave.h"
#include "NamecCharacterSave.generated.h"

UCLASS()
class NAMEC_API UNamecCharacterSave : public UNamecVersionedSave
{
    GENERATED_BODY()

public:
    UPROPERTY()
    FGuid CharacterGuid;

    UPROPERTY()
    FString CharacterName;
};
