using UnrealBuildTool;

public class SteamVRInput : ModuleRules
{
	public SteamVRInput(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Projects"
			}
			);
	}
}
