#include "NamecNaniteAuthoringValidator.h"
#include "Core/Tests/NamecTestFlags.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant3Vector.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
    UStaticMesh* NewFixtureMesh(bool bNanite, UMaterial* Material)
    {
        UStaticMesh* Mesh = NewObject<UStaticMesh>();
        Mesh->GetNaniteSettings().bEnabled = bNanite;
        Mesh->SetStaticMaterials({ FStaticMaterial(Material) });
        return Mesh;
    }

    UMaterial* NewWorldPositionOffsetMaterial(float MaxDisplacement)
    {
        UMaterial* Material = NewObject<UMaterial>();
        Material->GetEditorOnlyData()->WorldPositionOffset.Expression = NewObject<UMaterialExpressionConstant3Vector>(Material);
        Material->MaxWorldPositionOffsetDisplacement = MaxDisplacement;
        return Material;
    }

    UStaticMeshComponent* NewFixtureComponent(UStaticMesh* Mesh)
    {
        UStaticMeshComponent* Component = NewObject<UStaticMeshComponent>();
        Component->SetStaticMesh(Mesh);
        return Component;
    }

    bool HasViolationOfRule(const TArray<FNamecNaniteViolation>& Violations, int32 RuleNumber)
    {
        return Violations.ContainsByPredicate([RuleNumber](const FNamecNaniteViolation& Violation) { return Violation.RuleNumber == RuleNumber; });
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecNaniteValidatorMeshTest, "Namec.Foundation.NaniteValidator.MeshRules", NamecFoundationTestFlags)

bool FNamecNaniteValidatorMeshTest::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("A clean Nanite mesh passes"), NamecNaniteRules::CheckMesh(*NewFixtureMesh(true, NewObject<UMaterial>())).Num(), 0);

    UMaterial* Translucent = NewObject<UMaterial>();
    Translucent->BlendMode = BLEND_Translucent;
    const TArray<FNamecNaniteViolation> TranslucentViolations = NamecNaniteRules::CheckMesh(*NewFixtureMesh(true, Translucent));
    TestTrue(TEXT("A translucent material fails rule 4"), HasViolationOfRule(TranslucentViolations, 4));
    if (TranslucentViolations.Num() > 0)
    {
        TestEqual(TEXT("The violation names the material"), TranslucentViolations[0].Offender, Translucent->GetName());
    }

    TestTrue(TEXT("Unclamped WPO fails rule 3"), HasViolationOfRule(NamecNaniteRules::CheckMesh(*NewFixtureMesh(true, NewWorldPositionOffsetMaterial(0.f))), 3));
    TestEqual(TEXT("Clamped WPO passes the mesh rules"), NamecNaniteRules::CheckMesh(*NewFixtureMesh(true, NewWorldPositionOffsetMaterial(50.f))).Num(), 0);

    UMaterial* Tessellated = NewObject<UMaterial>();
    Tessellated->bEnableTessellation = true;
    TestTrue(TEXT("Nanite tessellation fails rule 6"), HasViolationOfRule(NamecNaniteRules::CheckMesh(*NewFixtureMesh(true, Tessellated)), 6));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecNaniteValidatorComponentTest, "Namec.Foundation.NaniteValidator.ComponentRules", NamecFoundationTestFlags)

bool FNamecNaniteValidatorComponentTest::RunTest(const FString& Parameters)
{
    UStaticMesh* CleanNaniteMesh = NewFixtureMesh(true, NewObject<UMaterial>());

    TestEqual(TEXT("A default component on a clean Nanite mesh passes"), NamecNaniteRules::CheckComponent(*NewFixtureComponent(CleanNaniteMesh)).Num(), 0);

    UStaticMeshComponent* OtherChannel = NewFixtureComponent(CleanNaniteMesh);
    OtherChannel->LightingChannels.bChannel1 = true;
    TestTrue(TEXT("A lighting channel other than the default fails rule 5"), HasViolationOfRule(NamecNaniteRules::CheckComponent(*OtherChannel), 5));

    UStaticMesh* WpoMesh = NewFixtureMesh(true, NewWorldPositionOffsetMaterial(50.f));
    TestTrue(TEXT("A WPO mesh on a default component fails rule 3"), HasViolationOfRule(NamecNaniteRules::CheckComponent(*NewFixtureComponent(WpoMesh)), 3));

    UStaticMeshComponent* Configured = NewFixtureComponent(WpoMesh);
    Configured->WorldPositionOffsetDisableDistance = 5000;
    Configured->ShadowCacheInvalidationBehavior = EShadowCacheInvalidationBehavior::Rigid;
    TestEqual(TEXT("With a disable distance and an invalidation behaviour it passes"), NamecNaniteRules::CheckComponent(*Configured).Num(), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecNaniteValidatorScopeTest, "Namec.Foundation.NaniteValidator.NonNaniteMeshIsNotValidated", NamecFoundationTestFlags)

bool FNamecNaniteValidatorScopeTest::RunTest(const FString& Parameters)
{
    UMaterial* Translucent = NewObject<UMaterial>();
    Translucent->BlendMode = BLEND_Translucent;
    UStaticMesh* NonNaniteMesh = NewFixtureMesh(false, Translucent);

    TestFalse(TEXT("A non-Nanite mesh is outside the validator"), NamecNaniteRules::IsNaniteMesh(NonNaniteMesh));
    TestTrue(TEXT("A Nanite mesh is inside it"), NamecNaniteRules::IsNaniteMesh(NewFixtureMesh(true, Translucent)));

    UStaticMeshComponent* Component = NewFixtureComponent(NonNaniteMesh);
    Component->LightingChannels.bChannel1 = true;
    TestEqual(TEXT("A component on a non-Nanite mesh is not checked"), NamecNaniteRules::CheckComponent(*Component).Num(), 0);
    return true;
}

#endif
