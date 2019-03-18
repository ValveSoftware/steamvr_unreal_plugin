#pragma once

#include "SlateBasics.h"
#include "SteamVREditorStyle.h"

class FSteamVREditorCommands : public TCommands<FSteamVREditorCommands>
{
public:

	FSteamVREditorCommands()
		: TCommands<FSteamVREditorCommands>(TEXT("SteamVREditor"), NSLOCTEXT("Contexts", "SteamVREditor", "SteamVREditor Plugin"), NAME_None, FSteamVREditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> PluginAction;
	TSharedPtr<FUICommandInfo> JsonActionManifest;
	TSharedPtr<FUICommandInfo> JsonControllerBindings;
	TSharedPtr<FUICommandInfo> LaunchBindingsURL;
};
