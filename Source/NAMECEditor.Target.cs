using UnrealBuildTool;

public class NAMECEditorTarget : TargetRules
{
    public NAMECEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new string[] { "NAMEC", "NAMECEditor" });
    }
}
