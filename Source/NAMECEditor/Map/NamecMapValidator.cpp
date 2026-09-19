#include "Map/NamecMapValidator.h"

bool UNamecMapValidator::CanValidateAsset_Implementation(const FAssetData& InAssetData,
    UObject* InObject, FDataValidationContext& InContext) const
{
    return false;
}

EDataValidationResult UNamecMapValidator::ValidateLoadedAsset_Implementation(
    const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
    return EDataValidationResult::NotValidated;
}
