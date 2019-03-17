#include "SteamVRInputDeviceFunctionLibrary.h"

#if STEAMVRCONTROLLER_SUPPORTED_PLATFORMS
#include "../ThirdParty/OpenVRSDK/headers/openvr.h"
#include "HAL/FileManagerGeneric.h"
using namespace vr;
#endif // STEAMVRCONTROLLER_SUPPORTED_PLATFORMS

void USteamVRInputDeviceFunctionLibrary::PlaySteamVR_HapticFeedback(bool VibrateLeft, float StartSecondsFromNow, float DurationSeconds, float Frequency, float Amplitude)
{
#if STEAMVRCONTROLLER_SUPPORTED_PLATFORMS
	EVRInitError SteamVRInitError = VRInitError_None;
	IVRSystem* SteamVRSystem = VR_Init(&SteamVRInitError, VRApplication_Scene);

	if (SteamVRInitError == VRInitError_None)
	{
		const FString ManifestPath = FPaths::GeneratedConfigDir() / ACTION_MANIFEST;
		EVRInputError Err = VRInput()->SetActionManifestPath(TCHAR_TO_UTF8(*IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*ManifestPath)));
		
		if (Err == VRInputError_None)
		{
			VRActionHandle_t vrKnucklesVibrationLeft;
			VRActionHandle_t vrKnucklesVibrationRight;

			if (Amplitude < 0.f)
			{
				Amplitude = 0.f;
			}
			else if (Amplitude > 1.f)
			{
				Amplitude = 1.f;
			}

			if (VibrateLeft)
			{
				Err = VRInput()->GetActionHandle(TCHAR_TO_UTF8(*FString(TEXT(ACTION_PATH_VIBRATE_LEFT))), &vrKnucklesVibrationLeft);
				Err = VRInput()->TriggerHapticVibrationAction(vrKnucklesVibrationLeft, StartSecondsFromNow,
					DurationSeconds, Frequency, Amplitude, k_ulInvalidInputValueHandle);
			}
			else
			{
				Err = VRInput()->GetActionHandle(TCHAR_TO_UTF8(*FString(TEXT(ACTION_PATH_VIBRATE_RIGHT))), &vrKnucklesVibrationRight);
				Err = VRInput()->TriggerHapticVibrationAction(vrKnucklesVibrationRight, StartSecondsFromNow,
					DurationSeconds, Frequency, Amplitude, k_ulInvalidInputValueHandle);
			}
		}
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

void USteamVRInputDeviceFunctionLibrary::GetCurlsAndSplaysState(bool& LeftHandState, bool& RightHandState)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		LeftHandState = SteamVRInputDevice->bCurlsAndSplaysEnabled_L;
		RightHandState = SteamVRInputDevice->bCurlsAndSplaysEnabled_R;
	}
}

void USteamVRInputDeviceFunctionLibrary::GetSkeletalTransformsState(bool& LeftHandState, bool& RightHandState)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		LeftHandState = SteamVRInputDevice->bSkeletalTransformsEnabled_L;
		RightHandState = SteamVRInputDevice->bSkeletalTransformsEnabled_R;
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

void USteamVRInputDeviceFunctionLibrary::SetSkeletalTransformsState(bool NewLeftHandState, bool NewRightHandState)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->bSkeletalTransformsEnabled_L = NewLeftHandState;
		SteamVRInputDevice->bSkeletalTransformsEnabled_R = NewRightHandState;
	}
}

void USteamVRInputDeviceFunctionLibrary::GetSkeletalMotionRange(bool& LeftHandWithController, bool& RightHandWithController)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		LeftHandWithController = SteamVRInputDevice->bMotionRangeWithControllerL;
		RightHandWithController = SteamVRInputDevice->bMotionRangeWithControllerR;
	}
}

void USteamVRInputDeviceFunctionLibrary::SetSkeletalMotionRange(bool LeftHandWithController, bool RightHandWithController)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		SteamVRInputDevice->bMotionRangeWithControllerL = LeftHandWithController;
		SteamVRInputDevice->bMotionRangeWithControllerR = RightHandWithController;
	}
}

void USteamVRInputDeviceFunctionLibrary::GetSkeletalTransform(FSteamVRSkeletonTransform& LeftHand, FSteamVRSkeletonTransform& RightHand)
{
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		// Left Hand
		LeftHand.Root = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[0]);
		LeftHand.Wrist = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[1]);

		LeftHand.Thumb_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[2]);
		LeftHand.Thumb_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[3]);
		LeftHand.Thumb_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[4]);
		LeftHand.Thumb_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[5]);

		LeftHand.Index_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[6]);
		LeftHand.Index_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[7]);
		LeftHand.Index_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[8]);
		LeftHand.Index_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[9]);
		LeftHand.Index_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[10]);

		LeftHand.Middle_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[11]);
		LeftHand.Middle_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[12]);
		LeftHand.Middle_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[13]);
		LeftHand.Middle_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[14]);
		LeftHand.Middle_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[15]);

		LeftHand.Ring_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[16]);
		LeftHand.Ring_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[17]);
		LeftHand.Ring_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[18]);
		LeftHand.Ring_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[19]);
		LeftHand.Ring_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[20]);

		LeftHand.Pinky_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[21]);
		LeftHand.Pinky_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[22]);
		LeftHand.Pinky_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[23]);
		LeftHand.Pinky_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[24]);
		LeftHand.Pinky_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[25]);

		LeftHand.Aux_Thumb = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[26]);
		LeftHand.Aux_Index = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[27]);
		LeftHand.Aux_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[28]);
		LeftHand.Aux_Ring = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[27]);
		LeftHand.Aux_Pinky = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[29]);

		LeftHand.Bone_Count = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[30]);

		// Right Hand
		RightHand.Root = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[0]);
		RightHand.Wrist = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[1]);

		RightHand.Thumb_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[2]);
		RightHand.Thumb_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[3]);
		RightHand.Thumb_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[4]);
		RightHand.Thumb_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[5]);

		RightHand.Index_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[6]);
		RightHand.Index_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[7]);
		RightHand.Index_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[8]);
		RightHand.Index_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[9]);
		RightHand.Index_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[10]);

		RightHand.Middle_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[11]);
		RightHand.Middle_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[12]);
		RightHand.Middle_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[13]);
		RightHand.Middle_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[14]);
		RightHand.Middle_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[15]);

		RightHand.Ring_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[16]);
		RightHand.Ring_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[17]);
		RightHand.Ring_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[18]);
		RightHand.Ring_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[19]);
		RightHand.Ring_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[20]);

		RightHand.Pinky_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[21]);
		RightHand.Pinky_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[22]);
		RightHand.Pinky_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[23]);
		RightHand.Pinky_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[24]);
		RightHand.Pinky_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[25]);

		RightHand.Aux_Thumb = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[26]);
		RightHand.Aux_Index = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[27]);
		RightHand.Aux_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[28]);
		RightHand.Aux_Ring = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[27]);
		RightHand.Aux_Pinky = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[29]);

		RightHand.Bone_Count = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[30]);

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

//void USteamVRInputDeviceFunctionLibrary::GetSkeletalTransform(FSteamVRSkeletonTransform& LeftHand, FSteamVRSkeletonTransform& RightHand)
//{
//	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
//	if (SteamVRInputDevice != nullptr)
//	{
//		LeftHand = SteamVRInputDevice->SkeletonTransform_L;
//		RightHand = SteamVRInputDevice->SkeletonTransform_R;
//	}
//}