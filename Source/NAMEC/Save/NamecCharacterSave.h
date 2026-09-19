#pragma once

#include "CoreMinimal.h"
#include "Save/NamecVersionedSave.h"
#include "NamecCharacterSave.generated.h"

UCLASS()
class NAMEC_API UNamecCharacterSave : public UNamecVersionedSave
{
    GENERATED_BODY()

public:
    // Every character file is `Character_<guid>.sav`, which is how the character list finds them.
    static const TCHAR* const FileNamePrefix;
    static FString MakeFileName(const FGuid& CharacterGuid);

    UPROPERTY()
    FGuid CharacterGuid;

    UPROPERTY()
    FString CharacterName;
};
