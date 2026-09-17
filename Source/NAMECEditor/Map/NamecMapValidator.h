#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "NamecMapValidator.generated.h"

UCLASS()
class NAMECEDITOR_API UNamecMapValidator : public UEditorValidatorBase
{
    GENERATED_BODY()

public:
    virtual bool CanValidateAsset_Implementation(const FAssetData& AssetData) const override;
    virtual EDataValidationResult ValidateLoadedAsset_Implementation(
        const FAssetData& AssetData, UObject* InAsset, TArray<FText>& ValidationErrors) override;
};
