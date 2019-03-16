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
		LeftHand.Wrist = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[0]);

		LeftHand.Thumb_Aux = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[1]);
		LeftHand.Thumb_Distal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[2]);
		LeftHand.Thumb_Metacarpal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[3]);
		LeftHand.Thumb_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[4]);
		LeftHand.Thumb_Proximal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[5]);
		LeftHand.Thumb_Tip = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[6]);

		LeftHand.Index_Aux = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[7]);
		LeftHand.Index_Distal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[8]);
		LeftHand.Index_Metacarpal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[9]);
		LeftHand.Index_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[10]);
		LeftHand.Index_Proximal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[11]);
		LeftHand.Index_Tip = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[12]);

		LeftHand.Middle_Aux = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[13]);
		LeftHand.Middle_Distal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[14]);
		LeftHand.Middle_Metacarpal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[15]);
		LeftHand.Middle_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[16]);
		LeftHand.Middle_Proximal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[17]);
		LeftHand.Middle_Tip = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[18]);

		LeftHand.Ring_Aux = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[19]);
		LeftHand.Ring_Distal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[20]);
		LeftHand.Ring_Metacarpal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[21]);
		LeftHand.Ring_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[22]);
		LeftHand.Ring_Proximal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[23]);
		LeftHand.Ring_Tip = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[24]);

		LeftHand.Pinky_Aux = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[25]);
		LeftHand.Pinky_Distal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[26]);
		LeftHand.Pinky_Metacarpal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[27]);
		LeftHand.Pinky_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[28]);
		LeftHand.Pinky_Proximal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[29]);
		LeftHand.Pinky_Tip = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[30]);

		// Right Hand
		RightHand.Wrist = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[0]);

		RightHand.Thumb_Aux = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[1]);
		RightHand.Thumb_Distal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[2]);
		RightHand.Thumb_Metacarpal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[3]);
		RightHand.Thumb_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[4]);
		RightHand.Thumb_Proximal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[5]);
		RightHand.Thumb_Tip = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[6]);

		RightHand.Index_Aux = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[7]);
		RightHand.Index_Distal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[8]);
		RightHand.Index_Metacarpal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[9]);
		RightHand.Index_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[10]);
		RightHand.Index_Proximal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[11]);
		RightHand.Index_Tip = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[12]);

		RightHand.Middle_Aux = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[13]);
		RightHand.Middle_Distal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[14]);
		RightHand.Middle_Metacarpal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[15]);
		RightHand.Middle_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[16]);
		RightHand.Middle_Proximal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[17]);
		RightHand.Middle_Tip = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[18]);

		RightHand.Ring_Aux = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[19]);
		RightHand.Ring_Distal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[20]);
		RightHand.Ring_Metacarpal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[21]);
		RightHand.Ring_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[22]);
		RightHand.Ring_Proximal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[23]);
		RightHand.Ring_Tip = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[24]);

		RightHand.Pinky_Aux = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[25]);
		RightHand.Pinky_Distal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[26]);
		RightHand.Pinky_Metacarpal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[27]);
		RightHand.Pinky_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[28]);
		RightHand.Pinky_Proximal = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[29]);
		RightHand.Pinky_Tip = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[30]);

	}
}

FTransform USteamVRInputDeviceFunctionLibrary::GetUETransform(VRBoneTransform_t SteamBoneTransform)
{
	FTransform RetTransform;

	// Calculate UE Bone Transform
	RetTransform = FTransform(FQuat(SteamBoneTransform.orientation.x,
		-SteamBoneTransform.orientation.y,
		SteamBoneTransform.orientation.z,
		-SteamBoneTransform.orientation.w),
		FVector(SteamBoneTransform.position.v[2] * -1.f,
			SteamBoneTransform.position.v[0],
			SteamBoneTransform.position.v[1]));

	// Ensure Rotations are Normalized
	if (!RetTransform.GetRotation().IsNormalized())
	{
		RetTransform.GetRotation().Normalize();
	}

	return RetTransform;
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