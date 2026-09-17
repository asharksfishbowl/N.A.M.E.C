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
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "StateTreeModule",
            "SmartObjectsModule",
            "PCG",
            "MutableRuntime",
            "IKRig",
            "PoseSearch",
            "MetasoundEngine",
            "OnlineSubsystem",
            "OnlineSubsystemNull",
        });
    }
}
