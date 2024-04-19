using UnrealBuildTool;

public class Oceanology_PluginEditor : ModuleRules
{
    public Oceanology_PluginEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivateIncludePaths.Add("Oceanology_PluginEditor/Private");

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "ComponentVisualizers",
                "DetailCustomizations",
                "Engine",
                "InputCore",
                "SlateCore",
                "Slate",
                "UnrealEd",
                "Oceanology_Plugin",
                "Projects",
                "PropertyEditor",
                "EditorFramework",
                "EditorSubsystem",
                "ComponentVisualizers",
                "DeveloperSettings",
                "AdvancedPreviewScene",
                "RenderCore",
                "RHI",
                "BSPUtils",
                "VirtualTexturingEditor"
            }
        );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
            }
        );
    }
}