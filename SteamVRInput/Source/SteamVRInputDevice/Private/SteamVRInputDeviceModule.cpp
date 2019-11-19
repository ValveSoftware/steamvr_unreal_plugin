/*
Copyright 2019 Valve Corporation under https://opensource.org/licenses/BSD-3-Clause

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/


#include "ISteamVRInputDeviceModule.h"
#include "IInputDevice.h"
#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"
#include "SteamVRInputDevice.h"

class FSteamVRInputDeviceModule : public ISteamVRInputDeviceModule
{
	/* Creates a new instance of SteamVR Input Controller **/
	virtual TSharedPtr<class IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override
	{
		TSharedPtr<class FSteamVRInputDevice> SteamVRInputDevice(new FSteamVRInputDevice(InMessageHandler));
		
		#if WITH_EDITOR
		FEditorDelegates::OnActionAxisMappingsChanged.AddSP(SteamVRInputDevice.ToSharedRef(), &FSteamVRInputDevice::OnBindingsChangeHandle);
		#endif

		return SteamVRInputDevice;
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

void FSteamVRInputDeviceModule::StartupModule()
{
	IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);

	// Unload the engine's stock SteamVR Controller
	FModuleManager& ModuleManager = FModuleManager::Get();
	ISteamVRControllerPlugin* StockController = ModuleManager.GetModulePtr<ISteamVRControllerPlugin>(FName("SteamVRController"));

	if (StockController != nullptr)
	{
		// Manually Unregister Module Feature instead of straight up unloading
		IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), StockController);
		StockController->ShutdownModule();
		StockController->~ISteamVRControllerPlugin();
	}
	else
	{
		// Unload UE4 Stock Engine SteamVRController Module (if present)
		if (ModuleManager.UnloadModule(FName("SteamVRController")))
		{
			UE_LOG(LogTemp, Warning, TEXT("[SteamVR Input] Unloaded UE4 SteamVR Controller"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[SteamVR Input] Unable to unload UE4 SteamVR Controller"));
		}
	}
}

void FSteamVRInputDeviceModule::ShutdownModule()
{
	IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);
}

IMPLEMENT_MODULE(FSteamVRInputDeviceModule, SteamVRInputDevice)
