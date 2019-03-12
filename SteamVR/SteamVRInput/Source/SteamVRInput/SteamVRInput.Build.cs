using UnrealBuildTool;
using System.IO;

public class SteamVRInput : ModuleRules
{
	public SteamVRInput(TargetInfo Target)
    {
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));
        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Projects",
                "OpenVRSDK",
                "OpenVR",
                "SteamVR"
            }
			);
    }
}
