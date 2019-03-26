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

#include "IInputDevice.h"
#include "Core.h"
#include "IMotionController.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformProcess.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "SteamVRInputTypes.h"
#include "SteamVRKnucklesKeys.h"
#include "SteamVRInputPublic.h"

class FSteamVRInputDevice : public IInputDevice, public IMotionController
{
public:
	FSteamVRInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);

	~FSteamVRInputDevice();


	virtual void Tick(float DeltaTime) override;

	virtual void SendControllerEvents() override;

	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;

	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const;
	virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const;

	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) override;

	
	/* SteamVR Sytem Handler **/
	IVRSystem* SteamVRSystem = nullptr;
	void InitSteamVRSystem();

	/* Current SteamVR Error **/
	EVRInputError SteamVRError = VRInputError_None;

	struct FInputDeviceState
	{
		EControllerHand Hand;
		uint32 PacketNum;

		float TouchPadXAnalog;
		float TouchPadYAnalog;

		float TriggerAnalog;

		// TODO PRIORITY: Move out to LiveLink
		float HandGripAnalog;
		float IndexGripAnalog;
		float MiddleGripAnalog;
		float RingGripAnalog;
		float PinkyGripAnalog;
		float ThumbGripAnalog;

		// TODO PRIORITY: Move out to LiveLink
		float ThumbIndexSplayAnalog;
		float IndexMiddleSplayAnalog;
		float MiddleRingSplayAnalog;
		float RingPinkySplayAnalog;

	};

	// These variables will be exposed to Blueprint via the Function Library (Skeletal Input System)
	bool bCurlsAndSplaysEnabled_L = true;
	bool bCurlsAndSplaysEnabled_R = true;

	/* Mappings between tracked devices and 0 indexed controllers */
	int32 NumControllersMapped;
	int32 NumTrackersMapped;
	int32 DeviceToControllerMap[k_unMaxTrackedDeviceCount];
	int32 UnrealControllerIdAndHandToDeviceIdMap[SteamVRInputDeviceConstants::MaxUnrealControllers][k_unMaxTrackedDeviceCount];
	int32 MaxUEHandCount[CONTROLLERS_PER_PLAYER];

	/* Controller states */
	FInputDeviceState ControllerStates[SteamVRInputDeviceConstants::MaxControllers];

	TArray<FSteamVRInputAction> Actions;
	TArray<FControllerType> ControllerTypes;
	VRActionSetHandle_t MainActionSet;

	VRActionHandle_t VRControllerHandleLeft;
	VRActionHandle_t VRControllerHandleRight;
	VRActionHandle_t VRSkeletalHandleLeft;
	VRActionHandle_t VRSkeletalHandleRight;

	VRSkeletalSummaryData_t VRSkeletalSummaryDataLeft;
	VRSkeletalSummaryData_t VRSkeletalSummaryDataRight;

	EVRInputError LastInputError = VRInputError_None;

	float InitialButtonRepeatDelay;
	float ButtonRepeatDelay;
	FGamepadKeyNames::Type Buttons[k_unMaxTrackedDeviceCount][ESteamVRInputButton::TotalButtonCount];

	// Project Name and Version from DefaultGame.ini
	FString GameFileName;
	FString GameProjectName;
	FString GameProjectVersion;
	FString EditorAppKey;

	void RegenerateActionManifest();
	void RegenerateControllerBindings();

	// Skeletal animations
	bool bIsSkeletalControllerLeftPresent = false;
	bool bIsSkeletalControllerRightPresent = false;
	bool GetSkeletalData(bool bLeftHand, EVRSkeletalMotionRange MotionRange, FTransform* OutBoneTransform, int32 OutBoneTransformCount);

	// Forcibly reload action manifest
	void ReloadActionManifest();

private:
	/* VR Input Error Handler */
	void GetInputError(EVRInputError InputError, FString InputAction);

	/* Message handler */
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;

	/* Previous SteamVR Error **/
	EVRInputError PrevSteamVRError = VRInputError_None;

	// Motion Controller Helper Functions
	void InitControllerMappings();
	void InitSkeletalControllerKeys();

	// Action event processing
	void SendSkeletalInputEvents();

	// Controller Bindings Helper Functions (Editor Only)
#if WITH_EDITOR
	FDelegateHandle ActionMappingsChangedHandle;
	void GenerateControllerBindings(const FString& BindingsPath, TArray<FControllerType>& InOutControllerTypes, TArray<TSharedPtr<FJsonValue>>& InOutDefaultBindings, TArray<FSteamVRInputAction>& InActionsArray, TArray<FInputMapping>& InInputMapping, bool bDeleteIfExists = false);
	void GenerateActionBindings(TArray<FInputMapping> &InInputMapping, TArray<TSharedPtr<FJsonValue>> &JsonValuesArray);
#endif

	// Action Manifest and Bindings Helper Functions
	void GenerateActionManifest(bool GenerateActions=true, bool GenerateBindings=true, bool RegisterApp=true, bool DeleteBindings=false);
	bool GenerateAppManifest(FString ManifestPath, FString ProjectName, FString& OutAppKey, FString& OutAppManifestPath);
	bool BuildJsonObject(TArray<FString> StringFields, TSharedRef<FJsonObject> OutJsonObject);
	void ProcessKeyInputMappings(const UInputSettings* InputSettings, TArray<FName> &InOutUniqueInputs);
	void ProcessKeyAxisMappings(const UInputSettings* InputSettings, TArray<FName> &InOutUniqueInputs);

	void SanitizeActions();
	void RegisterApplication(FString ManifestPath);
	void RegisterDeviceChanges();
	void RegisterDevice(uint32 id);
	void UnRegisterDevice(uint32 id);
	bool SetSkeletalHandle(char* ActionPath, VRActionHandle_t& SkeletalHandle);
	void CheckControllerHandSwap();

	TArray<FInputAxisKeyMapping> KeyAxisMappings;
	TArray<FInputActionKeyMapping> KeyMappings;
	void FindAxisMappings(const UInputSettings* InputSettings, const FName AxisName, TArray<FInputAxisKeyMapping>& OutMappings) const;
	void FindActionMappings(const UInputSettings* InputSettings, const FName ActionName, TArray<FInputActionKeyMapping>& OutMappings) const;
	void SendAnalogMessage(const ETrackedControllerRole TrackedControllerRole, const FGamepadKeyNames::Type AxisButton, float AnalogValue);
	FString SanitizeString(FString& InOutString);
};
