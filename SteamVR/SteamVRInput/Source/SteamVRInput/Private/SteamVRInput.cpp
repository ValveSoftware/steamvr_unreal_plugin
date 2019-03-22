/*
Copyright 2019 Valve Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files(the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "SteamVRInput.h"
#include "Core.h"
#include "../../OpenVRSDK/headers/openvr.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FSteamVRInputModule"

DEFINE_LOG_CATEGORY_STATIC(LogSteamInput, Log, All);

void FSteamVRInputModule::StartupModule()
{
	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("SteamVRInput")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString OpenVRPath;
#if PLATFORM_WINDOWS
	OpenVRPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/OpenVRSDK/bin/win64/openvr_api.dll"));
	UE_LOG(LogSteamInput, Display, TEXT("[STEAMVR INPUT] Loading OpenVR SDK %s"), *OpenVRPath);
#elif PLATFORM_LINUX
	OpenVRPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/OpenVRSDK/bin/linux64/libopenvr_api.so"));
#endif // PLATFORM_WINDOWS

	OpenVRSDKHandle = !OpenVRPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*OpenVRPath) : nullptr;
	
	if (OpenVRSDKHandle != nullptr)
	{
		UE_LOG(LogSteamInput, Display, TEXT("Latest OpenVR loaded from %s"), *OpenVRPath);
	}
	else
	{
		UE_LOG(LogSteamInput, Error, TEXT("Can't find OpenVR in %s/Source/ThirdParty/OpenVRSDK"), *BaseDir);
	}

	// Unload UE4 Stock Engine SteamVRController Module (if present)
	FModuleManager& ModuleManager = FModuleManager::Get();
	if (ModuleManager.UnloadModule(FName("SteamVRController")))
	{
		UE_LOG(LogSteamInput, Warning, TEXT("[SteamVR Input] Unloaded UE4 SteamVR Controller"));
	}
	else
	{
		UE_LOG(LogSteamInput, Error, TEXT("[SteamVR Input] Unable to unload UE4 SteamVR Controller"));
	}
}

void FSteamVRInputModule::ShutdownModule()
{
	FPlatformProcess::FreeDllHandle(OpenVRSDKHandle);
	OpenVRSDKHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSteamVRInputModule, SteamVRInput)
