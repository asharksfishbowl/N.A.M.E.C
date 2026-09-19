using UnrealBuildTool;

public class NAMECEditor : ModuleRules
{
    public NAMECEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.Add(ModuleDirectory);

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "NAMEC",
            "UnrealEd",
            "LevelEditor",
            "DataValidation",
            "Json",
            "JsonUtilities",
            "MeshDescription",
            "StaticMeshDescription",
            "AssetRegistry",
            "RenderCore",
            "PCG",
            "EnhancedInput",
            "InputCore",
        });
    }
}
