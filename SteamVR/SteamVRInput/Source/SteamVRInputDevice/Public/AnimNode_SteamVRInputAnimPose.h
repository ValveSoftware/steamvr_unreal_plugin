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

#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "SteamVRInputDeviceFunctionLibrary.h"
#include "SteamVRSkeletonDefinition.h"
#include "AnimNode_SteamVRInputAnimPose.generated.h"

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
	VR_UE4HandSkeleton 		UMETA(DisplayName = "UE4 Hand Skeleton"),
	VR_CustomSkeleton 		UMETA(DisplayName = "Custom Skeleton")
};

/** Axis where the skeleton faces forward from the fbx import */
UENUM(BlueprintType)
enum class ESkeletonForwardAxis : uint8
{
	VR_SkeletonAxisX 	UMETA(DisplayName = "X Axis"),
	VR_SkeletonAxisY	UMETA(DisplayName = "Y Axis")
};

/** 
* Custom animation node to retrieve poses from the Skeletal Input System
*/
USTRUCT()
struct FAnimNode_SteamVRInputAnimPose : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

	/** Range of motion for the skeletal input values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AlwaysAsPin))
	EMotionRange MotionRange;

	/** Which hand should the animation node retrieve skeletal input values for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AlwaysAsPin))
	EHand Hand;

	/** What kind of skeleton are we dealing with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AlwaysAsPin))
	EHandSkeleton HandSkeleton;

	/** Which axis is this skeleton facing forward to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AlwaysAsPin))
	ESkeletonForwardAxis SkeletonForwardAxis;

	/** A simple bone map SteamVR stock skeleton to this active skeleton */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	FSteamVRBoneMapping CustomBoneMapping;

	/** The UE4 euivalent of the SteamVR Transform values per bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links)
	FSteamVRSkeletonTransform SteamVRSkeletalTransform;

public:

	// FAnimNode_Base interface
	virtual void Initialize(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones(const FAnimationCacheBonesContext & Context) override;
	virtual void Update(const FAnimationUpdateContext & Context) override;
	virtual void Evaluate(FPoseContext& Output) override;
	// End of FAnimNode_Base interface

	FAnimNode_SteamVRInputAnimPose();

	/** Simple bone map for the current active skeleton to a stock SteamVR hand skeleton */
	TMap<FName, FName> BoneNameMap;

	/** A list of names that will be processed by this animation node */
	TArray<FName, TMemStackAllocator<>> TransformedBoneNames;

	/**
	* Retrieve the SteamVR bone index mapped to the given UE4 hand bone
	* @param UE4BoneIndex - The SteamVR Bone Transform value to get the UE coordinates for
	*/
	int32 GetSteamVRHandIndex(int32 UE4BoneIndex);

	/**
	* Do any custom mapping (if any) of bones for currently playing animation
	* @param BoneIndex - The mesh bone index to handle
	* @param SrcBoneName - The name of the bone
	*/
	void ProcessBoneMap(int32 BoneIndex, const FName& SrcBoneName);

	/**
	* Live retargetting of bones from bone names
	* @param SrcBoneName - The name of the bone from the SteamVR skeleton
	* @param RetargetName - The name of the bone for the active target skeleton
	*/
	void UpdateBoneMap(const FName& SrcBoneName, const FName RetargetName);

	/** Retrieve the first active SteamVRInput device present in this game */
	FSteamVRInputDevice* GetSteamVRInputDevice();

};
