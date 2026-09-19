#include "Save/NamecCharacterSave.h"

const TCHAR* const UNamecCharacterSave::FileNamePrefix = TEXT("Character_");

FString UNamecCharacterSave::MakeFileName(const FGuid& CharacterGuid)
{
    return FString::Printf(TEXT("%s%s.sav"), FileNamePrefix, *CharacterGuid.ToString(EGuidFormats::Digits));
}
