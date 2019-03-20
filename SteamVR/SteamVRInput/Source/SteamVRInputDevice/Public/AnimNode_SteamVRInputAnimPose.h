#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "SteamVRInputDeviceFunctionLibrary.h"
#include "AnimNode_SteamVRInputAnimPose.generated.h"

enum class ESteamVRBone : uint8
{
	EBone_Root = 0,
	EBone_Wrist = 1,
	EBone_Thumb0 = 2,
	EBone_Thumb1 = 2,
	EBone_Thumb2 = 3,
	EBone_Thumb3 = 4,
	EBone_Thumb4 = 5,
	EBone_IndexFinger0 = 6,
	EBone_IndexFinger1 = 7,
	EBone_IndexFinger2 = 8,
	EBone_IndexFinger3 = 9,
	EBone_IndexFinger4 = 10,
	EBone_MiddleFinger0 = 11,
	EBone_MiddleFinger1 = 12,
	EBone_MiddleFinger2 = 13,
	EBone_MiddleFinger3 = 14,
	EBone_MiddleFinger4 = 15,
	EBone_RingFinger0 = 16,
	EBone_RingFinger1 = 17,
	EBone_RingFinger2 = 18,
	EBone_RingFinger3 = 19,
	EBone_RingFinger4 = 20,
	EBone_PinkyFinger0 = 21,
	EBone_PinkyFinger1 = 22,
	EBone_PinkyFinger2 = 23,
	EBone_PinkyFinger3 = 24,
	EBone_PinkyFinger4 = 25,
	EBone_Aux_Thumb = 26,
	EBone_Aux_IndexFinger = 27,
	EBone_Aux_MiddleFinger = 28,
	EBone_Aux_RingFinger = 29,
	EBone_Aux_PinkyFinger = 30,
	EBone_Count = 31
};

UENUM(BlueprintType)
enum class EMotionRange : uint8
{
	VR_WithoutController 	UMETA(DisplayName = "Without Controller"),
	VR_WithController 		UMETA(DisplayName = "With Controller")
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
	TArray<ESteamVRBone> BoneKeypoints;
	TArray<FName> BoneNames;
	TArray<int32> BoneParents;

	FTransform GetUETransform(VRBoneTransform_t SteamBoneTransform, VRBoneTransform_t SteamBoneReference);
	void FillHandTransforms(FSteamVRInputDevice* SteamVRInputDevice);
	void FillHandTransformsWithController(FSteamVRInputDevice* SteamVRInputDevice);
	FSteamVRInputDevice* GetSteamVRInputDevice();
};
