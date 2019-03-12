#include "ISteamVRInputDeviceModule.h"
#include "IInputDevice.h"
#include "IPluginManager.h"
#include "SteamVRInputDevice.h"

class FSteamVRInputDeviceModule : public ISteamVRInputDeviceModule
{
	/* Creates a new instance of SteamVR Input Controller **/
	virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override
	{
		return TSharedPtr<class IInputDevice>(new FSteamVRInputDevice(InMessageHandler));
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FSteamVRInputDeviceModule::StartupModule()
{
	IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);

	//// Get the base directory of this plugin
	//FString BaseDir = IPluginManager::Get().FindPlugin("SteamVRInput")->GetBaseDir();
	//
	//// Add on the relative location of the third party dll and load it
	//FString OpenVRPath;
	////#if PLATFORM_WINDOWS
	//OpenVRPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/OpenVRSDK/bin/win64/openvr_api.dll"));
	////#elif PLATFORM_LINUX
	////	OpenVRPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/OpenVRSDK/bin/linux64/libopenvr_api.so"));
	////#endif // PLATFORM_WINDOWS
	//void*	OpenVRHandle;
	//OpenVRHandle = !OpenVRPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*OpenVRPath) : nullptr;

	//if (OpenVRHandle)
	//{
	//	UE_LOG(LogTemp, Display, TEXT("Latest OpenVR loaded from %s"), *OpenVRPath);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Can't find OpenVR in %s/Source/ThirdParty/OpenVRSDK"), *BaseDir);
	//}
}

void FSteamVRInputDeviceModule::ShutdownModule()
{
	IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);
}

IMPLEMENT_MODULE(FSteamVRInputDeviceModule, SteamVRInputDevice)
