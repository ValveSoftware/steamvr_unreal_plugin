#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "SteamVRInputDeviceFunctionLibrary.h"
#include "AnimNode_SteamVRInputAnimPose.generated.h"

USTRUCT()
struct FAnimNode_SteamVRInputAnimPose : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()

	/** Base Pose */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Links)
	FPoseLink BasePose;

	/** SteamVR Bone Transforms */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Links, meta = (PinShownByDefault))
	FSteamVRSkeletonTransform SteamVRTransform;
	
public:
	
	// FAnimNode_Base interface
	virtual void Initialize(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones(const FAnimationCacheBonesContext & Context) override;
	virtual void Update(const FAnimationUpdateContext & Context) override;
	virtual void Evaluate(FPoseContext& Output) override;
	// End of FAnimNode_Base interface

public:
	
	FAnimNode_SteamVRInputAnimPose();
};