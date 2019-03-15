#pragma once

#include "IInputDevice.h"
#include "Core.h"
#include "IMotionController.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformProcess.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "SteamVRInputTypes.h"
#include "SteamVRKnucklesKeys.h"

STEAMVRINPUT_API class FSteamVRInputDevice : public IInputDevice, public IMotionController
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

	void RegenerateActionManifest();
	void RegenerateControllerBindings();

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

	void RegisterApplication(FString ManifestPath);
	void RegisterDeviceChanges();
	void RegisterDevice(uint32 id);
	void UnRegisterDevice(uint32 id);
	void CheckControllerHandSwap();

	TArray<FInputAxisKeyMapping> KeyAxisMappings;
	TArray<FInputActionKeyMapping> KeyMappings;
	void FindAxisMappings(const UInputSettings* InputSettings, const FName AxisName, TArray<FInputAxisKeyMapping>& OutMappings) const;
	void FindActionMappings(const UInputSettings* InputSettings, const FName ActionName, TArray<FInputActionKeyMapping>& OutMappings) const;
	FString SanitizeString(FString& InOutString);
};
