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


#include "SteamVRInputDeviceFunctionLibrary.h"

#if STEAMVRCONTROLLER_SUPPORTED_PLATFORMS
#include "../ThirdParty/OpenVRSDK/headers/openvr.h"
#include "GameFramework/WorldSettings.h"
#include "HAL/FileManagerGeneric.h"
using namespace vr;
#endif // STEAMVRCONTROLLER_SUPPORTED_PLATFORMS

void USteamVRInputDeviceFunctionLibrary::PlaySteamVR_HapticFeedback(ESteamVRHand Hand, float StartSecondsFromNow, float DurationSeconds, float Frequency, float Amplitude)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (VRSystem() && VRInput())
	{
		if (Amplitude < 0.f)
		{
			Amplitude = 0.f;
		}
		else if (Amplitude > 1.f)
		{
			Amplitude = 1.f;
		}

		// Setup which hand data we will get from SteamVR
		VRActionHandle_t ActiveSkeletalHand = k_ulInvalidActionHandle;

		if (Hand == ESteamVRHand::VR_Left && SteamVRInputDevice->VRVibrationLeft != k_ulInvalidActionHandle && SteamVRInputDevice->bIsSkeletalControllerLeftPresent && SteamVRInputDevice->VRSkeletalHandleLeft != k_ulInvalidActionHandle)
		{
			if (SteamVRInputDevice->VRVibrationLeft == k_ulInvalidActionHandle)
			{
				return;
			}

			ActiveSkeletalHand = SteamVRInputDevice->VRSkeletalHandleLeft;
			VRInput()->TriggerHapticVibrationAction(SteamVRInputDevice->VRVibrationLeft, StartSecondsFromNow, DurationSeconds, Frequency, Amplitude, k_ulInvalidInputValueHandle);
		}
		else if (Hand == ESteamVRHand::VR_Right && SteamVRInputDevice->VRVibrationRight != k_ulInvalidActionHandle && SteamVRInputDevice->bIsSkeletalControllerRightPresent && SteamVRInputDevice->VRSkeletalHandleRight != k_ulInvalidActionHandle)
		{
			if (SteamVRInputDevice->VRVibrationRight == k_ulInvalidActionHandle)
			{
				return;
			}

			ActiveSkeletalHand = SteamVRInputDevice->VRSkeletalHandleRight;
			VRInput()->TriggerHapticVibrationAction(SteamVRInputDevice->VRVibrationRight, StartSecondsFromNow, DurationSeconds, Frequency, Amplitude, k_ulInvalidInputValueHandle);
		}
	}
}

void USteamVRInputDeviceFunctionLibrary::RegenActionManifest()
{
#if WITH_EDITOR
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->RegenerateActionManifest();
	}
#endif
}

void USteamVRInputDeviceFunctionLibrary::RegenControllerBindings()
{
#if WITH_EDITOR
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->RegenerateControllerBindings();
	}
#endif
}

void USteamVRInputDeviceFunctionLibrary::ReloadActionManifest()
{
#if WITH_EDITOR
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->ReloadActionManifest();
	}
#endif
}

void USteamVRInputDeviceFunctionLibrary::GetCurlsAndSplaysState(bool& LeftHandState, bool& RightHandState)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		LeftHandState = SteamVRInputDevice->bCurlsAndSplaysEnabled_L;
		RightHandState = SteamVRInputDevice->bCurlsAndSplaysEnabled_R;
	}
}

void USteamVRInputDeviceFunctionLibrary::GetSkeletalState(bool& LeftHandState, bool& RightHandState)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		LeftHandState = SteamVRInputDevice->bIsSkeletalControllerLeftPresent;
		RightHandState = SteamVRInputDevice->bIsSkeletalControllerRightPresent;
	}
}

void USteamVRInputDeviceFunctionLibrary::GetControllerFidelity(EControllerFidelity& LeftControllerFidelity, EControllerFidelity& RightControllerFidelity)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		// Update the skeletal tracking level from SteamVR
		SteamVRInputDevice->GetControllerFidelity();

		// Return the left controller fidelity
		switch (SteamVRInputDevice->LeftControllerFidelity)
		{
		case EVRSkeletalTrackingLevel::VRSkeletalTracking_Full:
			LeftControllerFidelity = EControllerFidelity::VR_ControllerFidelity_Full;
			break;

		case EVRSkeletalTrackingLevel::VRSkeletalTracking_Partial:
			LeftControllerFidelity = EControllerFidelity::VR_ControllerFidelity_Partial;
			break;

		case EVRSkeletalTrackingLevel::VRSkeletalTracking_Estimated:
			// falls through
		default:
			LeftControllerFidelity = EControllerFidelity::VR_ControllerFidelity_Estimated;
			break;
		}

		// Return the right controller fidelity
		switch (SteamVRInputDevice->RightControllerFidelity)
		{
		case EVRSkeletalTrackingLevel::VRSkeletalTracking_Full:
			RightControllerFidelity = EControllerFidelity::VR_ControllerFidelity_Full;
			break;

		case EVRSkeletalTrackingLevel::VRSkeletalTracking_Partial:
			RightControllerFidelity = EControllerFidelity::VR_ControllerFidelity_Partial;
			break;

		case EVRSkeletalTrackingLevel::VRSkeletalTracking_Estimated:
			// falls through
		default:
			RightControllerFidelity = EControllerFidelity::VR_ControllerFidelity_Estimated;
			break;
		}
	}
}

void USteamVRInputDeviceFunctionLibrary::SetCurlsAndSplaysState(bool NewLeftHandState, bool NewRightHandState)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->bCurlsAndSplaysEnabled_L = NewLeftHandState;
		SteamVRInputDevice->bCurlsAndSplaysEnabled_R = NewRightHandState;
	}
}

void USteamVRInputDeviceFunctionLibrary::GetPoseSource(bool& bUsingSkeletonPose)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		bUsingSkeletonPose = SteamVRInputDevice->bUseSkeletonPose;
	}
}

void USteamVRInputDeviceFunctionLibrary::SetPoseSource(bool bUseSkeletonPose)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->bUseSkeletonPose = bUseSkeletonPose;
	}
}

void USteamVRInputDeviceFunctionLibrary::GetSkeletalTransform(FSteamVRSkeletonTransform& LeftHand, FSteamVRSkeletonTransform& RightHand, bool bWithController)
{
	FTransform OutPose[STEAMVR_SKELETON_BONE_COUNT];

	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		// Setup Motion Range
		EVRSkeletalMotionRange MotionRange = bWithController ? VRSkeletalMotionRange_WithController : VRSkeletalMotionRange_WithoutController;

		// Left Hand - Grab Skeletal Data
		SteamVRInputDevice->GetSkeletalData(true, false, MotionRange, OutPose, STEAMVR_SKELETON_BONE_COUNT);

		LeftHand.Root = OutPose[0];
		LeftHand.Wrist = OutPose[1];

		LeftHand.Thumb_0 = OutPose[2];
		LeftHand.Thumb_1 = OutPose[3];
		LeftHand.Thumb_2 = OutPose[4];
		LeftHand.Thumb_3 = OutPose[5];

		LeftHand.Index_0 = OutPose[6];
		LeftHand.Index_1 = OutPose[7];
		LeftHand.Index_2 = OutPose[8];
		LeftHand.Index_3 = OutPose[9];
		LeftHand.Index_4 = OutPose[10];

		LeftHand.Middle_0 = OutPose[11];
		LeftHand.Middle_1 = OutPose[12];
		LeftHand.Middle_2 = OutPose[13];
		LeftHand.Middle_3 = OutPose[14];
		LeftHand.Middle_4 = OutPose[15];

		LeftHand.Ring_0 = OutPose[16];
		LeftHand.Ring_1 = OutPose[17];
		LeftHand.Ring_2 = OutPose[18];
		LeftHand.Ring_3 = OutPose[19];
		LeftHand.Ring_4 = OutPose[20];

		LeftHand.Pinky_0 = OutPose[21];
		LeftHand.Pinky_1 = OutPose[22];
		LeftHand.Pinky_2 = OutPose[23];
		LeftHand.Pinky_3 = OutPose[24];
		LeftHand.Pinky_4 = OutPose[25];

		LeftHand.Aux_Thumb = OutPose[26];
		LeftHand.Aux_Index = OutPose[27];
		LeftHand.Aux_Middle = OutPose[28];
		LeftHand.Aux_Ring = OutPose[29];
		LeftHand.Aux_Pinky = OutPose[30];

		LeftHand.Bone_Count = OutPose[31];

		// Right Hand - Grab Skeletal Data
		SteamVRInputDevice->GetSkeletalData(false, false, MotionRange, OutPose, STEAMVR_SKELETON_BONE_COUNT);

		RightHand.Root = OutPose[0];
		RightHand.Wrist = OutPose[1];

		RightHand.Thumb_0 = OutPose[2];
		RightHand.Thumb_1 = OutPose[3];
		RightHand.Thumb_2 = OutPose[4];
		RightHand.Thumb_3 = OutPose[5];

		RightHand.Index_0 = OutPose[6];
		RightHand.Index_1 = OutPose[7];
		RightHand.Index_2 = OutPose[8];
		RightHand.Index_3 = OutPose[9];
		RightHand.Index_4 = OutPose[10];

		RightHand.Middle_0 = OutPose[11];
		RightHand.Middle_1 = OutPose[12];
		RightHand.Middle_2 = OutPose[13];
		RightHand.Middle_3 = OutPose[14];
		RightHand.Middle_4 = OutPose[15];

		RightHand.Ring_0 = OutPose[16];
		RightHand.Ring_1 = OutPose[17];
		RightHand.Ring_2 = OutPose[18];
		RightHand.Ring_3 = OutPose[19];
		RightHand.Ring_4 = OutPose[20];

		RightHand.Pinky_0 = OutPose[21];
		RightHand.Pinky_1 = OutPose[22];
		RightHand.Pinky_2 = OutPose[23];
		RightHand.Pinky_3 = OutPose[24];
		RightHand.Pinky_4 = OutPose[25];

		RightHand.Aux_Thumb = OutPose[26];
		RightHand.Aux_Index = OutPose[27];
		RightHand.Aux_Middle = OutPose[28];
		RightHand.Aux_Ring = OutPose[29];
		RightHand.Aux_Pinky = OutPose[30];

		RightHand.Bone_Count = OutPose[31];
	}
}

void USteamVRInputDeviceFunctionLibrary::GetLeftHandPoseData(FVector& Position, FRotator& Orientation, FVector& AngularVelocity, FVector& Velocity)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->GetLeftHandPoseData(Position, Orientation, AngularVelocity, Velocity);
	}
}

void USteamVRInputDeviceFunctionLibrary::GetRightHandPoseData(FVector& Position, FRotator& Orientation, FVector& AngularVelocity, FVector& Velocity)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->GetRightHandPoseData(Position, Orientation, AngularVelocity, Velocity);
	}
}

void USteamVRInputDeviceFunctionLibrary::GetSteamVR_ActionArray(TArray<FSteamVRAction>& SteamVRActions)
{
	bool bAlreadyExists;
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		// Get the input actions and assign them to the defined actions list for developer consumption
		for (FSteamVRInputAction& InputAction : SteamVRInputDevice->ActionEvents)
		{
			// Check for duplicates
			bAlreadyExists = false;
			for (FSteamVRAction& SteamVRAction : SteamVRActions)
			{
				if (SteamVRAction.Handle == InputAction.Handle)
				{
					bAlreadyExists = true;
					break;
				}
			}

			// Add the Input Action to the list of Defined Actions if it's unique
			if (!bAlreadyExists)
			{
				SteamVRActions.Add(FSteamVRAction(InputAction.Name, InputAction.Path, InputAction.Handle, InputAction.ActiveOrigin));
			}
		}
	}
}

void USteamVRInputDeviceFunctionLibrary::FindSteamVR_Action(FName ActionName, bool& bResult, FSteamVRAction& FoundAction, FSteamVRActionSet& FoundActionSet, FName ActionSet)
{
	bool bActionFound = false;
	bool bActionSetFound = false;

	// Get current SteamVR Actions
	TArray<FSteamVRAction> SteamVRActions;
	GetSteamVR_ActionArray(SteamVRActions);

	// Find Action
	for (FSteamVRAction SteamVRDefinedAction : SteamVRActions)
	{
		if (SteamVRDefinedAction.Name.IsEqual(ActionName))
		{
			FoundAction = SteamVRDefinedAction;
			bActionFound = true;
			break;
		}
	}

	// Get current SteamVR Action Sets
	FString InActionSet = TEXT("/actions/") + ActionSet.ToString();
	TArray<FSteamVRActionSet> SteamVRActionSets;
	GetSteamVR_ActionSetArray(SteamVRActionSets);

	// Find Action Set
	for (FSteamVRActionSet SteamVRDefinedActionSet : SteamVRActionSets)
	{
		if (SteamVRDefinedActionSet.Path.Equals(InActionSet, ESearchCase::IgnoreCase))
		{
			FoundActionSet = SteamVRDefinedActionSet;
			bActionSetFound = true;
			break;
		}
	}

	// Update search result 
	bResult = bActionFound && bActionSetFound;
}

void USteamVRInputDeviceFunctionLibrary::GetSteamVR_ActionSetArray(TArray<FSteamVRActionSet>& SteamVRActionSets)
{
	// We only have one Action Set supported for this version of the plugin, so just return that
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRActionSets.Add(FSteamVRActionSet(TEXT(ACTION_SET), SteamVRInputDevice->MainActionSet));
	}
}

bool USteamVRInputDeviceFunctionLibrary::GetSteamVR_OriginTrackedDeviceInfo(FSteamVRAction SteamVRAction, FSteamVRInputOriginInfo& InputOriginInfo)
{
	if (VRSystem() && VRInput())
	{
		InputOriginInfo_t OriginInfo = {};
		EVRInputError Err = VRInput()->GetOriginTrackedDeviceInfo(SteamVRAction.ActiveOrigin, &OriginInfo, sizeof(OriginInfo));

		if (Err == VRInputError_None && OriginInfo.trackedDeviceIndex != k_unTrackedDeviceIndexInvalid)
		{
			// Get device model information
			char ModelBuffer[k_unMaxPropertyStringSize];
			uint32 StringBytes = VRSystem()->GetStringTrackedDeviceProperty(OriginInfo.trackedDeviceIndex, ETrackedDeviceProperty::Prop_ModelNumber_String, ModelBuffer, sizeof(ModelBuffer));

			// Set Input Origin Info
			InputOriginInfo.TrackedDeviceIndex = OriginInfo.trackedDeviceIndex;
			InputOriginInfo.RenderModelComponentName = *FString(UTF8_TO_TCHAR(OriginInfo.rchRenderModelComponentName));
			InputOriginInfo.TrackedDeviceModel = *FString(UTF8_TO_TCHAR(ModelBuffer));
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Error [%i] trying to retrieve Tracked Device Info or an invalid device was returned for Action [%s]"), (int)Err, *SteamVRAction.Path);
		}
	}

	return false;
}

void USteamVRInputDeviceFunctionLibrary::FindSteamVR_OriginTrackedDeviceInfo(FName ActionName, bool& bResult, FSteamVRInputOriginInfo& InputOriginInfo, FName ActionSet /*= FName("main")*/)
{
	bResult = false;
	FSteamVRAction FoundAction;
	FSteamVRActionSet FoundActionSet;

	FindSteamVR_Action(ActionName, bResult, FoundAction, FoundActionSet, ActionSet);

	if (bResult)
	{
		bResult = GetSteamVR_OriginTrackedDeviceInfo(FoundAction, InputOriginInfo);
	}
}

void USteamVRInputDeviceFunctionLibrary::GetSteamVR_OriginLocalizedName(FSteamVRAction SteamVRAction, TArray<ESteamVRInputStringBits> LocalizedParts, FString& OriginLocalizedName)
{
	if (VRSystem() && VRInput())
	{
		uint32 LocalizedPartsMask = 0;
		EVRInputStringBits SteamVREquivalentInputStringBits;

		for (ESteamVRInputStringBits LocalizedPart : LocalizedParts)
		{
			switch (LocalizedPart)
			{
			case ESteamVRInputStringBits::VR_InputString_Hand:
				SteamVREquivalentInputStringBits = EVRInputStringBits::VRInputString_Hand;
				break;

			case ESteamVRInputStringBits::VR_InputString_ControllerType:
				SteamVREquivalentInputStringBits = EVRInputStringBits::VRInputString_ControllerType;
				break;

			case ESteamVRInputStringBits::VR_InputString_InputSource:
				SteamVREquivalentInputStringBits = EVRInputStringBits::VRInputString_InputSource;
				break;

			case ESteamVRInputStringBits::VR_InputString_All:
				// Falls through

			default:
				SteamVREquivalentInputStringBits = EVRInputStringBits::VRInputString_All;
				break;
			}

			LocalizedPartsMask |= (int)SteamVREquivalentInputStringBits;
		}

		// Retrieve Localized Name
		char buf[k_unMaxPropertyStringSize];
		EVRInputError Err = VRInput()->GetOriginLocalizedName(SteamVRAction.ActiveOrigin, buf, sizeof(buf), LocalizedPartsMask);
		OriginLocalizedName = *FString(UTF8_TO_TCHAR(buf));

		// Provide debugging info if retrieval is unsuccessful
		//if (Err != VRInputError_None)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Error [%i] when retrieving Origin Localized Name for Action [%i] %s"), (int)Err, (int)SteamVRDefinedAction.Handle, *SteamVRDefinedAction.Path);
		//}
	}
}

void USteamVRInputDeviceFunctionLibrary::ShowSteamVR_ActionOrigin(FSteamVRAction SteamVRAction, FSteamVRActionSet SteamVRActionSet)
{
	if (VRSystem() && VRInput())
	{
		// Show the action origin in user's hmd
		EVRInputError Err = VRInput()->ShowActionOrigins(0, SteamVRAction.Handle);
		FSteamVRInputOriginInfo OriginInfo;

		if (GetSteamVR_OriginTrackedDeviceInfo(SteamVRAction, OriginInfo))
		{
			VRActiveActionSet_t ActiveActionSets[] = { 0 };
			ActiveActionSets[0].ulActionSet = SteamVRActionSet.Handle;
			VRInput()->ShowBindingsForActionSet(ActiveActionSets, sizeof(ActiveActionSets[0]), 1, SteamVRAction.ActiveOrigin);

			//UE_LOG(LogTemp, Warning, TEXT("Action [%s] triggered from Device [%i][%s] at Component [%s]"), *SteamVRAction.Name.ToString(), OriginInfo.TrackedDeviceIndex, *OriginInfo.TrackedDeviceModel, *OriginInfo.RenderModelComponentName);
		}
	}
}

bool USteamVRInputDeviceFunctionLibrary::FindSteamVR_ActionOrigin(FName ActionName, FName ActionSet)
{
	// Find SteamVR Action
	bool bIsActionFound = false;
	FSteamVRAction SteamVRAction;
	FSteamVRActionSet SteamVRActionSet;
	FindSteamVR_Action(ActionName, bIsActionFound, SteamVRAction, SteamVRActionSet, ActionSet);

	if (bIsActionFound && VRSystem() && VRInput())
	{
		ShowSteamVR_ActionOrigin(SteamVRAction, SteamVRActionSet);
		return true;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, (TEXT("Unable to find Action [%s] for Action Set [%s]"), *ActionName.ToString(), *ActionSet.ToString()));
	return false;
}

bool USteamVRInputDeviceFunctionLibrary::GetSteamVR_HandPoseRelativeToNow(FVector& Position, FRotator& Orientation, ESteamVRHand Hand /*= ESteamVRHand::VR_Left*/, float PredictedSecondsFromNow /*= 0.f*/)
{
	if (VRSystem() && VRInput())
	{
		FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
		if (SteamVRInputDevice != nullptr)
		{
			VRActionHandle_t HandActionHandle = (Hand == ESteamVRHand::VR_Left) ? SteamVRInputDevice->VRControllerHandleLeft : SteamVRInputDevice->VRControllerHandleRight;
			if (HandActionHandle != k_ulInvalidActionHandle)
			{
				InputPoseActionData_t PoseData = { 0 };
				EVRInputError InputError = VRInput()->GetPoseActionDataRelativeToNow(HandActionHandle, VRCompositor()->GetTrackingSpace(), PredictedSecondsFromNow, &PoseData, sizeof(PoseData), k_ulInvalidInputValueHandle);

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
					FQuat OrientationPose(Pose);
					OrientationQuat.X = -OrientationPose.Z;
					OrientationQuat.Y = OrientationPose.X;
					OrientationQuat.Z = OrientationPose.Y;
					OrientationQuat.W = -OrientationPose.W;

					FVector PositionPose = ((FVector(-Pose.M[3][2], Pose.M[3][0], Pose.M[3][1])) * GWorld->GetWorldSettings()->WorldToMeters);
					Position = PositionPose;

					//OutOrientation = BaseOrientation.Inverse() * OutOrientation;
					OrientationQuat.Normalize();
					Orientation = OrientationQuat.Rotator();
					Orientation.Normalize();

					return true;
				}
			}
		}
	}

	return false;
}

float USteamVRInputDeviceFunctionLibrary::GetSteamVR_GlobalPredictedSecondsFromNow()
{
	if (VRSystem() && VRInput())
	{
		FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
		if (SteamVRInputDevice != nullptr)
		{
			return SteamVRInputDevice->GlobalPredictedSecondsFromNow;
		}
	}

	return -9999.f;
}

float USteamVRInputDeviceFunctionLibrary::SetSteamVR_GlobalPredictedSecondsFromNow(float NewValue)
{
	if (VRSystem() && VRInput())
	{
		FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
		if (SteamVRInputDevice != nullptr)
		{
			SteamVRInputDevice->GlobalPredictedSecondsFromNow = NewValue;
			return SteamVRInputDevice->GlobalPredictedSecondsFromNow;
		}
	}

	return -9999.f;
}

void USteamVRInputDeviceFunctionLibrary::ShowAllSteamVR_ActionOrigins()
{
	VRActiveActionSet_t ActiveActionSets[1];
	VRInput()->ShowBindingsForActionSet(ActiveActionSets, sizeof(ActiveActionSets[0]), 0, 0);
}

TArray<FSteamVRInputBindingInfo> USteamVRInputDeviceFunctionLibrary::GetSteamVR_InputBindingInfo(FSteamVRAction SteamVRActionHandle)
{
	// Setup variables to be used by the OpenVR call
	TArray<FSteamVRInputBindingInfo> SteamVRBindingInfo;
	InputBindingInfo_t* InputBindingInfo = new InputBindingInfo_t[MAX_BINDINGINFO_COUNT];
	uint32_t BindingInfoCount = 0;
	EVRInputError BindingInfoError = VRInputError_NoData;

	// Execute OpenVR call GetActionBindingInfo if action handle is valid and VRInput is present
	if (VRInput() && !SteamVRActionHandle.Path.IsEmpty())
	{
		// Get binding info for provided action handle in the currently active controller type
		BindingInfoError = VRInput()->GetActionBindingInfo(SteamVRActionHandle.Handle, InputBindingInfo, sizeof(InputBindingInfo_t), MAX_BINDINGINFO_COUNT, &BindingInfoCount);

		// Check if call is successful
		if (BindingInfoError != VRInputError_None)
		{
			UE_LOG(LogTemp, Error, TEXT("Error retrieving binding info for active controller: %i"), (int)BindingInfoError);
		}
		else
		{
			// Check if there are binding info related to the action
			if (BindingInfoCount > 0)
			{
				// If there are, go through each binding info found
				for (int32 i = 0; i < (int32)BindingInfoCount; i++)
				{
					// Convert binding info text from a char array to a UE4 FString
					FString DevicePathName = FString(UTF8_TO_TCHAR(InputBindingInfo[i].rchDevicePathName));
					FString InputPathName = FString(UTF8_TO_TCHAR(InputBindingInfo[i].rchInputPathName));
					FString ModeName = FString(UTF8_TO_TCHAR(InputBindingInfo[i].rchModeName));
					FString SlotName = FString(UTF8_TO_TCHAR(InputBindingInfo[i].rchSlotName));

					// Create a BP compatible binding info struct and add this to the output array
					SteamVRBindingInfo.Add(FSteamVRInputBindingInfo(
						FName(*DevicePathName),
						FName(*InputPathName),
						FName(*ModeName),
						FName(*SlotName)
					));

					//UE_LOG(LogTemp, Error, TEXT("Found Binding Info: %s %s %s %s"), *DevicePathName, *InputPathName, *ModeName, *SlotName);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No binding info found for action: %s"), *SteamVRActionHandle.Path);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error calling GetSteamVRInputBindingInfo from Blueprint. OpenVRInput is not present or SteamVR Action Handle provided as input is invalid or empty!"));
	}

	return SteamVRBindingInfo;
}

TArray<FSteamVRInputBindingInfo> USteamVRInputDeviceFunctionLibrary::FindSteamVR_InputBindingInfo(FName ActionName, FName ActionSet /*= FName("main")*/)
{
	// Check for a valid OpenVR session and action name/set
	if (VRInput() && ActionName != NAME_None && ActionSet != NAME_None)
	{
		// Get the action handle for given action name and action set
		bool bFindResult = false;
		FSteamVRAction FoundAction;
		FSteamVRActionSet FoundActionSet;
		FindSteamVR_Action(ActionName, bFindResult, FoundAction, FoundActionSet, ActionSet);

		// Check if we found the action
		if (bFindResult)
		{
			// If found, get the binding info and return the result
			return GetSteamVR_InputBindingInfo(FoundAction);
		}
		else
		{
			// Otherwise, provide an error message
			UE_LOG(LogTemp, Error, TEXT("Error when calling FindSteamVRInputBindingInfo, there was no Action found with the name %s in action set %s"), *ActionName.ToString(), *ActionSet.ToString());
		}

	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error calling FindSteamVRInputBindingInfo from Blueprint. OpenVRInput is not present or SteamVR action name/set provided is empty!"));
	}

	// Return an empty array of binding info
	TArray<FSteamVRInputBindingInfo> EmptyInfo;
	return EmptyInfo;

}

bool USteamVRInputDeviceFunctionLibrary::ResetSeatedPosition()
{
	if (VRSystem() && VRInput())
	{
		if (VRCompositor()->GetTrackingSpace() == TrackingUniverseSeated)
		{
			VRSystem()->ResetSeatedZeroPose();
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Request to Reset Seated Position was ignored. Use \"Set Tracking Origin\" to \"Eye\" before calling this function."));
		}
	}

	return false;
}


float USteamVRInputDeviceFunctionLibrary::GetUserIPD()
{
	if (VRSystem())
	{
		return VRSystem()->GetFloatTrackedDeviceProperty(k_unTrackedDeviceIndex_Hmd, ETrackedDeviceProperty::Prop_UserIpdMeters_Float) * 1000; // Return IPD in mm
	}

	return 0.f;
}

void USteamVRInputDeviceFunctionLibrary::ShowBindingsUI(EHand Hand, FName ActionSet /*= FName("main")*/, bool bShowInVR /*= true*/)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice && VRSystem() && VRInput())
	{
		// Get current SteamVR Action Sets
		FString InActionSet = TEXT("/actions/") + ActionSet.ToString();
		TArray<FSteamVRActionSet> SteamVRActionSets;
		GetSteamVR_ActionSetArray(SteamVRActionSets);

		// Find Action Set
		FSteamVRActionSet FoundActionSet;
		bool bActionSetFound = false;
		for (FSteamVRActionSet SteamVRDefinedActionSet : SteamVRActionSets)
		{
			if (SteamVRDefinedActionSet.Path.Equals(InActionSet, ESearchCase::IgnoreCase))
			{
				FoundActionSet = SteamVRDefinedActionSet;
				bActionSetFound = true;
				break;
			}
		}

		// Set selected hand
		VRActionHandle_t SelectedHand;
		switch (Hand)
		{
			case EHand::VR_RightHand:
				SelectedHand = SteamVRInputDevice->VRControllerHandleRight;
				break;
		
			default:
				SelectedHand = SteamVRInputDevice->VRControllerHandleLeft;
				break;
		}

		if (bActionSetFound)
		{
			VRInput()->OpenBindingUI(nullptr, FoundActionSet.Handle, SelectedHand, !bShowInVR);	
		}
		else
		{
			VRInput()->OpenBindingUI(nullptr, k_ulInvalidActionSetHandle, SelectedHand, !bShowInVR);
		}
	}
}

bool USteamVRInputDeviceFunctionLibrary::DeleteUserInputIni(FString& UserInputFile)
{
	if (GEngine)
	{
		FString PlatformDir = UGameplayStatics::GetPlatformName();

		#if !WITH_EDITOR
		PlatformDir += FString(TEXT("NoEditor"));
		#endif

		UserInputFile = FPaths::ProjectUserDir() / "Saved" / "Config" / PlatformDir / "Input.ini";
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

		if (PlatformFile.FileExists(*UserInputFile) && PlatformFile.DeleteFile(*UserInputFile))
		{
			return true;
		}
	}

	return false;
}

FTransform USteamVRInputDeviceFunctionLibrary::GetUETransform(VRBoneTransform_t SteamBoneTransform)
{
	FTransform RetTransform;

	// Calculate UE Bone Transform
	FQuat OrientationQuat(SteamBoneTransform.orientation.x,
		-SteamBoneTransform.orientation.y,
		SteamBoneTransform.orientation.z,
		-SteamBoneTransform.orientation.w);
	OrientationQuat.Normalize();
	RetTransform = FTransform(OrientationQuat,
		FVector(SteamBoneTransform.position.v[2] * -1.f,
			SteamBoneTransform.position.v[0],
			SteamBoneTransform.position.v[1]) * 0.1f);
	return RetTransform;
}

void USteamVRInputDeviceFunctionLibrary::LaunchBindingsURL()
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		FString BindingsURL = "http://localhost:8998/dashboard/controllerbinding.html?app=" + SteamVRInputDevice->EditorAppKey;
		UE_LOG(LogTemp, Warning, TEXT("%s"), *BindingsURL);
		FPlatformProcess::LaunchURL(*BindingsURL, nullptr, nullptr);
	}
}

void USteamVRInputDeviceFunctionLibrary::GetFingerCurlsAndSplays(EHand Hand, FSteamVRFingerCurls& FingerCurls, FSteamVRFingerSplays& FingerSplays, ESkeletalSummaryDataType SummaryDataType)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr && VRSystem() && VRInput())
	{
		// Get action state this frame
		VRActiveActionSet_t ActiveActionSets[] = {
			{
				SteamVRInputDevice->MainActionSet,
				k_ulInvalidInputValueHandle,
				k_ulInvalidActionSetHandle
			}
		};

		EVRInputError UpdateActionStateError = VRInput()->UpdateActionState(ActiveActionSets, sizeof(VRActiveActionSet_t), 1);
		if (UpdateActionStateError != VRInputError_None)
		{
			FingerCurls = {};
			FingerSplays = {};
			return;
		}

		// Setup which hand data we will get from SteamVR
		VRActionHandle_t ActiveSkeletalHand = k_ulInvalidActionHandle;

		if (Hand == EHand::VR_LeftHand && SteamVRInputDevice->bIsSkeletalControllerLeftPresent && SteamVRInputDevice->VRSkeletalHandleLeft != k_ulInvalidActionHandle)
		{
			ActiveSkeletalHand = SteamVRInputDevice->VRSkeletalHandleLeft;
		}
		else if (Hand == EHand::VR_RightHand && SteamVRInputDevice->bIsSkeletalControllerRightPresent && SteamVRInputDevice->VRSkeletalHandleRight != k_ulInvalidActionHandle)
		{
			ActiveSkeletalHand = SteamVRInputDevice->VRSkeletalHandleRight;
		}

		if (ActiveSkeletalHand == k_ulInvalidActionHandle)
		{
			FingerCurls = {};
			FingerSplays = {};
			return;
		}

		InputSkeletalActionData_t actionData;
		EVRInputError GetSkeletalActionDataError = VRInput()->GetSkeletalActionData(ActiveSkeletalHand, &actionData, sizeof(InputSkeletalActionData_t));

		if (GetSkeletalActionDataError != VRInputError_None)
		{
			FingerCurls = {};
			FingerSplays = {};
			return;
		}

		if (actionData.bActive == false)
		{
			FingerCurls = {};
			FingerSplays = {};
			return;
		}

		// Get Skeletal Summary Data
		VRSkeletalSummaryData_t ActiveSkeletalSummaryData;
		if (ActiveSkeletalHand == k_ulInvalidActionHandle)
		{
			FingerCurls = {};
			FingerSplays = {};
			return;
		}

		EVRSummaryType SteamVRSummaryType = (SummaryDataType == ESkeletalSummaryDataType::VR_SummaryType_FromDevice) ? VRSummaryType_FromDevice : VRSummaryType_FromAnimation;
		EVRInputError GetSkeletalSummaryDataError = VRInput()->GetSkeletalSummaryData(ActiveSkeletalHand, SteamVRSummaryType, &ActiveSkeletalSummaryData);

		if (GetSkeletalSummaryDataError != VRInputError_None)
		{
			FingerCurls = {};
			FingerSplays = {};
			return;
		}
		else if (GetSkeletalSummaryDataError == VRInputError_None)
		{
			// Update curls and splay values for output
			FingerCurls.Thumb = ActiveSkeletalSummaryData.flFingerCurl[VRFinger_Thumb];
			FingerCurls.Index = ActiveSkeletalSummaryData.flFingerCurl[VRFinger_Index];
			FingerCurls.Middle = ActiveSkeletalSummaryData.flFingerCurl[VRFinger_Middle];
			FingerCurls.Ring = ActiveSkeletalSummaryData.flFingerCurl[VRFinger_Ring];
			FingerCurls.Pinky = ActiveSkeletalSummaryData.flFingerCurl[VRFinger_Pinky];

			FingerSplays.Index_Middle = ActiveSkeletalSummaryData.flFingerSplay[VRFingerSplay_Index_Middle];
			FingerSplays.Middle_Ring = ActiveSkeletalSummaryData.flFingerSplay[VRFingerSplay_Middle_Ring];
			FingerSplays.Ring_Pinky = ActiveSkeletalSummaryData.flFingerSplay[VRFingerSplay_Ring_Pinky];
			FingerSplays.Thumb_Index = ActiveSkeletalSummaryData.flFingerSplay[VRFingerSplay_Thumb_Index];
			return;
		}
	}

	// Unable to retrieve the curls and splay values for this hand, send zeroed out values back to the user
	FingerCurls = {};
	FingerSplays = {};
}

FSteamVRInputDevice* USteamVRInputDeviceFunctionLibrary::GetSteamVRInputDevice()
{
	TArray<IMotionController*> MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
	for (auto MotionController : MotionControllers)
	{
		FSteamVRInputDevice* TestSteamVRDevice = static_cast<FSteamVRInputDevice*>(MotionController);
		if (TestSteamVRDevice != nullptr && !FGenericPlatformMath::IsNaN(TestSteamVRDevice->DeviceSignature) && TestSteamVRDevice->DeviceSignature == 2019)
		{
			return TestSteamVRDevice;
		}
	}
	return nullptr;
}

