#include "NamecNaniteAuthoringValidator.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/StaticMesh.h"
#include "MaterialShared.h"
#include "Materials/Material.h"
#include "Materials/MaterialInterface.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "NamecNaniteAuthoringValidator"

namespace
{
    // Reads the material graph's inputs, not the compiled expression cache: a freshly built or
    // in-memory material has no cache until it compiles.
    bool DrivesWorldPositionOffset(const UMaterialInterface& MaterialInterface)
    {
        const UMaterial* Material = MaterialInterface.GetMaterial();
        return Material && Material->GetEditorOnlyData() && Material->GetEditorOnlyData()->WorldPositionOffset.IsConnected();
    }

    bool UsesPixelDepthOffset(const UMaterialInterface& MaterialInterface)
    {
        const UMaterial* Material = MaterialInterface.GetMaterial();
        return Material && Material->GetEditorOnlyData() && Material->GetEditorOnlyData()->PixelDepthOffset.IsConnected();
    }

    bool MeshHasWorldPositionOffsetMaterial(const UStaticMesh& Mesh)
    {
        return Mesh.GetStaticMaterials().ContainsByPredicate([](const FStaticMaterial& Slot)
        {
            return Slot.MaterialInterface && DrivesWorldPositionOffset(*Slot.MaterialInterface);
        });
    }
}

bool NamecNaniteRules::IsNaniteMesh(const UStaticMesh* Mesh)
{
    return Mesh && Mesh->GetNaniteSettings().bEnabled;
}

TArray<FNamecNaniteViolation> NamecNaniteRules::CheckMesh(const UStaticMesh& Mesh)
{
    TArray<FNamecNaniteViolation> Violations;

    if (Mesh.GetNaniteSettings().DisplacementMaps.Num() > 0)
    {
        Violations.Add({ 6, Mesh.GetName(), TEXT("the mesh has Nanite displacement maps") });
    }

    for (const FStaticMaterial& Slot : Mesh.GetStaticMaterials())
    {
        const UMaterialInterface* Material = Slot.MaterialInterface;
        if (!Material)
        {
            continue;
        }

        if (IsTranslucentBlendMode(Material->GetBlendMode()))
        {
            Violations.Add({ 4, Material->GetName(), TEXT("the material uses a translucent blend mode") });
        }
        if (Material->IsTessellationEnabled())
        {
            Violations.Add({ 6, Material->GetName(), TEXT("the material enables Nanite tessellation") });
        }
        if (DrivesWorldPositionOffset(*Material) && Material->GetMaxWorldPositionOffsetDisplacement() <= 0.f)
        {
            Violations.Add({ 3, Material->GetName(), TEXT("the material drives World Position Offset with no Max WPO Displacement") });
        }
        if (UsesPixelDepthOffset(*Material))
        {
            Violations.Add({ 3, Material->GetName(), TEXT("the material uses Pixel Depth Offset") });
        }
    }
    return Violations;
}

TArray<FNamecNaniteViolation> NamecNaniteRules::CheckComponent(const UStaticMeshComponent& Component)
{
    TArray<FNamecNaniteViolation> Violations;
    const UStaticMesh* Mesh = Component.GetStaticMesh();
    if (!IsNaniteMesh(Mesh))
    {
        return Violations;
    }

    const FLightingChannels DefaultChannels;
    if (GetLightingChannelMaskForStruct(Component.LightingChannels) != GetLightingChannelMaskForStruct(DefaultChannels))
    {
        Violations.Add({ 5, Component.GetName(), TEXT("the component uses Lighting Channels other than the default") });
    }

    if (MeshHasWorldPositionOffsetMaterial(*Mesh))
    {
        if (Component.WorldPositionOffsetDisableDistance <= 0)
        {
            Violations.Add({ 3, Component.GetName(), TEXT("the component sets no WPO Disable Distance") });
        }
        if (Component.ShadowCacheInvalidationBehavior == EShadowCacheInvalidationBehavior::Auto)
        {
            Violations.Add({ 3, Component.GetName(), TEXT("the component leaves Shadow Cache Invalidation Behavior on Auto") });
        }
    }
    return Violations;
}

TArray<const UStaticMeshComponent*> NamecNaniteRules::GetStaticMeshComponentTemplates(const UBlueprint& Blueprint)
{
    TArray<const UStaticMeshComponent*> Templates;
    const UBlueprintGeneratedClass* GeneratedClass = Cast<UBlueprintGeneratedClass>(Blueprint.GeneratedClass);
    if (!GeneratedClass)
    {
        return Templates;
    }

    if (const AActor* DefaultActor = Cast<AActor>(GeneratedClass->GetDefaultObject(false)))
    {
        TInlineComponentArray<UStaticMeshComponent*> NativeComponents(DefaultActor);
        Templates.Append(NativeComponents);
    }
    if (GeneratedClass->SimpleConstructionScript)
    {
        for (const USCS_Node* Node : GeneratedClass->SimpleConstructionScript->GetAllNodes())
        {
            if (const UStaticMeshComponent* Template = Cast<UStaticMeshComponent>(Node->ComponentTemplate))
            {
                Templates.Add(Template);
            }
        }
    }
    return Templates;
}

bool UNamecNaniteAuthoringValidator::CanValidateAsset_Implementation(const FAssetData& InAssetData, UObject* InObject, FDataValidationContext& InContext) const
{
    if (const UStaticMesh* Mesh = Cast<UStaticMesh>(InObject))
    {
        return NamecNaniteRules::IsNaniteMesh(Mesh);
    }
    if (const UBlueprint* Blueprint = Cast<UBlueprint>(InObject))
    {
        return NamecNaniteRules::GetStaticMeshComponentTemplates(*Blueprint).ContainsByPredicate([](const UStaticMeshComponent* Template)
        {
            return NamecNaniteRules::IsNaniteMesh(Template->GetStaticMesh());
        });
    }
    return false;
}

EDataValidationResult UNamecNaniteAuthoringValidator::ValidateLoadedAsset_Implementation(const FAssetData& InAssetData, UObject* InAsset, FDataValidationContext& Context)
{
    TArray<FNamecNaniteViolation> Violations;
    if (const UStaticMesh* Mesh = Cast<UStaticMesh>(InAsset))
    {
        Violations = NamecNaniteRules::CheckMesh(*Mesh);
    }
    else if (const UBlueprint* Blueprint = Cast<UBlueprint>(InAsset))
    {
        for (const UStaticMeshComponent* Template : NamecNaniteRules::GetStaticMeshComponentTemplates(*Blueprint))
        {
            Violations.Append(NamecNaniteRules::CheckComponent(*Template));
        }
    }

    for (const FNamecNaniteViolation& Violation : Violations)
    {
        Context.AddError(FText::Format(
            LOCTEXT("Violation", "{0}: Nanite authoring rule {1} is broken by '{2}': {3} (engine-tech Requirement 8)"),
            FText::FromName(InAssetData.AssetName), FText::AsNumber(Violation.RuleNumber), FText::FromString(Violation.Offender), FText::FromString(Violation.Detail)));
    }
    return Violations.IsEmpty() ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}

#undef LOCTEXT_NAMESPACE
