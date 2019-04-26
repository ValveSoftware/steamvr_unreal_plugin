using UnrealBuildTool;
using System.IO;

public class SteamVREditor : ModuleRules
{
	public SteamVREditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivatePCHHeaderFile = "Public/SteamVREditor.h";

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

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
