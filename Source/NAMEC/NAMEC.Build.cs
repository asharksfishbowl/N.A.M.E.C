using UnrealBuildTool;

public class NAMEC : ModuleRules
{
    public NAMEC(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.Add(ModuleDirectory);

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "NetCore",
            "GameplayAbilities",
            "GameplayTasks",
            "GameplayTags",
            "EnhancedInput",
            "AIModule",
            "GeometryCollectionEngine",
            "RenderCore",
            "RHI",
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "StateTreeModule",
            "SmartObjectsModule",
            "PCG",
            "CustomizableObject",
            "MutableRuntime",
            "IKRig",
            "PoseSearch",
            "MetasoundEngine",
            "OnlineSubsystem",
            "OnlineSubsystemNull",
        });
    }
}
