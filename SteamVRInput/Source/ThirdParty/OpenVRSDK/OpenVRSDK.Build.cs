using System.IO;
using UnrealBuildTool;

public class OpenVRSDK : ModuleRules
{
	public OpenVRSDK(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		string HeadersPath = Path.Combine(ModuleDirectory, "headers");
		string LibraryPath = Path.Combine(ModuleDirectory, "lib");
        string BinaryPath = Path.Combine(ModuleDirectory, "bin");

        // Setup OpenVR Paths based on build platform
        if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicLibraryPaths.Add(Path.Combine(LibraryPath, "win64"));
			PublicAdditionalLibraries.Add("openvr_api.lib");
			PublicDelayLoadDLLs.Add(Path.Combine(BinaryPath, "win64", "openvr_api.dll"));
            RuntimeDependencies.Add(Path.Combine(BinaryPath, "win64", "openvr_api.dll"));
        }
		else if (Target.Platform == UnrealTargetPlatform.Linux && Target.Architecture.StartsWith("x86_64"))
		{
			PublicLibraryPaths.Add(Path.Combine(LibraryPath, "linux64"));
			PublicAdditionalLibraries.Add("libopenvr_api.so");
            PublicDelayLoadDLLs.Add(Path.Combine(BinaryPath, "win64", "libopenvr_api.so"));
            RuntimeDependencies.Add(Path.Combine(BinaryPath, "win64", "libopenvr_api.so"));
        }

		// Verify if necessary OpenVR paths exists
		if (!Directory.Exists(HeadersPath) && !Directory.Exists(LibraryPath) && !Directory.Exists(BinaryPath))
		{
			string Err = string.Format("OpenVR SDK not found in {0}", ModuleDirectory);
			System.Console.WriteLine(Err);
			throw new BuildException(Err);
		}

		PublicIncludePaths.Add(HeadersPath);

	}
}
