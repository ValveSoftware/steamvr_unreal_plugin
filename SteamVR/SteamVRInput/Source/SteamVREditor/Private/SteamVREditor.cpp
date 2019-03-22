/*
Copyright 2019 Valve Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files(the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "SteamVREditor.h"
#include "IMotionController.h"
#include "Features/IModularFeatures.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "SteamVREditorStyle.h"
#include "SteamVREditorCommands.h"
#include "LevelEditor.h"

static const FName SteamVREditorTabName("SteamVREditor");

#define LOCTEXT_NAMESPACE "FSteamVREditorModule"

void FSteamVREditorModule::StartupModule()
{
	FSteamVREditorStyle::Initialize();
	FSteamVREditorStyle::ReloadTextures();

	FSteamVREditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	// Dummy action for main toolbar button
	PluginCommands->MapAction(
		FSteamVREditorCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSteamVREditorModule::PluginButtonClicked),
		FCanExecuteAction());
	
	// Regenerate Action Manifest
	PluginCommands->MapAction(
		FSteamVREditorCommands::Get().JsonActionManifest,
		FExecuteAction::CreateRaw(this, &FSteamVREditorModule::JsonRegenerateActionManifest),
		FCanExecuteAction());

	// Regenerate Controller Bindings
	PluginCommands->MapAction(
		FSteamVREditorCommands::Get().JsonControllerBindings,
		FExecuteAction::CreateRaw(this, &FSteamVREditorModule::JsonRegenerateControllerBindings),
		FCanExecuteAction());

	// Launch Bindings URL
	PluginCommands->MapAction(
		FSteamVREditorCommands::Get().LaunchBindingsURL,
		FExecuteAction::CreateRaw(this, &FSteamVREditorModule::LaunchBindingsURL),
		FCanExecuteAction());
	
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FSteamVREditorModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FSteamVREditorModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FSteamVREditorModule::ShutdownModule()
{
	FSteamVREditorStyle::Shutdown();

	FSteamVREditorCommands::Unregister();
}

void FSteamVREditorModule::PluginButtonClicked()
{
	// Empty on purpose
}

void FSteamVREditorModule::JsonRegenerateActionManifest()
{
	USteamVRInputDeviceFunctionLibrary::RegenActionManifest();
}

void FSteamVREditorModule::JsonRegenerateControllerBindings()
{
	USteamVRInputDeviceFunctionLibrary::RegenControllerBindings();
}

void FSteamVREditorModule::LaunchBindingsURL()
{
	USteamVRInputDeviceFunctionLibrary::LaunchBindingsURL();
}

void FSteamVREditorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FSteamVREditorCommands::Get().PluginAction);
}

void FSteamVREditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	//Builder.AddToolBarButton(FSteamVREditorCommands::Get().PluginAction);
	Builder.AddComboButton(
		FUIAction(FExecuteAction::CreateRaw(this, &FSteamVREditorModule::PluginButtonClicked)),
		FOnGetContent::CreateRaw(this, &FSteamVREditorModule::FillComboButton, PluginCommands),
		LOCTEXT("SteamVRInputBtn", "SteamVR Input"),
		LOCTEXT("SteamVRInputBtnTootlip", "SteamVR Input")
	);
}

TSharedRef<SWidget> FSteamVREditorModule::FillComboButton(TSharedPtr<class FUICommandList> Commands)
{
	FMenuBuilder MenuBuilder(true, Commands);

	MenuBuilder.AddMenuEntry(FSteamVREditorCommands::Get().JsonActionManifest);
	MenuBuilder.AddMenuEntry(FSteamVREditorCommands::Get().JsonControllerBindings);
	MenuBuilder.AddMenuEntry(FSteamVREditorCommands::Get().LaunchBindingsURL);

	return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSteamVREditorModule, SteamVREditor)