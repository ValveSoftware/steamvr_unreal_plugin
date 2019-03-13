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
}

void FSteamVRInputDeviceModule::ShutdownModule()
{
	IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);
}

IMPLEMENT_MODULE(FSteamVRInputDeviceModule, SteamVRInputDevice)
