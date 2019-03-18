#include "SteamVREditor.h"
#include "SteamVREditorCommands.h"

#define LOCTEXT_NAMESPACE "FSteamVREditorModule"

void FSteamVREditorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "SteamVREditor", "Execute SteamVREditor action", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(JsonActionManifest, "Regenerate Action Manifest", "Regenerate Action Manifest", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(JsonControllerBindings, "Regenerate Controller Bindings", "Regenerate Controller Bindings", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(LaunchBindingsURL, "Launch SteamVR Bindings Dashboard", "Launch SteamVR Bindings Dashboard", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
