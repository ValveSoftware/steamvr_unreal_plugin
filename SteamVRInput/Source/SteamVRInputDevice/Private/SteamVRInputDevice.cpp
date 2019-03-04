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
	GenerateActionManifest();

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
/* Editor Only - Generate the SteamVR Controller Binding files */
void FSteamVRInputDevice::GenerateControllerBindings(const FString& BindingsPath, TArray<FControllerType>& InOutControllerTypes, TArray<TSharedPtr<FJsonValue>>& DefaultBindings, TArray<FSteamVRInputAction>& InActionsArray, TArray<FInputMapping>& InInputMapping)
{
	// Create the bindings directory if it doesn't exist
	IFileManager& FileManager = FFileManagerGeneric::Get();
	if (!FileManager.DirectoryExists(*BindingsPath))
	{
		FileManager.MakeDirectory(*BindingsPath);
	}

	// Go through all supported controller types
	for (auto& SupportedController : InOutControllerTypes)
	{
		// If there is no user-defined controller binding or it hasn't been auto-generated yet, generate it
		if (!SupportedController.bIsGenerated)
		{
			// Creating bindings file
			TSharedRef<FJsonObject> BindingsObject = MakeShareable(new FJsonObject());
			BindingsObject->SetStringField(TEXT("name"), TEXT("Default bindings for ") + SupportedController.Description);
			BindingsObject->SetStringField(TEXT("controller_type"), SupportedController.Name.ToString());

			// Create Action Bindings in JSON Format
			TArray<TSharedPtr<FJsonValue>> JsonValuesArray;
			GenerateActionBindings(InInputMapping, JsonValuesArray);

			//Create Action Set
			TSharedRef<FJsonObject> ActionSetJsonObject = MakeShareable(new FJsonObject());
			ActionSetJsonObject->SetArrayField(TEXT("sources"), JsonValuesArray);

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

			// Set description of Bindings file to the Project Name
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

			// Save controller binding
			FString BindingsFilePath = BindingsPath / SupportedController.Name.ToString() + TEXT(".json");
			FString OutputJsonString;
			TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&OutputJsonString);
			FJsonSerializer::Serialize(BindingsObject, JsonWriter);
			FFileHelper::SaveStringToFile(OutputJsonString, *BindingsFilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);

			// Create Controller Binding Object for this binding file
			TSharedRef<FJsonObject> ControllerBindingObject = MakeShareable(new FJsonObject());
			TArray<FString> ControllerStringFields = { "controller_type", *SupportedController.Name.ToString(),
											 TEXT("binding_url"), *FileManager.ConvertToAbsolutePathForExternalAppForRead(*BindingsFilePath)
			};
			BuildJsonObject(ControllerStringFields, ControllerBindingObject);
			DefaultBindings.Add(MakeShareable(new FJsonValueObject(ControllerBindingObject)));

			// Tag this controller as generated
			SupportedController.bIsGenerated = true;
		}
	}
}

/* Editor Only - Utility function that generates appropriate action bindings based on UE Input Mappings */
void FSteamVRInputDevice::GenerateActionBindings(TArray<FInputMapping> &InInputMapping, TArray<TSharedPtr<FJsonValue>> &JsonValuesArray)
{
	for (int i = 0; i < InInputMapping.Num(); i++)
	{
		for (int32 j = 0; j < InInputMapping[i].Actions.Num(); j++)
		{
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
			bIsAxis = false;

			if (InInputMapping[i].InputKey.ToString().Contains(TEXT("_X"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				InInputMapping[i].InputKey.ToString().Contains(TEXT("X-Axis"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				InInputMapping[i].InputKey.ToString().Contains(TEXT("_Y"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				InInputMapping[i].InputKey.ToString().Contains(TEXT("_YAxis"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				InInputMapping[i].InputKey.ToString().Contains(TEXT("_Z"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				InInputMapping[i].InputKey.ToString().Contains(TEXT("_ZAxis"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				InInputMapping[i].InputKey.ToString().Contains(TEXT("Grip"), ESearchCase::CaseSensitive, ESearchDir::FromEnd))
			{
				bIsAxis = true;
			}
			else
			{
				// Check if any of the actions associated with this Input Key have the [XD] axis designation
				for (auto& Action : InInputMapping[i].Actions)
				{
					if (Action.Right(2) == TEXT("D]"))
					{
						bIsAxis = true;
						break;
					}
				}
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
			TSharedPtr<FJsonObject> ActionInputJsonObject = MakeShareable(new FJsonObject());

			// Create Action Path
			TSharedRef<FJsonObject> ActionPathJsonObject = MakeShareable(new FJsonObject());
			ActionPathJsonObject->SetStringField(TEXT("output"), InInputMapping[i].Actions[j]);

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

			// Set Inputs
			ActionSourceJsonObject->SetObjectField(TEXT("inputs"), ActionInputJsonObject);

			// Add to Sources Array
			TSharedRef<FJsonValueObject> JsonValueObject = MakeShareable(new FJsonValueObject(ActionSourceJsonObject));
			JsonValuesArray.Add(JsonValueObject);
		}
	}
}
#endif

/* Generate the SteamVR Input Action Manifest file*/
void FSteamVRInputDevice::GenerateActionManifest()
{
	// Set Action Manifest Path
	const FString ManifestPath = FPaths::GeneratedConfigDir() / ACTION_MANIFEST;
	UE_LOG(LogSteamVRInputDevice, Display, TEXT("Action Manifest Path: %s"), *ManifestPath);

	// Create Action Manifest json object
	TSharedRef<FJsonObject> ActionManifestObject = MakeShareable(new FJsonObject());
	TArray<FString> LocalizationFields = { "language_tag", "en"  };

	// Set where to look for controller binding files and prepare file manager
	const FString ControllerBindingsPath = FPaths::ProjectConfigDir() / CONTROLLER_BINDING_PATH;
	UE_LOG(LogSteamVRInputDevice, Display, TEXT("Controller Bindings Path: %s"), *ControllerBindingsPath);
	IFileManager& FileManager = FFileManagerGeneric::Get();

	// Define Controller Types supported by SteamVR
	TArray<TSharedPtr<FJsonValue>> ControllerBindings;
	ControllerTypes.Empty();
	ControllerTypes.Emplace(FControllerType(TEXT("knuckles"), TEXT("Knuckles Controllers")));
	ControllerTypes.Emplace(FControllerType(TEXT("vive_controller"), TEXT("Vive Controllers")));
	ControllerTypes.Emplace(FControllerType(TEXT("vive_tracker"), TEXT("Vive Trackers")));
	ControllerTypes.Emplace(FControllerType(TEXT("vive"), TEXT("Vive")));
	ControllerTypes.Emplace(FControllerType(TEXT("oculus_touch"), TEXT("Oculus Touch")));
	ControllerTypes.Emplace(FControllerType(TEXT("holographic_controller"), TEXT("Holographic Controller")));
	ControllerTypes.Emplace(FControllerType(TEXT("gamepad"), TEXT("Gamepads")));

	#pragma region ACTION SETS
		// Setup action set json objects
		TArray<TSharedPtr<FJsonValue>> ActionSets;
		TSharedRef<FJsonObject> ActionSetObject = MakeShareable(new FJsonObject());
	
		// Create action set objects
		TArray<FString> StringFields = { 
										 "name", TEXT(ACTION_SET),
	    								 "usage", TEXT("leftright")
										};
		{
			BuildJsonObject(StringFields, ActionSetObject);

			// Add action sets array to the Action Manifest object
			ActionSets.Add(MakeShareable(new FJsonValueObject(ActionSetObject)));
			ActionManifestObject->SetArrayField(TEXT("action_sets"), ActionSets);

			// Set localization text for the action set
			LocalizationFields.Add(TEXT(ACTION_SET));
			LocalizationFields.Add("Main Game Actions");
		}
	#pragma endregion

	#pragma region ACTIONS
		// Clear Actions cache
		Actions.Empty();
		
		// Setup Input Mappings cache
		TArray<FInputMapping> InputMappings;
		TArray<FName> UniqueInputs;

		// Check if this project have input settings
		auto InputSettings = GetDefault<UInputSettings>();
		if (InputSettings->IsValidLowLevelFast())
		{
			// Add project's input key mappings to SteamVR's Input Actions
			ProcessKeyInputMappings(InputSettings, UniqueInputs);

			// Add project's input axis mappings to SteamVR's Input Actions
			ProcessKeyAxisMappings(InputSettings, UniqueInputs);

			// Process all actions in this project (if any)
			TArray<TSharedPtr<FJsonValue>> InputActionsArray;

			// Setup cache for actions
			TArray<FString> UniqueActions;

			// Reorganize all unique inputs to SteamVR style Input-to-Actions association
			for (FName UniqueInput : UniqueInputs)
			{
				// Create New Input Mapping from Unique Input Key
				FInputMapping NewInputMapping = FInputMapping();
				FInputMapping NewAxisMapping = FInputMapping();
				NewInputMapping.InputKey = UniqueInput;
				NewAxisMapping.InputKey = UniqueInput;

				// Go through all the project actions
				for (FSteamVRInputAction& Action : Actions)
				{
					// Check for boolean/digital input
					if (Action.Type == EActionType::Boolean)
					{
						// Set Key Actions Linked To This Input Key
						TArray<FInputActionKeyMapping> KeyMappings;
						InputSettings->GetActionMappingByName(Action.Name, KeyMappings);
						for (FInputActionKeyMapping KeyMapping : KeyMappings)
						{
							if (UniqueInput.IsEqual(KeyMapping.Key.GetFName()))
							{
								NewInputMapping.Actions.AddUnique(Action.Path);
							}
						}
					}

					// Check for axes/analog input
					if (Action.Type == EActionType::Vector1 || Action.Type == EActionType::Vector2 || Action.Type == EActionType::Vector3)
					{
						// Set Axis Actions Linked To This Input Key
						FString ActionAxis = Action.Name.ToString().LeftChop(5); // Remove [XD] Axis indicator before doing any comparisons

						// Parse comma delimited action names into an array
						TArray<FString> ActionAxisArray;
						ActionAxis.ParseIntoArray(ActionAxisArray, TEXT(","), true);

						for (auto& ActionAxisName : ActionAxisArray)
						{
							TArray<FInputAxisKeyMapping> AxisMappings;
							InputSettings->GetAxisMappingByName(FName(*ActionAxisName), AxisMappings);
	
							for (FInputAxisKeyMapping AxisMapping : AxisMappings)
							{
								if (UniqueInput.IsEqual(AxisMapping.Key.GetFName()))
								{
									// Check for X Axis
									if (Action.KeyX.IsValid() && Action.KeyX.IsEqual(AxisMapping.Key.GetFName()))
									{
										// Add 1D Action
										NewAxisMapping.Actions.AddUnique(Action.Path);
	
										FString ActionDimension = Action.Name.ToString().Right(4);
	
										if (ActionDimension == TEXT("[2D]"))
										{
											// Add 2D Action
											FString Action2D = Action.Path.LeftChop(5) + TEXT(" [2D]");
											NewAxisMapping.Actions.AddUnique(Action2D);
										}
	
										if (ActionDimension == TEXT("[3D]"))
										{
											// Add 3D Action
											FString Action3D = Action.Path.LeftChop(5) + TEXT(" [3D]");
											NewAxisMapping.Actions.AddUnique(Action3D);
										}
									}
								}
							}
						}
					}

					// Setup the action fields
					TArray<FString> ActionFields = {
													 TEXT("name"), Action.Path,
													 TEXT("type"), Action.GetActionTypeName(),
					};

					// Add optional field if this isn't a required field
					if (!Action.bRequirement)
					{
						FString Optional[] = { TEXT("requirement"), TEXT("optional") };
						ActionFields.Append(Optional, 2);
					}

					if (!UniqueActions.Contains(Action.Name.ToString()))
					{
						// Add this action to the array of input actions
						TSharedRef<FJsonObject> ActionObject = MakeShareable(new FJsonObject());
						BuildJsonObject(ActionFields, ActionObject);
						InputActionsArray.AddUnique(MakeShareable(new FJsonValueObject(ActionObject)));

						// Add this action to a cache of unique actions for this project
						UniqueActions.AddUnique(Action.Name.ToString());

						// Set localization text for this action
						FString LocalizationArray[] = { Action.Path, Action.Name.ToString() };
						LocalizationFields.Append(LocalizationArray, 2);
					}
				}

				// Add this Input Mapping to the main Input Mappings array
				if (NewInputMapping.Actions.Num() > 0)
				{
					InputMappings.Add(NewInputMapping);
				}

				// Add this Axis Mapping to the main Input Mappings array
				if (NewAxisMapping.Actions.Num() > 0)
				{
					InputMappings.Add(NewAxisMapping);
				}
			}

			// If there are input actions, add them to the action manifest object
			if (InputActionsArray.Num() > 0)
			{
				ActionManifestObject->SetArrayField(TEXT("actions"), InputActionsArray);
			}
		}
		else
		{
			UE_LOG(LogSteamVRInputDevice, Error, TEXT("This project does not have any Input Actions defined! Please add Action to Input Mappings in Project Settings > Engine > Input."));
		}
	#pragma endregion

	#pragma region DEFAULT CONTROLLER BINDINGS
			// Start search for controller bindings files
			TArray<FString> ControllerBindingFiles;
			FileManager.FindFiles(ControllerBindingFiles, *ControllerBindingsPath, TEXT("*.json"));
			UE_LOG(LogSteamVRInputDevice, Log, TEXT("Searching for Controller Bindings files at: %s"), *ControllerBindingsPath);

			// Look for existing controller binding files
			for (FString& BindingFile : ControllerBindingFiles)
			{
				// Setup cache
				FString StringCache;
				FString ControllerType;

				// Load Binding File to a string
				FFileHelper::LoadFileToString(StringCache, *(ControllerBindingsPath / BindingFile));

				// Convert string to json object
				TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(StringCache);
				TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

				// Attempt to deserialize string cache to a json object
				if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
				{
					UE_LOG(LogSteamVRInputDevice, Warning, TEXT("Invalid json format for controller binding file, skipping: %s"), *(ControllerBindingsPath / BindingFile));
				}
				// Attempt to find what controller this binding file is for (yeah ended this comment with a preposition)
				else if (!JsonObject->TryGetStringField(TEXT("controller_type"), ControllerType) || ControllerType.IsEmpty())
				{
					UE_LOG(LogSteamVRInputDevice, Warning, TEXT("Unable to determine controller type for this binding file, skipping: %s"), *(ControllerBindingsPath / BindingFile));
				}
				else
				{
					// Create Controller Binding Object for this binding file
					TSharedRef<FJsonObject> ControllerBindingObject = MakeShareable(new FJsonObject());
					TArray<FString> ControllerStringFields = { "controller_type", *ControllerType,
													 TEXT("binding_url"), *FileManager.ConvertToAbsolutePathForExternalAppForRead(*(ControllerBindingsPath / BindingFile))
					};
					BuildJsonObject(ControllerStringFields, ControllerBindingObject);
					ControllerBindings.Add(MakeShareable(new FJsonValueObject(ControllerBindingObject)));

					// Tag this controller as generated
					for (auto& DefaultControllerType : ControllerTypes)
					{
						if (DefaultControllerType.Name == FName(*ControllerType))
						{
							DefaultControllerType.bIsGenerated = true;
						}
					}
				}
			}

			// If we're running in the editor, build the controller bindings if they don't exist yet
	#if WITH_EDITOR
			GenerateControllerBindings(ControllerBindingsPath, ControllerTypes, ControllerBindings, Actions, InputMappings);
	#endif

			// Add the default bindings object to the action manifest
			if (ControllerBindings.Num() == 0)
			{
				UE_LOG(LogSteamVRInputDevice, Error, TEXT("Unable to find and/or generate controller binding files in: %s"), *ControllerBindingsPath);
			}
			else
			{
				ActionManifestObject->SetArrayField(TEXT("default_bindings"), ControllerBindings);
			}
	#pragma endregion

	#pragma region LOCALIZATION
		// Setup localizations json objects
		TArray<TSharedPtr<FJsonValue>> Localizations;
		TSharedRef<FJsonObject> LocalizationsObject = MakeShareable(new FJsonObject());

		// Build & add localizations to the Action Manifest object
		BuildJsonObject(LocalizationFields, LocalizationsObject);
		Localizations.Add(MakeShareable(new FJsonValueObject(LocalizationsObject)));
		ActionManifestObject->SetArrayField(TEXT("localization"), Localizations);
	#pragma endregion

	// Serialize Action Manifest Object
	FString ActionManifest;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&ActionManifest);
	FJsonSerializer::Serialize(ActionManifestObject, JsonWriter);

	// Save json as a UTF8 file
	if (!FFileHelper::SaveStringToFile(ActionManifest, *ManifestPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("Error trying to generate action manifest in: %s"), *ManifestPath);
		return;
	}
}

/* Build a JSON object made up of string fields, all entries must be paired */
bool FSteamVRInputDevice::BuildJsonObject(TArray<FString> StringFields, TSharedRef<FJsonObject> OutJsonObject)
{
	// Check if StringFields array is even
	if (StringFields.Num() > 1 && StringFields.Num() % 2 == 0)
	{
		// Generate json object of string field pairs
		for (int32 i = 0; i < StringFields.Num(); i+=2)
		{
			OutJsonObject->SetStringField(StringFields[i], StringFields[i+1]);
		}

		return true;
	}

	return false;
}

void FSteamVRInputDevice::ProcessKeyInputMappings(const UInputSettings* InputSettings, TArray<FName> &InOutUniqueInputs)
{
	// Retrieve key actions setup in this project
	TArray<FName> KeyActionNames;
	InputSettings->GetActionNames(KeyActionNames);

	// Process all key actions found
	for (const FName& KeyActionName : KeyActionNames)
	{
		// Retrieve input keys associated with this action
		TArray<FInputActionKeyMapping> KeyMappings;
		InputSettings->GetActionMappingByName(KeyActionName, KeyMappings);

		for (auto& KeyMapping : KeyMappings)
		{
			if (KeyMapping.Key.GetFName().ToString().Contains(TEXT("MotionController")) ||
				KeyMapping.Key.GetFName().ToString().Contains(TEXT("SteamVR")) ||
				KeyMapping.Key.GetFName().ToString().Contains(TEXT("Oculus")))
			{
				// If there's a Motion Controller or valid device input, add to the SteamVR Input Actions
				Actions.Add(FSteamVRInputAction(
					FString(ACTION_PATH_IN) / KeyActionName.ToString(),
					KeyActionName,
					KeyMapping.Key.GetFName(),
					false));

				// Add input names here for use in the auto-generation of controller bindings
				InOutUniqueInputs.AddUnique(KeyMapping.Key.GetFName());
			}
		}
	}
}

void FSteamVRInputDevice::ProcessKeyAxisMappings(const UInputSettings* InputSettings, TArray<FName> &InOutUniqueInputs)
{
	// Retrieve Key Axis names
	TArray<FName> KeyAxisNames;
	InputSettings->GetAxisNames(KeyAxisNames);

	// [1D] All Axis Mappings will have a corresponding Vector1 Action associated with them
	for (const FName& XAxisName : KeyAxisNames)
	{
		// Set X Axis Key Name Cache
		FName XAxisNameKey = NAME_None;

		// Retrieve input axes associated with this action
		TArray<FInputAxisKeyMapping> AxisMappings;
		InputSettings->GetAxisMappingByName(XAxisName, AxisMappings);

		// Go through all axis mappings
		for (auto& AxisMapping : AxisMappings)
		{
			// Add axes names here for use in the auto-generation of controller bindings
			InOutUniqueInputs.AddUnique(AxisMapping.Key.GetFName());

			// Set Key Name
			XAxisNameKey = AxisMapping.Key.GetFName();

			// If this is an X Axis key, check for the corresponding Y & Z Axes as well
			FString KeySuffix = (AxisMapping.Key.GetFName().ToString()).Right(6);
			if (KeySuffix.Contains(TEXT("_X"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
				KeySuffix.Contains(TEXT("X-Axis"), ESearchCase::CaseSensitive, ESearchDir::FromEnd)
				)
			{
				// Axes caches
				FName YAxisName = NAME_None;
				FName ZAxisName = NAME_None;
				FName YAxisNameKey = NAME_None;
				FName ZAxisNameKey = NAME_None;

				// Go through all the axis names again looking for Y and Z inputs that correspond to this X input
				for (const FName& KeyAxisNameInner : KeyAxisNames)
				{
					// Retrieve input axes associated with this action
					TArray<FInputAxisKeyMapping> AxisMappingsInner;
					InputSettings->GetAxisMappingByName(KeyAxisNameInner, AxisMappingsInner);

					for (auto& AxisMappingInner : AxisMappingsInner)
					{
						// Find Y & Z axes
						FString KeyNameSuffix = (AxisMappingInner.Key.GetFName().ToString()).Right(6);
	
						// Populate Axes Caches
						if (KeyNameSuffix.Contains(TEXT("_Y"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
							KeyNameSuffix.Contains(TEXT("Y-Axis"), ESearchCase::CaseSensitive, ESearchDir::FromEnd)
							)
						{
							YAxisName = KeyAxisNameInner;
							YAxisNameKey = AxisMappingInner.Key.GetFName();
						}
						else if (KeyNameSuffix.Contains(TEXT("_Z"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
							KeyNameSuffix.Contains(TEXT("Z-Axis"), ESearchCase::CaseSensitive, ESearchDir::FromEnd)
							)
						{
							ZAxisName = KeyAxisNameInner;
							ZAxisNameKey = AxisMappingInner.Key.GetFName();
						}

						// Add axes names here for use in the auto-generation of controller bindings
						//InOutUniqueInputs.AddUnique(AxisMappingInner.Key.GetFName());
					}
				}

				if (YAxisName != NAME_None && ZAxisName == NAME_None)
				{
					// [2D] There's a Y Axis but no Z, this must be a Vector2
					FString AxisName2D = XAxisName.ToString() +
						TEXT(",") +
						YAxisName.ToString() +
						TEXT(" [2D]");
					FString ActionPath2D = FString(ACTION_PATH_IN) / AxisName2D;
					Actions.Add(FSteamVRInputAction(ActionPath2D, FName(*AxisName2D), XAxisNameKey, YAxisNameKey, FVector2D()));
				}
				else if (YAxisName != NAME_None && ZAxisName != NAME_None)
				{
					// [3D] There's a Z Axis, this must be a Vector3
					FString AxisName3D = XAxisName.ToString() +
						TEXT(",") +
						YAxisName.ToString() + TEXT(",") +
						ZAxisName.ToString() +
						TEXT(" [3D]");
					FString ActionPath3D = FString(ACTION_PATH_IN) / AxisName3D;
					Actions.Add(FSteamVRInputAction(ActionPath3D, FName(*AxisName3D), XAxisNameKey, YAxisNameKey, ZAxisNameKey, FVector()));
				}
			}

			// If we find at least one valid, then add this action to the list of SteamVR Input Actions as Vector1
			if (!XAxisName.IsNone())
			{
				// [1D] Populate all Vector1 actions
				FString AxisName1D = XAxisName.ToString() + TEXT(" [1D]");
				FString ActionPath = FString(ACTION_PATH_IN) / AxisName1D;
				Actions.Add(FSteamVRInputAction(ActionPath, FName(*AxisName1D), XAxisNameKey, 0.0f));
			}
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
