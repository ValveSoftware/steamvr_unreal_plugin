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

UENUM(BlueprintType)	
enum class ESteamVRHand : uint8
{
	VR_Left		UMETA(DisplayName = "Left"),
	VR_Right	UMETA(DisplayName = "Right")
};

/*
 * SteamVR Input Extended Functions
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
	static FTransform GetUETransform(VRBoneTransform_t SteamBoneTransform);

	static void RegenActionManifest();
	static void RegenControllerBindings();
	static void ReloadActionManifest();
	static void LaunchBindingsURL();
};
