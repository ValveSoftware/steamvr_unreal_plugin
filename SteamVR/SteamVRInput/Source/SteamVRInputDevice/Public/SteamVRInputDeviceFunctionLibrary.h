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

/** UE4 Bone definition of the SteamVR Skeleton */
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

/** Valid range of motion for a skeletal animation */
UENUM(BlueprintType)
enum class EMotionRange : uint8
{
	VR_WithoutController 	UMETA(DisplayName = "Without Controller"),
	VR_WithController 		UMETA(DisplayName = "With Controller")
};

/** Valid values for hands thats used for the Skeletal Input System calls */
UENUM(BlueprintType)
enum class EHand : uint8
{
	VR_LeftHand 	UMETA(DisplayName = "Left Hand"),
	VR_RightHand 	UMETA(DisplayName = "Right Hand")
};

/** Types of known skeletons that this animation node can handle */
UENUM(BlueprintType)
enum class EHandSkeleton : uint8
{
	VR_SteamVRHandSkeleton 	UMETA(DisplayName = "SteamVR Hand Skeleton"),
	VR_UE4HandSkeleton 		UMETA(DisplayName = "UE4 Hand Skeleton")
};

/** Skeletal Tracking Level of a controller */
UENUM(BlueprintType)
enum class EControllerFidelity : uint8
{
	VR_ControllerFidelity_Estimated 	UMETA(DisplayName = "Controller Fidelity Estimated"),
	VR_ControllerFidelity_Full 			UMETA(DisplayName = "Controller Fidelity Full"),
	VR_ControllerFidelity_Partial 		UMETA(DisplayName = "Controller Fidelity Partial")
};

/*
 * SteamVR Input Extended Functions
 * Functions and properties defined here are safe for developer use
 */
UCLASS()
class STEAMVRINPUTDEVICE_API USteamVRInputDeviceFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Retrieve the first available SteamVR Input device currently active in a game */
	static FSteamVRInputDevice* GetSteamVRInputDevice();

	/**
	* Generate haptic feedback in the requested controller
	* @param Hand - Which hand to send the controller feedback to
	* @param StartSecondsFromNow - hen to start the haptic feedback
	* @param DurationSeconds - How long to have the haptic feedback active
	* @param Frequency - Frequency used in the haptic feedback
	* @param Amplitude - Amplitude used in the haptic feedback
	*/
	UFUNCTION(BlueprintCallable, Category="SteamVR Input")
	static void PlaySteamVR_HapticFeedback(ESteamVRHand Hand, float StartSecondsFromNow, float DurationSeconds = 1.f,
			float Frequency = 1.f, float Amplitude = 0.5f);

	/**
	* Check Whether or not Curls and Splay values are being retrieved per frame from the SteamVR Input System
	* @return LeftHandState - Whether or not curls and splay values are being retrieved from the left hand
	* @return RightHandState -  Whether or not curls and splay values are being retrieved from the right hand
	*/
	UFUNCTION(BlueprintCallable, Category="SteamVR Input")
	static void GetCurlsAndSplaysState(bool& LeftHandState, bool& RightHandState);

	/**
	* Check Whether or not controllers attached to either hand have Skeletal Input support
	* @return LeftHandState - Whether or not the controller attached to the player's left hand have skeletal input support
	* @return RightHandState -  Whether or not the controller attached to the player's right hand have skeletal input support
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void GetSkeletalState(bool& LeftHandState, bool& RightHandState);

	/** 
	* Retrieve skeletal tracking level for all controllers 
	* @return LeftControllerFidelity - The skeletal tracking level of the left controller
	* @return RightControllerFidelity -  The skeletal tracking level of the right controller
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void GetControllerFidelity(EControllerFidelity& LeftControllerFidelity, EControllerFidelity& RightControllerFidelity);

	/**
	* Tell SteamVR Whether or not to retrieve Curls and Splay values per frame
	* @param NewLeftHandState - Whether or not curls and splay values will be retrieved for the left hand
	* @param NewRightHandState -  Whether or not curls and splay values will be retrieved for the right hand
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void SetCurlsAndSplaysState(bool NewLeftHandState, bool NewRightHandState);

	/**
	* Retrieve the live skeletal input bone values from SteamVR
	* @return LeftHand - Per bone transform values for the left hand skeleton
	* @return RightHand - Per bone transform values for the right hand skeleton
	* @param bWithController - Whether or not retrieve skeletal input values with controller
	* @param bXAxisForward - Whether or not the Skeleton has the X axis facing forward
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void GetSkeletalTransform(FSteamVRSkeletonTransform& LeftHand, FSteamVRSkeletonTransform& RightHand, bool bWithController=false);
	
	/**
	* Retrieve the left hand pose information - position, orientation and velocities
	* @return Position - Translation from the pose data matrix in UE coordinates
	* @return Orientation - Orientation derived from the pose data matrix in UE coordinates
	* @return AngularVelocity - The angular velocity of the hand this frame
	* @return Velocity - The velocity of the hand this frame
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void GetLeftHandPoseData(FVector& Position, FRotator& Orientation, FVector& AngularVelocity, FVector& Velocity);

	/**
	* Retrieve the right hand pose information - position, orientation and velocities
	* @return Position - Translation from the pose data matrix in UE coordinates
	* @return Orientation - Orientation derived from the pose data matrix in UE coordinates
	* @return AngularVelocity - The angular velocity of the hand this frame
	* @return Velocity - The velocity of the hand this frame
	*/
	UFUNCTION(BlueprintCallable, Category = "SteamVR Input")
	static void GetRightHandPoseData(FVector& Position, FRotator& Orientation, FVector& AngularVelocity, FVector& Velocity);

	/**
	* Get the SteamVR Bone Transform value in UE coordinates
	* @param SteamBoneTransform - The SteamVR Bone Transform value to get the UE coordinates for
	*/
	static FTransform GetUETransform(VRBoneTransform_t SteamBoneTransform);

	/** Regenerate the action manifest used by SteamVR Input System */
	static void RegenActionManifest();
	
	/** Regenerate Controller Bindings for supported SteamVR Controller types */
	static void RegenControllerBindings();
	
	/** Live reload the action manifest and register it to the SteamVR Input System */
	static void ReloadActionManifest();
	
	/** Open the SteamVR Controller Input Dashboard in the user#s default browser */
	static void LaunchBindingsURL();
};
