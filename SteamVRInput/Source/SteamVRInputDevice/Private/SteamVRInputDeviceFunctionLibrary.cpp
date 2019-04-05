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
#include "HAL/FileManagerGeneric.h"
using namespace vr;
#endif // STEAMVRCONTROLLER_SUPPORTED_PLATFORMS

void USteamVRInputDeviceFunctionLibrary::PlaySteamVR_HapticFeedback(ESteamVRHand Hand, float StartSecondsFromNow, float DurationSeconds, float Frequency, float Amplitude)
{
#if STEAMVRCONTROLLER_SUPPORTED_PLATFORMS
	EVRInitError SteamVRInitError = VRInitError_None;
	IVRSystem* SteamVRSystem = VR_Init(&SteamVRInitError, VRApplication_Scene);

	if (SteamVRInitError == VRInitError_None)
	{
		const FString ManifestPath = FPaths::GeneratedConfigDir() / ACTION_MANIFEST;

		VRActionHandle_t vrVibrationLeft;
		VRActionHandle_t vrVibrationRight;

		if (Amplitude < 0.f)
		{
			Amplitude = 0.f;
		}
		else if (Amplitude > 1.f)
		{
			Amplitude = 1.f;
		}

		EVRInputError Err;
		if (Hand == ESteamVRHand::VR_Left)
		{
			Err = VRInput()->GetActionHandle(TCHAR_TO_UTF8(*FString(TEXT(ACTION_PATH_VIBRATE_LEFT))), &vrVibrationLeft);
			Err = VRInput()->TriggerHapticVibrationAction(vrVibrationLeft, StartSecondsFromNow,
				DurationSeconds, Frequency, Amplitude, k_ulInvalidInputValueHandle);
			//UE_LOG(LogTemp, Warning, TEXT("[STEAMVR INPUT] Haptic (Left): %i"), (int32)Err);
		}
		else
		{
			Err = VRInput()->GetActionHandle(TCHAR_TO_UTF8(*FString(TEXT(ACTION_PATH_VIBRATE_RIGHT))), &vrVibrationRight);
			Err = VRInput()->TriggerHapticVibrationAction(vrVibrationRight, StartSecondsFromNow,
				DurationSeconds, Frequency, Amplitude, k_ulInvalidInputValueHandle);
			//UE_LOG(LogTemp, Warning, TEXT("[STEAMVR INPUT] Haptic (Right): %i"), (int32)Err);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[STEAMVR INPUT] Error initializing Steam VR during Haptic call: %i"), (int32)SteamVRInitError);
	}

#endif // STEAMVRCONTROLLER_SUPPORTED_PLATFORMS
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

		RightHand.Index_0 =  OutPose[ 6 ];
		RightHand.Index_1 =  OutPose[ 7 ];
		RightHand.Index_2 =  OutPose[ 8 ];
		RightHand.Index_3 =  OutPose[ 9 ];
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

		RightHand.Aux_Thumb =  OutPose[ 26 ];
		RightHand.Aux_Index =  OutPose[ 27 ];
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

void USteamVRInputDeviceFunctionLibrary::GetFingerCurlsAndSplays(EHand Hand, FSteamVRFingerCurls& FingerCurls, FSteamVRFingerSplays& FingerSplays)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr && VRInput() != nullptr)
	{
		// Get action state this frame
		VRActiveActionSet_t ActiveActionSets[] = {
			{
				SteamVRInputDevice->MainActionSet,
				k_ulInvalidInputValueHandle,
				k_ulInvalidActionSetHandle
			}
		};

		EVRInputError Err = VRInput()->UpdateActionState(ActiveActionSets, sizeof(VRActiveActionSet_t), 1);
		if (Err != VRInputError_None)
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

		// Get Skeletal Summary Data
		VRSkeletalSummaryData_t ActiveSkeletalSummaryData;
		if (ActiveSkeletalHand != k_ulInvalidActionHandle)
		{
			Err = VRInput()->GetSkeletalSummaryData(ActiveSkeletalHand, &ActiveSkeletalSummaryData);
		}
		else
		{
			FingerCurls = {};
			FingerSplays = {};
			return;
		}
		
		// Update curls and splay values for output
		if (Err == VRInputError_None)
		{
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

