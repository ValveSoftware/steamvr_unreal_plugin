/*
Copyright 2019 Valve Corporation under https://opensource.org/licenses/BSD-3-Clause

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this 
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "SteamVREditor.h"
#include "IMotionController.h"
#include "Features/IModularFeatures.h"
#include "SteamVREditorStyle.h"
#include "SteamVREditorCommands.h"
#include "SteamVRControllerKeys.h"
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

	// Reload Action Manifest
	PluginCommands->MapAction(
		FSteamVREditorCommands::Get().ReloadActionManifest,
		FExecuteAction::CreateRaw(this, &FSteamVREditorModule::ReloadActionManifest),
		FCanExecuteAction());

	// Launch Bindings URL
	PluginCommands->MapAction(
		FSteamVREditorCommands::Get().LaunchBindingsURL,
		FExecuteAction::CreateRaw(this, &FSteamVREditorModule::LaunchBindingsURL),
		FCanExecuteAction());
	
	// Add Sample Inputs
	PluginCommands->MapAction(
		FSteamVREditorCommands::Get().AddSampleInputs,
		FExecuteAction::CreateRaw(this, &FSteamVREditorModule::AddSampleInputs),
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

void FSteamVREditorModule::ReloadActionManifest()
{
	USteamVRInputDeviceFunctionLibrary::ReloadActionManifest();
}

void FSteamVREditorModule::LaunchBindingsURL()
{
	USteamVRInputDeviceFunctionLibrary::ShowBindingsUI(EHand::VR_LeftHand, FName("main"), false);
}

void FSteamVREditorModule::AddSampleInputs()
{
	// Get Existing Input Settings
	auto DefaultInputSettings = GetDefault<UInputSettings>();
	TArray<FInputAxisKeyMapping> ExistingAxisKeys = DefaultInputSettings->GetAxisMappings();
	TArray<FInputActionKeyMapping> ExistingActionKeys = DefaultInputSettings->GetActionMappings();

	// Create new Input Settings
	UInputSettings* InputSettings = GetMutableDefault<UInputSettings>();

	if (InputSettings->IsValidLowLevel())
	{
		// Teleport
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportLeft")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Trackpad_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportLeft")), ViveControllerKeys::SteamVR_Vive_Controller_Trackpad_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportLeft")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Joystick_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportLeft")), OculusTouchKeys::SteamVR_Oculus_Touch_Joystick_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportLeft")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Trackpad_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportLeft")), FGamepadKeyNames::MotionController_Left_Thumbstick);

		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportRight")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Trackpad_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportRight")), ViveControllerKeys::SteamVR_Vive_Controller_Trackpad_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportRight")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Joystick_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportRight")), OculusTouchKeys::SteamVR_Oculus_Touch_Joystick_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportRight")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Trackpad_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("TeleportRight")), FGamepadKeyNames::MotionController_Right_Thumbstick);

		// Grab
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabLeft")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Grip_Grab_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabLeft")), ViveControllerKeys::SteamVR_Vive_Controller_Trigger_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabLeft")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Grip_Click_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabLeft")), OculusTouchKeys::SteamVR_Oculus_Touch_Grip_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabLeft")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Grip_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabLeft")), FGamepadKeyNames::MotionController_Left_Trigger);

		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabRight")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Grip_Grab_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabRight")), ViveControllerKeys::SteamVR_Vive_Controller_Trigger_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabRight")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Grip_Click_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabRight")), OculusTouchKeys::SteamVR_Oculus_Touch_Grip_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabRight")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Grip_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("GrabRight")), FGamepadKeyNames::MotionController_Right_Trigger);

		// Fire Arrow
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowLeft")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Pinch_Grab_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowLeft")), ViveControllerKeys::SteamVR_Vive_Controller_Trigger_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowLeft")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Trigger_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowLeft")), OculusTouchKeys::SteamVR_Oculus_Touch_Trigger_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowLeft")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Trigger_Press_Left);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowLeft")), FGamepadKeyNames::MotionController_Left_Trigger);

		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowRight")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Pinch_Grab_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowRight")), ViveControllerKeys::SteamVR_Vive_Controller_Trigger_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowRight")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Trigger_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowRight")), OculusTouchKeys::SteamVR_Oculus_Touch_Trigger_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowRight")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Trigger_Press_Right);
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("FireArrowRight")), FGamepadKeyNames::MotionController_Right_Trigger);

		// HMD Proximity
		AddUniqueActionMapping(ExistingActionKeys, InputSettings, FName(TEXT("HeadsetOn")), GenericKeys::SteamVR_HMD_Proximity);

		// Move Right
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_Y")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Thumbstick_Y_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_Y")), ViveControllerKeys::SteamVR_Vive_Controller_Trackpad_Y_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_Y")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Joystick_Y_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_Y")), OculusTouchKeys::SteamVR_Oculus_Touch_Joystick_Y_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_Y")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Joystick_Y_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_Y")), FGamepadKeyNames::MotionController_Right_Thumbstick_Y);

		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_X")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Thumbstick_X_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_X")), ViveControllerKeys::SteamVR_Vive_Controller_Trackpad_X_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_X")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Joystick_X_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_X")), OculusTouchKeys::SteamVR_Oculus_Touch_Joystick_X_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_X")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Joystick_X_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveRight_X")), FGamepadKeyNames::MotionController_Right_Thumbstick_X);

		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_Y")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Thumbstick_Y_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_Y")), ViveControllerKeys::SteamVR_Vive_Controller_Trackpad_Y_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_Y")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Joystick_Y_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_Y")), OculusTouchKeys::SteamVR_Oculus_Touch_Joystick_Y_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_Y")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Joystick_Y_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_Y")), FGamepadKeyNames::MotionController_Left_Thumbstick_Y);

		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_X")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Thumbstick_X_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_X")), ViveControllerKeys::SteamVR_Vive_Controller_Trackpad_X_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_X")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Joystick_X_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_X")), OculusTouchKeys::SteamVR_Oculus_Touch_Joystick_X_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_X")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Joystick_X_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("MoveLeft_X")), FGamepadKeyNames::MotionController_Left_Thumbstick_X);

		// Teleport Direction
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_Y")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Trackpad_Y_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_Y")), ViveControllerKeys::SteamVR_Vive_Controller_Trackpad_Y_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_Y")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Joystick_Y_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_Y")), OculusTouchKeys::SteamVR_Oculus_Touch_Joystick_Y_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_Y")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Trackpad_Y_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_Y")), FGamepadKeyNames::MotionController_Right_Thumbstick_Y);

		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_X")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Trackpad_X_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_X")), ViveControllerKeys::SteamVR_Vive_Controller_Trackpad_X_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_X")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Joystick_X_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_X")), OculusTouchKeys::SteamVR_Oculus_Touch_Joystick_X_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_X")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Trackpad_X_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionRight_X")), FGamepadKeyNames::MotionController_Right_Thumbstick_X);

		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_Y")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Trackpad_Y_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_Y")), ViveControllerKeys::SteamVR_Vive_Controller_Trackpad_Y_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_Y")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Joystick_Y_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_Y")), OculusTouchKeys::SteamVR_Oculus_Touch_Joystick_Y_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_Y")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Trackpad_Y_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_Y")), FGamepadKeyNames::MotionController_Left_Thumbstick_Y);

		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_X")), IndexControllerKeys::SteamVR_Valve_Index_Controller_Trackpad_X_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_X")), ViveControllerKeys::SteamVR_Vive_Controller_Trackpad_X_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_X")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Joystick_X_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_X")), OculusTouchKeys::SteamVR_Oculus_Touch_Joystick_X_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_X")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Trackpad_X_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("TeleportDirectionLeft_X")), FGamepadKeyNames::MotionController_Left_Thumbstick_X);

		// Squeeze
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeLeft")), IndexControllerKeys::SteamVR_Valve_Index_Controller_GripForce_Axis_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeLeft")), ViveControllerKeys::SteamVR_Vive_Controller_Trigger_Pull_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeLeft")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Trigger_Pull_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeLeft")), OculusTouchKeys::SteamVR_Oculus_Touch_Trigger_Pull_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeLeft")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Trigger_Pull_Left);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeLeft")), FGamepadKeyNames::MotionController_Left_TriggerAxis);

		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeRight")), IndexControllerKeys::SteamVR_Valve_Index_Controller_GripForce_Axis_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeRight")), ViveControllerKeys::SteamVR_Vive_Controller_Trigger_Pull_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeRight")), CosmosControllerKeys::SteamVR_HTC_Cosmos_Trigger_Pull_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeRight")), OculusTouchKeys::SteamVR_Oculus_Touch_Trigger_Pull_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeRight")), WindowsMRKeys::SteamVR_Windows_MR_Controller_Trigger_Pull_Right);
		AddUniqueAxisMapping(ExistingAxisKeys, InputSettings, FName(TEXT("SqueezeRight")), FGamepadKeyNames::MotionController_Right_TriggerAxis);
		

		// Update the config file
		InputSettings->SaveKeyMappings();
		InputSettings->UpdateDefaultConfigFile();
	}
}

bool FSteamVREditorModule::AddUniqueAxisMapping(TArray<FInputAxisKeyMapping> ExistingAxisKeys, UInputSettings* InputSettings, FName ActionName, FKey ActionKey)
{
	// Create new axis mapping
	FInputAxisKeyMapping NewAxisMapping = FInputAxisKeyMapping(ActionName, ActionKey);

	// Check if this mapping already exists in the project
	if (ExistingAxisKeys.Find(NewAxisMapping) < 1)
	{
		// If none, create a new one
		InputSettings->AddAxisMapping(NewAxisMapping);
		return true;
	}
	
return false;
}

bool FSteamVREditorModule::AddUniqueActionMapping(TArray<FInputActionKeyMapping> ExistingActionKeys, UInputSettings* InputSettings, FName ActionName, FKey ActionKey)
{
	// Create new action mapping
	FInputActionKeyMapping NewActionMapping = FInputActionKeyMapping(ActionName, ActionKey);

	// Check if this mapping already exists in the project
	if (ExistingActionKeys.Find(NewActionMapping) < 1)
	{
		// If none, create a new one
		InputSettings->AddActionMapping(NewActionMapping);
		return true;
	}

	return false;
}

void FSteamVREditorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FSteamVREditorCommands::Get().PluginAction);
}

void FSteamVREditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	FSteamVREditorStyle MenuStyle = FSteamVREditorStyle();
	MenuStyle.Initialize();

	Builder.AddComboButton(
		FUIAction(FExecuteAction::CreateRaw(this, &FSteamVREditorModule::PluginButtonClicked)),
		FOnGetContent::CreateRaw(this, &FSteamVREditorModule::FillComboButton, PluginCommands),
		LOCTEXT("SteamVRInputBtn", "SteamVR Input"),
		LOCTEXT("SteamVRInputBtnTootlip", "SteamVR Input"),
		FSlateIcon(FSteamVREditorStyle::GetStyleSetName(), "SteamVREditor.PluginAction")
	);
}

TSharedRef<SWidget> FSteamVREditorModule::FillComboButton(TSharedPtr<class FUICommandList> Commands)
{
	FMenuBuilder MenuBuilder(true, Commands);

	MenuBuilder.AddMenuEntry(FSteamVREditorCommands::Get().JsonActionManifest, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FSteamVREditorStyle::GetStyleSetName(), "SteamVREditor.JsonActionManifest"));
	MenuBuilder.AddMenuEntry(FSteamVREditorCommands::Get().JsonControllerBindings, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FSteamVREditorStyle::GetStyleSetName(), "SteamVREditor.JsonControllerBindings"));
	MenuBuilder.AddMenuEntry(FSteamVREditorCommands::Get().ReloadActionManifest, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FSteamVREditorStyle::GetStyleSetName(), "SteamVREditor.ReloadActionManifest"));
	MenuBuilder.AddMenuEntry(FSteamVREditorCommands::Get().LaunchBindingsURL, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FSteamVREditorStyle::GetStyleSetName(), "SteamVREditor.LaunchBindingsURL"));
	MenuBuilder.AddMenuEntry(FSteamVREditorCommands::Get().AddSampleInputs, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FSteamVREditorStyle::GetStyleSetName(), "SteamVREditor.AddSampleInputs"));

	return MenuBuilder.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSteamVREditorModule, SteamVREditor)