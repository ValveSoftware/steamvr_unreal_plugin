#pragma once

#include "IInputDevice.h"
#include "Core.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "SteamVRInputTypes.h"
#include "SteamVRKnucklesKeys.h"

class FSteamVRInputDevice : public IInputDevice
{
public:
	FSteamVRInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler);
	~FSteamVRInputDevice();

	virtual void Tick(float DeltaTime) override;

	virtual void SendControllerEvents() override;

	virtual void SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;

	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

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
	int32 UnrealControllerHandUsageCount[CONTROLLERS_PER_PLAYER];

	/* Controller states */
	FInputDeviceState ControllerStates[SteamVRInputDeviceConstants::MaxControllers];

	TArray<FSteamVRInputAction> Actions;
	TArray<FControllerType> ControllerTypes;
	VRActionSetHandle_t MainActionSet;

	VRActionHandle_t VRSkeletalHandleLeft;
	VRActionHandle_t VRSkeletalHandleRight;

	VRSkeletalSummaryData_t VRSkeletalSummaryDataLeft;
	VRSkeletalSummaryData_t VRSkeletalSummaryDataRight;

	EVRInputError LastInputError = vr::VRInputError_None;

	float InitialButtonRepeatDelay;
	float ButtonRepeatDelay;
	FGamepadKeyNames::Type Buttons[vr::k_unMaxTrackedDeviceCount][ESteamVRInputButton::TotalButtonCount];

private:
	/* VR Input Error Handler */
	void GetInputError(EVRInputError InputError, FString InputAction);

	/* Message handler */
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;

	/* Previous SteamVR Error **/
	EVRInputError PrevSteamVRError = VRInputError_None;

	// Motion Controller Helper Functions
	void SetUnrealControllerIdToControllerIndex(const int32 UnrealControllerId, const EControllerHand Hand, int32 value);
	void InitControllerMappings();
	void InitKnucklesControllerKeys();

	// Action event processing
	void SendDeviceInputEvents();
	void SendSkeletalInputEvents();

	// Controller Bindings Helper Functions (Editor Only)
#if WITH_EDITOR
	FDelegateHandle ActionMappingsChangedHandle;
	void GenerateControllerBindings(const FString& BindingsPath, TArray<FControllerType>& InOutControllerTypes, TArray<TSharedPtr<FJsonValue>>& InOutDefaultBindings, TArray<FSteamVRInputAction>& InActionsArray, TArray<FInputMapping>& InInputMapping);
	void GenerateActionBindings(TArray<FInputMapping> &InInputMapping, TArray<TSharedPtr<FJsonValue>> &JsonValuesArray);
#endif

	// Action Manifest and Bindings Helper Functions
	void GenerateActionManifest();
	bool BuildJsonObject(TArray<FString> StringFields, TSharedRef<FJsonObject> OutJsonObject);
	void ProcessKeyInputMappings(const UInputSettings* InputSettings, TArray<FName> &InOutUniqueInputs);
	void ProcessKeyAxisMappings(const UInputSettings* InputSettings, TArray<FName> &InOutUniqueInputs);

	void RegisterDeviceChanges();
	bool RegisterController(uint32 DeviceIndex);
	void DetectHandednessSwap();
	bool RegisterTracker(uint32 DeviceIndex);
	void UnregisterController(uint32 DeviceIndex);
	void UnregisterTracker(uint32 DeviceIndex);
	void UnregisterDevice(uint32 DeviceIndex);

	static bool MatchKeyNamePrefix(const FKey& Key, const TCHAR* Prefix)
	{
		// TODO PRIORITY: Implement
		return true;
	};

	static bool MatchKeyNameSuffix(const FKey& Key, const TCHAR* Suffix)
	{
		// TODO PRIORITY: Implement
		return true;
	};

	static FName FindAxisKeyMapping(TArray<FInputAxisKeyMapping>& Mappings, bool& bOutIsXAxis)
	{
		// TODO PRIORITY: Implement
		FName temp;
		return temp;
	}

	static FString MergeActionNames(const FString& A, const FString& B)
	{
		// TODO PRIORITY: Implement
		FString temp;
		return temp;
	}
};
