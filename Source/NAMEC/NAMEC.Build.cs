using UnrealBuildTool;

public class NAMEC : ModuleRules
{
    public NAMEC(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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
