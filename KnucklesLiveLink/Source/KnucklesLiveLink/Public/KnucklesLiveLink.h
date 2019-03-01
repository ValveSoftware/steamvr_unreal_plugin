#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "InputDevice/Public/IInputDeviceModule.h"
#include "openvr.h"
#include "KnucklesLiveLinkSource.h"

class FKnucklesLiveLinkModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
