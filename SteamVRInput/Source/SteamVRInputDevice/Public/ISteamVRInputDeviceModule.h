#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IInputDeviceModule.h"

class ISteamVRInputDeviceModule : public IInputDeviceModule
{

public:
	static inline ISteamVRInputDeviceModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ISteamVRInputDeviceModule>("SteamVRInputDevice");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("SteamVRInputDevice");
	}
};
