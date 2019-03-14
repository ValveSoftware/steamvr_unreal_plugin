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

	return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSteamVREditorModule, SteamVREditor)