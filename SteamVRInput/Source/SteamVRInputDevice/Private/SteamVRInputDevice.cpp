#include "SteamVRInputDevice.h"
#include "IInputInterface.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/FileHelper.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#endif

#if WITH_EDITOR
#include "Editor.h"
#endif

#define LOCTEXT_NAMESPACE "SteamVRInputDevice"
DEFINE_LOG_CATEGORY_STATIC(LogSteamVRInputDevice, Log, All);

FSteamVRInputDevice::FSteamVRInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) 
	: MessageHandler(InMessageHandler)
{
	FMemory::Memzero(ControllerStates, sizeof(ControllerStates));
	NumControllersMapped = 0;
	NumTrackersMapped = 0;

	InitialButtonRepeatDelay = 0.2f;
	ButtonRepeatDelay = 0.1f;

	InitControllerMappings();
	InitKnucklesControllerKeys();
	BuildActionManifest();

	// Initialize OpenVR
	EVRInitError SteamVRInitError = VRInitError_None;
	SteamVRSystem = VR_Init(&SteamVRInitError, VRApplication_Scene);

	if (SteamVRInitError != VRInitError_None)
	{
		SteamVRSystem = NULL;
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("Unable to init SteamVR runtime %u.%u.%u: %s"), k_nSteamVRVersionMajor, k_nSteamVRVersionMinor, k_nSteamVRVersionBuild, *FString(VR_GetVRInitErrorAsEnglishDescription(SteamVRInitError)));
		return;
	}
	else
	{
		UE_LOG(LogSteamVRInputDevice, Display, TEXT("SteamVR runtime %u.%u.%u initialised with status: %s"), k_nSteamVRVersionMajor, k_nSteamVRVersionMinor, k_nSteamVRVersionBuild, *FString(VR_GetVRInitErrorAsEnglishDescription(SteamVRInitError)));

		for (unsigned int id = 0; id < k_unMaxTrackedDeviceCount; ++id)
		{
			ETrackedDeviceClass trackedDeviceClass = SteamVRSystem->GetTrackedDeviceClass(id);
			char buf[32];
			
			if (trackedDeviceClass != ETrackedDeviceClass::TrackedDeviceClass_Invalid)
			{
				uint32 StringBytes = SteamVRSystem->GetStringTrackedDeviceProperty(id, ETrackedDeviceProperty::Prop_ModelNumber_String, buf, sizeof(buf));
				FString stringCache = *FString(UTF8_TO_TCHAR(buf));
				UE_LOG(LogSteamVRInputDevice, Display, TEXT("Found the following device: [%i] %s"), id, *stringCache);
			}
		}
	}
}

FSteamVRInputDevice::~FSteamVRInputDevice()
{
#if WITH_EDITOR
	if (ActionMappingsChangedHandle.IsValid())
	{
		FEditorDelegates::OnActionAxisMappingsChanged.Remove(ActionMappingsChangedHandle);
		ActionMappingsChangedHandle.Reset();
	}
#endif
}

void FSteamVRInputDevice::Tick(float DeltaTime)
{
	// Check for changes in active controller
	if (SteamVRSystem != NULL)
	{
		RegisterDeviceChanges();
		DetectHandednessSwap();
	}

	// Send Skeletal data
	// TODO: Add check to control skeletal data flow
	SendSkeletalInputEvents();
}

void FSteamVRInputDevice::SendSkeletalInputEvents()
{
	if (SteamVRSystem != NULL && VRInput() != nullptr)
	{
		VRActiveActionSet_t ActiveActionSets[] = {
			{
				MainActionSet,
				k_ulInvalidInputValueHandle,
				k_ulInvalidActionSetHandle
			}
		};

		EVRInputError Err = VRInput()->UpdateActionState(ActiveActionSets, sizeof(VRActiveActionSet_t), 1);
		if (Err != VRInputError_None && Err != LastInputError)
		{
			UE_LOG(LogSteamVRInputDevice, Warning, TEXT("UpdateActionState returned error: %d"), (int32)Err);
			return;
		}
		Err = LastInputError;

		// Process Skeletal Data
		for (uint32 DeviceIndex = 0; DeviceIndex < k_unMaxTrackedDeviceCount; ++DeviceIndex)
		{
			// see what kind of hardware this is
			ETrackedDeviceClass DeviceClass = SteamVRSystem->GetTrackedDeviceClass(DeviceIndex);

			// skip non-controller or non-tracker devices
			if (DeviceClass != TrackedDeviceClass_Controller)
			{
				continue;
			}

			FInputDeviceState& ControllerState = ControllerStates[DeviceIndex];
			EVRSkeletalTrackingLevel vrSkeletalTrackingLevel;
			bool bIsKnucklesLeftPresent = false;
			bool bIsKnucklesRightPresent = false;

			// Set Skeletal Action Handles
			Err = VRInput()->GetActionHandle(TCHAR_TO_ANSI(*FString(TEXT("/actions/main/in/SkeletonLeft"))), &VRSkeletalHandleLeft);
			if ((Err != VRInputError_None || !VRSkeletalHandleLeft) && Err != LastInputError)
			{
				UE_LOG(LogSteamVRInputDevice, Warning, TEXT("Couldn't get skeletal action handle for Left Skeleton. Error: %d"), (int32)Err);
			}
			else
			{
				// Check for Left Knuckles
				VRInput()->GetSkeletalTrackingLevel(VRSkeletalHandleLeft, &vrSkeletalTrackingLevel);
				//UE_LOG(LogKnucklesLivelinkSource, Warning, TEXT("[KNUCKLES VR CONTROLLER] Left Skeletal Tracking Level: %i"), vrSkeletalTrackingLevel);

				if (vrSkeletalTrackingLevel >= VRSkeletalTracking_Partial)
				{
					bIsKnucklesLeftPresent = true;
					//UE_LOG(LogKnucklesLivelinkSource, Warning, TEXT("[KNUCKLES LIVELINK] Knuckles Left found and is ACTIVE"));
				}
			}
			Err = LastInputError;

			Err = VRInput()->GetActionHandle(TCHAR_TO_ANSI(*FString(TEXT("/actions/main/in/SkeletonRight"))), &VRSkeletalHandleRight);
			if ((Err != VRInputError_None || !VRSkeletalHandleRight) && Err != LastInputError)
			{
				UE_LOG(LogSteamVRInputDevice, Warning, TEXT("Couldn't get skeletal action handle for Right Skeleton. Error: %d"), (int32)Err);
			}
			else
			{
				// Check for Right Knuckles
				VRInput()->GetSkeletalTrackingLevel(VRSkeletalHandleRight, &vrSkeletalTrackingLevel);
				//UE_LOG(LogKnucklesLivelinkSource, Warning, TEXT("[KNUCKLES VR CONTROLLER] Left Skeletal Tracking Level: %i"), vrSkeletalTrackingLevel);

				if (vrSkeletalTrackingLevel >= VRSkeletalTracking_Partial)
				{
					bIsKnucklesRightPresent = true;
					//UE_LOG(LogKnucklesLivelinkSource, Warning, TEXT("[KNUCKLES LIVELINK] Knuckles Left found and is ACTIVE"));
				}
			}
			Err = LastInputError;

			// Get Skeletal Summary Data
			VRActionHandle_t ActiveSkeletalHand;
			VRSkeletalSummaryData_t ActiveSkeletalSummaryData;

			if (bIsKnucklesLeftPresent && SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand)
			{
				ActiveSkeletalHand = VRSkeletalHandleLeft;
				ActiveSkeletalSummaryData = VRSkeletalSummaryDataLeft;
			}
			else if (bIsKnucklesRightPresent && SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_RightHand)
			{
				ActiveSkeletalHand = VRSkeletalHandleRight;
				ActiveSkeletalSummaryData = VRSkeletalSummaryDataRight;
			}
			else
			{
				continue;
			}

			// Get Skeletal Summary Data
			Err = VRInput()->GetSkeletalSummaryData(ActiveSkeletalHand, &ActiveSkeletalSummaryData);
			if (Err != VRInputError_None && Err != LastInputError)
			{
				UE_LOG(LogSteamVRInputDevice, Warning, TEXT("Unable to read Skeletal Summary Data: %d"), (int32)Err);
			}
			LastInputError = Err;

			// Knuckles Finger Curls
			if (ControllerState.IndexGripAnalog != ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Index])
			{
				const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
					KnucklesVRControllerKeyNames::SteamVR_Knuckles_Left_Finger_Index_Curl : KnucklesVRControllerKeyNames::SteamVR_Knuckles_Right_Finger_Index_Curl;
				ControllerState.IndexGripAnalog = ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Index];
				MessageHandler->OnControllerAnalog(AxisButton, 0, ControllerState.IndexGripAnalog);
			}

			if (ControllerState.MiddleGripAnalog != ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Middle])
			{
				const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
					KnucklesVRControllerKeyNames::SteamVR_Knuckles_Left_Finger_Middle_Curl : KnucklesVRControllerKeyNames::SteamVR_Knuckles_Right_Finger_Middle_Curl;
				ControllerState.MiddleGripAnalog = ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Middle];
				MessageHandler->OnControllerAnalog(AxisButton, 0, ControllerState.MiddleGripAnalog);
			}

			if (ControllerState.RingGripAnalog != ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Ring])
			{
				const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
					KnucklesVRControllerKeyNames::SteamVR_Knuckles_Left_Finger_Ring_Curl : KnucklesVRControllerKeyNames::SteamVR_Knuckles_Right_Finger_Ring_Curl;
				ControllerState.RingGripAnalog = ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Ring];
				MessageHandler->OnControllerAnalog(AxisButton, 0, ControllerState.RingGripAnalog);
			}

			if (ControllerState.PinkyGripAnalog != ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Pinky])
			{
				const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
					KnucklesVRControllerKeyNames::SteamVR_Knuckles_Left_Finger_Pinky_Curl : KnucklesVRControllerKeyNames::SteamVR_Knuckles_Right_Finger_Pinky_Curl;
				ControllerState.PinkyGripAnalog = ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Pinky];
				MessageHandler->OnControllerAnalog(AxisButton, 0, ControllerState.PinkyGripAnalog);
			}

			if (ControllerState.ThumbGripAnalog != ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Thumb])
			{
				const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
					KnucklesVRControllerKeyNames::SteamVR_Knuckles_Left_Finger_Thumb_Curl : KnucklesVRControllerKeyNames::SteamVR_Knuckles_Right_Finger_Thumb_Curl;
				ControllerState.ThumbGripAnalog = ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Thumb];
				MessageHandler->OnControllerAnalog(AxisButton, 0, ControllerState.ThumbGripAnalog);
			}


			// Knuckles Finger Splays
			if (ControllerState.ThumbIndexSplayAnalog != ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Thumb_Index])
			{
				const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
					KnucklesVRControllerKeyNames::SteamVR_Knuckles_Left_Finger_ThumbIndex_Splay : KnucklesVRControllerKeyNames::SteamVR_Knuckles_Right_Finger_ThumbIndex_Splay;
				ControllerState.ThumbIndexSplayAnalog = ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Thumb_Index];
				MessageHandler->OnControllerAnalog(AxisButton, 0, ControllerState.ThumbIndexSplayAnalog);
			}

			if (ControllerState.IndexMiddleSplayAnalog != ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Index_Middle])
			{
				const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
					KnucklesVRControllerKeyNames::SteamVR_Knuckles_Left_Finger_IndexMiddle_Splay : KnucklesVRControllerKeyNames::SteamVR_Knuckles_Right_Finger_IndexMiddle_Splay;
				ControllerState.IndexMiddleSplayAnalog = ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Index_Middle];
				MessageHandler->OnControllerAnalog(AxisButton, 0, ControllerState.IndexMiddleSplayAnalog);
			}

			if (ControllerState.MiddleRingSplayAnalog != ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Middle_Ring])
			{
				const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
					KnucklesVRControllerKeyNames::SteamVR_Knuckles_Left_Finger_RingPinky_Splay : KnucklesVRControllerKeyNames::SteamVR_Knuckles_Right_Finger_RingPinky_Splay;
				ControllerState.MiddleRingSplayAnalog = ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Middle_Ring];
				MessageHandler->OnControllerAnalog(AxisButton, 0, ControllerState.MiddleRingSplayAnalog);
			}

			if (ControllerState.RingPinkySplayAnalog != ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Ring_Pinky])
			{
				const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
					KnucklesVRControllerKeyNames::SteamVR_Knuckles_Left_Finger_RingPinky_Splay : KnucklesVRControllerKeyNames::SteamVR_Knuckles_Right_Finger_RingPinky_Splay;
				ControllerState.RingPinkySplayAnalog = ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Ring_Pinky];
				MessageHandler->OnControllerAnalog(AxisButton, 0, ControllerState.RingPinkySplayAnalog);
			}
		}
	}
}

void FSteamVRInputDevice::SendControllerEvents()
{
	// TODO PRIORITY: Remove extra call
	SendDeviceInputEvents();
}

void FSteamVRInputDevice::SendDeviceInputEvents()
{
	if (SteamVRSystem != NULL && VRInput() != nullptr)
	{
		VRActiveActionSet_t ActiveActionSets[] = {
			{
				MainActionSet,
				k_ulInvalidInputValueHandle,
				k_ulInvalidActionSetHandle
			}
		};

		EVRInputError Err = VRInput()->UpdateActionState(ActiveActionSets, sizeof(VRActiveActionSet_t), 1);
		if (Err != VRInputError_None)
		{
			UE_LOG(LogSteamVRInputDevice, Warning, TEXT("UpdateActionState returned error: %d"), (int32)Err);
			return;
		}

		// Go through Action Sets
		for (auto& Action : Actions)
		{
			// TODO PRIORITY: Implement
		}
	}
}

void FSteamVRInputDevice::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

bool FSteamVRInputDevice::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	return false;
}

void FSteamVRInputDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	// Empty on purpose
}

void FSteamVRInputDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values)
{
	// Empty on purpose
}

void FSteamVRInputDevice::SetUnrealControllerIdToControllerIndex(const int32 UnrealControllerId, const EControllerHand Hand, int32 value)
{
	UnrealControllerIdAndHandToDeviceIdMap[UnrealControllerId][(int32)Hand] = value;
}

void FSteamVRInputDevice::InitControllerMappings()
{
	// TODO PRIORITY: Implement
}

#if WITH_EDITOR
void FSteamVRInputDevice::BuildDefaultActionBindings(const FString& BindingsDir, TArray<TSharedPtr<FJsonValue>>& DefaultBindings, TArray<FSteamVRAction>& InActionsArray, TArray<FInputMapping>& InInputMapping)
{
	IFileManager& FileManager = FFileManagerGeneric::Get();

	TSet<FString> ExistingBindings;
	// TODO PRIORITY: Implement

	if (!FileManager.DirectoryExists(*BindingsDir))
	{
		FileManager.MakeDirectory(*BindingsDir);
	}

	for (auto& Item : CommonControllerTypes)
	{
		if (ExistingBindings.Contains(Item.Key))
		{
			continue;
		}

		FString BindingsPath;
		// TODO PRIORITY: Implement

		// Creating bindings file
		TSharedRef<FJsonObject> BindingsObject = MakeShareable(new FJsonObject());
		BindingsObject->SetStringField(TEXT("name"), *FText::Format(NSLOCTEXT("SteamVR", "DefaultBindingsFor", "Default bindings for {0}"), Item.Value).ToString());
		BindingsObject->SetStringField(TEXT("controller_type"), Item.Key);

		// Create Action Bindings in JSON Format
		TArray<TSharedPtr<FJsonValue>> JsonValuesArray;
		GenerateActionBindings(InInputMapping, JsonValuesArray);

		//Create Action Set
		TSharedRef<FJsonObject> ActionSetJsonObject = MakeShareable(new FJsonObject());
		ActionSetJsonObject->SetArrayField(TEXT("sources"), JsonValuesArray);

		// TODO: Read from mappings instead (low pri) - perhaps more efficient pulling from template file?
		// TODO: Only generate for supported controllers
		// Add Skeleton Mappings
		TArray<TSharedPtr<FJsonValue>> SkeletonValuesArray;

		// Add Skeleton: Left Hand 
		TSharedRef<FJsonObject> SkeletonLeftJsonObject = MakeShareable(new FJsonObject());
		SkeletonLeftJsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_SKELETON_LEFT));
		SkeletonLeftJsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_USER_SKEL_LEFT));

		TSharedRef<FJsonValueObject> SkeletonLeftJsonValueObject = MakeShareable(new FJsonValueObject(SkeletonLeftJsonObject));
		SkeletonValuesArray.Add(SkeletonLeftJsonValueObject);

		// Add Skeleton: Right Hand
		TSharedRef<FJsonObject> SkeletonRightJsonObject = MakeShareable(new FJsonObject());
		SkeletonRightJsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_SKELETON_RIGHT));
		SkeletonRightJsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_USER_SKEL_RIGHT));

		TSharedRef<FJsonValueObject> SkeletonRightJsonValueObject = MakeShareable(new FJsonValueObject(SkeletonRightJsonObject));
		SkeletonValuesArray.Add(SkeletonRightJsonValueObject);

		// Add Skeleton Input Array To Action Set
		ActionSetJsonObject->SetArrayField(TEXT("skeleton"), SkeletonValuesArray);

		// TODO: Read from mappings instead (lowpri) - see similar note to Skeleton Mappings
		// Add Haptic Mappings
		TArray<TSharedPtr<FJsonValue>> HapticValuesArray;

		// Add Haptic: Left Hand 
		TSharedRef<FJsonObject> HapticLeftJsonObject = MakeShareable(new FJsonObject());
		HapticLeftJsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_VIBRATE_LEFT));
		HapticLeftJsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_USER_VIB_LEFT));

		TSharedRef<FJsonValueObject> HapticLeftJsonValueObject = MakeShareable(new FJsonValueObject(HapticLeftJsonObject));
		HapticValuesArray.Add(HapticLeftJsonValueObject);

		// Add Haptic: Right Hand
		TSharedRef<FJsonObject> HapticRightJsonObject = MakeShareable(new FJsonObject());
		HapticRightJsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_VIBRATE_RIGHT));
		HapticRightJsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_USER_VIB_RIGHT));

		TSharedRef<FJsonValueObject> HapticRightJsonValueObject = MakeShareable(new FJsonValueObject(HapticRightJsonObject));
		HapticValuesArray.Add(HapticRightJsonValueObject);

		// Add Haptic Output Array To Action Set
		ActionSetJsonObject->SetArrayField(TEXT("haptics"), HapticValuesArray);

		// Create Bindings File that includes all Action Sets
		TSharedRef<FJsonObject> BindingsJsonObject = MakeShareable(new FJsonObject());
		BindingsJsonObject->SetObjectField(TEXT(ACTION_SET), ActionSetJsonObject);
		BindingsObject->SetObjectField(TEXT("bindings"), BindingsJsonObject);

		// Set description of Bindings stub to Project Name
		if (GConfig)
		{
			// Retrieve Project Name and Version from DefaultGame.ini
			FString ProjectName;
			FString ProjectVersion;

			GConfig->GetString(
				TEXT("/Script/EngineSettings.GeneralProjectSettings"),
				TEXT("ProjectName"),
				ProjectName,
				GGameIni
			);

			// Add Project Name and Version to Bindings stub
			BindingsObject->SetStringField(TEXT("description"), (TEXT("%s"), *ProjectName));
		}
		else
		{
			BindingsObject->SetStringField(TEXT("description"), TEXT("SteamVRInput UE4 Plugin Generated Bindings"));
		}

		// Save bindings file
		// TODO PRIORITY: Implement

		// Add path of generated device input file to the action manifest
		// TODO PRIORITY: Implement
	}
}

void FSteamVRInputDevice::GenerateActionBindings(TArray<FInputMapping> &InInputMapping, TArray<TSharedPtr<FJsonValue>> &JsonValuesArray)
{
	for (int i = 0; i < InInputMapping.Num(); i++)
	{
		// Create Action Input
		TSharedRef<FJsonObject> ActionInputJsonObject = MakeShareable(new FJsonObject());
		//ActionInputJsonObject->SetObjectField(TEXT("force"), ActionPathJsonObject);

		// TODO: Catch curls and splays in action events
		if (InInputMapping[i].InputKey.ToString().Contains(TEXT("Curl"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
			InInputMapping[i].InputKey.ToString().Contains(TEXT("Splay"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
			continue;

		// TODO: Bitmask - consider button press abstractions; perhaps several enums?
		// Set Input Type
		bool bIsAxis;
		bool bIsTrigger;
		bool bIsThumbstick;
		bool bIsTrackpad;
		bool bIsGrip;
		bool bIsCapSense;
		bool bIsLeft;
		bool bIsFaceButton1;   // TODO: Better way of abstracting buttons
		bool bIsFaceButton2;

		// Set Cache Vars
		FName CacheMode;
		FString CacheType;
		FString CachePath;

		if (InInputMapping[i].InputKey.ToString().Contains(TEXT("_X"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
			InInputMapping[i].InputKey.ToString().Contains(TEXT("_Y"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
			InInputMapping[i].InputKey.ToString().Contains(TEXT("Grip"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
			InInputMapping[i].InputKey.ToString().Contains(TEXT("Axis"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
		{
			bIsAxis = true;
		}
		else
		{
			bIsAxis = false;
		}

		bIsTrigger = InInputMapping[i].InputKey.ToString().Contains(TEXT("Trigger"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		bIsThumbstick = InInputMapping[i].InputKey.ToString().Contains(TEXT("Thumbstick"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		bIsTrackpad = InInputMapping[i].InputKey.ToString().Contains(TEXT("Trackpad"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		bIsGrip = InInputMapping[i].InputKey.ToString().Contains(TEXT("Grip"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		bIsCapSense = InInputMapping[i].InputKey.ToString().Contains(TEXT("CapSense"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		bIsLeft = InInputMapping[i].InputKey.ToString().Contains(TEXT("Left"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		bIsFaceButton1 = InInputMapping[i].InputKey.ToString().Contains(TEXT("FaceButton1"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		bIsFaceButton2 = InInputMapping[i].InputKey.ToString().Contains(TEXT("FaceButton2"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);

		// Set Cache Mode
		CacheMode = bIsTrigger || bIsGrip ? FName(TEXT("trigger")) : FName(TEXT("button"));
		CacheMode = bIsTrackpad ? FName(TEXT("trackpad")) : CacheMode;
		CacheMode = bIsGrip ? FName(TEXT("force_sensor")) : CacheMode;
		CacheMode = bIsThumbstick ? FName(TEXT("joystick")) : CacheMode;

		// Set Cache Path
		if (bIsTrigger)
		{
			CachePath = bIsLeft ? FString(TEXT(ACTION_PATH_TRIGGER_LEFT)) : FString(TEXT(ACTION_PATH_TRIGGER_RIGHT));
		}
		else if (bIsThumbstick)
		{
			CachePath = bIsLeft ? FString(TEXT(ACTION_PATH_THUMBSTICK_LEFT)) : FString(TEXT(ACTION_PATH_THUMBSTICK_RIGHT));
		}
		else if (bIsTrackpad)
		{
			CachePath = bIsLeft ? FString(TEXT(ACTION_PATH_TRACKPAD_LEFT)) : FString(TEXT(ACTION_PATH_TRACKPAD_RIGHT));
		}
		else if (bIsGrip)
		{
			CachePath = bIsLeft ? FString(TEXT(ACTION_PATH_GRIP_LEFT)) : FString(TEXT(ACTION_PATH_GRIP_RIGHT));
		}
		else if (bIsFaceButton1)
		{
			CachePath = bIsLeft ? FString(TEXT(ACTION_PATH_BTN_A_LEFT)) : FString(TEXT(ACTION_PATH_BTN_A_RIGHT));
		}
		else if (bIsFaceButton2)
		{
			CachePath = bIsLeft ? FString(TEXT(ACTION_PATH_BTN_B_LEFT)) : FString(TEXT(ACTION_PATH_BTN_B_RIGHT));
		}

		// Create Action Source
		FActionSource ActionSource = FActionSource(CacheMode, CachePath);
		TSharedRef<FJsonObject> ActionSourceJsonObject = MakeShareable(new FJsonObject());
		ActionSourceJsonObject->SetStringField(TEXT("mode"), ActionSource.Mode.ToString());

		// Set Action Path
		ActionSourceJsonObject->SetStringField(TEXT("path"), ActionSource.Path);

		// Set Key Mappings
		for (FString Action : InInputMapping[i].Actions)
		{
			// Create Action Path
			FActionPath ActionPath = FActionPath(Action);
			TSharedRef<FJsonObject> ActionPathJsonObject = MakeShareable(new FJsonObject());
			ActionPathJsonObject->SetStringField(TEXT("output"), ActionPath.Path);

			// Set Cache Type
			if (bIsAxis)
			{
				CacheType = (bIsThumbstick || bIsTrackpad) ? FString(TEXT("position")) : FString(TEXT("pull"));
				CacheType = (bIsGrip) ? FString(TEXT("force")) : CacheType;
			}
			else
			{
				CacheType = (bIsCapSense) ? FString(TEXT("touch")) : FString(TEXT("click"));
			}

			// Set Action Input Type
			ActionInputJsonObject->SetObjectField(CacheType, ActionPathJsonObject);
		}

		// Set Inputs
		ActionSourceJsonObject->SetObjectField(TEXT("inputs"), ActionInputJsonObject);

		// Add to Sources Array
		TSharedRef<FJsonValueObject> JsonValueObject = MakeShareable(new FJsonValueObject(ActionSourceJsonObject));
		JsonValuesArray.Add(JsonValueObject);
	}
}
#endif

void FSteamVRInputDevice::BuildActionManifest()
{
	if (VRInput() != nullptr)
	{

		TArray<FInputMapping> InputMappings;
		TArray<FName> UniqueInputs;

		// Get Project Action Settings
		Actions.Empty();
		auto InputSettings = GetDefault<UInputSettings>();
		if (InputSettings != nullptr)
		{
			// Get all Action Key Mappings
			TArray<FName> ActionNames;
			InputSettings->GetActionNames(ActionNames);
			for (const auto& ActionName : ActionNames)
			{
				// TODO PRIORITY: Implement
			}

			// Get All Action Axis Mappings
			TArray<FName> AxisNames;
			InputSettings->GetAxisNames(AxisNames);
			for (const auto& AxisName : AxisNames)
			{
				// TODO PRIORITY: Implement
			}

			for (FName UniqueInput : UniqueInputs)
			{
				// TODO PRIORITY: Implement
			}

			// Skeletal Data
			{
				FString ConstActionPath = FString(TEXT(ACTION_PATH_SKELETON_LEFT));
				Actions.Add(FSteamVRAction(ConstActionPath, EActionType::Skeleton, true,
					FName(TEXT("Skeleton (Left)")), FString(TEXT(ACTION_PATH_SKEL_HAND_LEFT))));
			}
			{
				FString ConstActionPath = FString(TEXT(ACTION_PATH_SKELETON_RIGHT));
				Actions.Add(FSteamVRAction(ConstActionPath, EActionType::Skeleton, true,
					FName(TEXT("Skeleton (Right)")), FString(TEXT(ACTION_PATH_SKEL_HAND_RIGHT))));
			}

			// Open console
			{
				// TODO PRIORITY: Implement
			}

			// Haptics
			{
				FString ConstActionPath = FString(TEXT(ACTION_PATH_VIBRATE_LEFT));
				Actions.Add(FSteamVRAction(ConstActionPath, EActionType::Vibration, true, FName(TEXT("Haptic (Left)"))));
			}
			{
				FString ConstActionPath = FString(TEXT(ACTION_PATH_VIBRATE_RIGHT));
				Actions.Add(FSteamVRAction(ConstActionPath, EActionType::Vibration, true, FName(TEXT("Haptic (Right)"))));
			}
		}

		if (Actions.Num() > 0)
		{
			const FString ManifestPath = FPaths::GeneratedConfigDir() / ACTION_MANIFEST;
			UE_LOG(LogSteamVRInputDevice, Display, TEXT("Manifest Path: %s"), *ManifestPath);

			const FString BindingsDir;
			UE_LOG(LogSteamVRInputDevice, Display, TEXT("Bindings Path: %s"), *BindingsDir);

			TSharedPtr<FJsonObject> DescriptionsObject = MakeShareable(new FJsonObject);

			TArray<TSharedPtr<FJsonValue>> ActionsArray;
			for (auto Action : Actions)
			{
				TSharedRef<FJsonObject> ActionObject = MakeShareable(new FJsonObject());
				// TODO PRIORITY: Implement

				// Add hand if skeleton
				if (!Action.Skel.IsEmpty())
				{
					ActionObject->SetStringField(TEXT("skeleton"), Action.Skel);
				}

				// Add requirement field for optionals
				// TODO PRIORITY: Implement
			}

			TArray<TSharedPtr<FJsonValue>> DefaultBindings;
			{
				IFileManager& FileManager = FFileManagerGeneric::Get();

				TArray<FString> FoundFiles;
				FileManager.FindFiles(FoundFiles, *BindingsDir, TEXT("*.json"));
				UE_LOG(LogSteamVRInputDevice, Log, TEXT("Searching for device input bindings files in %s"), *BindingsDir);
				for (FString& File : FoundFiles)
				{
					// TODO PRIORITY: Implement
				}

#if WITH_EDITOR
				BuildDefaultActionBindings(BindingsDir, DefaultBindings, Actions, InputMappings);
#else
				// TODO PRIORITY: Implement

#endif
			}

			TArray<TSharedPtr<FJsonValue>> ActionSets;
			{
				// TODO PRIORITY: Implement
			}

			DescriptionsObject->SetStringField(TEXT("language_tag"), TEXT("en"));
			TArray<TSharedPtr<FJsonValue>> Localization;
			{
				Localization.Add(MakeShareable(new FJsonValueObject(DescriptionsObject)));
			}

			TSharedRef<FJsonObject> RootObject = MakeShareable(new FJsonObject());
			// TODO PRIORITY: Implement

			// TODO PRIORITY: Implement

			EVRInputError Err = VRInput()->SetActionManifestPath(TCHAR_TO_ANSI(*IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*ManifestPath)));

			if (Err != VRInputError_None)
			{
				UE_LOG(LogSteamVRInputDevice, Error, TEXT("Failed to pass action manifest, %s, to SteamVR. Error: %d"), *ManifestPath, (int32)Err);
			}

			Err = VRInput()->GetActionSetHandle(ACTION_SET, &MainActionSet);
			if (Err != VRInputError_None)
			{
				UE_LOG(LogSteamVRInputDevice, Error, TEXT("Couldn't get main action set handle. Error: %d"), (int32)Err);
			}

			for (auto& Action : Actions)
			{
				// TODO PRIORITY: Implement
			}

#if WITH_EDITOR
			if (!ActionMappingsChangedHandle.IsValid())
			{
				ActionMappingsChangedHandle = FEditorDelegates::OnActionAxisMappingsChanged.AddLambda([this]()
				{
					UE_LOG(LogSteamVRInputDevice, Warning, TEXT("You will need to quit and restart both SteamVR and the Editor in order to use the modified input actions or axes."));
				});
			}
#endif
		}
	}
}

void FSteamVRInputDevice::RegisterDeviceChanges()
{
	// TODO PRIORITY: Implement
}

bool FSteamVRInputDevice::RegisterController(uint32 DeviceIndex)
{
	// TODO PRIORITY: Implement

	return true;
}

void FSteamVRInputDevice::DetectHandednessSwap()
{
	// TODO PRIORITY: Implement
}

bool FSteamVRInputDevice::RegisterTracker(uint32 DeviceIndex)
{
	// TODO PRIORITY: Implement

	return true;
}

void FSteamVRInputDevice::UnregisterController(uint32 DeviceIndex)
{
	// TODO PRIORITY: Implement
}

void FSteamVRInputDevice::UnregisterTracker(uint32 DeviceIndex)
{
	// TODO PRIORITY: Implement
}

void FSteamVRInputDevice::UnregisterDevice(uint32 DeviceIndex)
{
// TODO PRIORITY: Implement
}
#pragma endregion Kept for backwards compatibility and possible future merge back to Engine, note changes however

void FSteamVRInputDevice::InitKnucklesControllerKeys()
{
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_A_CapSense, LOCTEXT("Knuckles_Left_A_CapSense", "SteamVR Knuckles (L) A CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_A_CapSense, LOCTEXT("Knuckles_Right_A_CapSense", "SteamVR Knuckles (R) A CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_B_CapSense, LOCTEXT("Knuckles_Left_B_CapSense", "SteamVR Knuckles (L) B CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_B_CapSense, LOCTEXT("Knuckles_Right_B_CapSense", "SteamVR Knuckles (R) B CapSense"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Trigger_CapSense, LOCTEXT("Knuckles_Left_Trigger_CapSense", "SteamVR Knuckles (L) Trigger CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Trigger_CapSense, LOCTEXT("Knuckles_Right_Trigger_CapSense", "SteamVR Knuckles (R) Trigger CapSense"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Thumbstick_CapSense, LOCTEXT("Knuckles_Left_Thumbstick_CapSense", "SteamVR Knuckles (L) Thumbstick CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Thumbstick_CapSense, LOCTEXT("Knuckles_Right_Thumbstick_CapSense", "SteamVR Knuckles (R) Thumbstick CapSense"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Trackpad_CapSense, LOCTEXT("Knuckles_Left_Trackpad_CapSense", "SteamVR Knuckles (L) Trackpad CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Trackpad_CapSense, LOCTEXT("Knuckles_Right_Trackpad_CapSense", "SteamVR Knuckles (R) Trackpad CapSense"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Trackpad_GripForce, LOCTEXT("Knuckles_Left_Trackpad_GripForce", "SteamVR Knuckles (L) Trackpad GripForce"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Trackpad_GripForce, LOCTEXT("Knuckles_Right_Trackpad_GripForce", "SteamVR Knuckles (R) Trackpad GripForce"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Trackpad_X, LOCTEXT("Knuckles_Left_Trackpad_X", "SteamVR Knuckles (L) Trackpad X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Trackpad_X, LOCTEXT("Knuckles_Right_Trackpad_X", "SteamVR Knuckles (R) Trackpad X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Trackpad_Y, LOCTEXT("Knuckles_Left_Trackpad_Y", "SteamVR Knuckles (L) Trackpad Y"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Trackpad_Y, LOCTEXT("Knuckles_Right_Trackpad_Y", "SteamVR Knuckles (R) Trackpad Y"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	// Knuckles Curls
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Finger_Index_Curl, LOCTEXT("Knuckles_Left_Finger_Index_Curl", "SteamVR Knuckles (L) Finger Index Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Finger_Index_Curl, LOCTEXT("Knuckles_Right_Finger_Index_Curl", "SteamVR Knuckles (R) Finger Index Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Finger_Middle_Curl, LOCTEXT("Knuckles_Left_Finger_Middle_Curl", "SteamVR Knuckles (L) Finger Middle Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Finger_Middle_Curl, LOCTEXT("Knuckles_Right_Finger_Middle_Curl", "SteamVR Knuckles (R) Finger Middle Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Finger_Ring_Curl, LOCTEXT("Knuckles_Left_Finger_Ring_Curl", "SteamVR Knuckles (L) Finger Ring Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Finger_Ring_Curl, LOCTEXT("Knuckles_Right_Finger_Ring_Curl", "SteamVR Knuckles (R) Finger Ring Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Finger_Pinky_Curl, LOCTEXT("Knuckles_Left_Finger_Pinky_Curl", "SteamVR Knuckles (L) Finger Pinky Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Finger_Pinky_Curl, LOCTEXT("Knuckles_Right_Finger_Pinky_Curl", "SteamVR Knuckles (R) Finger Pinky Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Finger_Thumb_Curl, LOCTEXT("Knuckles_Left_Finger_Thumb_Curl", "SteamVR Knuckles (L) Finger Thumb Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Finger_Thumb_Curl, LOCTEXT("Knuckles_Right_Finger_Thumb_Curl", "SteamVR Knuckles (R) Finger Thumb Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	// Knuckles Splays
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Finger_ThumbIndex_Splay, LOCTEXT("Knuckles_Left_Finger_ThumbIndex_Splay", "SteamVR Knuckles (L) Finger Thumb-Index Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Finger_ThumbIndex_Splay, LOCTEXT("Knuckles_Right_Finger_ThumbIndex_Splay", "SteamVR Knuckles (R) Finger Thumb-Index Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Finger_IndexMiddle_Splay, LOCTEXT("Knuckles_Left_Finger_IndexMiddle_Splay", "SteamVR Knuckles (L) Finger Index-Middle Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Finger_IndexMiddle_Splay, LOCTEXT("Knuckles_Right_Finger_IndexMiddle_Splay", "SteamVR Knuckles (R) Finger Index-Middle Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Finger_MiddleRing_Splay, LOCTEXT("Knuckles_Left_Finger_MiddleRing_Splay", "SteamVR Knuckles (L) Finger Middle-Ring Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Finger_MiddleRing_Splay, LOCTEXT("Knuckles_Right_Finger_MiddleRing_Splay", "SteamVR Knuckles (R) Finger Middle-Ring Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Left_Finger_RingPinky_Splay, LOCTEXT("Knuckles_Left_Finger_RingPinky_Splay", "SteamVR Knuckles (L) Finger Ring-Pinky Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(KnucklesVRControllerKeys::SteamVR_Knuckles_Right_Finger_RingPinky_Splay, LOCTEXT("Knuckles_Right_Finger_RingPinky_Splay", "SteamVR Knuckles (R) Finger Ring-Pinky Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
}

void FSteamVRInputDevice::GetInputError(EVRInputError InputError, FString InputAction)
{
	// TODO: Refactor strings
	switch (InputError)
	{
	case VRInputError_None:
		UE_LOG(LogSteamVRInputDevice, Display, TEXT("[STEAMVR INPUT ERROR] %s: Success"), *InputAction);
		break;
	case VRInputError_NameNotFound:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Name Not Found"), *InputAction);
		break;
	case VRInputError_WrongType:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Wrong Type"), *InputAction);
		break;
	case VRInputError_InvalidHandle:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Invalid Handle"), *InputAction);
		break;
	case VRInputError_InvalidParam:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Invalid Param"), *InputAction);
		break;
	case VRInputError_NoSteam:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: No Steam"), *InputAction);
		break;
	case VRInputError_MaxCapacityReached:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s:  Max Capacity Reached"), *InputAction);
		break;
	case VRInputError_IPCError:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: IPC Error"), *InputAction);
		break;
	case VRInputError_NoActiveActionSet:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: No Active Action Set"), *InputAction);
		break;
	case VRInputError_InvalidDevice:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Invalid Device"), *InputAction);
		break;
	case VRInputError_InvalidSkeleton:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Invalid Skeleton"), *InputAction);
		break;
	case VRInputError_InvalidBoneCount:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Invalid Bone Count"), *InputAction);
		break;
	case VRInputError_InvalidCompressedData:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Invalid Compressed Data"), *InputAction);
		break;
	case VRInputError_NoData:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: No Data"), *InputAction);
		break;
	case VRInputError_BufferTooSmall:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Buffer Too Small"), *InputAction);
		break;
	case VRInputError_MismatchedActionManifest:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Mismatched Action Manifest"), *InputAction);
		break;
	case VRInputError_MissingSkeletonData:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Missing Skeleton Data"), *InputAction);
		break;
	default:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT ERROR] %s: Unknown Error"), *InputAction);
		break;
	}

	return;
}

#undef LOCTEXT_NAMESPACE //"SteamVRInputDevice"
