#pragma once

#include "Modules/ModuleManager.h"

class FSteamVRInputModule : public IModuleInterface
{
public:

	/* IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/* Handle to the OpenVR Library */
	void*	OpenVRHandle;
};
