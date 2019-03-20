#include "AnimNode_SteamVRInputAnimPose.h"
#include "ISteamVRInputDeviceModule.h"
#include "AnimationRuntime.h"
#include "AnimInstanceProxy.h"
#include "SteamVRInputDevice.h"

#define STEAMVRBONE(keyenum, name, parent) BoneKeypoints.Add(keyenum); BoneNames.Add(name); BoneParents.Add(parent);

FAnimNode_SteamVRInputAnimPose::FAnimNode_SteamVRInputAnimPose()
{
}

void FAnimNode_SteamVRInputAnimPose::Initialize(const FAnimationInitializeContext& Context)
{
	// Initialize Base Pose
	//BasePose.Initialize(Context);

	// Setup SteamVR Bones Hierarchy

	STEAMVRBONE(ESteamVRBone::EBone_Root, FName(TEXT("Root")), -1);
	STEAMVRBONE(ESteamVRBone::EBone_Wrist, FName(TEXT("wrist_r")), 0);

	STEAMVRBONE(ESteamVRBone::EBone_Thumb1, FName(TEXT("finger_thumb_0_r")), 1);
	STEAMVRBONE(ESteamVRBone::EBone_Thumb2, FName(TEXT("finger_thumb_1_r")), 2);
	STEAMVRBONE(ESteamVRBone::EBone_Thumb3, FName(TEXT("finger_thumb_2_r")), 3);
	STEAMVRBONE(ESteamVRBone::EBone_Thumb4, FName(TEXT("finger_thumb_r_end")), 4);

	STEAMVRBONE(ESteamVRBone::EBone_IndexFinger0, FName(TEXT("finger_index_meta_r")), 1);
	STEAMVRBONE(ESteamVRBone::EBone_IndexFinger1, FName(TEXT("finger_index_0_r")), 6);
	STEAMVRBONE(ESteamVRBone::EBone_IndexFinger2, FName(TEXT("finger_index_1_r")), 7);
	STEAMVRBONE(ESteamVRBone::EBone_IndexFinger3, FName(TEXT("finger_index_2_r")), 8);
	STEAMVRBONE(ESteamVRBone::EBone_IndexFinger4, FName(TEXT("finger_index_r_end")), 9);

	STEAMVRBONE(ESteamVRBone::EBone_MiddleFinger0, FName(TEXT("finger_middle_meta_r")), 1);
	STEAMVRBONE(ESteamVRBone::EBone_MiddleFinger1, FName(TEXT("finger_middle_0_r")), 11);
	STEAMVRBONE(ESteamVRBone::EBone_MiddleFinger2, FName(TEXT("finger_middle_1_r")), 12);
	STEAMVRBONE(ESteamVRBone::EBone_MiddleFinger3, FName(TEXT("finger_middle_2_r")), 13);
	STEAMVRBONE(ESteamVRBone::EBone_MiddleFinger4, FName(TEXT("finger_middle_r_end")), 14);

	STEAMVRBONE(ESteamVRBone::EBone_RingFinger0, FName(TEXT("finger_ring_meta_r")), 1);
	STEAMVRBONE(ESteamVRBone::EBone_RingFinger1, FName(TEXT("finger_ring_0_r")), 16);
	STEAMVRBONE(ESteamVRBone::EBone_RingFinger2, FName(TEXT("finger_ring_1_r")), 17);
	STEAMVRBONE(ESteamVRBone::EBone_RingFinger3, FName(TEXT("finger_ring_2_r")), 18);
	STEAMVRBONE(ESteamVRBone::EBone_RingFinger4, FName(TEXT("finger_ring_r_end")), 19);

	STEAMVRBONE(ESteamVRBone::EBone_PinkyFinger0, FName(TEXT("finger_pinky_meta_r")), 1);
	STEAMVRBONE(ESteamVRBone::EBone_PinkyFinger1, FName(TEXT("finger_pinky_0_r")), 21);
	STEAMVRBONE(ESteamVRBone::EBone_PinkyFinger2, FName(TEXT("finger_pinky_1_r")), 22);
	STEAMVRBONE(ESteamVRBone::EBone_PinkyFinger3, FName(TEXT("finger_pinky_2_r")), 23);
	STEAMVRBONE(ESteamVRBone::EBone_PinkyFinger4, FName(TEXT("finger_pinky_r_end")), 24);

	STEAMVRBONE(ESteamVRBone::EBone_Aux_Thumb, FName(TEXT("finger_thumb_r_aux")), 0);
	STEAMVRBONE(ESteamVRBone::EBone_Aux_IndexFinger, FName(TEXT("finger_index_r_aux")), 0);
	STEAMVRBONE(ESteamVRBone::EBone_Aux_MiddleFinger, FName(TEXT("finger_middle_r_aux")), 0);
	STEAMVRBONE(ESteamVRBone::EBone_Aux_RingFinger, FName(TEXT("finger_ring_r_aux")), 0);
	STEAMVRBONE(ESteamVRBone::EBone_Aux_PinkyFinger, FName(TEXT("finger_pinky_r_aux")), 0);

	// Left Hand
	STEAMVRBONE(ESteamVRBone::EBone_Wrist, FName(TEXT("wrist_l")), 0);

	STEAMVRBONE(ESteamVRBone::EBone_Thumb1, FName(TEXT("finger_thumb_0_l")), 31);
	STEAMVRBONE(ESteamVRBone::EBone_Thumb2, FName(TEXT("finger_thumb_1_l")), 32);
	STEAMVRBONE(ESteamVRBone::EBone_Thumb3, FName(TEXT("finger_thumb_2_l")), 33);
	STEAMVRBONE(ESteamVRBone::EBone_Thumb4, FName(TEXT("finger_thumb_l_end")), 34);

	STEAMVRBONE(ESteamVRBone::EBone_IndexFinger0, FName(TEXT("finger_index_meta_l")), 31);
	STEAMVRBONE(ESteamVRBone::EBone_IndexFinger1, FName(TEXT("finger_index_0_l")), 36);
	STEAMVRBONE(ESteamVRBone::EBone_IndexFinger2, FName(TEXT("finger_index_1_l")), 37);
	STEAMVRBONE(ESteamVRBone::EBone_IndexFinger3, FName(TEXT("finger_index_2_l")), 38);
	STEAMVRBONE(ESteamVRBone::EBone_IndexFinger4, FName(TEXT("finger_index_l_end")), 39);

	STEAMVRBONE(ESteamVRBone::EBone_MiddleFinger0, FName(TEXT("finger_middle_meta_l")), 31);
	STEAMVRBONE(ESteamVRBone::EBone_MiddleFinger1, FName(TEXT("finger_middle_0_l")), 41);
	STEAMVRBONE(ESteamVRBone::EBone_MiddleFinger2, FName(TEXT("finger_middle_1_l")), 42);
	STEAMVRBONE(ESteamVRBone::EBone_MiddleFinger3, FName(TEXT("finger_middle_2_l")), 43);
	STEAMVRBONE(ESteamVRBone::EBone_MiddleFinger4, FName(TEXT("finger_middle_l_end")), 44);

	STEAMVRBONE(ESteamVRBone::EBone_RingFinger0, FName(TEXT("finger_ring_meta_l")), 31);
	STEAMVRBONE(ESteamVRBone::EBone_RingFinger1, FName(TEXT("finger_ring_0_l")), 46);
	STEAMVRBONE(ESteamVRBone::EBone_RingFinger2, FName(TEXT("finger_ring_1_l")), 47);
	STEAMVRBONE(ESteamVRBone::EBone_RingFinger3, FName(TEXT("finger_ring_2_l")), 48);
	STEAMVRBONE(ESteamVRBone::EBone_RingFinger4, FName(TEXT("finger_ring_l_end")), 49);

	STEAMVRBONE(ESteamVRBone::EBone_PinkyFinger0, FName(TEXT("finger_pinky_meta_l")), 31);
	STEAMVRBONE(ESteamVRBone::EBone_PinkyFinger1, FName(TEXT("finger_pinky_0_l")), 51);
	STEAMVRBONE(ESteamVRBone::EBone_PinkyFinger2, FName(TEXT("finger_pinky_1_l")), 52);
	STEAMVRBONE(ESteamVRBone::EBone_PinkyFinger3, FName(TEXT("finger_pinky_2_l")), 53);
	STEAMVRBONE(ESteamVRBone::EBone_PinkyFinger4, FName(TEXT("finger_pinky_l_end")), 54);

	STEAMVRBONE(ESteamVRBone::EBone_Aux_Thumb, FName(TEXT("finger_thumb_l_aux")), 0);
	STEAMVRBONE(ESteamVRBone::EBone_Aux_IndexFinger, FName(TEXT("finger_index_l_aux")), 0);
	STEAMVRBONE(ESteamVRBone::EBone_Aux_MiddleFinger, FName(TEXT("finger_middle_l_aux")), 0);
	STEAMVRBONE(ESteamVRBone::EBone_Aux_RingFinger, FName(TEXT("finger_ring_l_aux")), 0);
	STEAMVRBONE(ESteamVRBone::EBone_Aux_PinkyFinger, FName(TEXT("finger_pinky_l_aux")), 0);
}

void FAnimNode_SteamVRInputAnimPose::CacheBones(const FAnimationCacheBonesContext & Context)
{
	//BasePose.CacheBones(Context);
}

void FAnimNode_SteamVRInputAnimPose::Update(const FAnimationUpdateContext & Context)
{
	EvaluateGraphExposedInputs.Execute(Context);

	// ...
	//BasePose.Update(Context);
}

void FAnimNode_SteamVRInputAnimPose::Evaluate(FPoseContext& Output)
{
	Output.ResetToRefPose();
	
	// ...
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{

		switch (MotionRange)
		{
		case EMotionRange::VR_WithController:
			FillHandTransformsWithController(SteamVRInputDevice);
			break;

		case EMotionRange::VR_WithoutController:
			// Falls through
		default:
			FillHandTransforms(SteamVRInputDevice);
			break;
		}

		TArray<FName, TMemStackAllocator<>> TransformedBoneNames;
		TransformedBoneNames.Reserve(BoneNames.Num());

		for (const FName& SrcBoneName : BoneNames)
		{
			// Prep for retargetting to UE Hand
			FName* TargetBoneName = BoneNameMap.Find(SrcBoneName);
			if (TargetBoneName == nullptr)
			{
				FName NewName = SrcBoneName;		// TODO: Remap to UE4 Hand
				TransformedBoneNames.Add(NewName);
				BoneNameMap.Add(SrcBoneName, NewName);
			}
			else
			{
				TransformedBoneNames.Add(*TargetBoneName);
			}
		}

		for (int32 i = 0; i < TransformedBoneNames.Num(); ++i)
		{
			FTransform BoneTransform = FTransform();
			FName BoneName = TransformedBoneNames[i];

			if (i <= 30)
			{
				switch (BoneKeypoints[i])
				{
				case ESteamVRBone::EBone_Root:
					BoneTransform = RightHand.Root;
					break;

				case ESteamVRBone::EBone_Wrist:
					BoneTransform = RightHand.Wrist;
					break;

				case ESteamVRBone::EBone_Thumb1:
					BoneTransform = RightHand.Thumb_0;
					break;

				case ESteamVRBone::EBone_Thumb2:
					BoneTransform = RightHand.Thumb_1;
					break;

				case ESteamVRBone::EBone_Thumb3:
					BoneTransform = RightHand.Thumb_2;
					break;

				case ESteamVRBone::EBone_Thumb4:
					BoneTransform = RightHand.Thumb_3;
					break;

				case ESteamVRBone::EBone_IndexFinger0:
					BoneTransform = RightHand.Index_0;
					break;

				case ESteamVRBone::EBone_IndexFinger1:
					BoneTransform = RightHand.Index_1;
					break;

				case ESteamVRBone::EBone_IndexFinger2:
					BoneTransform = RightHand.Index_2;
					break;

				case ESteamVRBone::EBone_IndexFinger3:
					BoneTransform = RightHand.Index_3;
					break;

				case ESteamVRBone::EBone_IndexFinger4:
					BoneTransform = RightHand.Index_4;
					break;

				case ESteamVRBone::EBone_MiddleFinger0:
					BoneTransform = RightHand.Middle_0;
					break;

				case ESteamVRBone::EBone_MiddleFinger1:
					BoneTransform = RightHand.Middle_1;
					break;

				case ESteamVRBone::EBone_MiddleFinger2:
					BoneTransform = RightHand.Middle_2;
					break;

				case ESteamVRBone::EBone_MiddleFinger3:
					BoneTransform = RightHand.Middle_3;
					break;

				case ESteamVRBone::EBone_MiddleFinger4:
					BoneTransform = RightHand.Middle_4;
					break;

				case ESteamVRBone::EBone_RingFinger0:
					BoneTransform = RightHand.Ring_0;
					break;

				case ESteamVRBone::EBone_RingFinger1:
					BoneTransform = RightHand.Ring_1;
					break;

				case ESteamVRBone::EBone_RingFinger2:
					BoneTransform = RightHand.Ring_2;
					break;

				case ESteamVRBone::EBone_RingFinger3:
					BoneTransform = RightHand.Ring_3;
					break;

				case ESteamVRBone::EBone_RingFinger4:
					BoneTransform = RightHand.Ring_4;
					break;

				case ESteamVRBone::EBone_PinkyFinger0:
					BoneTransform = RightHand.Pinky_0;
					break;

				case ESteamVRBone::EBone_PinkyFinger1:
					BoneTransform = RightHand.Pinky_1;
					break;

				case ESteamVRBone::EBone_PinkyFinger2:
					BoneTransform = RightHand.Pinky_2;
					break;

				case ESteamVRBone::EBone_PinkyFinger3:
					BoneTransform = RightHand.Pinky_3;
					break;

				case ESteamVRBone::EBone_PinkyFinger4:
					BoneTransform = RightHand.Pinky_4;
					break;

				default:
					break;
				}
			}
			else
			{
				switch (BoneKeypoints[i])
				{
				case ESteamVRBone::EBone_Root:
					BoneTransform = LeftHand.Root;
					break;

				case ESteamVRBone::EBone_Wrist:
					BoneTransform = LeftHand.Wrist;
					break;

				case ESteamVRBone::EBone_Thumb1:
					BoneTransform = LeftHand.Thumb_0;
					break;

				case ESteamVRBone::EBone_Thumb2:
					BoneTransform = LeftHand.Thumb_1;
					break;

				case ESteamVRBone::EBone_Thumb3:
					BoneTransform = LeftHand.Thumb_2;
					break;

				case ESteamVRBone::EBone_Thumb4:
					BoneTransform = LeftHand.Thumb_3;
					break;

				case ESteamVRBone::EBone_IndexFinger0:
					BoneTransform = LeftHand.Index_0;
					break;

				case ESteamVRBone::EBone_IndexFinger1:
					BoneTransform = LeftHand.Index_1;
					break;

				case ESteamVRBone::EBone_IndexFinger2:
					BoneTransform = LeftHand.Index_2;
					break;

				case ESteamVRBone::EBone_IndexFinger3:
					BoneTransform = LeftHand.Index_3;
					break;

				case ESteamVRBone::EBone_IndexFinger4:
					BoneTransform = LeftHand.Index_4;
					break;

				case ESteamVRBone::EBone_MiddleFinger0:
					BoneTransform = LeftHand.Middle_0;
					break;

				case ESteamVRBone::EBone_MiddleFinger1:
					BoneTransform = LeftHand.Middle_1;
					break;

				case ESteamVRBone::EBone_MiddleFinger2:
					BoneTransform = LeftHand.Middle_2;
					break;

				case ESteamVRBone::EBone_MiddleFinger3:
					BoneTransform = LeftHand.Middle_3;
					break;

				case ESteamVRBone::EBone_MiddleFinger4:
					BoneTransform = LeftHand.Middle_4;
					break;

				case ESteamVRBone::EBone_RingFinger0:
					BoneTransform = LeftHand.Ring_0;
					break;

				case ESteamVRBone::EBone_RingFinger1:
					BoneTransform = LeftHand.Ring_1;
					break;

				case ESteamVRBone::EBone_RingFinger2:
					BoneTransform = LeftHand.Ring_2;
					break;

				case ESteamVRBone::EBone_RingFinger3:
					BoneTransform = LeftHand.Ring_3;
					break;

				case ESteamVRBone::EBone_RingFinger4:
					BoneTransform = LeftHand.Ring_4;
					break;

				case ESteamVRBone::EBone_PinkyFinger0:
					BoneTransform = LeftHand.Pinky_0;
					break;

				case ESteamVRBone::EBone_PinkyFinger1:
					BoneTransform = LeftHand.Pinky_1;
					break;

				case ESteamVRBone::EBone_PinkyFinger2:
					BoneTransform = LeftHand.Pinky_2;
					break;

				case ESteamVRBone::EBone_PinkyFinger3:
					BoneTransform = LeftHand.Pinky_3;
					break;

				case ESteamVRBone::EBone_PinkyFinger4:
					BoneTransform = LeftHand.Pinky_4;
					break;

				default:
					break;
				}
			}

			int32 MeshIndex = Output.Pose.GetBoneContainer().GetPoseBoneIndexForBoneName(BoneName);
			if (MeshIndex != INDEX_NONE)
			{
				FCompactPoseBoneIndex BoneIndex = Output.Pose.GetBoneContainer().MakeCompactPoseIndex(FMeshPoseBoneIndex(MeshIndex));
				if (BoneIndex != INDEX_NONE)
				{
					FQuat NewRotation;
					if (BoneTransform.GetRotation().Equals(FQuat()))
					{
						NewRotation = Output.Pose[BoneIndex].GetRotation();
					}
					else
					{
						NewRotation = BoneTransform.GetRotation();
					}

					//FVector NewTranslation;
					//if (BoneTransform.GetLocation() == FVector::ZeroVector || 
					//	BoneTransform.GetLocation().X < -9999 || 
					//	BoneTransform.GetLocation().Y < -9999 ||
					//	BoneTransform.GetLocation().Z > 9999)
					//{
					//	NewTranslation = Output.Pose[BoneIndex].GetTranslation();
					//}
					//else
					//{
					//	NewTranslation = BoneTransform.GetLocation();
					//}
					UE_LOG(LogTemp, Warning, TEXT("[Current Translate %s] [Bone Translate %s]"), *Output.Pose[BoneIndex].GetTranslation().ToString(), *(BoneTransform.GetLocation()).ToString());

					FTransform BaseTransform = FTransform(Output.Pose[BoneIndex].GetRotation(), Output.Pose[BoneIndex].GetTranslation(), Output.Pose[BoneIndex].GetScale3D());
					//BaseTransform.SetLocation(NewTranslation);
					BaseTransform.SetRotation(NewRotation);

					Output.Pose[BoneIndex] = BaseTransform;
				}
			}
		}

	}
}

void FAnimNode_SteamVRInputAnimPose::FillHandTransforms(FSteamVRInputDevice* SteamVRInputDevice)
{
	// Left Hand
	LeftHand.Root = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[0], SteamVRInputDevice->SkeletonTransform_L[0]);
	LeftHand.Wrist = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[1], SteamVRInputDevice->SkeletonTransform_L[1]);

	LeftHand.Thumb_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[2], SteamVRInputDevice->SkeletonTransform_L[2]);
	LeftHand.Thumb_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[3], SteamVRInputDevice->SkeletonTransform_L[3]);
	LeftHand.Thumb_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[4], SteamVRInputDevice->SkeletonTransform_L[4]);
	LeftHand.Thumb_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[5], SteamVRInputDevice->SkeletonTransform_L[5]);

	LeftHand.Index_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[6], SteamVRInputDevice->SkeletonTransform_L[6]);
	LeftHand.Index_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[7], SteamVRInputDevice->SkeletonTransform_L[7]);
	LeftHand.Index_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[8], SteamVRInputDevice->SkeletonTransform_L[8]);
	LeftHand.Index_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[9], SteamVRInputDevice->SkeletonTransform_L[9]);
	LeftHand.Index_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[10], SteamVRInputDevice->SkeletonTransform_L[10]);

	LeftHand.Middle_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[11], SteamVRInputDevice->SkeletonTransform_L[11]);
	LeftHand.Middle_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[12], SteamVRInputDevice->SkeletonTransform_L[12]);
	LeftHand.Middle_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[13], SteamVRInputDevice->SkeletonTransform_L[13]);
	LeftHand.Middle_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[14], SteamVRInputDevice->SkeletonTransform_L[14]);
	LeftHand.Middle_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[15], SteamVRInputDevice->SkeletonTransform_L[15]);

	LeftHand.Ring_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[16], SteamVRInputDevice->SkeletonTransform_L[16]);
	LeftHand.Ring_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[17], SteamVRInputDevice->SkeletonTransform_L[17]);
	LeftHand.Ring_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[18], SteamVRInputDevice->SkeletonTransform_L[18]);
	LeftHand.Ring_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[19], SteamVRInputDevice->SkeletonTransform_L[19]);
	LeftHand.Ring_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[20], SteamVRInputDevice->SkeletonTransform_L[20]);

	LeftHand.Pinky_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[21], SteamVRInputDevice->SkeletonTransform_L[21]);
	LeftHand.Pinky_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[22], SteamVRInputDevice->SkeletonTransform_L[22]);
	LeftHand.Pinky_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[23], SteamVRInputDevice->SkeletonTransform_L[23]);
	LeftHand.Pinky_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[24], SteamVRInputDevice->SkeletonTransform_L[24]);
	LeftHand.Pinky_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[25], SteamVRInputDevice->SkeletonTransform_L[25]);

	LeftHand.Aux_Thumb = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[26], SteamVRInputDevice->SkeletonTransform_L[26]);
	LeftHand.Aux_Index = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[27], SteamVRInputDevice->SkeletonTransform_L[27]);
	LeftHand.Aux_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[28], SteamVRInputDevice->SkeletonTransform_L[28]);
	LeftHand.Aux_Ring = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[29], SteamVRInputDevice->SkeletonTransform_L[29]);
	LeftHand.Aux_Pinky = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[30], SteamVRInputDevice->SkeletonTransform_L[30]);

	LeftHand.Bone_Count = GetUETransform(SteamVRInputDevice->SkeletonTransform_L[31], SteamVRInputDevice->SkeletonTransform_L[31]);

	// Right Hand
	RightHand.Root = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[0], SteamVRInputDevice->SkeletonTransform_R[0]);
	RightHand.Wrist = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[1], SteamVRInputDevice->SkeletonTransform_R[1]);

	RightHand.Thumb_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[2], SteamVRInputDevice->SkeletonTransform_R[2]);
	RightHand.Thumb_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[3], SteamVRInputDevice->SkeletonTransform_R[3]);
	RightHand.Thumb_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[4], SteamVRInputDevice->SkeletonTransform_R[4]);
	RightHand.Thumb_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[5], SteamVRInputDevice->SkeletonTransform_R[5]);

	RightHand.Index_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[6], SteamVRInputDevice->SkeletonTransform_R[6]);
	RightHand.Index_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[7], SteamVRInputDevice->SkeletonTransform_R[7]);
	RightHand.Index_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[8], SteamVRInputDevice->SkeletonTransform_R[8]);
	RightHand.Index_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[9], SteamVRInputDevice->SkeletonTransform_R[9]);
	RightHand.Index_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[10], SteamVRInputDevice->SkeletonTransform_R[10]);

	RightHand.Middle_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[11], SteamVRInputDevice->SkeletonTransform_R[11]);
	RightHand.Middle_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[12], SteamVRInputDevice->SkeletonTransform_R[12]);
	RightHand.Middle_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[13], SteamVRInputDevice->SkeletonTransform_R[13]);
	RightHand.Middle_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[14], SteamVRInputDevice->SkeletonTransform_R[14]);
	RightHand.Middle_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[15], SteamVRInputDevice->SkeletonTransform_R[15]);

	RightHand.Ring_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[16], SteamVRInputDevice->SkeletonTransform_R[16]);
	RightHand.Ring_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[17], SteamVRInputDevice->SkeletonTransform_R[17]);
	RightHand.Ring_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[18], SteamVRInputDevice->SkeletonTransform_R[18]);
	RightHand.Ring_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[19], SteamVRInputDevice->SkeletonTransform_R[19]);
	RightHand.Ring_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[20], SteamVRInputDevice->SkeletonTransform_R[20]);

	RightHand.Pinky_0 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[21], SteamVRInputDevice->SkeletonTransform_R[21]);
	RightHand.Pinky_1 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[22], SteamVRInputDevice->SkeletonTransform_R[22]);
	RightHand.Pinky_2 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[23], SteamVRInputDevice->SkeletonTransform_R[23]);
	RightHand.Pinky_3 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[24], SteamVRInputDevice->SkeletonTransform_R[24]);
	RightHand.Pinky_4 = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[25], SteamVRInputDevice->SkeletonTransform_R[25]);

	RightHand.Aux_Thumb = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[26], SteamVRInputDevice->SkeletonTransform_R[26]);
	RightHand.Aux_Index = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[27], SteamVRInputDevice->SkeletonTransform_R[27]);
	RightHand.Aux_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[28], SteamVRInputDevice->SkeletonTransform_R[28]);
	RightHand.Aux_Ring = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[29], SteamVRInputDevice->SkeletonTransform_R[29]);
	RightHand.Aux_Pinky = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[30], SteamVRInputDevice->SkeletonTransform_R[30]);

	RightHand.Bone_Count = GetUETransform(SteamVRInputDevice->SkeletonTransform_R[31], SteamVRInputDevice->SkeletonTransform_R[31]);
}

void FAnimNode_SteamVRInputAnimPose::FillHandTransformsWithController(FSteamVRInputDevice* SteamVRInputDevice)
{
	// Left Hand
	LeftHand.Root = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[0], SteamVRInputDevice->SkeletonTransformC_L[0]);
	LeftHand.Wrist = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[1], SteamVRInputDevice->SkeletonTransformC_L[1]);

	LeftHand.Thumb_0 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[2], SteamVRInputDevice->SkeletonTransformC_L[2]);
	LeftHand.Thumb_1 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[3], SteamVRInputDevice->SkeletonTransformC_L[3]);
	LeftHand.Thumb_2 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[4], SteamVRInputDevice->SkeletonTransformC_L[4]);
	LeftHand.Thumb_3 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[5], SteamVRInputDevice->SkeletonTransformC_L[5]);

	LeftHand.Index_0 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[6], SteamVRInputDevice->SkeletonTransformC_L[6]);
	LeftHand.Index_1 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[7], SteamVRInputDevice->SkeletonTransformC_L[7]);
	LeftHand.Index_2 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[8], SteamVRInputDevice->SkeletonTransformC_L[8]);
	LeftHand.Index_3 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[9], SteamVRInputDevice->SkeletonTransformC_L[9]);
	LeftHand.Index_4 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[10], SteamVRInputDevice->SkeletonTransformC_L[10]);

	LeftHand.Middle_0 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[11], SteamVRInputDevice->SkeletonTransformC_L[11]);
	LeftHand.Middle_1 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[12], SteamVRInputDevice->SkeletonTransformC_L[12]);
	LeftHand.Middle_2 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[13], SteamVRInputDevice->SkeletonTransformC_L[13]);
	LeftHand.Middle_3 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[14], SteamVRInputDevice->SkeletonTransformC_L[14]);
	LeftHand.Middle_4 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[15], SteamVRInputDevice->SkeletonTransformC_L[15]);

	LeftHand.Ring_0 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[16], SteamVRInputDevice->SkeletonTransformC_L[16]);
	LeftHand.Ring_1 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[17], SteamVRInputDevice->SkeletonTransformC_L[17]);
	LeftHand.Ring_2 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[18], SteamVRInputDevice->SkeletonTransformC_L[18]);
	LeftHand.Ring_3 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[19], SteamVRInputDevice->SkeletonTransformC_L[19]);
	LeftHand.Ring_4 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[20], SteamVRInputDevice->SkeletonTransformC_L[20]);

	LeftHand.Pinky_0 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[21], SteamVRInputDevice->SkeletonTransformC_L[21]);
	LeftHand.Pinky_1 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[22], SteamVRInputDevice->SkeletonTransformC_L[22]);
	LeftHand.Pinky_2 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[23], SteamVRInputDevice->SkeletonTransformC_L[23]);
	LeftHand.Pinky_3 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[24], SteamVRInputDevice->SkeletonTransformC_L[24]);
	LeftHand.Pinky_4 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[25], SteamVRInputDevice->SkeletonTransformC_L[25]);

	LeftHand.Aux_Thumb = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[26], SteamVRInputDevice->SkeletonTransformC_L[26]);
	LeftHand.Aux_Index = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[27], SteamVRInputDevice->SkeletonTransformC_L[27]);
	LeftHand.Aux_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[28], SteamVRInputDevice->SkeletonTransformC_L[28]);
	LeftHand.Aux_Ring = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[29], SteamVRInputDevice->SkeletonTransformC_L[29]);
	LeftHand.Aux_Pinky = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[30], SteamVRInputDevice->SkeletonTransformC_L[30]);

	LeftHand.Bone_Count = GetUETransform(SteamVRInputDevice->SkeletonTransformC_L[31], SteamVRInputDevice->SkeletonTransformC_L[31]);

	// Right Hand
	RightHand.Root = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[0], SteamVRInputDevice->SkeletonTransformC_R[0]);
	RightHand.Wrist = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[1], SteamVRInputDevice->SkeletonTransformC_R[1]);

	RightHand.Thumb_0 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[2], SteamVRInputDevice->SkeletonTransformC_R[2]);
	RightHand.Thumb_1 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[3], SteamVRInputDevice->SkeletonTransformC_R[3]);
	RightHand.Thumb_2 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[4], SteamVRInputDevice->SkeletonTransformC_R[4]);
	RightHand.Thumb_3 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[5], SteamVRInputDevice->SkeletonTransformC_R[5]);

	RightHand.Index_0 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[6], SteamVRInputDevice->SkeletonTransformC_R[6]);
	RightHand.Index_1 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[7], SteamVRInputDevice->SkeletonTransformC_R[7]);
	RightHand.Index_2 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[8], SteamVRInputDevice->SkeletonTransformC_R[8]);
	RightHand.Index_3 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[9], SteamVRInputDevice->SkeletonTransformC_R[9]);
	RightHand.Index_4 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[10], SteamVRInputDevice->SkeletonTransformC_R[10]);

	RightHand.Middle_0 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[11], SteamVRInputDevice->SkeletonTransformC_R[11]);
	RightHand.Middle_1 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[12], SteamVRInputDevice->SkeletonTransformC_R[12]);
	RightHand.Middle_2 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[13], SteamVRInputDevice->SkeletonTransformC_R[13]);
	RightHand.Middle_3 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[14], SteamVRInputDevice->SkeletonTransformC_R[14]);
	RightHand.Middle_4 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[15], SteamVRInputDevice->SkeletonTransformC_R[15]);

	RightHand.Ring_0 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[16], SteamVRInputDevice->SkeletonTransformC_R[16]);
	RightHand.Ring_1 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[17], SteamVRInputDevice->SkeletonTransformC_R[17]);
	RightHand.Ring_2 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[18], SteamVRInputDevice->SkeletonTransformC_R[18]);
	RightHand.Ring_3 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[19], SteamVRInputDevice->SkeletonTransformC_R[19]);
	RightHand.Ring_4 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[20], SteamVRInputDevice->SkeletonTransformC_R[20]);

	RightHand.Pinky_0 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[21], SteamVRInputDevice->SkeletonTransformC_R[21]);
	RightHand.Pinky_1 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[22], SteamVRInputDevice->SkeletonTransformC_R[22]);
	RightHand.Pinky_2 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[23], SteamVRInputDevice->SkeletonTransformC_R[23]);
	RightHand.Pinky_3 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[24], SteamVRInputDevice->SkeletonTransformC_R[24]);
	RightHand.Pinky_4 = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[25], SteamVRInputDevice->SkeletonTransformC_R[25]);

	RightHand.Aux_Thumb = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[26], SteamVRInputDevice->SkeletonTransformC_R[26]);
	RightHand.Aux_Index = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[27], SteamVRInputDevice->SkeletonTransformC_R[27]);
	RightHand.Aux_Middle = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[28], SteamVRInputDevice->SkeletonTransformC_R[28]);
	RightHand.Aux_Ring = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[29], SteamVRInputDevice->SkeletonTransformC_R[29]);
	RightHand.Aux_Pinky = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[30], SteamVRInputDevice->SkeletonTransformC_R[30]);

	RightHand.Bone_Count = GetUETransform(SteamVRInputDevice->SkeletonTransformC_R[31], SteamVRInputDevice->SkeletonTransformC_R[31]);
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
		FVector());
		//FVector(SteamBoneTransform.position.v[0],
		//		-SteamBoneTransform.position.v[1],
		//		SteamBoneTransform.position.v[2]));

	return RetTransform;
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
