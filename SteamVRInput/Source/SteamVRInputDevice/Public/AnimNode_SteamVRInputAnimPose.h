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

/**
* Custom animation node to retrieve poses from the Skeletal Input System
*/
USTRUCT(BlueprintType)
struct STEAMVRINPUTDEVICE_API FAnimNode_SteamVRInputAnimPose : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

	/** Range of motion for the skeletal input values */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AlwaysAsPin))
	EMotionRange MotionRange = EMotionRange::VR_WithoutController;

	/** Which hand should the animation node retrieve skeletal input values for */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AlwaysAsPin))
	EHand Hand = EHand::VR_LeftHand;

	/** What kind of skeleton are we dealing with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AlwaysAsPin))
	EHandSkeleton HandSkeleton = EHandSkeleton::VR_SteamVRHandSkeleton;

	/** Should the pose be mirrored so it can be applied to the opposite hand */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = ( AlwaysAsPin ))
	bool Mirror = false;

	/** The UE4 equivalent of the SteamVR Transform values per bone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SteamVRInput)
	FSteamVRSkeletonTransform SteamVRSkeletalTransform;

	/** SteamVR Skeleton to UE4 retargetting cache */
	UPROPERTY()
	FUE4RetargettingRefs UE4RetargettingRefs;

public:

	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext & Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	// End of FAnimNode_Base interface

	FAnimNode_SteamVRInputAnimPose();

	/** 
	 * Retarget the given array of bone transforms for the SteamVR skeleton to the UE4 hand skeleton and apply it to the given FPoseContext. 
	 * The bone transforms are in the bone's local space.  Assumes that PoseContest.Pose has already been set to its reference pose
	*/
	void PoseUE4HandSkeleton(FCompactPose& Pose, const FTransform* BoneTransformsLS, int32 BoneTransformCount);

	/** Retrieve the first active SteamVRInput device present in this game */
	FSteamVRInputDevice* GetSteamVRInputDevice();

	/** Recursively calculate the model-space transform of the given bone from the local-space transforms on the given pose */
	FTransform CalcModelSpaceTransform(const FCompactPose& Pose, FCompactPoseBoneIndex BoneIndex);

};
