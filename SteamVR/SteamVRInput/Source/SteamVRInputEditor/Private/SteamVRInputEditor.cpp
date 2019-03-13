#include "SteamVRInputEditor.h"

#include "SlateBasics.h"
#include "SlateExtras.h"

#include "SteamVRInputEditorStyle.h"
#include "SteamVRInputEditorCommands.h"

#include "LevelEditor.h"

static const FName SteamVRInputEditorTabName("SteamVRInputEditor");

#define LOCTEXT_NAMESPACE "FSteamVRInputEditorModule"

void FSteamVRInputEditorModule::StartupModule()
{
	FSteamVRInputEditorStyle::Initialize();
	FSteamVRInputEditorStyle::ReloadTextures();

	FSteamVRInputEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSteamVRInputEditorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FSteamVRInputEditorModule::PluginButtonClicked),
		FCanExecuteAction());
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FSteamVRInputEditorModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
	
	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FSteamVRInputEditorModule::AddToolbarExtension));
		
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SteamVRInputEditorTabName, FOnSpawnTab::CreateRaw(this, &FSteamVRInputEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FSteamVRInputEditorTabTitle", "SteamVRInputEditor"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FSteamVRInputEditorModule::ShutdownModule()
{
	FSteamVRInputEditorStyle::Shutdown();

	FSteamVRInputEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SteamVRInputEditorTabName);
}

TSharedRef<SDockTab> FSteamVRInputEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to populate this SteamVR Input Menu system"),
		FText::FromString(TEXT("FSteamVRInputEditorModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("SteamVRInputEditor.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FSteamVRInputEditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(SteamVRInputEditorTabName);
}

void FSteamVRInputEditorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FSteamVRInputEditorCommands::Get().OpenPluginWindow);
}

void FSteamVRInputEditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FSteamVRInputEditorCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSteamVRInputEditorModule, SteamVRInputEditor)