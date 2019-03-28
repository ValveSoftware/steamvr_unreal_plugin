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

	// Update Skeletal Animation
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		FTransform OutPose[STEAMVR_SKELETON_BONE_COUNT];

		EVRSkeletalMotionRange SteamVRMotionRange = (MotionRange == EMotionRange::VR_WithController) ? VRSkeletalMotionRange_WithController : VRSkeletalMotionRange_WithoutController;

		bool bIsLeftHand = (Hand == EHand::VR_LeftHand);
		bool bIsXAxisForward = (SkeletonForwardAxis == ESkeletonForwardAxis::VR_SkeletonAxisX);

		// Attempt to read the current skeletal pose from SteamVR
		if (SteamVRInputDevice->GetSkeletalData(bIsLeftHand, bIsXAxisForward, SteamVRMotionRange, OutPose, STEAMVR_SKELETON_BONE_COUNT))
		{
			for (int32 i = 0; i < TransformedBoneNames.Num(); ++i)
			{
				FTransform BoneTransform = FTransform();
				FName BoneName = TransformedBoneNames[i];

				if (HandSkeleton == EHandSkeleton::VR_SteamVRHandSkeleton)
				{
					BoneTransform = OutPose[i];
				}
				else if (HandSkeleton == EHandSkeleton::VR_UE4HandSkeleton)
				{
					int32 SteamVRHandIndex = GetSteamVRHandIndex(i);
					if (SteamVRHandIndex != INDEX_NONE)
					{
						BoneTransform = OutPose[SteamVRHandIndex];
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
							BoneTransform = OutPose[i];
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
						FTransform oldTransform = Output.Pose[BoneIndex];
						(void)oldTransform;
						Output.Pose[BoneIndex] = OutTransform;
					}
				}
			}
		}
	}
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

	case ESteamVRBone::EBone_Aux_Thumb:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Aux_Thumb);
		break;

	case ESteamVRBone::EBone_Aux_IndexFinger:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Aux_Index);
		break;

	case ESteamVRBone::EBone_Aux_MiddleFinger:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Aux_Middle);
		break;

	case ESteamVRBone::EBone_Aux_RingFinger:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Aux_Ring);
		break;

	case ESteamVRBone::EBone_Aux_PinkyFinger:
		UpdateBoneMap(SrcBoneName, CustomBoneMapping.Aux_Pinky);
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
