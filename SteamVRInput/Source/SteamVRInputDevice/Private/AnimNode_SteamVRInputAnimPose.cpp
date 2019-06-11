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


#include "AnimNode_SteamVRInputAnimPose.h"
#include "ISteamVRInputDeviceModule.h"
#include "AnimationRuntime.h"
#include "Runtime/Engine/Public/Animation/AnimInstanceProxy.h"
#include "SteamVRInputDevice.h"
#include "UE4HandSkeletonDefinition.h"

// Lookup table that maps each bone in the UE4 hand skeleton to the corresponding
// bone in the SteamVR hand skeleton
static const int32 kUE4BoneToSteamVRBone[] = {
	ESteamVRBone_Wrist,			// EUE4HandBone_Wrist
	ESteamVRBone_IndexFinger1,	// EUE4HandBone_Index_01
	ESteamVRBone_IndexFinger2,	// EUE4HandBone_Index_02
	ESteamVRBone_IndexFinger3,	// EUE4HandBone_Index_03
	ESteamVRBone_MiddleFinger1, // EUE4HandBone_Middle_01
	ESteamVRBone_MiddleFinger2, // EUE4HandBone_Middle_02
	ESteamVRBone_MiddleFinger3, // EUE4HandBone_Middle_03
	ESteamVRBone_PinkyFinger1,	// EUE4HandBone_Pinky_01
	ESteamVRBone_PinkyFinger2,	// EUE4HandBone_Pinky_02
	ESteamVRBone_PinkyFinger3,	// EUE4HandBone_Pinky_03
	ESteamVRBone_RingFinger1,	// EUE4HandBone_Ring_01
	ESteamVRBone_RingFinger2,	// EUE4HandBone_Ring_02
	ESteamVRBone_RingFinger3,	// EUE4HandBone_Ring_03
	ESteamVRBone_Thumb0,		// EUE4HandBone_Thumb_01
	ESteamVRBone_Thumb1,		// EUE4HandBone_Thumb_02
	ESteamVRBone_Thumb2			// EUE4HandBone_Thumb_03
};
static_assert( sizeof(kUE4BoneToSteamVRBone) / sizeof(kUE4BoneToSteamVRBone[0]) == EUE4HandBone_Count, "Mapping from UE4 hand bones to their corresponding SteamVR hand bones is the wrong size" );

// List of the knuckle bones of the SteamVR hand skeleton
static const int32 kSteamVRKnuckleBones[] = {
	ESteamVRBone_IndexFinger1,
	ESteamVRBone_MiddleFinger1,
	ESteamVRBone_RingFinger1,
	ESteamVRBone_PinkyFinger1
};
static_assert( sizeof(kSteamVRKnuckleBones) / sizeof(kSteamVRKnuckleBones[0]) == 4, "List of SteamVR knuckle bones should only have 4 entries" );

// List of the knuckle bones of the UE4 hand skeleton
static const int32 kUE4KnuckleBones[] = {
	EUE4HandBone_Index_01,
	EUE4HandBone_Middle_01,
	EUE4HandBone_Ring_01,
	EUE4HandBone_Pinky_01
};
static_assert( sizeof(kUE4KnuckleBones) / sizeof(kUE4KnuckleBones[0]) == 4, "List of UE4 knuckle bones should only have 4 entries" );


FAnimNode_SteamVRInputAnimPose::FAnimNode_SteamVRInputAnimPose()
{
}

void FAnimNode_SteamVRInputAnimPose::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
}

void FAnimNode_SteamVRInputAnimPose::CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)
{
}

void FAnimNode_SteamVRInputAnimPose::Update_AnyThread(const FAnimationUpdateContext & Context)
{
	// Grab node inputs
	GetEvaluateGraphExposedInputs().Execute(Context);
}

void FAnimNode_SteamVRInputAnimPose::Evaluate_AnyThread(FPoseContext& Output)
{
	Output.ResetToRefPose();

	// Update Skeletal Animation
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		// Attempt to read the current skeletal pose from SteamVR.  The data returned will be the bone transforms of the SteamVR hand skeleton,
		// transformed into the UE4 coordinate system

		FTransform BoneTransforms[STEAMVR_SKELETON_BONE_COUNT];
		EVRSkeletalMotionRange SteamVRMotionRange = (MotionRange == EMotionRange::VR_WithController) ? VRSkeletalMotionRange_WithController : VRSkeletalMotionRange_WithoutController;
		bool bIsLeftHand = (Hand == EHand::VR_LeftHand);
		
		if (SteamVRInputDevice->GetSkeletalData(bIsLeftHand, Mirror, SteamVRMotionRange, BoneTransforms, STEAMVR_SKELETON_BONE_COUNT))
		{
			// If the target hand skeleton is the SteamVR skeleton, then we can just copy the transforms directly into the pose
			if (HandSkeleton == EHandSkeleton::VR_SteamVRHandSkeleton)
			{
				for (int32 SrcBoneindex = 0; SrcBoneindex < STEAMVR_SKELETON_BONE_COUNT; ++SrcBoneindex)
				{
					FCompactPoseBoneIndex TargetBoneIndex = Output.Pose.GetBoneContainer().MakeCompactPoseIndex(FMeshPoseBoneIndex(SrcBoneindex));
					if (TargetBoneIndex != INDEX_NONE)
					{
						Output.Pose[TargetBoneIndex] = BoneTransforms[SrcBoneindex];
					}
				}
			}
			else
			{
				// If the target hand skeleton is the UE4 reference VR hand skeleton, we need to retarget the transforms
				// we read from SteamVR to fit it.  
				PoseUE4HandSkeleton(Output.Pose, BoneTransforms, STEAMVR_SKELETON_BONE_COUNT);
			}
		}
	}
}


FQuat CalcRotationAboutAxis(const FVector& FromDirection, const FVector& ToDirection, const FVector& Axis)
{
	FVector FromDirectionCp = FVector::CrossProduct(Axis, FromDirection);
	FVector ToDirectionCp = FVector::CrossProduct(Axis, ToDirection);

	return FQuat::FindBetweenVectors(FromDirectionCp, ToDirectionCp);
}


void FAnimNode_SteamVRInputAnimPose::PoseUE4HandSkeleton(FCompactPose& Pose, const FTransform* BoneTransformsLS, int32 BoneTransformCount)
{
	check(BoneTransformsLS != nullptr);
	check(BoneTransformCount == SteamVRSkeleton::GetBoneCount());
	check(Pose.GetNumBones() == UE4HandSkeleton::GetBoneCount());

	// It is easier to do the retargetting in model space, so calculate the model space transforms for the SteamVR skeleton
	// from the given local space transforms
	FTransform BoneTransformsMS[STEAMVR_SKELETON_BONE_COUNT];
	for ( int32 BoneIndex = 0; BoneIndex < SteamVRSkeleton::GetBoneCount(); ++BoneIndex)
	{
		int32 ParentIndex = SteamVRSkeleton::GetParentIndex(BoneIndex);
		if (ParentIndex != -1)
		{
			BoneTransformsMS[BoneIndex] = BoneTransformsLS[BoneIndex] * BoneTransformsMS[ParentIndex];
		}
		else
		{
			BoneTransformsMS[BoneIndex] = BoneTransformsLS[BoneIndex];
		}
	}

	// Remove any scale, as its now baked into the position
	for (int32 BoneIndex = 0; BoneIndex < SteamVRSkeleton::GetBoneCount(); ++BoneIndex)
	{
		BoneTransformsMS[BoneIndex].SetScale3D(FVector::ZeroVector);
	}

	FQuat TargetBoneRotationsMS[EUE4HandBone_Count];

	// Cache UE4 retargetting references at first anim frame as FAnimPoseBase init does not appear to have a valid Pose to process at Initialize()
	if (!UE4RetargettingRefs.bIsInitialized)
	{
		// Determine which hand we're working with
		UE4RetargettingRefs.bIsRightHanded = (Hand == EHand::VR_RightHand && !Mirror) || (Hand == EHand::VR_LeftHand && Mirror);

		// Calculate the average position of the UE4 knuckles bones and add it to the cache
		for (int32 KnuckleIndex = 0; KnuckleIndex < 4; ++KnuckleIndex)
		{
			const FCompactPoseBoneIndex BoneIndex_UE4 = FCompactPoseBoneIndex(kUE4KnuckleBones[KnuckleIndex]);
			UE4RetargettingRefs.KnuckleAverageMS_UE4 += CalcModelSpaceTransform(Pose, BoneIndex_UE4).GetTranslation();
		}

		UE4RetargettingRefs.KnuckleAverageMS_UE4 /= 4.f;

		// Obtain the UE4 wrist Side & Forward directions from first animation frame and place in cache 
		FCompactPoseBoneIndex WristBoneIndexCompact = Pose.GetBoneContainer().MakeCompactPoseIndex(FMeshPoseBoneIndex(EUE4HandBone_Wrist));
		FTransform WristTransform_UE4 = Pose[WristBoneIndexCompact];
		FVector ToKnuckleAverageMS_UE4 = UE4RetargettingRefs.KnuckleAverageMS_UE4 - WristTransform_UE4.GetTranslation();
		ToKnuckleAverageMS_UE4.Normalize();

		UE4RetargettingRefs.WristForwardLS_UE4 = WristTransform_UE4.GetRotation().UnrotateVector(ToKnuckleAverageMS_UE4);
		UE4RetargettingRefs.WristSideDirectionLS_UE4 = FVector::CrossProduct(UE4RetargettingRefs.WristForwardLS_UE4, FVector::RightVector);

		// Set initialized flag to true so that subsequent frames will skip the above calculations
		UE4RetargettingRefs.bIsInitialized = true;
	}

	// Calculate the rotation to make the target wrist bone match the orientation of the source wrist bone
	{
		// Calculate the average position of the knuckles bones for each hand, then rotate the wrist of the target skeleton
		// so that its knuckle-average coincides with the knuckle-average of the source pose
		FVector KnuckleAverageMS_SteamVR = FVector::ZeroVector;

		for (int32 KnuckleIndex = 0; KnuckleIndex < 4; ++KnuckleIndex)
		{
			const int32 BoneIndex_SteamVR = kSteamVRKnuckleBones[KnuckleIndex];
			KnuckleAverageMS_SteamVR	+= BoneTransformsMS[BoneIndex_SteamVR].GetTranslation();
		}

		KnuckleAverageMS_SteamVR /= 4.f;

		FVector ToKnuckleAverageMS_SteamVR = KnuckleAverageMS_SteamVR - BoneTransformsMS[ESteamVRBone_Wrist].GetTranslation();
		ToKnuckleAverageMS_SteamVR.Normalize();
		FVector WristForwardLS_SteamVR = BoneTransformsMS[ESteamVRBone_Wrist].GetRotation().UnrotateVector(ToKnuckleAverageMS_SteamVR);

		// Get the axis that most closely matches the direction the palm of the hand is facing
		FVector WristSideDirectionLS_SteamVR = (UE4RetargettingRefs.bIsRightHanded) ? FVector(0.f, -1.f, 0.f) : FVector(0.f, 1.f, 0.f);

		// Take the cross product with the forward vector for the SteamVR hand to ensure that the side vector is perpendicular to the forward vector
		WristSideDirectionLS_SteamVR = FVector::CrossProduct(WristForwardLS_SteamVR, WristSideDirectionLS_SteamVR);

		// Find the model-space directions of the forward and side vectors based on the current pose
		const FVector WristForwardMS_SteamVR = BoneTransformsMS[ESteamVRBone_Wrist].GetRotation() * WristForwardLS_SteamVR;
		const FVector WristSideDirectionMS_SteamVR = BoneTransformsMS[ESteamVRBone_Wrist].GetRotation() * WristSideDirectionLS_SteamVR;

		// Calculate the rotation that will align the UE4 hand's forward vector with the SteamVR hand's forward
		FQuat AlignmentRot = FQuat::FindBetweenNormals(UE4RetargettingRefs.WristForwardLS_UE4, WristForwardMS_SteamVR);

		// Rotate about the aligned forward direction to make the side directions align
		FVector WristSideDirectionMS_UE4 = AlignmentRot * UE4RetargettingRefs.WristSideDirectionLS_UE4;
		FQuat TwistRotation = CalcRotationAboutAxis(WristSideDirectionMS_UE4, WristSideDirectionMS_SteamVR, WristForwardMS_SteamVR);

		// Apply the rotation to the hand
		TargetBoneRotationsMS[EUE4HandBone_Wrist] = TwistRotation * AlignmentRot;
	}

	// For all the remaining bones, use their child bone as a reference to calculate their orientation
	const FVector FingerForwardDefault_SteamVR(-1.f, 0.f, 0.f);
	const FVector FingerForwardDefault_UE4(-1.f, 0.f, 0.f);
	for ( int32 BoneIndex = EUE4HandBone_Wrist+1; BoneIndex < Pose.GetNumBones(); ++BoneIndex)
	{
		// Determine which direction is "forward" in the bone's local space by looking at the direction
		// to its child
		FVector FingerForwardLS_UE4 = FingerForwardDefault_UE4;
		if (UE4HandSkeleton::GetChildCount(BoneIndex) > 0)
		{
			int32 ChildBoneIndex = UE4HandSkeleton::GetChildIndex(BoneIndex, 0);
			FCompactPoseBoneIndex ChildBoneIndexCompact = Pose.GetBoneContainer().MakeCompactPoseIndex(FMeshPoseBoneIndex(ChildBoneIndex));
			FingerForwardLS_UE4 = Pose[ChildBoneIndexCompact].GetTranslation();
			FingerForwardLS_UE4.Normalize();
		}

		int32 ParentBoneIndex = UE4HandSkeleton::GetParentIndex(BoneIndex);
		check(ParentBoneIndex != -1);

		FQuat StartingTransformMS = TargetBoneRotationsMS[ParentBoneIndex];

		// Include the default orientation of the bone, so that it is used as the starting point for the adjustment rotation
		FCompactPoseBoneIndex BoneIndexCompact = Pose.GetBoneContainer().MakeCompactPoseIndex(FMeshPoseBoneIndex(BoneIndex));
		StartingTransformMS = StartingTransformMS * Pose[BoneIndexCompact].GetRotation();

		// Convert the bone's forward direction to model space
		FVector FingerForwardMS_UE4 = StartingTransformMS.RotateVector(FingerForwardLS_UE4);
		
		// Calculate the direction that the bone's forward vector should be pointing
		const int32 BoneIndex_SteamVR = kUE4BoneToSteamVRBone[BoneIndex];
		FVector FingerForwardMS_SteamVR = FingerForwardDefault_SteamVR;
		if (SteamVRSkeleton::GetChildCount(BoneIndex_SteamVR) > 0)
		{
			int32 ChildIndex = SteamVRSkeleton::GetChildIndex(BoneIndex_SteamVR, 0);
			FingerForwardMS_SteamVR = BoneTransformsMS[ChildIndex].GetTranslation() - BoneTransformsMS[BoneIndex_SteamVR].GetTranslation();
			FingerForwardMS_SteamVR.Normalize();
		}

		// Find the rotation between the current forward vector and the desired forward vector, in model space
		FQuat AdjustmentRot = FQuat::FindBetweenVectors(FingerForwardMS_UE4, FingerForwardMS_SteamVR);

		TargetBoneRotationsMS[BoneIndex] = AdjustmentRot * StartingTransformMS;
	}

	// Convert the target rotations from model-space to local-space and apply them on the output pose
	for (int32 BoneIndex = 0; BoneIndex < Pose.GetNumBones(); ++BoneIndex)
	{
		FCompactPoseBoneIndex TargetBoneIndex = Pose.GetBoneContainer().MakeCompactPoseIndex(FMeshPoseBoneIndex(BoneIndex));
		if (TargetBoneIndex != INDEX_NONE)
		{
			FQuat BoneRotation = TargetBoneRotationsMS[BoneIndex];

			int32 ParentIndex = UE4HandSkeleton::GetParentIndex(BoneIndex);
			if (ParentIndex != -1)
			{
				BoneRotation = TargetBoneRotationsMS[ParentIndex].Inverse() * BoneRotation;
			}

			Pose[TargetBoneIndex].SetRotation(BoneRotation);
		}
	}

	// Set the wrist bone position
	{
		FCompactPoseBoneIndex WristBoneIndex = Pose.GetBoneContainer().MakeCompactPoseIndex(FMeshPoseBoneIndex(EUE4HandBone_Wrist));
		Pose[WristBoneIndex].SetTranslation(BoneTransformsMS[ESteamVRBone_Wrist].GetTranslation());
	}
}


FSteamVRInputDevice* FAnimNode_SteamVRInputAnimPose::GetSteamVRInputDevice()
{
	TArray<IMotionController*> MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
	for (auto MotionController : MotionControllers)
	{
		FSteamVRInputDevice* TestSteamVRDevice = static_cast<FSteamVRInputDevice*>(MotionController);
		if (TestSteamVRDevice != nullptr && !FGenericPlatformMath::IsNaN(TestSteamVRDevice->DeviceSignature) && TestSteamVRDevice->DeviceSignature == 2019)
		{
			return TestSteamVRDevice;
		}
	}
	return nullptr;
}


FTransform FAnimNode_SteamVRInputAnimPose::CalcModelSpaceTransform(const FCompactPose& Pose, FCompactPoseBoneIndex BoneIndex)
{
	FTransform BoneTransform = Pose[BoneIndex];

	FCompactPoseBoneIndex ParentIndex = Pose.GetParentBoneIndex(BoneIndex);
	if (ParentIndex != INDEX_NONE)
	{
		BoneTransform = BoneTransform * CalcModelSpaceTransform(Pose, ParentIndex);
	}

	return BoneTransform;
}
