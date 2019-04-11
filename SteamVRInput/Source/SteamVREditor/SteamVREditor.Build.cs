using UnrealBuildTool;
using System.IO;

public class SteamVREditor : ModuleRules
{
	public SteamVREditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "Public"
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "Private",
            }
            );

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "InputCore",
                "ApplicationCore",
                "InputDevice",
                "OpenVRSDK",
                "SteamVRInput",
                "SteamVRInputDevice",
                "BlueprintGraph",
                "AnimGraph"
            }
			);
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore"
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
