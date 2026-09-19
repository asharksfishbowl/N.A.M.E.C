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
            "NavigationSystem",
            "GeometryCollectionEngine",
            "RenderCore",
            "RHI",
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "StateTreeModule",
            "GameplayStateTreeModule",
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
