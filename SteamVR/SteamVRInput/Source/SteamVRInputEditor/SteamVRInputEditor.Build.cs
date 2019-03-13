using UnrealBuildTool;

public class SteamVRInputEditor : ModuleRules
{
	public SteamVRInputEditor(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(
			new string[] {
				"SteamVRInputEditor/Public"
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"SteamVRInputEditor/Private",
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
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
				"SlateCore",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
			}
			);
	}
}
