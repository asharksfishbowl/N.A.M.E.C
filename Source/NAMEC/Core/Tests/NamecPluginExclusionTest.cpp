#include "Core/Tests/NamecTestFlags.h"
#include "Algo/AnyOf.h"
#include "Interfaces/IProjectManager.h"
#include "PluginReferenceDescriptor.h"
#include "ProjectDescriptor.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
    // engine-tech Requirement 4: features the project must not adopt. Descriptor names measured
    // in the 5.8 install on 2026-09-19. The Voxel Plugin is absent: it is installed nowhere on the
    // reference PC, so its descriptor name cannot be confirmed from its own .uplugin yet.
    const TCHAR* const ForbiddenPluginNames[] =
    {
        TEXT("Mover"),
        TEXT("MoverIntegrations"),
        TEXT("MoverAnimNext"),
        TEXT("GameplayCameras"),
        TEXT("MeshTerrainMode"),
        TEXT("DynamicWind"),
        TEXT("NaniteAssemblyEditorUtils"),
        TEXT("ProceduralVegetationEditor"),
        TEXT("MetaHumanCharacter"),
        TEXT("MetaHumanCrowd"),
        TEXT("Chooser"),
    };

    // Reads the project descriptor's own list, never IPluginManager's enabled set: an adopted
    // plugin may pull a listed one in as its dependency, and that is Epic's choice, not ours.
    TArray<FString> FindForbiddenPlugins(const FProjectDescriptor& Project)
    {
        TArray<FString> Found;
        for (const FPluginReferenceDescriptor& Plugin : Project.Plugins)
        {
            const bool bForbidden = Algo::AnyOf(ForbiddenPluginNames, [&Plugin](const TCHAR* Name) { return Plugin.Name == Name; });
            if (Plugin.bEnabled && bForbidden)
            {
                Found.Add(Plugin.Name);
            }
        }
        return Found;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecPluginExclusionTest, "Namec.Foundation.Engine.PluginExclusion", NamecFoundationTestFlags)

bool FNamecPluginExclusionTest::RunTest(const FString& Parameters)
{
    const FProjectDescriptor* Project = IProjectManager::Get().GetCurrentProject();
    if (!TestNotNull(TEXT("Current project descriptor"), Project))
    {
        return true;
    }

    for (const FString& PluginName : FindForbiddenPlugins(*Project))
    {
        AddError(FString::Printf(TEXT("NAMEC.uproject enables '%s', which engine-tech Requirement 4 excludes"), *PluginName));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNamecPluginExclusionRuleTest, "Namec.Foundation.Engine.PluginExclusionRule", NamecFoundationTestFlags)

bool FNamecPluginExclusionRuleTest::RunTest(const FString& Parameters)
{
    FProjectDescriptor Project;
    Project.Plugins.Add(FPluginReferenceDescriptor(TEXT("EnhancedInput"), true));
    Project.Plugins.Add(FPluginReferenceDescriptor(TEXT("Chooser"), false));
    TestEqual(TEXT("A listed plugin that is disabled is not reported"), FindForbiddenPlugins(Project).Num(), 0);

    Project.Plugins.Add(FPluginReferenceDescriptor(TEXT("Mover"), true));
    const TArray<FString> Found = FindForbiddenPlugins(Project);
    if (TestEqual(TEXT("One enabled forbidden plugin is reported"), Found.Num(), 1))
    {
        TestEqual(TEXT("It is named"), Found[0], FString(TEXT("Mover")));
    }
    return true;
}

#endif
