#pragma once

#include "ILiveLinkSource.h"
#include "Containers/Ticker.h"
#include "GameFramework/InputSettings.h"
#include "openvr.h"

using namespace vr;

enum class EKnucklesBone : uint8
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

class ILiveLinkClient;

class KNUCKLESLIVELINK_API FKnucklesLiveLinkSource : public ILiveLinkSource
{
public:

	FKnucklesLiveLinkSource();
	virtual ~FKnucklesLiveLinkSource();

	// Begin ILiveLinkSource Interface
	virtual void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;
	virtual bool IsSourceStillValid() override;
	virtual bool RequestSourceShutdown() override;
	virtual FText GetSourceType() const override { return SourceType; };
	virtual FText GetSourceMachineName() const override { return SourceMachineName; }
	virtual FText GetSourceStatus() const override { return SourceStatus; }
	// End ILiveLinkSource Interface

	// Add a Tick Function to this livelink source
	bool Tick(float DeltaTime);

	// Update LiveLinkData
	void UpdateLiveLink();

	// SteamVRSytem
	IVRSystem* SteamVRSystem = nullptr;

	// SteamVR States
	bool bSteamVRPresent = false;
	bool bLeftKnucklesPresent = false;
	bool bRightKnucklesPresent = false;

	// SteamVR Knuckles Anim Configurable States
	bool bRangeWithControllerL = false;
	bool bRangeWithControllerR = false;
	bool bWithKnucklesAnim = true;

private:

	// Tick Delegate Handles
	FTickerDelegate TickDelegate;
	FDelegateHandle TickDelegateHandle;

	// Livelink Identifiers
	bool bIsInitialized = false;
	ILiveLinkClient* LiveLinkClient;
	FGuid LiveLinkSourceGuid;
	FName SubjectNameKnuckles = FName(TEXT("KnucklesAnimation"));

	// Livelink Source Parameters
	FText SourceType;
	FText SourceMachineName;
	FText SourceStatus;

	// Skeletal Motion Range
	EVRSkeletalMotionRange MotionRangeLeft = VRSkeletalMotionRange_WithoutController;
	EVRSkeletalMotionRange MotionRangeRight = VRSkeletalMotionRange_WithoutController;

	// Skeleton Info
	static const uint32 BoneCount = 31;
	FLiveLinkRefSkeleton LiveLinkRefSkeleton;
	TArray<EKnucklesBone> BoneKeypoints;
	TArray<FName> BoneNames;
	TArray<int32> BoneParents;

	// SteamVR Input System
	VRActionSetHandle_t SteamVRActionSetDefault;

	VRActionHandle_t SteamVRSkeletonLeft;
	VRActionHandle_t SteamVRSkeletonRight;

	VRInputValueHandle_t SteamActiveOriginLeft;
	VRInputValueHandle_t SteamActiveOriginRight;

	EVRSkeletalTrackingLevel vrSkeletalTrackingLevel;
	InputSkeletalActionData_t vrSeketalActionDataLeft = { 0 };
	InputSkeletalActionData_t vrSeketalActionDataRight = { 0 };

	int KnucklesControllerIdLeft = -1;
	VRControllerState_t controllerStateLeft = { 0 };
	TrackedDevicePose_t controllerPoseLeft = { 0 };

	int KnucklesControllerIdRight = -1;
	VRControllerState_t controllerStateRight = { 0 };
	TrackedDevicePose_t controllerPoseRight = { 0 };

	VRBoneTransform_t vrBoneTransformLeft[BoneCount];
	VRBoneTransform_t vrBoneTransformRight[BoneCount];

	void GetInputError(EVRInputError InputError, FString InputAction);
	void CheckForSkeletalController();
	FTransform GetUEBoneTransform(VRBoneTransform_t SteamBoneTransform, FVector OverrideVector = FVector::ZeroVector, bool bOverrideVector = false);
};
