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

#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "SteamVRInputDeviceFunctionLibrary.h"
#include "SteamVRSkeletonDefinition.h"
#include "AnimNode_SteamVRInputAnimPose.generated.h"


UENUM(BlueprintType)
enum class EMotionRange : uint8
{
	VR_WithoutController 	UMETA(DisplayName = "Without Controller"),
	VR_WithController 		UMETA(DisplayName = "With Controller")
};

UENUM(BlueprintType)
enum class EHand : uint8
{
	VR_LeftHand 	UMETA(DisplayName = "Left Hand"),
	VR_RightHand 	UMETA(DisplayName = "Right Hand")
};

UENUM(BlueprintType)
enum class EHandSkeleton : uint8
{
	VR_SteamVRHandSkeleton 	UMETA(DisplayName = "SteamVR Hand Skeleton"),
	VR_UE4HandSkeleton 		UMETA(DisplayName = "UE4 Hand Skeleton"),
	VR_CustomSkeleton 		UMETA(DisplayName = "Custom Skeleton")
};


USTRUCT()
struct FAnimNode_SteamVRInputAnimPose : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

	/** Base Pose 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Links)
	FPoseLink BasePose;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AlwaysAsPin))
	EMotionRange MotionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AlwaysAsPin))
	EHand Hand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AlwaysAsPin))
	EHandSkeleton HandSkeleton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (PinShownByDefault))
	FSteamVRBoneMapping SkeletonRetarget;

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
	FSteamVRSkeletonTransform LeftHand;
	FSteamVRSkeletonTransform RightHand;
	TMap<FName, FName> BoneNameMap;
	TArray<FName, TMemStackAllocator<>> TransformedBoneNames;

	FTransform GetUETransform(VRBoneTransform_t SteamBoneTransform, VRBoneTransform_t SteamBoneReference);
	int32 GetSteamVRHandIndex(int32 UE4BoneIndex);
	void FillSteamVRHandTransforms(FSteamVRInputDevice* SteamVRInputDevice, VRBoneTransform_t* OutPose, VRBoneTransform_t* ReferencePose);
	void ProcessBoneMap(int32 BoneIndex, const FName& SrcBoneName);
	void UpdateBoneMap(const FName& SrcBoneName, const FName RetargetName);
	void GetSteamVRBoneTransform(int32 SteamVRBoneIndex, FTransform& OutTransform);
	FSteamVRInputDevice* GetSteamVRInputDevice();

};
