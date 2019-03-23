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


#include "AnimNode_SteamVRInputAnimPose.h"
#include "ISteamVRInputDeviceModule.h"
#include "AnimationRuntime.h"
#include "AnimInstanceProxy.h"
#include "SteamVRInputDevice.h"

FAnimNode_SteamVRInputAnimPose::FAnimNode_SteamVRInputAnimPose()
{
}

void FAnimNode_SteamVRInputAnimPose::Initialize(const FAnimationInitializeContext& Context)
{
	TransformedBoneNames.Reserve(SteamVRSkeleton::GetBoneCount());
}

void FAnimNode_SteamVRInputAnimPose::CacheBones(const FAnimationCacheBonesContext & Context)
{
}

void FAnimNode_SteamVRInputAnimPose::Update(const FAnimationUpdateContext & Context)
{
	// Grab node inputs
	EvaluateGraphExposedInputs.Execute(Context);

	// Setup any retargetting here
	TransformedBoneNames.Empty();
	BoneNameMap.Empty();
	for (int32 BoneIndex = 0; BoneIndex < SteamVRSkeleton::GetBoneCount(); ++BoneIndex)
	{
		const FName& SrcBoneName = SteamVRSkeleton::GetBoneName(BoneIndex);

		ProcessBoneMap(BoneIndex, SrcBoneName);

	}
}

void FAnimNode_SteamVRInputAnimPose::Evaluate(FPoseContext& Output)
{
	Output.ResetToRefPose();

	// Set BoneCount
	int32 BoneCount = (HandSkeleton == EHandSkeleton::VR_SteamVRHandSkeleton) ? SteamVRSkeleton::GetBoneCount() : Output.Pose.GetNumBones();
	VRBoneTransform_t OutPose[STEAMVR_SKELETON_BONE_COUNT];
	VRBoneTransform_t ReferencePose[STEAMVR_SKELETON_BONE_COUNT];

	// Update Skeletal Animation
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		FillSteamVRHandTransforms(SteamVRInputDevice, OutPose, ReferencePose);

		for (int32 i = 0; i < TransformedBoneNames.Num(); ++i)
		{
			FTransform BoneTransform = FTransform();
			FName BoneName = TransformedBoneNames[i];

			if (HandSkeleton == EHandSkeleton::VR_SteamVRHandSkeleton)
			{
				GetSteamVRBoneTransform(i, BoneTransform);
			}
			else if (HandSkeleton == EHandSkeleton::VR_UE4HandSkeleton)
			{
				int32 SteamVRHandIndex = GetSteamVRHandIndex(i);
				if (SteamVRHandIndex != INDEX_NONE)
				{
					GetSteamVRBoneTransform(SteamVRHandIndex, BoneTransform);
				}
			}
			else
			{
				// Check if there's a mapping for this bone to the SteamVR skeleton
				if (i < SteamVRSkeleton::GetBoneCount())
				{
					FName MappedBone = BoneNameMap.FindRef(SteamVRSkeleton::GetBoneName(i));

					if (MappedBone.IsValid() && MappedBone != NAME_None)
					{
						GetSteamVRBoneTransform(i, BoneTransform);
					}
				}
			}

			int32 MeshIndex;
			if (HandSkeleton == EHandSkeleton::VR_CustomSkeleton)
			{
				MeshIndex = Output.Pose.GetBoneContainer().GetPoseBoneIndexForBoneName(BoneName);
				//UE_LOG(LogTemp, Warning, TEXT("Bone Map [%s] from SteamVR Index[%i] to MeshIndex [%i]"), *BoneName.ToString(), i, MeshIndex);
			}
			else
			{
				MeshIndex = i;
			}
			
			if (MeshIndex != INDEX_NONE)
			{
				FCompactPoseBoneIndex BoneIndex = Output.Pose.GetBoneContainer().MakeCompactPoseIndex(FMeshPoseBoneIndex(MeshIndex));
				if (BoneIndex != INDEX_NONE)
				{
					FQuat NewRotation;
					if (BoneTransform.GetRotation().Equals(FQuat()) ||
						BoneTransform.GetRotation().ContainsNaN())
					{
						NewRotation = Output.Pose[BoneIndex].GetRotation();
					}
					else
					{
						NewRotation = BoneTransform.GetRotation();
					}

					FVector NewTranslation;
					if (BoneTransform.GetLocation() == FVector::ZeroVector || BoneTransform.ContainsNaN() || HandSkeleton != EHandSkeleton::VR_SteamVRHandSkeleton)
					{
						NewTranslation = Output.Pose[BoneIndex].GetTranslation();
					}
					else
					{
						NewTranslation = BoneTransform.GetLocation();
					}
					//UE_LOG(LogTemp, Warning, TEXT("[Current Translate %s] [Bone Translate %s]"), *Output.Pose[BoneIndex].GetTranslation().ToString(), *(BoneTransform.GetLocation()).ToString());

					FTransform OutTransform = FTransform(Output.Pose[BoneIndex].GetRotation(), Output.Pose[BoneIndex].GetTranslation(), Output.Pose[BoneIndex].GetScale3D());
					OutTransform.SetLocation(NewTranslation);
					OutTransform.SetRotation(NewRotation);

					// Set new bone transform
					Output.Pose[BoneIndex] = OutTransform;
				}
			}
		}
	}
}

FTransform FAnimNode_SteamVRInputAnimPose::GetUETransform(VRBoneTransform_t SteamBoneTransform, VRBoneTransform_t SteamBoneReference)
{
	FTransform RetTransform;

	FQuat OrientationQuat(SteamBoneTransform.orientation.x,
		-SteamBoneTransform.orientation.y,
		 SteamBoneTransform.orientation.z,
		-SteamBoneTransform.orientation.w);
	OrientationQuat.Normalize();
	RetTransform = FTransform(OrientationQuat,
		//FVector());
		FVector(SteamBoneReference.position.v[0],
				-SteamBoneReference.position.v[1],
				SteamBoneReference.position.v[2]));

	return RetTransform;
}

int32 FAnimNode_SteamVRInputAnimPose::GetSteamVRHandIndex(int32 UE4BoneIndex)
{
	switch (UE4BoneIndex)
	{
	case 0:
		return 1;
	case 13:
		return 2;
	case 14:
		return 3;
	case 15:
		return 4;
	case 1:
		return 7;
	case 2:
		return 8;
	case 3:
		return 9;
	case 4:
		return 12;
	case 5:
		return 13;
	case 6:
		return 14;
	case 10:
		return 17;
	case 11:
		return 18;
	case 12:
		return 19;
	case 7:
		return 22;
	case 8:
		return 23;
	case 9:
		return 24;
	default:
		break;
	}

	return INDEX_NONE;
}

void FAnimNode_SteamVRInputAnimPose::GetSteamVRBoneTransform(int32 SteamVRBoneIndex, FTransform& OutTransform)
{
	if (Hand == EHand::VR_RightHand)
	{
		switch ((ESteamVRBone)SteamVRBoneIndex)
		{
		case ESteamVRBone::EBone_Root:
			//OutTransform = RightHand.Root;
			break;

		case ESteamVRBone::EBone_Wrist:
			OutTransform = RightHand.Wrist;
			if (HandSkeleton == EHandSkeleton::VR_SteamVRHandSkeleton)
			{
				OutTransform.SetLocation(FVector::ZeroVector);
				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(180.f, 0.f, 45.f))));
			}
			else if (HandSkeleton == EHandSkeleton::VR_UE4HandSkeleton)
			{
				OutTransform.SetLocation(FVector::ZeroVector);
				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(0.f, 0.f, -100.f))));
			}
			break;

		case ESteamVRBone::EBone_Thumb1:
			OutTransform = RightHand.Thumb_0;
			if (HandSkeleton != EHandSkeleton::VR_SteamVRHandSkeleton)
			{
				OutTransform = RightHand.Thumb_0;
				OutTransform.SetLocation(FVector::ZeroVector);
				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(-15.f, -100.f, 180.f))));
			}
			break;

		case ESteamVRBone::EBone_Thumb2:
			OutTransform = RightHand.Thumb_1;
			break;

		case ESteamVRBone::EBone_Thumb3:
			OutTransform = RightHand.Thumb_2;
			break;

		case ESteamVRBone::EBone_Thumb4:
			OutTransform = RightHand.Thumb_3;
			break;

		case ESteamVRBone::EBone_IndexFinger0:
			OutTransform = RightHand.Index_0;
			break;

		case ESteamVRBone::EBone_IndexFinger1:
			OutTransform = RightHand.Index_1;
			break;

		case ESteamVRBone::EBone_IndexFinger2:
			OutTransform = RightHand.Index_2;
			break;

		case ESteamVRBone::EBone_IndexFinger3:
			OutTransform = RightHand.Index_3;
			break;

		case ESteamVRBone::EBone_IndexFinger4:
			OutTransform = RightHand.Index_4;
			break;

		case ESteamVRBone::EBone_MiddleFinger0:
			OutTransform = RightHand.Middle_0;
			break;

		case ESteamVRBone::EBone_MiddleFinger1:
			OutTransform = RightHand.Middle_1;
			break;

		case ESteamVRBone::EBone_MiddleFinger2:
			OutTransform = RightHand.Middle_2;
			break;

		case ESteamVRBone::EBone_MiddleFinger3:
			OutTransform = RightHand.Middle_3;
			break;

		case ESteamVRBone::EBone_MiddleFinger4:
			OutTransform = RightHand.Middle_4;
			break;

		case ESteamVRBone::EBone_RingFinger0:
			OutTransform = RightHand.Ring_0;
			break;

		case ESteamVRBone::EBone_RingFinger1:
			OutTransform = RightHand.Ring_1;
			break;

		case ESteamVRBone::EBone_RingFinger2:
			OutTransform = RightHand.Ring_2;
			break;

		case ESteamVRBone::EBone_RingFinger3:
			OutTransform = RightHand.Ring_3;
			break;

		case ESteamVRBone::EBone_RingFinger4:
			OutTransform = RightHand.Ring_4;
			break;

		case ESteamVRBone::EBone_PinkyFinger0:
			OutTransform = RightHand.Pinky_0;
			break;

		case ESteamVRBone::EBone_PinkyFinger1:
			OutTransform = RightHand.Pinky_1;
			break;

		case ESteamVRBone::EBone_PinkyFinger2:
			OutTransform = RightHand.Pinky_2;
			break;

		case ESteamVRBone::EBone_PinkyFinger3:
			OutTransform = RightHand.Pinky_3;
			break;

		case ESteamVRBone::EBone_PinkyFinger4:
			OutTransform = RightHand.Pinky_4;
			break;

		default:
			break;
		}
	}
	else
	{
		switch ((ESteamVRBone)SteamVRBoneIndex)
		{
		case ESteamVRBone::EBone_Root:
			//OutTransform = LeftHand.Root;
			break;

		case ESteamVRBone::EBone_Wrist:
			OutTransform = LeftHand.Wrist;
			if (HandSkeleton == EHandSkeleton::VR_SteamVRHandSkeleton)
			{
				OutTransform.SetLocation(FVector::ZeroVector);
				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(180.f, 0.f, 45.f))));
			}
			else if (HandSkeleton == EHandSkeleton::VR_UE4HandSkeleton)
			{
				OutTransform.SetLocation(FVector::ZeroVector);
				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(0.f, 0.f, -100.f))));
			}
			break;

		case ESteamVRBone::EBone_Thumb1:
			OutTransform = LeftHand.Thumb_0;
			if (HandSkeleton != EHandSkeleton::VR_SteamVRHandSkeleton)
			{
				OutTransform = RightHand.Thumb_0;
				OutTransform.SetLocation(FVector::ZeroVector);
				OutTransform.GetRotation();
				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(-45.f, -45.f, 90.f))));
			}
			break;

		case ESteamVRBone::EBone_Thumb2:
			OutTransform = LeftHand.Thumb_1;
			break;

		case ESteamVRBone::EBone_Thumb3:
			OutTransform = LeftHand.Thumb_2;
			break;

		case ESteamVRBone::EBone_Thumb4:
			OutTransform = LeftHand.Thumb_3;
			break;

		case ESteamVRBone::EBone_IndexFinger0:
			OutTransform = LeftHand.Index_0;
			break;

		case ESteamVRBone::EBone_IndexFinger1:
			OutTransform = LeftHand.Index_1;
			break;

		case ESteamVRBone::EBone_IndexFinger2:
			OutTransform = LeftHand.Index_2;
			break;

		case ESteamVRBone::EBone_IndexFinger3:
			OutTransform = LeftHand.Index_3;
			break;

		case ESteamVRBone::EBone_IndexFinger4:
			OutTransform = LeftHand.Index_4;
			break;

		case ESteamVRBone::EBone_MiddleFinger0:
			OutTransform = LeftHand.Middle_0;
			break;

		case ESteamVRBone::EBone_MiddleFinger1:
			OutTransform = LeftHand.Middle_1;
			break;

		case ESteamVRBone::EBone_MiddleFinger2:
			OutTransform = LeftHand.Middle_2;
			break;

		case ESteamVRBone::EBone_MiddleFinger3:
			OutTransform = LeftHand.Middle_3;
			break;

		case ESteamVRBone::EBone_MiddleFinger4:
			OutTransform = LeftHand.Middle_4;
			break;

		case ESteamVRBone::EBone_RingFinger0:
			OutTransform = LeftHand.Ring_0;
			break;

		case ESteamVRBone::EBone_RingFinger1:
			OutTransform = LeftHand.Ring_1;
			break;

		case ESteamVRBone::EBone_RingFinger2:
			OutTransform = LeftHand.Ring_2;
			break;

		case ESteamVRBone::EBone_RingFinger3:
			OutTransform = LeftHand.Ring_3;
			break;

		case ESteamVRBone::EBone_RingFinger4:
			OutTransform = LeftHand.Ring_4;
			break;

		case ESteamVRBone::EBone_PinkyFinger0:
			OutTransform = LeftHand.Pinky_0;
			break;

		case ESteamVRBone::EBone_PinkyFinger1:
			OutTransform = LeftHand.Pinky_1;
			break;

		case ESteamVRBone::EBone_PinkyFinger2:
			OutTransform = LeftHand.Pinky_2;
			break;

		case ESteamVRBone::EBone_PinkyFinger3:
			OutTransform = LeftHand.Pinky_3;
			break;

		case ESteamVRBone::EBone_PinkyFinger4:
			OutTransform = LeftHand.Pinky_4;
			break;

		default:
			break;
		}
	}
}

void FAnimNode_SteamVRInputAnimPose::FillSteamVRHandTransforms(FSteamVRInputDevice* SteamVRInputDevice, VRBoneTransform_t* OutPose, VRBoneTransform_t* ReferencePose)
{
	if (SteamVRInputDevice != nullptr)
	{
		// Setup Motion Range
		EVRSkeletalMotionRange SteamVRMotionRange = (MotionRange == EMotionRange::VR_WithController) ? VRSkeletalMotionRange_WithController : VRSkeletalMotionRange_WithoutController;

		if (Hand == EHand::VR_LeftHand)
		{
			// Left Hand - Grab Skeletal Data
			if (SteamVRInputDevice->GetSkeletalData(true, SteamVRMotionRange, OutPose, ReferencePose))
			{
				LeftHand.Root = GetUETransform(OutPose[0], ReferencePose[0]);
				LeftHand.Wrist = GetUETransform(OutPose[1], ReferencePose[1]);

				LeftHand.Thumb_0 = GetUETransform(OutPose[2], ReferencePose[2]);
				LeftHand.Thumb_1 = GetUETransform(OutPose[3], ReferencePose[3]);
				LeftHand.Thumb_2 = GetUETransform(OutPose[4], ReferencePose[4]);
				LeftHand.Thumb_3 = GetUETransform(OutPose[5], ReferencePose[5]);

				LeftHand.Index_0 = GetUETransform(OutPose[6], ReferencePose[6]);
				LeftHand.Index_1 = GetUETransform(OutPose[7], ReferencePose[7]);
				LeftHand.Index_2 = GetUETransform(OutPose[8], ReferencePose[8]);
				LeftHand.Index_3 = GetUETransform(OutPose[9], ReferencePose[9]);
				LeftHand.Index_4 = GetUETransform(OutPose[10], ReferencePose[10]);

				LeftHand.Middle_0 = GetUETransform(OutPose[11], ReferencePose[11]);
				LeftHand.Middle_1 = GetUETransform(OutPose[12], ReferencePose[12]);
				LeftHand.Middle_2 = GetUETransform(OutPose[13], ReferencePose[13]);
				LeftHand.Middle_3 = GetUETransform(OutPose[14], ReferencePose[14]);
				LeftHand.Middle_4 = GetUETransform(OutPose[15], ReferencePose[15]);

				LeftHand.Ring_0 = GetUETransform(OutPose[16], ReferencePose[16]);
				LeftHand.Ring_1 = GetUETransform(OutPose[17], ReferencePose[17]);
				LeftHand.Ring_2 = GetUETransform(OutPose[18], ReferencePose[18]);
				LeftHand.Ring_3 = GetUETransform(OutPose[19], ReferencePose[19]);
				LeftHand.Ring_4 = GetUETransform(OutPose[20], ReferencePose[20]);

				LeftHand.Pinky_0 = GetUETransform(OutPose[21], ReferencePose[21]);
				LeftHand.Pinky_1 = GetUETransform(OutPose[22], ReferencePose[22]);
				LeftHand.Pinky_2 = GetUETransform(OutPose[23], ReferencePose[23]);
				LeftHand.Pinky_3 = GetUETransform(OutPose[24], ReferencePose[24]);
				LeftHand.Pinky_4 = GetUETransform(OutPose[25], ReferencePose[25]);

				LeftHand.Aux_Thumb = GetUETransform(OutPose[26], ReferencePose[26]);
				LeftHand.Aux_Index = GetUETransform(OutPose[27], ReferencePose[27]);
				LeftHand.Aux_Middle = GetUETransform(OutPose[28], ReferencePose[28]);
				LeftHand.Aux_Ring = GetUETransform(OutPose[29], ReferencePose[29]);
				LeftHand.Aux_Pinky = GetUETransform(OutPose[30], ReferencePose[30]);

				LeftHand.Bone_Count = GetUETransform(OutPose[31], ReferencePose[31]);
			}
		}
		else
		{
			// Right Hand - Grab Skeletal Data
			SteamVRInputDevice->GetSkeletalData(false, SteamVRMotionRange, OutPose, ReferencePose);

			RightHand.Root = GetUETransform(OutPose[0], ReferencePose[0]);
			RightHand.Wrist = GetUETransform(OutPose[1], ReferencePose[1]);

			RightHand.Thumb_0 = GetUETransform(OutPose[2], ReferencePose[2]);
			RightHand.Thumb_1 = GetUETransform(OutPose[3], ReferencePose[3]);
			RightHand.Thumb_2 = GetUETransform(OutPose[4], ReferencePose[4]);
			RightHand.Thumb_3 = GetUETransform(OutPose[5], ReferencePose[5]);

			RightHand.Index_0 = GetUETransform(OutPose[6], ReferencePose[6]);
			RightHand.Index_1 = GetUETransform(OutPose[7], ReferencePose[7]);
			RightHand.Index_2 = GetUETransform(OutPose[8], ReferencePose[8]);
			RightHand.Index_3 = GetUETransform(OutPose[9], ReferencePose[9]);
			RightHand.Index_4 = GetUETransform(OutPose[10], ReferencePose[10]);

			RightHand.Middle_0 = GetUETransform(OutPose[11], ReferencePose[11]);
			RightHand.Middle_1 = GetUETransform(OutPose[12], ReferencePose[12]);
			RightHand.Middle_2 = GetUETransform(OutPose[13], ReferencePose[13]);
			RightHand.Middle_3 = GetUETransform(OutPose[14], ReferencePose[14]);
			RightHand.Middle_4 = GetUETransform(OutPose[15], ReferencePose[15]);

			RightHand.Ring_0 = GetUETransform(OutPose[16], ReferencePose[16]);
			RightHand.Ring_1 = GetUETransform(OutPose[17], ReferencePose[17]);
			RightHand.Ring_2 = GetUETransform(OutPose[18], ReferencePose[18]);
			RightHand.Ring_3 = GetUETransform(OutPose[19], ReferencePose[19]);
			RightHand.Ring_4 = GetUETransform(OutPose[20], ReferencePose[20]);

			RightHand.Pinky_0 = GetUETransform(OutPose[21], ReferencePose[21]);
			RightHand.Pinky_1 = GetUETransform(OutPose[22], ReferencePose[22]);
			RightHand.Pinky_2 = GetUETransform(OutPose[23], ReferencePose[23]);
			RightHand.Pinky_3 = GetUETransform(OutPose[24], ReferencePose[24]);
			RightHand.Pinky_4 = GetUETransform(OutPose[25], ReferencePose[25]);

			RightHand.Aux_Thumb = GetUETransform(OutPose[26], ReferencePose[26]);
			RightHand.Aux_Index = GetUETransform(OutPose[27], ReferencePose[27]);
			RightHand.Aux_Middle = GetUETransform(OutPose[28], ReferencePose[28]);
			RightHand.Aux_Ring = GetUETransform(OutPose[29], ReferencePose[29]);
			RightHand.Aux_Pinky = GetUETransform(OutPose[30], ReferencePose[30]);

			RightHand.Bone_Count = GetUETransform(OutPose[31], ReferencePose[31]);
		}
	}
}

void FAnimNode_SteamVRInputAnimPose::ProcessBoneMap(int32 BoneIndex, const FName& SrcBoneName)
{
	switch ((ESteamVRBone)BoneIndex)
	{
	case ESteamVRBone::EBone_Root:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Root);
		break;

	case ESteamVRBone::EBone_Wrist:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Wrist);
		break;

	case ESteamVRBone::EBone_Thumb1:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Thumb_0);
		break;

	case ESteamVRBone::EBone_Thumb2:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Thumb_1);
		break;

	case ESteamVRBone::EBone_Thumb3:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Thumb_2);
		break;

	case ESteamVRBone::EBone_Thumb4:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Thumb_3);
		break;

	case ESteamVRBone::EBone_IndexFinger0:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Index_0);
		break;

	case ESteamVRBone::EBone_IndexFinger1:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Index_1);
		break;

	case ESteamVRBone::EBone_IndexFinger2:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Index_2);
		break;

	case ESteamVRBone::EBone_IndexFinger3:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Index_3);
		break;

	case ESteamVRBone::EBone_IndexFinger4:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Index_4);
		break;

	case ESteamVRBone::EBone_MiddleFinger0:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Middle_0);
		break;

	case ESteamVRBone::EBone_MiddleFinger1:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Middle_1);
		break;

	case ESteamVRBone::EBone_MiddleFinger2:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Middle_2);
		break;

	case ESteamVRBone::EBone_MiddleFinger3:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Middle_3);
		break;

	case ESteamVRBone::EBone_MiddleFinger4:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Middle_4);
		break;

	case ESteamVRBone::EBone_RingFinger0:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Ring_0);
		break;

	case ESteamVRBone::EBone_RingFinger1:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Ring_1);
		break;

	case ESteamVRBone::EBone_RingFinger2:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Ring_2);
		break;

	case ESteamVRBone::EBone_RingFinger3:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Ring_3);
		break;

	case ESteamVRBone::EBone_RingFinger4:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Ring_4);
		break;

	case ESteamVRBone::EBone_PinkyFinger0:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Pinky_0);
		break;

	case ESteamVRBone::EBone_PinkyFinger1:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Pinky_1);
		break;

	case ESteamVRBone::EBone_PinkyFinger2:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Pinky_2);
		break;

	case ESteamVRBone::EBone_PinkyFinger3:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Pinky_3);
		break;

	case ESteamVRBone::EBone_PinkyFinger4:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Pinky_4);
		break;

	default:
		break;
	}
}

void FAnimNode_SteamVRInputAnimPose::UpdateBoneMap(const FName& SrcBoneName, const FName RetargetName)
{
	FName NewName = NAME_None;
	if (RetargetName != NAME_None || !RetargetName.IsEqual(""))
	{
		NewName = RetargetName;
	}

	TransformedBoneNames.Add(NewName);
	BoneNameMap.Add(SrcBoneName, NewName);
}

FSteamVRInputDevice* FAnimNode_SteamVRInputAnimPose::GetSteamVRInputDevice()
{
	TArray<IMotionController*> MotionControllers = IModularFeatures::Get().GetModularFeatureImplementations<IMotionController>(IMotionController::GetModularFeatureName());
	for (auto MotionController : MotionControllers)
	{
		FName DeviceName = MotionController->GetModularFeatureName();
		return static_cast<FSteamVRInputDevice*>(MotionController);
	}

	return nullptr;
}
