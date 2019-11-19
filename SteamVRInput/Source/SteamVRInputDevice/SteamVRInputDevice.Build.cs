using UnrealBuildTool;
using System.IO;

public class SteamVRInputDevice : ModuleRules
{
	public SteamVRInputDevice(TargetInfo Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Public/ISteamVRInputDeviceModule.h";

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Private"));

        PrivateIncludePathModuleNames.AddRange(new string[]
         {
            "TargetPlatform",
            "SteamVRInput",
            "SteamVR"
        });

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "HeadMountedDisplay",
                "SteamVR",
                "SteamVRController",
                "Json",
                "JsonUtilities"
			}
			);

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "OpenVRSDK",
                "SteamVR",
                "SteamVRInput",
                "InputCore",
                "InputDevice"
            }
            );

        if (UEBuildConfiguration.bBuildEditor == true)
        {
            PublicDependencyModuleNames.Add("UnrealEd");
        }

        //AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenVR");

        if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32 || (Target.Platform == UnrealTargetPlatform.Linux && Target.Architecture.StartsWith("x86_64")))
        {
            AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenGL");
            PrivateDependencyModuleNames.Add("OpenGLDrv");
        }
    }
}
