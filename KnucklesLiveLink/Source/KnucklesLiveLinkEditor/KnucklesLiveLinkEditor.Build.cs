using UnrealBuildTool;
using System.IO;

public class KnucklesLiveLinkEditor : ModuleRules
{
    public KnucklesLiveLinkEditor(ReadOnlyTargetRules Target) : base(Target)
    {
		// TODO: Clean up dependencies
		
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "UnrealEd",
                "Engine",
                "Projects",
                "DetailCustomizations",
                "OpenVRSDK",
                "KnucklesLiveLink",
				"LiveLinkInterface",
                "BlueprintGraph"
            }
            );
            
        
        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "WorkspaceMenuStructure",
                "EditorStyle",
                "SlateCore",
                "Slate",
                "InputCore"
            }
            );
    }
}
