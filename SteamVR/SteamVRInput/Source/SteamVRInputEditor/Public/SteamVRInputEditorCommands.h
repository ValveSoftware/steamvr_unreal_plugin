#pragma once

#include "SlateBasics.h"
#include "SteamVRInputEditorStyle.h"

class FSteamVRInputEditorCommands : public TCommands<FSteamVRInputEditorCommands>
{
public:

	FSteamVRInputEditorCommands()
		: TCommands<FSteamVRInputEditorCommands>(TEXT("SteamVRInputEditor"), NSLOCTEXT("Contexts", "SteamVRInputEditor", "SteamVRInputEditor Plugin"), NAME_None, FSteamVRInputEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};