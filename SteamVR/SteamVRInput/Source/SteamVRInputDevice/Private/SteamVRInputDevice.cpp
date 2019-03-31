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


#include "SteamVRInputDevice.h"
#include "IInputInterface.h"
#include "HAL/FileManagerGeneric.h"
#include "Misc/FileHelper.h"
#include "GameFramework/PlayerInput.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "../../OpenVRSDK/headers/openvr.h"
#include "GameFramework/WorldSettings.h"
#include "Features/IModularFeatures.h"
#include "MotionControllerComponent.h"
#include "IMotionController.h"
#include "SteamVRSkeletonDefinition.h"

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#endif

#if WITH_EDITOR
#include "Editor.h"
#endif

#define LOCTEXT_NAMESPACE "SteamVRInputDevice"
DEFINE_LOG_CATEGORY_STATIC(LogSteamVRInputDevice, Log, All);

// List of bones that are effectively in model space because
// they are children of the root
static const int32 kModelSpaceBones[] = {
	ESteamVRBone_Wrist,
	ESteamVRBone_Aux_Thumb,
	ESteamVRBone_Aux_IndexFinger,
	ESteamVRBone_Aux_MiddleFinger,
	ESteamVRBone_Aux_RingFinger,
	ESteamVRBone_Aux_PinkyFinger,
};

// List of the metacarpal bones of the SteamVR skeleton
static const int32 kMetacarpalBones[] = {
	ESteamVRBone_Thumb0,
	ESteamVRBone_IndexFinger0,
	ESteamVRBone_MiddleFinger0,
	ESteamVRBone_RingFinger0,
	ESteamVRBone_PinkyFinger0
};

// List of bones that only need to have their translation mirrored in the SteamVR skeleton
static const int32 kMirrorTranslationOnlyBones[] = {
	ESteamVRBone_Thumb1,
	ESteamVRBone_Thumb2,
	ESteamVRBone_Thumb3,
	ESteamVRBone_IndexFinger1,
	ESteamVRBone_IndexFinger2,
	ESteamVRBone_IndexFinger3,
	ESteamVRBone_IndexFinger4,
	ESteamVRBone_MiddleFinger1,
	ESteamVRBone_MiddleFinger2,
	ESteamVRBone_MiddleFinger3,
	ESteamVRBone_MiddleFinger4,
	ESteamVRBone_RingFinger1,
	ESteamVRBone_RingFinger2,
	ESteamVRBone_RingFinger3,
	ESteamVRBone_RingFinger4,
	ESteamVRBone_PinkyFinger1,
	ESteamVRBone_PinkyFinger2,
	ESteamVRBone_PinkyFinger3,
	ESteamVRBone_PinkyFinger4
};


FSteamVRInputDevice::FSteamVRInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
	: MessageHandler(InMessageHandler)
{
	FMemory::Memzero(ControllerStates, sizeof(ControllerStates));
	NumControllersMapped = 0;
	NumTrackersMapped = 0;

	InitialButtonRepeatDelay = 0.2f;
	ButtonRepeatDelay = 0.1f;

	InitControllerMappings();
	InitSkeletalControllerKeys();
	GenerateActionManifest();

	// Set Skeletal Handles
	bIsSkeletalControllerLeftPresent = SetSkeletalHandle(TCHAR_TO_UTF8(*FString(TEXT(ACTION_PATH_SKELETON_LEFT))), VRSkeletalHandleLeft);
	bIsSkeletalControllerRightPresent = SetSkeletalHandle(TCHAR_TO_UTF8(*FString(TEXT(ACTION_PATH_SKELETON_RIGHT))), VRSkeletalHandleRight);

#if WITH_EDITOR
	// Auto-enable SteamVR Input Developer Mode 
	if (VRSettings() != nullptr)
	{
		EVRInitError SteamVRInitError = VRInitError_Driver_NotLoaded;
		SteamVRSystem = vr::VR_Init(&SteamVRInitError, vr::VRApplication_Overlay);

		EVRSettingsError BindingFlagError = VRSettingsError_None;
		VRSettings()->SetBool(k_pch_SteamVR_Section, k_pch_SteamVR_DebugInputBinding, true, &BindingFlagError);
		UE_LOG(LogSteamVRInputDevice, Display, TEXT("[STEAMVR INPUT] Enable SteamVR Input Developer Mode: %s"), *FString(UTF8_TO_TCHAR(VRSettings()->GetSettingsErrorNameFromEnum(BindingFlagError))));
		//VRSettings()->SetBool(k_pch_SteamVR_Section, k_pch_SteamVR_DebugInput, true, &BindingFlagError);
		//UE_LOG(LogSteamVRInputDevice, Display, TEXT("[STEAMVR INPUT] Enable SteamVR Debug Input: %s"), *FString(UTF8_TO_TCHAR(VRSettings()->GetSettingsErrorNameFromEnum(BindingFlagError))));

		VR_ShutdownInternal();
	}
#endif

	InitSteamVRSystem();
	IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);
}

FSteamVRInputDevice::~FSteamVRInputDevice()
{
	IModularFeatures::Get().UnregisterModularFeature(GetModularFeatureName(), this);
}

void FSteamVRInputDevice::InitSteamVRSystem()
{
	// Initialize OpenVR
	EVRInitError SteamVRInitError = VRInitError_Driver_NotLoaded;
	SteamVRSystem = vr::VR_Init(&SteamVRInitError, vr::VRApplication_Scene);

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

void FSteamVRInputDevice::Tick(float DeltaTime)
{
	// Send Skeletal data
	SendSkeletalInputEvents();
}

void FSteamVRInputDevice::FindAxisMappings(const UInputSettings* InputSettings, const FName InAxisName, TArray<FInputAxisKeyMapping>& OutMappings) const
{
	if (InAxisName.IsValid())
	{
		for (int32 AxisIndex = InputSettings->AxisMappings.Num() - 1; AxisIndex >= 0; --AxisIndex)
		{
			if (InputSettings->AxisMappings[AxisIndex].AxisName == InAxisName)
			{
				OutMappings.Add(InputSettings->AxisMappings[AxisIndex]);
			}
		}
	}
}

void FSteamVRInputDevice::FindActionMappings(const UInputSettings* InputSettings, const FName InActionName, TArray<FInputActionKeyMapping>& OutMappings) const
{
	if (InActionName.IsValid())
	{
		for (int32 ActionIndex = InputSettings->ActionMappings.Num() - 1; ActionIndex >= 0; --ActionIndex)
		{
			if (InputSettings->ActionMappings[ActionIndex].ActionName == InActionName)
			{
				OutMappings.Add(InputSettings->ActionMappings[ActionIndex]);
			}
		}
	}
}

FString FSteamVRInputDevice::SanitizeString(FString& InString)
{
	FString SanitizedString = InString.Replace(TEXT(" "), TEXT("-"));
	SanitizedString = SanitizedString.Replace(TEXT("*"), TEXT("-"));
	SanitizedString = SanitizedString.Replace(TEXT("."), TEXT("-"));

	return SanitizedString;
}

void FSteamVRInputDevice::SendSkeletalInputEvents()
{
	if (SteamVRSystem && VRInput() && (bCurlsAndSplaysEnabled_L || bCurlsAndSplaysEnabled_R))
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
			GetInputError(Err, TEXT("UpdateActionState returned an error."));
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
				continue;

			char buf[32];
			uint32 StringBytes = SteamVRSystem->GetStringTrackedDeviceProperty(DeviceIndex, ETrackedDeviceProperty::Prop_ModelNumber_String, buf, sizeof(buf));
			FString stringCache = *FString(UTF8_TO_TCHAR(buf));
			//UE_LOG(LogSteamVRInputDevice, Display, TEXT("Found the following device: [%i] %s"), DeviceIndex, *stringCache);

			FInputDeviceState& ControllerState = ControllerStates[DeviceIndex];

			// Get Skeletal Data
			VRActionHandle_t ActiveSkeletalHand;
			VRSkeletalSummaryData_t ActiveSkeletalSummaryData;
			//VRBoneTransform_t ActiveBoneTransform;

			if (bIsSkeletalControllerLeftPresent &&
				(bCurlsAndSplaysEnabled_L) &&
				SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand)
			{
				ActiveSkeletalHand = VRSkeletalHandleLeft;
				ActiveSkeletalSummaryData = VRSkeletalSummaryDataLeft;
			}
			else if (bIsSkeletalControllerRightPresent &&
				(bCurlsAndSplaysEnabled_R) &&
				SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_RightHand)
			{
				ActiveSkeletalHand = VRSkeletalHandleRight;
				ActiveSkeletalSummaryData = VRSkeletalSummaryDataRight;
			}
			else
			{
				continue;
			}

#pragma region CURLS AND SPLAYS
			if (bCurlsAndSplaysEnabled_L || bCurlsAndSplaysEnabled_R)
			{
				// Get Skeletal Summary Data
				Err = VRInput()->GetSkeletalSummaryData(ActiveSkeletalHand, &ActiveSkeletalSummaryData);

				// Skeleton Finger Curls
				if (ControllerState.IndexGripAnalog != ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Index])
				{
					const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
						SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Left_Finger_Index_Curl : SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Right_Finger_Index_Curl;
					ControllerState.IndexGripAnalog = ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Index];
					SendAnalogMessage(SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex), AxisButton, ControllerState.IndexGripAnalog);
				}

				if (ControllerState.MiddleGripAnalog != ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Middle])
				{
					const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
						SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Left_Finger_Middle_Curl : SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Right_Finger_Middle_Curl;
					ControllerState.MiddleGripAnalog = ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Middle];
					SendAnalogMessage(SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex), AxisButton, ControllerState.MiddleGripAnalog);
				}

				if (ControllerState.RingGripAnalog != ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Ring])
				{
					const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
						SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Left_Finger_Ring_Curl : SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Right_Finger_Ring_Curl;
					ControllerState.RingGripAnalog = ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Ring];
					SendAnalogMessage(SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex), AxisButton, ControllerState.RingGripAnalog);
				}

				if (ControllerState.PinkyGripAnalog != ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Pinky])
				{
					const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
						SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Left_Finger_Pinky_Curl : SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Right_Finger_Pinky_Curl;
					ControllerState.PinkyGripAnalog = ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Pinky];
					SendAnalogMessage(SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex), AxisButton, ControllerState.PinkyGripAnalog);
				}

				if (ControllerState.ThumbGripAnalog != ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Thumb])
				{
					const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
						SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Left_Finger_Thumb_Curl : SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Right_Finger_Thumb_Curl;
					ControllerState.ThumbGripAnalog = ActiveSkeletalSummaryData.flFingerCurl[EVRFinger::VRFinger_Thumb];
					SendAnalogMessage(SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex), AxisButton, ControllerState.ThumbGripAnalog);
				}


				// Skeleton Finger Splays
				if (ControllerState.ThumbIndexSplayAnalog != ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Thumb_Index])
				{
					const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
						SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Left_Finger_ThumbIndex_Splay : SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Right_Finger_ThumbIndex_Splay;
					ControllerState.ThumbIndexSplayAnalog = ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Thumb_Index];
					SendAnalogMessage(SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex), AxisButton, ControllerState.ThumbIndexSplayAnalog);
				}

				if (ControllerState.IndexMiddleSplayAnalog != ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Index_Middle])
				{
					const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
						SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Left_Finger_IndexMiddle_Splay : SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Right_Finger_IndexMiddle_Splay;
					ControllerState.IndexMiddleSplayAnalog = ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Index_Middle];
					SendAnalogMessage(SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex), AxisButton, ControllerState.IndexMiddleSplayAnalog);
				}

				if (ControllerState.MiddleRingSplayAnalog != ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Middle_Ring])
				{
					const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
						SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Left_Finger_RingPinky_Splay : SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Right_Finger_RingPinky_Splay;
					ControllerState.MiddleRingSplayAnalog = ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Middle_Ring];
					SendAnalogMessage(SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex), AxisButton, ControllerState.MiddleRingSplayAnalog);
				}

				if (ControllerState.RingPinkySplayAnalog != ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Ring_Pinky])
				{
					const FGamepadKeyNames::Type AxisButton = (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex) == ETrackedControllerRole::TrackedControllerRole_LeftHand) ?
						SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Left_Finger_RingPinky_Splay : SteamVRSkeletalControllerKeyNames::SteamVR_Skeleton_Right_Finger_RingPinky_Splay;
					ControllerState.RingPinkySplayAnalog = ActiveSkeletalSummaryData.flFingerSplay[EVRFingerSplay::VRFingerSplay_Ring_Pinky];
					SendAnalogMessage(SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(DeviceIndex), AxisButton, ControllerState.RingPinkySplayAnalog);
				}
			}
#pragma endregion
		}
	}
}

FTransform CalcModelSpaceTransform(const FTransform* OutBoneTransform, int32 BoneIndex)
{
	FTransform BoneTransformMS = OutBoneTransform[BoneIndex];

	while (BoneIndex != -1)
	{
		int32 parentIndex = SteamVRSkeleton::GetParentIndex(BoneIndex);
		if (parentIndex != -1)
		{
			BoneTransformMS = BoneTransformMS * OutBoneTransform[parentIndex];
			BoneIndex = parentIndex;
		}
		else
		{
			break;
		}
	}

	return BoneTransformMS;
}

bool FSteamVRInputDevice::GetSkeletalData(bool bLeftHand, bool bMirror, EVRSkeletalMotionRange MotionRange, FTransform* OutBoneTransform, int32 OutBoneTransformCount)
{
	// Check that the size of the buffer we will be writing into is big enough to hold all the bone transforms
	if (OutBoneTransformCount < STEAMVR_SKELETON_BONE_COUNT)
	{
		return false;
	}

	if (VRInput() != nullptr && SteamVRSystem != nullptr)
	{
		// Get the handle for the skeletal action.  If its invalid (the necessary skeletal action is not in the manifest) then return false
		vr::VRActionHandle_t ActionHandle = (bLeftHand) ? VRSkeletalHandleLeft : VRSkeletalHandleRight;
		if (ActionHandle == k_ulInvalidActionHandle)
		{
			return false;
		}

		// Get skeletal data
		VRBoneTransform_t SteamVRBoneTransforms[STEAMVR_SKELETON_BONE_COUNT];
		EVRInputError Err = VRInput()->GetSkeletalBoneData(ActionHandle, vr::EVRSkeletalTransformSpace::VRSkeletalTransformSpace_Parent, MotionRange, SteamVRBoneTransforms, STEAMVR_SKELETON_BONE_COUNT);
		if (Err != VRInputError_None)
		{
			return false;
		}

		// Optionally mirror the pose to the opposite hand
		if (bMirror)
		{
			MirrorSteamVRSkeleton(SteamVRBoneTransforms, STEAMVR_SKELETON_BONE_COUNT);
		}

		// GetSkeletalBoneData returns bone transforms are in SteamVR's coordinate system, so
		// we need to convert them to UE4's coordinate system.  
		// SteamVR coords:	X=right,	Y=up,		Z=backwards,	right-handed,	scale is meters
		// UE4 coords:		X=forward,	Y=right,	Z=up,			left-handed,	scale is centimeters

		// The root is positioned at the controller's anchor position with zero rotation.  
		// However because of the conversion from SteamVR coordinates to Unreal coordinates the root bone is scaled
		// to the new coordinate system
		FTransform& RootTransform = OutBoneTransform[ESteamVRBone_Root];
		RootTransform.SetComponents(FQuat::Identity, FVector::ZeroVector, FVector(100.f, 100.f, 100.f));

		// Transform all the non-root bones to the new coordinate system
		for (int32 BoneIndex = ESteamVRBone_Root + 1; BoneIndex < STEAMVR_SKELETON_BONE_COUNT; ++BoneIndex)
		{
			const VRBoneTransform_t& SrcTransform = SteamVRBoneTransforms[BoneIndex];

			FQuat NewRotation(
				SrcTransform.orientation.z,
				-SrcTransform.orientation.x,
				SrcTransform.orientation.y,
				-SrcTransform.orientation.w
			);

			FVector NewTranslation(
				SrcTransform.position.v[2],
				-SrcTransform.position.v[0],
				SrcTransform.position.v[1]
			);

			FTransform& DstTransform = OutBoneTransform[BoneIndex];
			DstTransform.SetRotation(NewRotation);
			DstTransform.SetTranslation(NewTranslation);
		}

		// Apply an extra transformation to the children of the root bone to compensate for the changes made to the root
		// to make it fit the new coordinate system even though it has zero rotation
		FQuat FixupRotation(FVector(0.f, 0.f, 1.f), PI);

		for (int32 ChildIndex = 0; ChildIndex < SteamVRSkeleton::GetChildCount(ESteamVRBone_Root); ++ChildIndex)
		{
			int32 BoneIndex = SteamVRSkeleton::GetChildIndex(ESteamVRBone_Root, ChildIndex);

			FTransform& DstTransform = OutBoneTransform[BoneIndex];

			FVector NewTranslation = DstTransform.GetTranslation() * FVector(-1.f, -1.f, 1.f);
			FQuat NewRotation = FixupRotation * DstTransform.GetRotation();

			DstTransform.SetRotation(NewRotation);
			DstTransform.SetTranslation(NewTranslation);
		}

		return true;
	}

	return false;
}

void FSteamVRInputDevice::SendAnalogMessage(const ETrackedControllerRole TrackedControllerRole, const FGamepadKeyNames::Type AxisButton, float AnalogValue)
{
	if (TrackedControllerRole == ETrackedControllerRole::TrackedControllerRole_LeftHand && bCurlsAndSplaysEnabled_L)
	{
		MessageHandler->OnControllerAnalog(AxisButton, 0, AnalogValue);
		//UE_LOG(LogSteamVRInputDevice, Warning, TEXT("Left Index value: %f for axis %s"), ControllerState.IndexGripAnalog, *AxisButton.ToString());
	}
	else if (TrackedControllerRole == ETrackedControllerRole::TrackedControllerRole_RightHand && bCurlsAndSplaysEnabled_R)
	{
		MessageHandler->OnControllerAnalog(AxisButton, 0, AnalogValue);
		//UE_LOG(LogSteamVRInputDevice, Warning, TEXT("Left Index value: %f for axis %s"), ControllerState.IndexGripAnalog, *AxisButton.ToString());
	}
}

void FSteamVRInputDevice::SendControllerEvents()
{
	if (SteamVRSystem && VRInput())
	{
		// Set our active action set here
		VRActiveActionSet_t ActiveActionSets[] = {
			{
				MainActionSet,
				k_ulInvalidInputValueHandle,
				k_ulInvalidActionSetHandle
			}
		};

		EVRInputError ActionStateError = VRInput()->UpdateActionState(ActiveActionSets, sizeof(VRActiveActionSet_t), 1);
		if (ActionStateError != VRInputError_None)
		{
			GetInputError(LastInputError, TEXT("Error encountered when trying to update the action state"));
			return;
		}

		// Go through Actions
		for (auto& Action : Actions)
		{
			if (Action.Type == EActionType::Boolean)
			{
				// Get digital data from SteamVR
				InputDigitalActionData_t DigitalData;
				ActionStateError = VRInput()->GetDigitalActionData(Action.Handle, &DigitalData, sizeof(DigitalData), k_ulInvalidInputValueHandle);
				if (ActionStateError == VRInputError_None &&
					DigitalData.bActive &&
					DigitalData.bState != Action.bState
					)
				{
					// Update our action to the value reported by SteamVR
					Action.bState = DigitalData.bState;

					// Sent event back to Engine
					if (Action.bState && !Action.KeyX.IsNone())
					{
						MessageHandler->OnControllerButtonPressed(Action.KeyX, 0, false);
					}
					else
					{
						if (!Action.KeyX.IsNone())
						{
							MessageHandler->OnControllerButtonReleased(Action.KeyX, 0, false);
						}
					}
				}
				else if (ActionStateError != Action.LastError)
				{
					GetInputError(ActionStateError, TEXT("Error encountered retrieving digital data from SteamVR"));
				}
				Action.LastError = ActionStateError;
			}
			else if (Action.Type == EActionType::Vector1
				|| Action.Type == EActionType::Vector2
				|| Action.Type == EActionType::Vector3)
			{
				// Get analog data from SteamVR
				InputAnalogActionData_t AnalogData;
				ActionStateError = VRInput()->GetAnalogActionData(Action.Handle, &AnalogData, sizeof(AnalogData), k_ulInvalidInputValueHandle);
				if (ActionStateError == VRInputError_None && AnalogData.bActive)
				{
					// Send individual float axis value back to the Engine
					if (!Action.KeyX.IsNone() && AnalogData.x != Action.Value.X)
					{
						Action.Value.X = AnalogData.x;
						MessageHandler->OnControllerAnalog(Action.KeyX, 0, Action.Value.X);
					}
					if (!Action.KeyY.IsNone() && AnalogData.y != Action.Value.Y)
					{
						Action.Value.Y = AnalogData.y;
						MessageHandler->OnControllerAnalog(Action.KeyY, 0, Action.Value.Y);
					}
					if (!Action.KeyZ.IsNone() && AnalogData.z != Action.Value.Z)
					{
						Action.Value.Z = AnalogData.z;
						MessageHandler->OnControllerAnalog(Action.KeyZ, 0, Action.Value.Z);
					}
				}
				else if (ActionStateError != Action.LastError)
				{
					GetInputError(ActionStateError, TEXT("Error encountered retrieving analog data from SteamVR"));
				}
				Action.LastError = ActionStateError;
			}
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

bool FSteamVRInputDevice::GetControllerOrientationAndPosition(const int32 ControllerIndex, const EControllerHand DeviceHand, FRotator& OutOrientation, FVector& OutPosition) const
{
	if (VRInput() != nullptr && VRCompositor())
	{
		InputPoseActionData_t PoseData = {};
		EVRInputError InputError = VRInputError_NoData;

		switch (DeviceHand)
		{
		case EControllerHand::Left:
			InputError = VRInput()->GetPoseActionData(VRControllerHandleLeft, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Right:
			InputError = VRInput()->GetPoseActionData(VRControllerHandleRight, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_1:
			InputError = VRInput()->GetPoseActionData(VRSpecial1, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_2:
			InputError = VRInput()->GetPoseActionData(VRSpecial2, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_3:
			InputError = VRInput()->GetPoseActionData(VRSpecial3, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_4:
			InputError = VRInput()->GetPoseActionData(VRSpecial4, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_5:
			InputError = VRInput()->GetPoseActionData(VRSpecial5, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_6:
			InputError = VRInput()->GetPoseActionData(VRSpecial6, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_7:
			InputError = VRInput()->GetPoseActionData(VRSpecial7, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_8:
			InputError = VRInput()->GetPoseActionData(VRSpecial8, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		default:
			break;
		}

		if (InputError == VRInputError_None)
		{
			// Get SteamVR Transform Matrix for this controller
			HmdMatrix34_t Matrix = PoseData.pose.mDeviceToAbsoluteTracking;

			// Transform SteamVR Pose to Unreal Pose
			FMatrix Pose = FMatrix(
				FPlane(Matrix.m[0][0], Matrix.m[1][0], Matrix.m[2][0], 0.0f),
				FPlane(Matrix.m[0][1], Matrix.m[1][1], Matrix.m[2][1], 0.0f),
				FPlane(Matrix.m[0][2], Matrix.m[1][2], Matrix.m[2][2], 0.0f),
				FPlane(Matrix.m[0][3], Matrix.m[1][3], Matrix.m[2][3], 1.0f)
			);


			// Transform SteamVR Rotation Quaternion to a UE FRotator
			FQuat OrientationQuat;
			FQuat Orientation(Pose);
			OrientationQuat.X = -Orientation.Z;
			OrientationQuat.Y = Orientation.X;
			OrientationQuat.Z = Orientation.Y;
			OrientationQuat.W = -Orientation.W;


			FVector Position = ((FVector(-Pose.M[3][2], Pose.M[3][0], Pose.M[3][1])) * GWorld->GetWorldSettings()->WorldToMeters);
			OutPosition = Position;

			//OutOrientation = BaseOrientation.Inverse() * OutOrientation;
			OutOrientation.Normalize();
			OutOrientation = OrientationQuat.Rotator();
		}
	}

	return true;
}

ETrackingStatus FSteamVRInputDevice::GetControllerTrackingStatus(const int32 ControllerIndex, const EControllerHand DeviceHand) const
{
	ETrackingStatus TrackingStatus = ETrackingStatus::NotTracked;

	if (VRInput() != nullptr && VRCompositor() != nullptr)
	{
		InputPoseActionData_t PoseData = {};
		EVRInputError InputError = VRInputError_NoData;

		switch (DeviceHand)
		{
		case EControllerHand::Left:
			InputError = VRInput()->GetPoseActionData(VRControllerHandleLeft, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Right:
			InputError = VRInput()->GetPoseActionData(VRControllerHandleRight, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_1:
			InputError = VRInput()->GetPoseActionData(VRSpecial1, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_2:
			InputError = VRInput()->GetPoseActionData(VRSpecial2, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_3:
			InputError = VRInput()->GetPoseActionData(VRSpecial3, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_4:
			InputError = VRInput()->GetPoseActionData(VRSpecial4, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_5:
			InputError = VRInput()->GetPoseActionData(VRSpecial5, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_6:
			InputError = VRInput()->GetPoseActionData(VRSpecial6, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_7:
			InputError = VRInput()->GetPoseActionData(VRSpecial7, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		case EControllerHand::Special_8:
			InputError = VRInput()->GetPoseActionData(VRSpecial8, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
			break;
		default:
			break;
		}

		if (InputError == VRInputError_None && PoseData.pose.bDeviceIsConnected)
		{
			TrackingStatus = ETrackingStatus::Tracked;
		}
	}

	return TrackingStatus;
}

void FSteamVRInputDevice::GetControllerFidelity()
{
	if (VRInput() !=nullptr && VRCompositor() !=nullptr)
	{
		InputPoseActionData_t PoseData = {};
		EVRInputError InputError = VRInputError_NoData;

		InputError = VRInput()->GetPoseActionData(VRControllerHandleLeft, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
		if (PoseData.bActive && PoseData.pose.bDeviceIsConnected)
		{
			VRInput()->GetSkeletalTrackingLevel(VRSkeletalHandleLeft, &LeftControllerFidelity);
			bIsSkeletalControllerLeftPresent = (LeftControllerFidelity >= VRSkeletalTracking_Partial);
		}
		else
		{
			bIsSkeletalControllerLeftPresent = false;
			LeftControllerFidelity = EVRSkeletalTrackingLevel::VRSkeletalTracking_Estimated;
		}

		InputError = VRInput()->GetPoseActionData(VRControllerHandleRight, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
		if (PoseData.bActive && PoseData.pose.bDeviceIsConnected)
		{
			VRInput()->GetSkeletalTrackingLevel(VRSkeletalHandleRight, &RightControllerFidelity);
			bIsSkeletalControllerRightPresent = (RightControllerFidelity >= VRSkeletalTracking_Partial);
		} 
		else
		{
			bIsSkeletalControllerRightPresent = false;
			RightControllerFidelity = EVRSkeletalTrackingLevel::VRSkeletalTracking_Estimated;
		}
	}
}

void FSteamVRInputDevice::GetLeftHandPoseData(FVector& Position, FRotator& Orientation, FVector& AngularVelocity, FVector& Velocity)
{
	InputPoseActionData_t PoseData = {};
	EVRInputError InputError = VRInputError_NoData;

	if (bIsSkeletalControllerRightPresent && VRInput() != nullptr)
	{
		InputError = VRInput()->GetPoseActionData(VRSkeletalHandleLeft, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
		if (PoseData.bActive && PoseData.pose.bDeviceIsConnected && InputError == VRInputError_None)
		{
			GetUETransform(PoseData, Position, Orientation);
			AngularVelocity = FVector(
				PoseData.pose.vAngularVelocity.v[2],
				-PoseData.pose.vAngularVelocity.v[0],
				PoseData.pose.vAngularVelocity.v[1]
			);
			Velocity = FVector(
				PoseData.pose.vVelocity.v[2],
				-PoseData.pose.vVelocity.v[0],
				PoseData.pose.vVelocity.v[1]
			);
		}
	}
}

void FSteamVRInputDevice::GetRightHandPoseData(FVector& Position, FRotator& Orientation, FVector& AngularVelocity, FVector& Velocity)
{
	InputPoseActionData_t PoseData = {};
	EVRInputError InputError = VRInputError_NoData;

	if (bIsSkeletalControllerRightPresent && VRInput() != nullptr)
	{
		InputError = VRInput()->GetPoseActionData(VRSkeletalHandleRight, VRCompositor()->GetTrackingSpace(), 0, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);
		if (PoseData.bActive && PoseData.pose.bDeviceIsConnected && InputError == VRInputError_None)
		{
			GetUETransform(PoseData, Position, Orientation);
			AngularVelocity = FVector(
										PoseData.pose.vAngularVelocity.v[2],
										-PoseData.pose.vAngularVelocity.v[0],
										PoseData.pose.vAngularVelocity.v[1]
									 );
			Velocity = FVector(
								PoseData.pose.vVelocity.v[2],
								-PoseData.pose.vVelocity.v[0],
								PoseData.pose.vVelocity.v[1]
								);
		}
	}
}

void FSteamVRInputDevice::GetUETransform(InputPoseActionData_t PoseData, FVector& OutPosition, FRotator& OutOrientation)
{
	// Get SteamVR Transform Matrix for this skeleton
	HmdMatrix34_t Matrix = PoseData.pose.mDeviceToAbsoluteTracking;

	// Transform SteamVR Pose to Unreal Pose
	FMatrix Pose = FMatrix(
		FPlane(Matrix.m[0][0], Matrix.m[1][0], Matrix.m[2][0], 0.0f),
		FPlane(Matrix.m[0][1], Matrix.m[1][1], Matrix.m[2][1], 0.0f),
		FPlane(Matrix.m[0][2], Matrix.m[1][2], Matrix.m[2][2], 0.0f),
		FPlane(Matrix.m[0][3], Matrix.m[1][3], Matrix.m[2][3], 1.0f)
	);


	// Transform SteamVR Rotation Quaternion to a UE FRotator
	FQuat OrientationQuat;
	FQuat Orientation(Pose);
	OrientationQuat.X = -Orientation.Z;
	OrientationQuat.Y = Orientation.X;
	OrientationQuat.Z = Orientation.Y;
	OrientationQuat.W = -Orientation.W;


	FVector Position = ((FVector(-Pose.M[3][2], Pose.M[3][0], Pose.M[3][1])) * GWorld->GetWorldSettings()->WorldToMeters);
	OutPosition = Position;

	//OutOrientation = BaseOrientation.Inverse() * OutOrientation;
	OutOrientation.Normalize();
	OutOrientation = OrientationQuat.Rotator();			
}

void FSteamVRInputDevice::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	// Empty on purpose
}

void FSteamVRInputDevice::SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values)
{
	// Empty on purpose
}

void FSteamVRInputDevice::InitControllerMappings()
{
	for (unsigned int i = 0; i < k_unMaxTrackedDeviceCount; ++i)
	{
		DeviceToControllerMap[i] = INDEX_NONE;
	}

	for (unsigned int id = 0; id < SteamVRInputDeviceConstants::MaxUnrealControllers; ++id)
	{
		for (unsigned int hand = 0; hand < k_unMaxTrackedDeviceCount; ++hand)
		{
			if (DeviceToControllerMap[id] < SteamVRInputDeviceConstants::MaxUnrealControllers && hand < k_unMaxTrackedDeviceCount)
			{
				UnrealControllerIdAndHandToDeviceIdMap[DeviceToControllerMap[id]][hand] = INDEX_NONE;
			}
		}
	}

	for (int32& HandCount : MaxUEHandCount)
	{
		HandCount = 0;
	}
}

#if WITH_EDITOR
void FSteamVRInputDevice::RegenerateActionManifest()
{
	this->GenerateActionManifest(true, false, true, true);
}

void FSteamVRInputDevice::RegenerateControllerBindings()
{
	this->GenerateActionManifest(false, true, true, true);
}

bool FSteamVRInputDevice::GenerateAppManifest(FString ManifestPath, FString ProjectName, FString& OutAppKey, FString& OutAppManifestPath)
{
	// Set SteamVR AppKey
	OutAppKey = (TEXT(APP_MANIFEST_PREFIX) + SanitizeString(GameProjectName) + TEXT(".") + ProjectName).ToLower();
	EditorAppKey = FString(OutAppKey);

	// Set Application Manifest Path - same directory where the action manifest will be
	OutAppManifestPath = FPaths::GameConfigDir() / APP_MANIFEST_FILE;
	IFileManager& FileManager = FFileManagerGeneric::Get();

	// Create Application Manifest json objects
	TSharedRef<FJsonObject> AppManifestObject = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> ManifestApps;

	// Add current engine version being used as source
	AppManifestObject->SetStringField("source", FString::Printf(TEXT("UE")));

	// Define the application setting that will be registered with SteamVR
	TArray<TSharedPtr<FJsonValue>> ManifestApp;

	// Create Application Object 
	TSharedRef<FJsonObject> ApplicationObject = MakeShareable(new FJsonObject());
	TArray<FString> AppStringFields = { "app_key",  OutAppKey,
										"launch_type", "url",
										"url", "steam://launch/",
										"action_manifest_path", *IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*ManifestPath)
	};
	BuildJsonObject(AppStringFields, ApplicationObject);

	// Create localization object
	TSharedPtr<FJsonObject> LocStringsObject = MakeShareable(new FJsonObject());
	TSharedRef<FJsonObject> AppNameObject = MakeShareable(new FJsonObject());
	AppNameObject->SetStringField("name", GameProjectName + " [UE Editor]");
	LocStringsObject->SetObjectField("en_US", AppNameObject);
	ApplicationObject->SetObjectField("strings", LocStringsObject);

	// Assemble the json app manifest
	ManifestApps.Add(MakeShareable(new FJsonValueObject(ApplicationObject)));
	AppManifestObject->SetArrayField(TEXT("applications"), ManifestApps);

	// Serialize json app manifest
	FString AppManifestString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&AppManifestString);
	FJsonSerializer::Serialize(AppManifestObject, JsonWriter);

	// Save json as a UTF8 file
	if (!FFileHelper::SaveStringToFile(AppManifestString, *OutAppManifestPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("Error trying to generate application manifest in: %s"), *OutAppManifestPath);
		return false;
	}

	return true;
}

void FSteamVRInputDevice::ReloadActionManifest()
{
#if WITH_EDITOR

	if (VRInput() != nullptr && VRApplications() !=nullptr)
	{
		// Restart SteamVR
		if (SteamVRSystem)
		{
			VR_Shutdown();
		}
		InitSteamVRSystem();

		if (SteamVRSystem != nullptr)
		{
			// Set Action Manifest Path
			const FString ManifestPath = FPaths::GameConfigDir() / CONTROLLER_BINDING_PATH / ACTION_MANIFEST;
			UE_LOG(LogSteamVRInputDevice, Display, TEXT("Reloading Action Manifest in: %s"), *ManifestPath);
		
			// Set Action Manifest
			EVRInputError InputError = VRInput()->SetActionManifestPath(TCHAR_TO_UTF8(*IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*ManifestPath)));
			GetInputError(InputError, FString(TEXT("Setting Action Manifest Path")));
		
			// Load application manifest
			FString AppManifestPath = FPaths::GameConfigDir() / APP_MANIFEST_FILE;
			EVRApplicationError AppError = VRApplications()->AddApplicationManifest(TCHAR_TO_UTF8(*IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*AppManifestPath)), false);
			UE_LOG(LogSteamVRInputDevice, Display, TEXT("[STEAMVR INPUT] Registering Application Manifest %s : %s"), *AppManifestPath, *FString(UTF8_TO_TCHAR(VRApplications()->GetApplicationsErrorNameFromEnum(AppError))));
		
			// Get the App Process Id
			uint32 AppProcessId = FPlatformProcess::GetCurrentProcessId();
		
			// Set SteamVR AppKey
			FString AppFileName = FPaths::GetCleanFilename(FPlatformProcess::GetApplicationName(AppProcessId));
			FString SteamVRAppKey = (TEXT(APP_MANIFEST_PREFIX) + SanitizeString(GameProjectName) + TEXT(".") + AppFileName).ToLower();
		
			// Set AppKey for this Editor Session
			AppError = VRApplications()->IdentifyApplication(AppProcessId, TCHAR_TO_UTF8(*SteamVRAppKey));
			UE_LOG(LogSteamVRInputDevice, Display, TEXT("[STEAMVR INPUT] Editor Application [%d][%s] identified to SteamVR: %s"), AppProcessId, *SteamVRAppKey, *FString(UTF8_TO_TCHAR(VRApplications()->GetApplicationsErrorNameFromEnum(AppError))));
		}
	}
#endif
}

void FSteamVRInputDevice::GenerateControllerBindings(const FString& BindingsPath, TArray<FControllerType>& InOutControllerTypes, TArray<TSharedPtr<FJsonValue>>& DefaultBindings, TArray<FSteamVRInputAction>& InActionsArray, TArray<FInputMapping>& InInputMapping, bool bDeleteIfExists)
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
		if (!SupportedController.bIsGenerated || bDeleteIfExists)
		{
			// Creating bindings file
			TSharedRef<FJsonObject> BindingsObject = MakeShareable(new FJsonObject());
			BindingsObject->SetStringField(TEXT("name"), TEXT("Default bindings for ") + SupportedController.Description);
			BindingsObject->SetStringField(TEXT("controller_type"), SupportedController.Name.ToString());

			// Create Action Bindings in JSON Format
			TArray<TSharedPtr<FJsonValue>> JsonValuesArray;
			GenerateActionBindings(InInputMapping, JsonValuesArray);

			// Create Action Set
			TSharedRef<FJsonObject> ActionSetJsonObject = MakeShareable(new FJsonObject());
			ActionSetJsonObject->SetArrayField(TEXT("sources"), JsonValuesArray);

			// Add Controller Pose Mappings
			TArray<TSharedPtr<FJsonValue>> ControllerPoseArray;

			// Add Pose: Left Controller 
			TSharedRef<FJsonObject> ControllerLeftJsonObject = MakeShareable(new FJsonObject());
			ControllerLeftJsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_CONTROLLER_LEFT));
			ControllerLeftJsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_CONT_RAW_LEFT));

			TSharedRef<FJsonValueObject> ControllerLeftJsonValueObject = MakeShareable(new FJsonValueObject(ControllerLeftJsonObject));
			ControllerPoseArray.Add(ControllerLeftJsonValueObject);

			// Add Pose: Right Controller
			TSharedRef<FJsonObject> ControllerRightJsonObject = MakeShareable(new FJsonObject());
			ControllerRightJsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_CONTROLLER_RIGHT));
			ControllerRightJsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_CONT_RAW_RIGHT));

			TSharedRef<FJsonValueObject> ControllerRightJsonValueObject = MakeShareable(new FJsonValueObject(ControllerRightJsonObject));
			ControllerPoseArray.Add(ControllerRightJsonValueObject);

			if (SupportedController.Name.ToString().Equals(TEXT("tracker")))
			{
				// Add Pose: Special 1
				TSharedRef<FJsonObject> Special1JsonObject = MakeShareable(new FJsonObject());
				Special1JsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_SPECIAL_BACK_L));
				Special1JsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_SPCL_BACK_LEFT));
	
				TSharedRef<FJsonValueObject> Special1JsonValueObject = MakeShareable(new FJsonValueObject(Special1JsonObject));
				ControllerPoseArray.Add(Special1JsonValueObject);
	
				// Add Pose: Special 2
				TSharedRef<FJsonObject> Special2JsonObject = MakeShareable(new FJsonObject());
				Special2JsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_SPECIAL_BACK_R));
				Special2JsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_SPCL_BACK_RIGHT));
	
				TSharedRef<FJsonValueObject> Special2JsonObjectJsonValueObject = MakeShareable(new FJsonValueObject(Special2JsonObject));
				ControllerPoseArray.Add(Special2JsonObjectJsonValueObject);
	
				// Add Pose: Special 3
				TSharedRef<FJsonObject> Special3JsonObject = MakeShareable(new FJsonObject());
				Special3JsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_SPECIAL_FRONT_L));
				Special3JsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_SPCL_FRONT_LEFT));
	
				TSharedRef<FJsonValueObject> Special3JsonValueObject = MakeShareable(new FJsonValueObject(Special3JsonObject));
				ControllerPoseArray.Add(Special3JsonValueObject);
	
				// Add Pose: Special 4
				TSharedRef<FJsonObject> Special4JsonObject = MakeShareable(new FJsonObject());
				Special4JsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_SPECIAL_FRONT_R));
				Special4JsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_SPCL_FRONT_RIGHT));
	
				TSharedRef<FJsonValueObject> Special4JsonValueObject = MakeShareable(new FJsonValueObject(Special4JsonObject));
				ControllerPoseArray.Add(Special4JsonValueObject);
	
				// Add Pose: Special 5
				TSharedRef<FJsonObject> Special5JsonObject = MakeShareable(new FJsonObject());
				Special5JsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_SPECIAL_FRONTR_L));
				Special5JsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_SPCL_FRONTR_LEFT));
	
				TSharedRef<FJsonValueObject> Special5JsonValueObject = MakeShareable(new FJsonValueObject(Special5JsonObject));
				ControllerPoseArray.Add(Special5JsonValueObject);
	
				// Add Pose: Special 6
				TSharedRef<FJsonObject> Special6JsonObject = MakeShareable(new FJsonObject());
				Special6JsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_SPECIAL_FRONTR_R));
				Special6JsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_SPCL_FRONTR_RIGHT));
	
				TSharedRef<FJsonValueObject> Special6JsonValueObject = MakeShareable(new FJsonValueObject(Special6JsonObject));
				ControllerPoseArray.Add(Special6JsonValueObject);
	
				// Add Pose: Special 7
				TSharedRef<FJsonObject> Special7JsonObject = MakeShareable(new FJsonObject());
				Special7JsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_SPECIAL_PISTOL_L));
				Special7JsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_SPCL_PISTOL_LEFT));
	
				TSharedRef<FJsonValueObject> Special7JsonValueObject = MakeShareable(new FJsonValueObject(Special7JsonObject));
				ControllerPoseArray.Add(Special7JsonValueObject);
	
				// Add Pose: Special 8
				TSharedRef<FJsonObject> Special8JsonObject = MakeShareable(new FJsonObject());
				Special8JsonObject->SetStringField(TEXT("output"), TEXT(ACTION_PATH_SPECIAL_PISTOL_R));
				Special8JsonObject->SetStringField(TEXT("path"), TEXT(ACTION_PATH_SPCL_PISTOL_RIGHT));

				TSharedRef<FJsonValueObject> Special8JsonValueObject = MakeShareable(new FJsonValueObject(Special8JsonObject));
				ControllerPoseArray.Add(Special8JsonValueObject);
			}

			// Add Controller Input Array To Action Set
			ActionSetJsonObject->SetArrayField(TEXT("poses"), ControllerPoseArray);

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
			BindingsObject->SetStringField(TEXT("description"), GameProjectName);

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

			// Set Cache Vars
			FSteamVRInputState InputState;
			FName CacheMode;
			FString CacheType;
			FString CachePath;

			// Check if any of the actions associated with this Input Key have the [XD] axis designation
			for (auto& Action : InInputMapping[i].Actions)
			{

				if (Action.Right(6) == TEXT("X_axis"))
				{
					InputState.bIsAxis = true;
					break;
				}
				else if (Action.Right(7) == TEXT("_axis2d"))
				{
					InputState.bIsAxis = true;
					InputState.bIsAxis2 = true;
					break;
				}
				else if (Action.Right(7) == TEXT("_axis3d"))
				{
					InputState.bIsAxis = true;
					InputState.bIsAxis3 = true;
					break;
				}
			}

			// Set Input State
			FString CurrentInputKeyName = InInputMapping[i].InputKey.ToString();
			InputState.bIsTrigger = CurrentInputKeyName.Contains(TEXT("Trigger"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			InputState.bIsThumbstick = CurrentInputKeyName.Contains(TEXT("Thumbstick"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			InputState.bIsTrackpad = CurrentInputKeyName.Contains(TEXT("Trackpad"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			InputState.bIsGrip = CurrentInputKeyName.Contains(TEXT("Grip"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			InputState.bIsCapSense = CurrentInputKeyName.Contains(TEXT("CapSense"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			InputState.bIsLeft = CurrentInputKeyName.Contains(TEXT("Left"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			InputState.bIsFaceButton1 = CurrentInputKeyName.Contains(TEXT("FaceButton1"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			InputState.bIsFaceButton2 = CurrentInputKeyName.Contains(TEXT("FaceButton2"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);

			// Handle Special Actions for Knuckles Keys
			if (CurrentInputKeyName.Contains(TEXT("Knuckles"), ESearchCase::IgnoreCase, ESearchDir::FromStart) &&
				CurrentInputKeyName.Contains(TEXT("Pinch"), ESearchCase::IgnoreCase, ESearchDir::FromEnd)
				)
			{
				InputState.bIsPinchGrab = true;
				InputState.bIsGrip = false;
				InputState.bIsAxis = false;
			}
			else if (CurrentInputKeyName.Contains(TEXT("Knuckles"), ESearchCase::IgnoreCase, ESearchDir::FromStart) &&
				CurrentInputKeyName.Contains(TEXT("Grip"), ESearchCase::IgnoreCase, ESearchDir::FromEnd) &&
				CurrentInputKeyName.Contains(TEXT("Grab"), ESearchCase::IgnoreCase, ESearchDir::FromEnd)
				)
			{
				InputState.bIsGripGrab = true;
				InputState.bIsGrip = false;
				InputState.bIsAxis = false;
			}
			else
			{
				InputState.bIsPinchGrab = false;
				InputState.bIsGripGrab = false;
			}

			// Set Cache Mode
			CacheMode = InputState.bIsTrigger || InputState.bIsGrip ? FName(TEXT("trigger")) : FName(TEXT("button"));
			CacheMode = InputState.bIsTrackpad ? FName(TEXT("trackpad")) : CacheMode;
			CacheMode = InputState.bIsGrip ? FName(TEXT("force_sensor")) : CacheMode;
			CacheMode = InputState.bIsThumbstick ? FName(TEXT("joystick")) : CacheMode;
			CacheMode = InputState.bIsPinchGrab || InputState.bIsGripGrab ? FName(TEXT("grab")) : CacheMode;

			// Set Cache Path
			if (InputState.bIsTrigger)
			{
				CachePath = InputState.bIsLeft ? FString(TEXT(ACTION_PATH_TRIGGER_LEFT)) : FString(TEXT(ACTION_PATH_TRIGGER_RIGHT));
			}
			else if (InputState.bIsThumbstick)
			{
				CachePath = InputState.bIsLeft ? FString(TEXT(ACTION_PATH_THUMBSTICK_LEFT)) : FString(TEXT(ACTION_PATH_THUMBSTICK_RIGHT));
			}
			else if (InputState.bIsTrackpad)
			{
				CachePath = InputState.bIsLeft ? FString(TEXT(ACTION_PATH_TRACKPAD_LEFT)) : FString(TEXT(ACTION_PATH_TRACKPAD_RIGHT));
			}
			else if (InputState.bIsGrip)
			{
				CachePath = InputState.bIsLeft ? FString(TEXT(ACTION_PATH_GRIP_LEFT)) : FString(TEXT(ACTION_PATH_GRIP_RIGHT));
			}
			else if (InputState.bIsFaceButton1)
			{
				CachePath = InputState.bIsLeft ? FString(TEXT(ACTION_PATH_BTN_A_LEFT)) : FString(TEXT(ACTION_PATH_BTN_A_RIGHT));
			}
			else if (InputState.bIsFaceButton2)
			{
				CachePath = InputState.bIsLeft ? FString(TEXT(ACTION_PATH_BTN_B_LEFT)) : FString(TEXT(ACTION_PATH_BTN_B_RIGHT));
			}

			// Handle Special Actions
			if (InputState.bIsPinchGrab)
			{
				CachePath = InputState.bIsLeft ? FString(TEXT(ACTION_PATH_PINCH_GRAB_LEFT)) : FString(TEXT(ACTION_PATH_PINCH_GRAB_RIGHT));
			}
			else if (InputState.bIsGripGrab)
			{
				CachePath = InputState.bIsLeft ? FString(TEXT(ACTION_PATH_GRIP_GRAB_LEFT)) : FString(TEXT(ACTION_PATH_GRIP_GRAB_RIGHT));
			}

			// Create Action Source
			FActionSource ActionSource = FActionSource(CacheMode, CachePath);
			TSharedRef<FJsonObject> ActionSourceJsonObject = MakeShareable(new FJsonObject());
			ActionSourceJsonObject->SetStringField(TEXT("mode"), ActionSource.Mode.ToString());

			// Set Action Path
			if (!ActionSource.Path.IsEmpty())
			{
				ActionSourceJsonObject->SetStringField(TEXT("path"), ActionSource.Path);
			}
			else
			{
				continue;
			}

			// Set Key Mappings
			TSharedPtr<FJsonObject> ActionInputJsonObject = MakeShareable(new FJsonObject());

			// Create Action Path
			TSharedRef<FJsonObject> ActionPathJsonObject = MakeShareable(new FJsonObject());
			ActionPathJsonObject->SetStringField(TEXT("output"), InInputMapping[i].Actions[j]);

			// Set Cache Type
			if (InputState.bIsAxis && InputState.bIsAxis2)
			{
				if (InputState.bIsGrip)
				{
					CacheType = FString(TEXT("force"));
				}
				else
				{
					if (CacheMode.IsEqual(TEXT("trigger")))
					{
						CacheType = FString(TEXT("pull"));
					}
					else
					{
						CacheType = FString(TEXT("position"));
					}
				}
			}
			else if (InputState.bIsAxis && !InputState.bIsAxis2)
			{
				if (InputState.bIsGrip)
				{
					CacheType = FString(TEXT("force"));
				}
				else if (!InputState.bIsThumbstick && !InputState.bIsTrackpad)
				{
					CacheType = FString(TEXT("pull"));
				}
				else
				{
					CacheType = "";
				}
			}
			else if (!InputState.bIsAxis)
			{
				CacheType = (InputState.bIsCapSense) ? FString(TEXT("touch")) : FString(TEXT("click"));
			}
			else
			{
				CacheType = "";
			}

			// Handle special actions
			if (InputState.bIsPinchGrab || InputState.bIsGripGrab)
			{
				CacheType = FString(TEXT("grab"));
			}

			// Special handling for axes
			if (CacheMode.IsEqual(TEXT("joystick"))
				&& InInputMapping[i].Actions[j].Right(6) == TEXT("X_axis")
				&& CacheType == TEXT("position"))
			{
				CacheType = "";
			}

			if (!CacheType.IsEmpty())
			{
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
}
#endif

void FSteamVRInputDevice::GenerateActionManifest(bool GenerateActions, bool GenerateBindings, bool RegisterApp, bool DeleteIfExists)
{
	// Set Input Settings
	auto InputSettings = GetDefault<UInputSettings>();

	// Set Action Manifest Path
	const FString ManifestPath = FPaths::GameConfigDir() / CONTROLLER_BINDING_PATH / ACTION_MANIFEST;
	UE_LOG(LogSteamVRInputDevice, Display, TEXT("Action Manifest Path: %s"), *ManifestPath);

	// Create Action Manifest json object
	TSharedRef<FJsonObject> ActionManifestObject = MakeShareable(new FJsonObject());
	TArray<FString> LocalizationFields = {"language_tag", "en_US"};

	// Set where to look for controller binding files and prepare file manager
	const FString ControllerBindingsPath = FPaths::GameConfigDir() / CONTROLLER_BINDING_PATH;
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

#pragma region ACTIONS
	// Clear Actions cache
	Actions.Empty();

	// Setup Input Mappings cache
	TArray<FInputMapping> InputMappings;
	TArray<FName> UniqueInputs;

	// Check if this project have input settings
	if (InputSettings->IsValidLowLevelFast())
	{
		// Process all actions in this project (if any)
		TArray<TSharedPtr<FJsonValue>> InputActionsArray;

		// Setup cache for actions
		TArray<FString> UniqueActions;

		// Controller poses
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_CONTROLLER_LEFT));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Pose, true,
				FName(TEXT("Left Controller [Pose]")), FString(TEXT(ACTION_PATH_CONT_RAW_LEFT))));
		}
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_CONTROLLER_RIGHT));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Pose, true,
				FName(TEXT("Right Controller [Pose]")), FString(TEXT(ACTION_PATH_CONT_RAW_RIGHT))));
		}

		// Other poses
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_SPECIAL_BACK_L));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Pose, true,
				FName(TEXT("Special 1 [Tracker]")), FString(TEXT(ACTION_PATH_SPCL_BACK_LEFT))));
		}
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_SPECIAL_BACK_R));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Pose, true,
				FName(TEXT("Special 2 [Tracker]")), FString(TEXT(ACTION_PATH_SPCL_BACK_RIGHT))));
		}
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_SPECIAL_FRONT_L));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Pose, true,
				FName(TEXT("Special 3 [Tracker]")), FString(TEXT(ACTION_PATH_SPCL_FRONT_LEFT))));
		}
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_SPECIAL_FRONT_R));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Pose, true,
				FName(TEXT("Special 4 [Tracker]")), FString(TEXT(ACTION_PATH_SPCL_FRONT_RIGHT))));
		}
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_SPECIAL_FRONTR_L));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Pose, true,
				FName(TEXT("Special 5 [Tracker]")), FString(TEXT(ACTION_PATH_SPCL_FRONTR_LEFT))));
		}
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_SPECIAL_FRONTR_R));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Pose, true,
				FName(TEXT("Special 6 [Tracker]")), FString(TEXT(ACTION_PATH_SPCL_FRONTR_RIGHT))));
		}
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_SPECIAL_PISTOL_L));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Pose, true,
				FName(TEXT("Special 7 [Tracker]")), FString(TEXT(ACTION_PATH_SPCL_PISTOL_LEFT))));
		}
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_SPECIAL_PISTOL_R));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Pose, true,
				FName(TEXT("Special 8 [Tracker]")), FString(TEXT(ACTION_PATH_SPCL_PISTOL_RIGHT))));
		}

		// Skeletal Data
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_SKELETON_LEFT));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Skeleton, true,
				FName(TEXT("Skeleton (Left)")), FString(TEXT(ACTION_PATH_SKEL_HAND_LEFT))));
		}
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_SKELETON_RIGHT));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Skeleton, true,
				FName(TEXT("Skeleton (Right)")), FString(TEXT(ACTION_PATH_SKEL_HAND_RIGHT))));
		}

		// Open console
		{
			const FKey* ConsoleKey = InputSettings->ConsoleKeys.FindByPredicate([](FKey& Key) { return Key.IsValid(); });
			if (ConsoleKey != nullptr)
			{
				Actions.Add(FSteamVRInputAction(FString(TEXT(ACTION_PATH_OPEN_CONSOLE)), FName(TEXT("Open Console")), ConsoleKey->GetFName(), false));
			}
		}

		// Haptics
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_VIBRATE_LEFT));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Vibration, true, FName(TEXT("Haptic (Left)"))));
		}
		{
			FString ConstActionPath = FString(TEXT(ACTION_PATH_VIBRATE_RIGHT));
			Actions.Add(FSteamVRInputAction(ConstActionPath, EActionType::Vibration, true, FName(TEXT("Haptic (Right)"))));
		}

		// Add base actions to the action manifest
		ActionManifestObject->SetArrayField(TEXT("actions"), InputActionsArray);

		// Add project's input key mappings to SteamVR's Input Actions
		ProcessKeyInputMappings(InputSettings, UniqueInputs);

		// Add project's input axis mappings to SteamVR's Input Actions
		ProcessKeyAxisMappings(InputSettings, UniqueInputs);

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
					TArray<FInputActionKeyMapping> ActionKeyMappings;
					FindActionMappings(InputSettings, Action.Name, ActionKeyMappings);
					for (FInputActionKeyMapping ActionKeyMapping : ActionKeyMappings)
					{
						if (UniqueInput.IsEqual(ActionKeyMapping.Key.GetFName()))
						{
							NewInputMapping.Actions.AddUnique(Action.Path);
						}
					}
				}

				// Check for axes/analog input
				if (Action.Type == EActionType::Vector1 || Action.Type == EActionType::Vector2 || Action.Type == EActionType::Vector3)
				{
					// Set Axis Actions Linked To This Input Key
					FString ActionAxis = Action.Name.ToString().LeftChop(7); // Remove " a_axis"  Axis indicator before doing any comparisons

					// Parse comma delimited action names into an array
					TArray<FString> ActionAxisArray;
					ActionAxis.ParseIntoArray(ActionAxisArray, TEXT(","), true);
					TArray<FInputAxisKeyMapping> FoundAxisMappings;

					for (auto& ActionAxisName : ActionAxisArray)
					{
						FindAxisMappings(InputSettings, FName(*ActionAxisName), FoundAxisMappings);

						for (FInputAxisKeyMapping AxisMapping : FoundAxisMappings)
						{
							if (UniqueInput.IsEqual(AxisMapping.Key.GetFName()))
							{
								// Check for X Axis
								if (!Action.KeyX.IsNone() && Action.KeyX.IsEqual(AxisMapping.Key.GetFName()))
								{
									// Add 1D Action
									NewAxisMapping.Actions.AddUnique(Action.Path);

									FString ActionDimension = Action.Name.ToString().Right(7);

									if (ActionDimension == TEXT("_axis2d"))
									{
										// Add 2D Action
										FString Action2D = Action.Path.LeftChop(11) + TEXT(" X Y_axis2d");
										NewAxisMapping.Actions.AddUnique(Action2D);
									}

									if (ActionDimension == TEXT("_axis3d"))
									{
										// Add 3D Action
										FString Action3D = Action.Path.LeftChop(11) + TEXT(" X Y_axis3d");
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

				// Add hand if skeleton
				if (Action.Type == EActionType::Skeleton)
				{
					ActionFields.Append({ TEXT("skeleton"), Action.StringPath });
				}

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
					FString ActionName = Action.Name.ToString();
					if (ActionName.Contains("_axis"))
					{
						if (ActionName.Contains(","))
						{
							TArray<FString> ActionNameArray;
							ActionName.ParseIntoArray(ActionNameArray, TEXT(","), true);

							if (ActionNameArray.Num() > 0)
							{
								ActionName = FString(ActionNameArray[0]); // Grab only the first action name
							}
						}
						else
						{
							ActionName = ActionName.LeftChop(7); // Remove " a_axis" for the localization string
						}
					}
					FString LocalizationArray[] = { Action.Path, ActionName };
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
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("Error trying to retrieve Input Settings."));
	}
#pragma endregion

#pragma region ACTION SETS
	// Setup action set json objects
	TArray<TSharedPtr<FJsonValue>> ActionSets;
	TSharedRef<FJsonObject> ActionSetObject = MakeShareable(new FJsonObject());

	// Create action set objects
	TArray<FString> StringFields = {
									 "name", TEXT(ACTION_SET),
									 "usage", TEXT("leftright")
	};

	BuildJsonObject(StringFields, ActionSetObject);

	// Add action sets array to the Action Manifest object
	ActionSets.Add(MakeShareable(new FJsonValueObject(ActionSetObject)));
	ActionManifestObject->SetArrayField(TEXT("action_sets"), ActionSets);

	// Set localization text for the action set
	LocalizationFields.Add(TEXT(ACTION_SET));
	LocalizationFields.Add("Main Game Actions");
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
											 TEXT("binding_url"), *BindingFile //*FileManager.ConvertToAbsolutePathForExternalAppForRead(*(ControllerBindingsPath / BindingFile))
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
	if (GenerateBindings)
	{
		GenerateControllerBindings(ControllerBindingsPath, ControllerTypes, ControllerBindings, Actions, InputMappings, DeleteIfExists);
	}
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
	if (GenerateActions)
	{
		if (!FileManager.FileExists(*ManifestPath) || (FileManager.FileExists(*ManifestPath) && DeleteIfExists))
		{
			if (!FFileHelper::SaveStringToFile(ActionManifest, *ManifestPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
			{
				UE_LOG(LogSteamVRInputDevice, Error, TEXT("Error trying to generate action manifest in: %s"), *ManifestPath);
				return;
			}
		}
	}

	// Register Application to SteamVR
	if (RegisterApp)
	{
		RegisterApplication(ManifestPath);
	}
}

bool FSteamVRInputDevice::BuildJsonObject(TArray<FString> StringFields, TSharedRef<FJsonObject> OutJsonObject)
{
	// Check if StringFields array is even
	if (StringFields.Num() > 1 && StringFields.Num() % 2 == 0)
	{
		// Generate json object of string field pairs
		for (int32 i = 0; i < StringFields.Num(); i += 2)
		{
			OutJsonObject->SetStringField(StringFields[i], StringFields[i + 1]);
		}

		return true;
	}

	return false;
}

void FSteamVRInputDevice::ProcessKeyInputMappings(const UInputSettings* InputSettings, TArray<FName> &InOutUniqueInputs)
{
	// Retrieve key actions setup in this project
	KeyMappings.Empty();
	TArray<FName> KeyActionNames;
	InputSettings->GetActionNames(KeyActionNames);

	// Process all key actions found
	for (const FName& KeyActionName : KeyActionNames)
	{
		TArray<FInputActionKeyMapping> KeyInputMappings;

		// Retrieve input keys associated with this action
		FindActionMappings(InputSettings, KeyActionName, KeyInputMappings);

		for (auto& KeyMapping : KeyInputMappings)
		{
			if (KeyMapping.Key.GetFName().ToString().Contains(TEXT("MotionController")) ||
				KeyMapping.Key.GetFName().ToString().Contains(TEXT("SteamVR")) ||
				KeyMapping.Key.GetFName().ToString().Contains(TEXT("Knuckles")) ||
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
	KeyAxisMappings.Empty();

	// [1D] All Axis Mappings will have a corresponding Vector1 Action associated with them
	for (const FName& XAxisName : KeyAxisNames)
	{
		// Set X Axis Key Name Cache
		FName XAxisNameKey = NAME_None;

		// Retrieve input axes associated with this action
		FindAxisMappings(InputSettings, XAxisName, KeyAxisMappings);

		// Go through all axis mappings
		for (auto& AxisMapping : KeyAxisMappings)
		{
			// Add axes names here for use in the auto-generation of controller bindings
			InOutUniqueInputs.AddUnique(AxisMapping.Key.GetFName());

			// Set Key Name
			XAxisNameKey = AxisMapping.Key.GetFName();

			// If this is an X Axis key, check for the corresponding Y & Z Axes as well
			uint32 KeyHand = 0;

			// Check Hand if any, we have a third state in cases where a UE key does not signify handedness
			if (AxisMapping.Key.GetFName().ToString().Contains(TEXT("(L)"), ESearchCase::CaseSensitive) ||
				AxisMapping.Key.GetFName().ToString().Contains(TEXT("Left"), ESearchCase::CaseSensitive)
				)
			{
				KeyHand = 1;
			}
			else if (AxisMapping.Key.GetFName().ToString().Contains(TEXT("(R)")) ||
				AxisMapping.Key.GetFName().ToString().Contains(TEXT("Right"))
				)
			{
				KeyHand = 2;
			}
			else
			{
				KeyHand = 0;
			}

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
					FindAxisMappings(InputSettings, KeyAxisNameInner, AxisMappingsInner);

					for (auto& AxisMappingInner : AxisMappingsInner)
					{
						// Find Y & Z axes
						FString KeyNameSuffix = (AxisMappingInner.Key.GetFName().ToString()).Right(6);

						// Populate Axes Caches
						if (KeyNameSuffix.Contains(TEXT("_Y"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) ||
							KeyNameSuffix.Contains(TEXT("Y-Axis"), ESearchCase::CaseSensitive, ESearchDir::FromEnd)
							)
						{

							if (((AxisMappingInner.Key.GetFName().ToString().Contains(TEXT("(L)"), ESearchCase::CaseSensitive) ||
								AxisMappingInner.Key.GetFName().ToString().Contains(TEXT("Left"), ESearchCase::CaseSensitive))
								&& KeyHand == 1) ||
								((AxisMappingInner.Key.GetFName().ToString().Contains(TEXT("(R)"), ESearchCase::CaseSensitive) ||
									AxisMappingInner.Key.GetFName().ToString().Contains(TEXT("Right"), ESearchCase::CaseSensitive))
									&& KeyHand == 2)
								)
							{
								YAxisName = KeyAxisNameInner;
								YAxisNameKey = AxisMappingInner.Key.GetFName();
							}
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
					FString AxisName2D = AxisMapping.AxisName.ToString() +
						TEXT(",") +
						YAxisName.ToString() +
						TEXT(" X Y_axis2d");
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
						TEXT(" X Y Z_axis3d");
					FString ActionPath3D = FString(ACTION_PATH_IN) / AxisName3D;
					Actions.Add(FSteamVRInputAction(ActionPath3D, FName(*AxisName3D), XAxisNameKey, YAxisNameKey, ZAxisNameKey, FVector()));
				}
			}

			// If we find at least one valid, then add this action to the list of SteamVR Input Actions as Vector1
			if (!XAxisName.IsNone())
			{
				// [1D] Populate all Vector1 actions
				FString AxisName1D = AxisMapping.AxisName.ToString() + TEXT(" X_axis");
				FString ActionPath = FString(ACTION_PATH_IN) / AxisName1D;
				Actions.Add(FSteamVRInputAction(ActionPath, FName(*AxisName1D), XAxisNameKey, 0.0f));
			}
		}
	}

	// Cleanup action set
	SanitizeActions();
}

void FSteamVRInputDevice::SanitizeActions()
{
	for (int32 i = 0; i < Actions.Num(); i++)
	{
		// Check for X,Y keys validity
		//if (!(Actions[i].KeyX.ToString().Contains(TEXT("_X")) || Actions[i].KeyX.ToString().Contains(TEXT("X-Axis"))))
		//{
		//	Actions.RemoveAt(i, 1, false);
		//}
		//else if (!(Actions[i].KeyY.ToString().Contains(TEXT("_Y")) || Actions[i].KeyY.ToString().Contains(TEXT("Y-Axis"))))
		//{
		//	Actions.RemoveAt(i, 1, false);
		//}
		if (!Actions[i].KeyX.IsNone() && !Actions[i].KeyY.IsNone() && Actions[i].KeyZ.IsNone())
		{
			// Check for X & Y Handedness Match for Vector2s
			if ((Actions[i].Name.ToString().Contains(TEXT("Left")) && Actions[i].Name.ToString().Contains(TEXT("Right"))))
			{
				Actions.RemoveAt(i, 1, false);
			}
		}
	}

	Actions.Shrink();
}

void FSteamVRInputDevice::RegisterApplication(FString ManifestPath)
{
	if (VRApplications() != nullptr && VRInput() != nullptr)
	{
		// Get Project Name this plugin is used in
		uint32 AppProcessId = FPlatformProcess::GetCurrentProcessId();
		GameFileName = FPaths::GetCleanFilename(FPlatformProcess::GetApplicationName(AppProcessId));
		if (GConfig)
		{
			GConfig->GetString(
				TEXT("/Script/EngineSettings.GeneralProjectSettings"),
				TEXT("ProjectName"),
				GameProjectName,
				GGameIni
			);

		}
		else
		{

			// Unable to retrieve project name, reverting to raw application executable name
			GameProjectName = GameFileName;
		}

		// Restart SteamVR
		if (SteamVRSystem)
		{
			VR_Shutdown();
		}
		InitSteamVRSystem();

#if WITH_EDITOR
		// Generate Application Manifest
		FString AppKey, AppManifestPath;

		GenerateAppManifest(ManifestPath, GameFileName, AppKey, AppManifestPath);

		char* SteamVRAppKey = TCHAR_TO_UTF8(*AppKey);

		// Load application manifest
		EVRApplicationError AppError = VRApplications()->AddApplicationManifest(TCHAR_TO_UTF8(*IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*AppManifestPath)), false);
		UE_LOG(LogSteamVRInputDevice, Display, TEXT("[STEAMVR INPUT] Registering Application Manifest %s : %s"), *AppManifestPath, *FString(UTF8_TO_TCHAR(VRApplications()->GetApplicationsErrorNameFromEnum(AppError))));

		// Set AppKey for this Editor Session
		AppError = VRApplications()->IdentifyApplication(AppProcessId, SteamVRAppKey);
		UE_LOG(LogSteamVRInputDevice, Display, TEXT("[STEAMVR INPUT] Editor Application [%d][%s] identified to SteamVR: %s"), AppProcessId, *AppKey, *FString(UTF8_TO_TCHAR(VRApplications()->GetApplicationsErrorNameFromEnum(AppError))));
#endif

		// Set Action Manifest
		EVRInputError InputError = VRInput()->SetActionManifestPath(TCHAR_TO_UTF8(*IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*ManifestPath)));
		GetInputError(InputError, FString(TEXT("Setting Action Manifest Path")));

		// Set Main Action Set
		InputError = VRInput()->GetActionSetHandle(ACTION_SET, &MainActionSet);
		GetInputError(InputError, FString(TEXT("Setting main action set")));

		// Fill in Action handles for each registered action
		for (auto& Action : Actions)
		{
			VRActionHandle_t Handle;
			InputError = VRInput()->GetActionHandle(TCHAR_TO_UTF8(*Action.Path), &Handle);
			Action.Handle = Handle;

			// Test if this is a pose
			if (Action.Path == TEXT(ACTION_PATH_CONTROLLER_LEFT))
			{
				VRControllerHandleLeft = Action.Handle;
			}
			else if (Action.Path == TEXT(ACTION_PATH_CONTROLLER_RIGHT))
			{
				VRControllerHandleRight = Action.Handle;
			}
			else if (Action.Path == TEXT(ACTION_PATH_SPECIAL_BACK_L))
			{
				VRSpecial1 = Action.Handle;
			}
			else if (Action.Path == TEXT(ACTION_PATH_SPECIAL_BACK_R))
			{
				VRSpecial2 = Action.Handle;
			}
			else if (Action.Path == TEXT(ACTION_PATH_SPECIAL_FRONT_L))
			{
				VRSpecial3 = Action.Handle;
			}
			else if (Action.Path == TEXT(ACTION_PATH_SPECIAL_FRONT_R))
			{
				VRSpecial4 = Action.Handle;
			}
			else if (Action.Path == TEXT(ACTION_PATH_SPECIAL_FRONTR_L))
			{
				VRSpecial5 = Action.Handle;
			}
			else if (Action.Path == TEXT(ACTION_PATH_SPECIAL_FRONTR_R))
			{
				VRSpecial6 = Action.Handle;
			}
			else if (Action.Path == TEXT(ACTION_PATH_SPECIAL_PISTOL_L))
			{
				VRSpecial7 = Action.Handle;
			}
			else if (Action.Path == TEXT(ACTION_PATH_SPECIAL_PISTOL_R))
			{
				VRSpecial8 = Action.Handle;
			}

			UE_LOG(LogSteamVRInputDevice, Display, TEXT("Retrieving Action Handle: %s"), *Action.Path);
			GetInputError(InputError, FString(TEXT("Setting Action Handle Path Result")));
		}
	}
}

bool FSteamVRInputDevice::SetSkeletalHandle(char* ActionPath, VRActionHandle_t& SkeletalHandle)
{
	if (VRInput() != nullptr)
	{
		// Get Skeletal Handle
		EVRInputError Err = VRInput()->GetActionHandle(ActionPath, &SkeletalHandle);
		if ((Err != VRInputError_None || !SkeletalHandle) && Err != LastInputError)
		{
			GetInputError(Err, TEXT("Couldn't get skeletal action handle for Skeleton."));
			Err = LastInputError;
		}
		else
		{
			return true;
		}
	}
	return false;
}

void FSteamVRInputDevice::InitSkeletalControllerKeys()
{
	// Standard Keys
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Left_A_CapSense, LOCTEXT("Knuckles_Left_A_CapSense", "SteamVR Knuckles (L) A CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Right_A_CapSense, LOCTEXT("Knuckles_Right_A_CapSense", "SteamVR Knuckles (R) A CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Left_B_CapSense, LOCTEXT("Knuckles_Left_B_CapSense", "SteamVR Knuckles (L) B CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Right_B_CapSense, LOCTEXT("Knuckles_Right_B_CapSense", "SteamVR Knuckles (R) B CapSense"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Left_Trigger_CapSense, LOCTEXT("Knuckles_Left_Trigger_CapSense", "SteamVR Knuckles (L) Trigger CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Right_Trigger_CapSense, LOCTEXT("Knuckles_Right_Trigger_CapSense", "SteamVR Knuckles (R) Trigger CapSense"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Left_Thumbstick_CapSense, LOCTEXT("Knuckles_Left_Thumbstick_CapSense", "SteamVR Knuckles (L) Thumbstick CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Right_Thumbstick_CapSense, LOCTEXT("Knuckles_Right_Thumbstick_CapSense", "SteamVR Knuckles (R) Thumbstick CapSense"), FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Left_Trackpad_CapSense, LOCTEXT("Knuckles_Left_Trackpad_CapSense", "SteamVR Knuckles (L) Trackpad CapSense"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Right_Trackpad_CapSense, LOCTEXT("Knuckles_Right_Trackpad_CapSense", "SteamVR Knuckles (R) Trackpad CapSense"), FKeyDetails::GamepadKey));

	// Special Action Keys
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Left_Grip_Grab, LOCTEXT("Knuckles_Left_Grip_Grab", "SteamVR Knuckles (L) Grip Grab"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Right_Grip_Grab, LOCTEXT("Knuckles_Right_Grip_Grab", "SteamVR Knuckles (R) Grip Grab"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Left_Pinch_Grab, LOCTEXT("Knuckles_Left_Pinch_Grab", "SteamVR Knuckles (L) Pinch Grab"), FKeyDetails::GamepadKey));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Right_Pinch_Grab, LOCTEXT("Knuckles_Right_Pinch_Grab", "SteamVR Knuckles (R) Pinch Grab"), FKeyDetails::GamepadKey));

	// Grip Force
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Left_Trackpad_GripForce, LOCTEXT("Knuckles_Left_Trackpad_GripForce", "SteamVR Knuckles (L) Trackpad GripForce"), FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Right_Trackpad_GripForce, LOCTEXT("Knuckles_Right_Trackpad_GripForce", "SteamVR Knuckles (R) Trackpad GripForce"), FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Left_Trackpad_X, LOCTEXT("Knuckles_Left_Trackpad_X", "SteamVR Knuckles (L) Trackpad X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Right_Trackpad_X, LOCTEXT("Knuckles_Right_Trackpad_X", "SteamVR Knuckles (R) Trackpad X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Left_Trackpad_Y, LOCTEXT("Knuckles_Left_Trackpad_Y", "SteamVR Knuckles (L) Trackpad Y"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Knuckles_Right_Trackpad_Y, LOCTEXT("Knuckles_Right_Trackpad_Y", "SteamVR Knuckles (R) Trackpad Y"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	// Skeleton Curls
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Left_Finger_Index_Curl, LOCTEXT("Skeleton_Left_Finger_Index_Curl", "SteamVR Skeleton (L) Finger Index Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Right_Finger_Index_Curl, LOCTEXT("Skeleton_Right_Finger_Index_Curl", "SteamVR Skeleton (R) Finger Index Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Left_Finger_Middle_Curl, LOCTEXT("Skeleton_Left_Finger_Middle_Curl", "SteamVR Skeleton (L) Finger Middle Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Right_Finger_Middle_Curl, LOCTEXT("Skeleton_Right_Finger_Middle_Curl", "SteamVR Skeleton (R) Finger Middle Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Left_Finger_Ring_Curl, LOCTEXT("Skeleton_Left_Finger_Ring_Curl", "SteamVR Skeleton (L) Finger Ring Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Right_Finger_Ring_Curl, LOCTEXT("Skeleton_Right_Finger_Ring_Curl", "SteamVR Skeleton (R) Finger Ring Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Left_Finger_Pinky_Curl, LOCTEXT("Skeleton_Left_Finger_Pinky_Curl", "SteamVR Skeleton (L) Finger Pinky Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Right_Finger_Pinky_Curl, LOCTEXT("Skeleton_Right_Finger_Pinky_Curl", "SteamVR Skeleton (R) Finger Pinky Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Left_Finger_Thumb_Curl, LOCTEXT("Skeleton_Left_Finger_Thumb_Curl", "SteamVR Skeleton (L) Finger Thumb Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Right_Finger_Thumb_Curl, LOCTEXT("Skeleton_Right_Finger_Thumb_Curl", "SteamVR Skeleton (R) Finger Thumb Curl"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	// Skeleton Splays
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Left_Finger_ThumbIndex_Splay, LOCTEXT("Skeleton_Left_Finger_ThumbIndex_Splay", "SteamVR Skeleton (L) Finger Thumb-Index Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Right_Finger_ThumbIndex_Splay, LOCTEXT("Skeleton_Right_Finger_ThumbIndex_Splay", "SteamVR Skeleton (R) Finger Thumb-Index Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Left_Finger_IndexMiddle_Splay, LOCTEXT("Skeleton_Left_Finger_IndexMiddle_Splay", "SteamVR Skeleton (L) Finger Index-Middle Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Right_Finger_IndexMiddle_Splay, LOCTEXT("Skeleton_Right_Finger_IndexMiddle_Splay", "SteamVR Skeleton (R) Finger Index-Middle Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Left_Finger_MiddleRing_Splay, LOCTEXT("Skeleton_Left_Finger_MiddleRing_Splay", "SteamVR Skeleton (L) Finger Middle-Ring Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Right_Finger_MiddleRing_Splay, LOCTEXT("Skeleton_Right_Finger_MiddleRing_Splay", "SteamVR Skeleton (R) Finger Middle-Ring Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));

	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Left_Finger_RingPinky_Splay, LOCTEXT("Skeleton_Left_Finger_RingPinky_Splay", "SteamVR Skeleton (L) Finger Ring-Pinky Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
	EKeys::AddKey(FKeyDetails(SteamVRSkeletalControllerKeys::SteamVR_Skeleton_Right_Finger_RingPinky_Splay, LOCTEXT("Skeleton_Right_Finger_RingPinky_Splay", "SteamVR Skeleton (R) Finger Ring-Pinky Splay"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis));
}

void FSteamVRInputDevice::GetInputError(EVRInputError InputError, FString InputAction)
{
	switch (InputError)
	{
	case VRInputError_None:
		UE_LOG(LogSteamVRInputDevice, Display, TEXT("[STEAMVR INPUT] %s: Success"), *InputAction);
		break;
	case VRInputError_NameNotFound:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Name Not Found"), *InputAction);
		break;
	case VRInputError_WrongType:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Wrong Type"), *InputAction);
		break;
	case VRInputError_InvalidHandle:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Invalid Handle"), *InputAction);
		break;
	case VRInputError_InvalidParam:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Invalid Param"), *InputAction);
		break;
	case VRInputError_NoSteam:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: No Steam"), *InputAction);
		break;
	case VRInputError_MaxCapacityReached:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s:  Max Capacity Reached"), *InputAction);
		break;
	case VRInputError_IPCError:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: IPC Error"), *InputAction);
		break;
	case VRInputError_NoActiveActionSet:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: No Active Action Set"), *InputAction);
		break;
	case VRInputError_InvalidDevice:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Invalid Device"), *InputAction);
		break;
	case VRInputError_InvalidSkeleton:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Invalid Skeleton"), *InputAction);
		break;
	case VRInputError_InvalidBoneCount:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Invalid Bone Count"), *InputAction);
		break;
	case VRInputError_InvalidCompressedData:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Invalid Compressed Data"), *InputAction);
		break;
	case VRInputError_NoData:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: No Data"), *InputAction);
		break;
	case VRInputError_BufferTooSmall:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Buffer Too Small"), *InputAction);
		break;
	case VRInputError_MismatchedActionManifest:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Mismatched Action Manifest"), *InputAction);
		break;
	case VRInputError_MissingSkeletonData:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Missing Skeleton Data"), *InputAction);
		break;
	default:
		UE_LOG(LogSteamVRInputDevice, Error, TEXT("[STEAMVR INPUT] %s: Unknown Error"), *InputAction);
		break;
	}

	return;
}

void FSteamVRInputDevice::MirrorSteamVRSkeleton(VRBoneTransform_t* BoneTransformsLS, int32 BoneTransformCount) const
{
	check(BoneTransformsLS != nullptr);
	check(BoneTransformCount == SteamVRSkeleton::GetBoneCount());

	// Mirror the bones whose rotations transfer directly and only the translation needs to be fixed
	{
		const int32 TranslationOnlyBoneCount = sizeof(kMirrorTranslationOnlyBones) / sizeof(kMirrorTranslationOnlyBones[0]);
		for (int32 i = 0; i < TranslationOnlyBoneCount; ++i)
		{
			const int32 BoneIndex = kMirrorTranslationOnlyBones[i];

			vr::HmdVector4_t& Position = BoneTransformsLS[BoneIndex].position;
			Position.v[0] *= -1.f;
			Position.v[1] *= -1.f;
			Position.v[2] *= -1.f;
		}
	}

	// Mirror the metacarpals
	{
		const int32 MetaCarpalBoneCount = sizeof(kMetacarpalBones) / sizeof(kMetacarpalBones[0]);
		for (int32 i = 0; i < MetaCarpalBoneCount; ++i)
		{
			const int32 BoneIndex = kMetacarpalBones[i];

			vr::VRBoneTransform_t& BoneTransform = BoneTransformsLS[BoneIndex];

			BoneTransform.position.v[0] *= -1.f;
			
			vr::HmdQuaternionf_t OriginalRotation = BoneTransform.orientation;
			BoneTransform.orientation.w = OriginalRotation.x;
			BoneTransform.orientation.x = -OriginalRotation.w;
			BoneTransform.orientation.y = OriginalRotation.z;
			BoneTransform.orientation.z = -OriginalRotation.y;
		}
	}

	// Mirror the children of the root
	{
		const int32 ModelSpaceBoneCount = sizeof(kModelSpaceBones) / sizeof(kModelSpaceBones[0]);
		for (int32 i = 0; i < ModelSpaceBoneCount; ++i)
		{
			const int32 BoneIndex = kModelSpaceBones[i];

			vr::VRBoneTransform_t& BoneTransform = BoneTransformsLS[BoneIndex];
			BoneTransform.position.v[0] *= -1.f;
			BoneTransform.orientation.y *= -1.f;
			BoneTransform.orientation.z *= -1.f;
		}
	}
}

#undef LOCTEXT_NAMESPACE //"SteamVRInputDevice"
