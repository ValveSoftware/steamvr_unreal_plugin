#pragma once

#include "Core.h"
#include "SteamVRInputDevice.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Features/IModularFeatures.h"
#include "IMotionController.h"
#include "SteamVRInputDeviceFunctionLibrary.generated.h"

#define STEAMVRCONTROLLER_SUPPORTED_PLATFORMS (PLATFORM_MAC || (PLATFORM_LINUX && PLATFORM_CPU_X86_FAMILY && PLATFORM_64BITS) || (PLATFORM_WINDOWS && WINVER > 0x0502))

//#define ACTION_MANIFEST					"steamvr_manifest.json"
#define ACTION_PATH_VIBRATE_LEFT		"/actions/main/out/vibrateleft"
#define ACTION_PATH_VIBRATE_RIGHT		"/actions/main/out/vibrateright"


USTRUCT(BlueprintType)
struct STEAMVRINPUTDEVICE_API FSteamVRSkeletonTransform
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Wrist;

	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Thumb_Metacarpal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Thumb_Proximal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Thumb_Middle;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Thumb_Distal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Thumb_Tip;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Thumb_Aux;

	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Index_Metacarpal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Index_Proximal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Index_Middle;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Index_Distal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Index_Tip;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Index_Aux;

	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Middle_Metacarpal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Middle_Proximal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Middle_Middle;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Middle_Distal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Middle_Tip;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Middle_Aux;

	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Ring_Metacarpal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Ring_Proximal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Ring_Middle;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Ring_Distal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Ring_Tip;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Ring_Aux;

	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Pinky_Metacarpal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Pinky_Proximal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Pinky_Middle;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Pinky_Distal;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Pinky_Tip;
	UPROPERTY(EditAnywhere, Category = "SteamVR Input")
	FTransform Pinky_Aux;
};

/*
 * Helper Library for Knuckles Controllers
 */
UCLASS()
class STEAMVRINPUTDEVICE_API USteamVRInputDeviceFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FSteamVRInputDevice* GetSteamVRInputDevice();

	UFUNCTION(BlueprintCallable, Category="SteamVR Input")
	static void PlaySteamVR_HapticFeedback(bool VibrateLeft, float StartSecondsFromNow, float DurationSeconds = 1.f,
			float Frequency = 1.f, float Amplitude = 0.5f);

	UFUNCTION(BlueprintCallable, Category="SteamVR Input")
	static void GetCurlsAndSplaysState(bool& LeftHandState, bool& RightHandState);

	UFUNCTION(BlueprintCallable, Category="SteamVR Input")
	static void GetSkeletalTransformsState(bool& LeftHandState, bool& RightHandState);

	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void SetCurlsAndSplaysState(bool NewLeftHandState, bool NewRightHandState);

	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void SetSkeletalTransformsState(bool NewLeftHandState, bool NewRightHandState);

	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void GetSkeletalMotionRange(bool& LeftHandWithController, bool& RightHandWithController);

	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void SetSkeletalMotionRange(bool LeftHandWithController, bool RightHandWithController);

	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void GetSkeletalTransform(FSteamVRSkeletonTransform& LeftHand, FSteamVRSkeletonTransform& RightHand);
	static FTransform GetUETransform(VRBoneTransform_t SteamBoneTransform);

	static void RegenActionManifest();
	static void RegenControllerBindings();
};
