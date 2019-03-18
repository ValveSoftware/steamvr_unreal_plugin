#pragma once

#include "ModuleManager.h"
#include "UIAction.h"
#include "../../SteamVRInputDevice/Public/SteamVRInputDevice.h"
#include "../../SteamVRInputDevice/Public/SteamVRInputDeviceFunctionLibrary.h"

class FToolBarBuilder;
class FMenuBuilder;
class SWidget;

class FSteamVREditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** These functions will be bound to Commands */
	void PluginButtonClicked();
	
	void JsonRegenerateActionManifest();
	void JsonRegenerateControllerBindings();
	void LaunchBindingsURL();
private:

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedRef<SWidget> FillComboButton(TSharedPtr<class FUICommandList> Commands);
};
