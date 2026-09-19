#pragma once

#include "CoreMinimal.h"
#include "EditorValidatorBase.h"
#include "NamecNaniteAuthoringValidator.generated.h"

class UBlueprint;
class UStaticMesh;
class UStaticMeshComponent;

// One broken rule of engine-tech Requirement 8, and the material or component that breaks it.
struct FNamecNaniteViolation
{
    int32 RuleNumber = 0;
    FString Offender;
    FString Detail;
};

namespace NamecNaniteRules
{
    NAMECEDITOR_API bool IsNaniteMesh(const UStaticMesh* Mesh);

    // Rules 3 (material side), 4 and 6: everything a static mesh asset itself can answer.
    NAMECEDITOR_API TArray<FNamecNaniteViolation> CheckMesh(const UStaticMesh& Mesh);

    // Rules 3 (component side) and 5. WPO Disable Distance, Shadow Cache Invalidation Behavior and
    // Lighting Channels are component properties in 5.8, not mesh properties, so only a component
    // that uses a Nanite mesh can be asked.
    NAMECEDITOR_API TArray<FNamecNaniteViolation> CheckComponent(const UStaticMeshComponent& Component);

    // Every static mesh component template a Blueprint class contributes: native default
    // subobjects and the nodes of its construction script.
    NAMECEDITOR_API TArray<const UStaticMeshComponent*> GetStaticMeshComponentTemplates(const UBlueprint& Blueprint);
}

// Fails the content build for a Nanite static mesh, or a Blueprint component using one, that
// breaks rule 3, 4, 5 or 6 (Edge Case 5).
UCLASS()
class NAMECEDITOR_API UNamecNaniteAuthoringValidator : public UEditorValidatorBase
{
    GENERATED_BODY()

protected:
    virtual bool CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject,
        FDataValidationContext& InContext) const override;
    virtual EDataValidationResult ValidateLoadedAsset_Implementation(const FAssetData& InAssetData,
        UObject* InAsset, FDataValidationContext& Context) override;
};
