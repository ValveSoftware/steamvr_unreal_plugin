#include "SteamVRInput.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FSteamVRInputModule"

DEFINE_LOG_CATEGORY_STATIC(LogSteamInput, Log, All);

void FSteamVRInputModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*IPluginManager::Get().FindPlugin("SteamVRInput")->GetBaseDir());

	// Add on the relative location of the third party dll and load it
	FString OpenVRPath;
#if PLATFORM_WINDOWS
	OpenVRPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/OpenVRSDK/bin/win64/openvr_api.dll"));
#elif PLATFORM_LINUX
	OpenVRPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/OpenVRSDK/bin/linux64/libopenvr_api.so"));
#endif // PLATFORM_WINDOWS

	OpenVRHandle = !OpenVRPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*OpenVRPath) : nullptr;

	if (OpenVRHandle)
	{
		UE_LOG(LogSteamInput, Display, TEXT("Latest OpenVR loaded from %s"), *OpenVRPath);
	}
	else
	{
		UE_LOG(LogSteamInput, Error, TEXT("Can't find OpenVR in %s/Source/ThirdParty/OpenVRSDK"), *BaseDir);
	}
}

void FSteamVRInputModule::ShutdownModule()
{
	FPlatformProcess::FreeDllHandle(OpenVRHandle);
	OpenVRHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSteamVRInputModule, SteamVRInput)
