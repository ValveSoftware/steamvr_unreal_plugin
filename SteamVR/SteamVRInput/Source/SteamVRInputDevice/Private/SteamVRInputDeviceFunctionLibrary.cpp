/*
Copyright 2019 Valve Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files(the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->RegenerateActionManifest();
	}
}

void USteamVRInputDeviceFunctionLibrary::RegenControllerBindings()
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->RegenerateControllerBindings();
	}
}

void USteamVRInputDeviceFunctionLibrary::ReloadActionManifest()
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->ReloadActionManifest();
	}
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

void USteamVRInputDeviceFunctionLibrary::SetCurlsAndSplaysState(bool NewLeftHandState, bool NewRightHandState)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->bCurlsAndSplaysEnabled_L = NewLeftHandState;
		SteamVRInputDevice->bCurlsAndSplaysEnabled_R = NewRightHandState;
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
		SteamVRInputDevice->GetSkeletalData(true, MotionRange, OutPose, STEAMVR_SKELETON_BONE_COUNT);

		LeftHand.Root = OutPose[0];
		LeftHand.Wrist = OutPose[1];

		LeftHand.Thumb_0 = OutPose[ 2 ];
		LeftHand.Thumb_1 = OutPose[ 3 ];
		LeftHand.Thumb_2 = OutPose[ 4 ];
		LeftHand.Thumb_3 = OutPose[ 5 ];

		LeftHand.Index_0 = OutPose[ 6 ];
		LeftHand.Index_1 = OutPose[ 7 ];
		LeftHand.Index_2 = OutPose[ 8 ];
		LeftHand.Index_3 = OutPose[ 9 ];
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
		SteamVRInputDevice->GetSkeletalData(true, MotionRange, OutPose, STEAMVR_SKELETON_BONE_COUNT);

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

	// Transform SteamVR Rotation Quaternion to a UE FRotator
	//FQuat OrientationQuat;
	//OrientationQuat.X = -SteamBoneTransform.orientation.z;
	//OrientationQuat.Y = SteamBoneTransform.orientation.x;
	//OrientationQuat.Z = SteamBoneTransform.orientation.y;
	//OrientationQuat.W = -SteamBoneTransform.orientation.w;
	//OrientationQuat.Normalize();

	//// Transform Position
	//FVector Position = FVector((-SteamBoneTransform.position.v[2], SteamBoneTransform.position.v[0], SteamBoneTransform.position.v[1]))* GWorld->GetWorldSettings()->WorldToMeters;
	//
	//return FTransform(OrientationQuat, Position);
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

FSteamVRInputDevice* USteamVRInputDeviceFunctionLibrary::GetSteamVRInputDevice()
{
	TArray<IMotionController*> MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
	for (auto MotionController : MotionControllers)
	{
		FName DeviceName = MotionController->GetModularFeatureName();
		return static_cast<FSteamVRInputDevice*>(MotionController);
	}

	return nullptr;
}
