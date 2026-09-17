using UnrealBuildTool;

public class NAMECTarget : TargetRules
{
    public NAMECTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("NAMEC");
    }
}
