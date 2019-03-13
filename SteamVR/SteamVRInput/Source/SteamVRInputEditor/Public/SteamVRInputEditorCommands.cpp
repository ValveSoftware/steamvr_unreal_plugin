#include "SteamVRInputEditor.h"
#include "SteamVRInputEditorCommands.h"

#define LOCTEXT_NAMESPACE "FSteamVRInputEditorModule"

void FSteamVRInputEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "SteamVRInputEditor", "Bring up SteamVRInputEditor window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
