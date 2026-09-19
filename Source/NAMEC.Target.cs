using UnrealBuildTool;

public class NAMECTarget : TargetRules
{
    public NAMECTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.Add("NAMEC");
    }
}
