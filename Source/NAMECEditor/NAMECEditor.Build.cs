using UnrealBuildTool;

public class NAMECEditor : ModuleRules
{
    public NAMECEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "NAMEC",
            "UnrealEd",
            "LevelEditor",
            "DataValidationModule",
        });
    }
}
