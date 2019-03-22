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
	FSteamVRSkeletonRetarget SkeletonRetarget;

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
	void FillSteamVRHandTransforms(FSteamVRInputDevice* SteamVRInputDevice, VRBoneTransform_t* OutPose, VRBoneTransform_t* ReferencePose);
	void ProcessBoneMap(int32 BoneIndex, const FName& SrcBoneName);
	void UpdateBoneMap(const FName& SrcBoneName, const FName RetargetName);
	void GetSteamVRBoneTransform(int32 SteamVRBoneIndex, FTransform& OutTransform);
	FSteamVRInputDevice* GetSteamVRInputDevice();

};
