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