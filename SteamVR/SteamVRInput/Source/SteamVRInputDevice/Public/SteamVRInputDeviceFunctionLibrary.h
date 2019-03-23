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


#pragma once

#include "Core.h"
#include "SteamVRInputDevice.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Features/IModularFeatures.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformProcess.h"
#include "IMotionController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "SteamVRInputDeviceFunctionLibrary.generated.h"

#define STEAMVRCONTROLLER_SUPPORTED_PLATFORMS (PLATFORM_MAC || (PLATFORM_LINUX && PLATFORM_CPU_X86_FAMILY && PLATFORM_64BITS) || (PLATFORM_WINDOWS && WINVER > 0x0502))

//#define ACTION_MANIFEST					"steamvr_manifest.json"
#define ACTION_PATH_VIBRATE_LEFT		"/actions/main/out/vibrateleft"
#define ACTION_PATH_VIBRATE_RIGHT		"/actions/main/out/vibrateright"


USTRUCT(BlueprintType)
struct STEAMVRINPUTDEVICE_API FSteamVRSkeletonTransform
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Root;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Wrist;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Thumb_0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Thumb_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Thumb_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Thumb_3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Index_0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Index_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Index_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Index_3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Index_4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Middle_0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Middle_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Middle_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Middle_3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Middle_4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Ring_0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Ring_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Ring_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Ring_3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Ring_4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Pinky_0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Pinky_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Pinky_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Pinky_3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Pinky_4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Aux_Thumb;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Aux_Index;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Aux_Middle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Aux_Ring;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Aux_Pinky;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SteamVR Input")
	FTransform Bone_Count;
};

USTRUCT(BlueprintType)
struct STEAMVRINPUTDEVICE_API FSteamVRBoneMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Wrist;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Thumb_0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Thumb_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Thumb_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Thumb_3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Index_0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Index_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Index_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Index_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Index_4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Middle_0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Middle_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Middle_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Middle_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Middle_4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Ring_0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Ring_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Ring_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Ring_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Ring_4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Pinky_0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Pinky_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Pinky_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Pinky_3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SteamVR Input")
	FName Pinky_4;
};

UENUM(BlueprintType)	
enum class ESteamVRHand : uint8
{
	VR_Left		UMETA(DisplayName = "Left"),
	VR_Right	UMETA(DisplayName = "Right")
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
	static void PlaySteamVR_HapticFeedback(ESteamVRHand Hand, float StartSecondsFromNow, float DurationSeconds = 1.f,
			float Frequency = 1.f, float Amplitude = 0.5f);

	UFUNCTION(BlueprintCallable, Category="SteamVR Input")
	static void GetCurlsAndSplaysState(bool& LeftHandState, bool& RightHandState);

	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void SetCurlsAndSplaysState(bool NewLeftHandState, bool NewRightHandState);

	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void GetSkeletalTransform(FSteamVRSkeletonTransform& LeftHand, FSteamVRSkeletonTransform& RightHand, bool bWithController=false);
	static FTransform GetUETransform(VRBoneTransform_t SteamBoneTransform, VRBoneTransform_t SteamBoneReference);

	static void RegenActionManifest();
	static void RegenControllerBindings();
	static void ReloadActionManifest();
	static void LaunchBindingsURL();
};
