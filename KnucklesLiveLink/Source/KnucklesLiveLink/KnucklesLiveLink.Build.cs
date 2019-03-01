using UnrealBuildTool;
using System.IO;

public class KnucklesLiveLink : ModuleRules
{
	public KnucklesLiveLink(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));

        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "LiveLinkInterface",
                "InputDevice",
                "InputCore",
                "HeadMountedDisplay",
                "OpenVRSDK",
                "Json"
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
			);

    }
}
