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

	// Update Skeletal Animation
	FSteamVRInputDevice* SteamVRInputDevice = GetSteamVRInputDevice();
	if (SteamVRInputDevice != nullptr)
	{
		FTransform OutPose[STEAMVR_SKELETON_BONE_COUNT];

		EVRSkeletalMotionRange SteamVRMotionRange = ( MotionRange == EMotionRange::VR_WithController ) ? VRSkeletalMotionRange_WithController : VRSkeletalMotionRange_WithoutController;

		bool bIsLeftHand = ( Hand == EHand::VR_LeftHand );

		// Attempt to read the current skeletal pose from SteamVR
		if ( SteamVRInputDevice->GetSkeletalData( bIsLeftHand, SteamVRMotionRange, OutPose, STEAMVR_SKELETON_BONE_COUNT ) )
		{
			for ( int32 i = 0; i < TransformedBoneNames.Num(); ++i )
			{
				FTransform BoneTransform = FTransform();
				FName BoneName = TransformedBoneNames[ i ];

				if ( HandSkeleton == EHandSkeleton::VR_SteamVRHandSkeleton )
				{
					BoneTransform = OutPose[ i ];
				}
				else if ( HandSkeleton == EHandSkeleton::VR_UE4HandSkeleton )
				{
					int32 SteamVRHandIndex = GetSteamVRHandIndex( i );
					if ( SteamVRHandIndex != INDEX_NONE )
					{
						BoneTransform = OutPose[ SteamVRHandIndex ];
					}
				}
				else
				{
					// Check if there's a mapping for this bone to the SteamVR skeleton
					if ( i < SteamVRSkeleton::GetBoneCount() )
					{
						FName MappedBone = BoneNameMap.FindRef( SteamVRSkeleton::GetBoneName( i ) );

						if ( MappedBone.IsValid() && MappedBone != NAME_None )
						{
							BoneTransform = OutPose[ i ];
						}
					}
				}

				int32 MeshIndex;
				if ( HandSkeleton == EHandSkeleton::VR_CustomSkeleton )
				{
					MeshIndex = Output.Pose.GetBoneContainer().GetPoseBoneIndexForBoneName( BoneName );
					//UE_LOG(LogTemp, Warning, TEXT("Bone Map [%s] from SteamVR Index[%i] to MeshIndex [%i]"), *BoneName.ToString(), i, MeshIndex);
				}
				else
				{
					MeshIndex = i;
				}

				if ( MeshIndex != INDEX_NONE )
				{
					FCompactPoseBoneIndex BoneIndex = Output.Pose.GetBoneContainer().MakeCompactPoseIndex( FMeshPoseBoneIndex( MeshIndex ) );
					if ( BoneIndex != INDEX_NONE )
					{
						FQuat NewRotation;
						if ( BoneTransform.GetRotation().Equals( FQuat() ) ||
							BoneTransform.GetRotation().ContainsNaN() )
						{
							NewRotation = Output.Pose[ BoneIndex ].GetRotation();
						}
						else
						{
							NewRotation = BoneTransform.GetRotation();
						}

						FVector NewTranslation;
						if ( BoneTransform.GetLocation() == FVector::ZeroVector || BoneTransform.ContainsNaN() || HandSkeleton != EHandSkeleton::VR_SteamVRHandSkeleton )
						{
							NewTranslation = Output.Pose[ BoneIndex ].GetTranslation();
						}
						else
						{
							NewTranslation = BoneTransform.GetLocation();
						}
						//UE_LOG(LogTemp, Warning, TEXT("[Current Translate %s] [Bone Translate %s]"), *Output.Pose[BoneIndex].GetTranslation().ToString(), *(BoneTransform.GetLocation()).ToString());

						FTransform OutTransform = FTransform( Output.Pose[ BoneIndex ].GetRotation(), Output.Pose[ BoneIndex ].GetTranslation(), Output.Pose[ BoneIndex ].GetScale3D() );
						OutTransform.SetLocation( NewTranslation );
						OutTransform.SetRotation( NewRotation );

						// Set new bone transform
						FTransform oldTransform = Output.Pose[ BoneIndex ];
						( void )oldTransform;
						Output.Pose[ BoneIndex ] = OutTransform;
					}
				}
			}
		}
	}
}

FTransform FAnimNode_SteamVRInputAnimPose::GetUETransform(VRBoneTransform_t SteamBoneTransform)
{
	FTransform RetTransform;

	FQuat OrientationQuat(
		SteamBoneTransform.orientation.x,
		-SteamBoneTransform.orientation.y,
		 SteamBoneTransform.orientation.z,
		-SteamBoneTransform.orientation.w);
	OrientationQuat.Normalize();

	RetTransform = FTransform(OrientationQuat,

		FVector(SteamBoneTransform.position.v[0],
				-SteamBoneTransform.position.v[1],
			SteamBoneTransform.position.v[2]));

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

//void FAnimNode_SteamVRInputAnimPose::GetSteamVRBoneTransform(int32 SteamVRBoneIndex, FTransform& OutTransform)
//{
//	if (Hand == EHand::VR_RightHand)
//	{
//		switch ((ESteamVRBone)SteamVRBoneIndex)
//		{
//		case ESteamVRBone::EBone_Root:
//			//OutTransform = RightHand.Root;
//			break;
//
//		case ESteamVRBone::EBone_Wrist:
//			OutTransform = RightHand.Wrist;
//			if (HandSkeleton == EHandSkeleton::VR_SteamVRHandSkeleton)
//			{
//				OutTransform.SetLocation(FVector::ZeroVector);
//				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(180.f, 0.f, 45.f))));
//			}
//			else if (HandSkeleton == EHandSkeleton::VR_UE4HandSkeleton)
//			{
//				OutTransform.SetLocation(FVector::ZeroVector);
//				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(0.f, 0.f, -100.f))));
//			}
//			break;
//
//		case ESteamVRBone::EBone_Thumb1:
//			OutTransform = RightHand.Thumb_0;
//			if (HandSkeleton != EHandSkeleton::VR_SteamVRHandSkeleton)
//			{
//				OutTransform = RightHand.Thumb_0;
//				OutTransform.SetLocation(FVector::ZeroVector);
//				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(-15.f, -100.f, 180.f))));
//			}
//			break;
//
//		case ESteamVRBone::EBone_Thumb2:
//			OutTransform = RightHand.Thumb_1;
//			break;
//
//		case ESteamVRBone::EBone_Thumb3:
//			OutTransform = RightHand.Thumb_2;
//			break;
//
//		case ESteamVRBone::EBone_Thumb4:
//			OutTransform = RightHand.Thumb_3;
//			break;
//
//		case ESteamVRBone::EBone_IndexFinger0:
//			OutTransform = RightHand.Index_0;
//			break;
//
//		case ESteamVRBone::EBone_IndexFinger1:
//			OutTransform = RightHand.Index_1;
//			break;
//
//		case ESteamVRBone::EBone_IndexFinger2:
//			OutTransform = RightHand.Index_2;
//			break;
//
//		case ESteamVRBone::EBone_IndexFinger3:
//			OutTransform = RightHand.Index_3;
//			break;
//
//		case ESteamVRBone::EBone_IndexFinger4:
//			OutTransform = RightHand.Index_4;
//			break;
//
//		case ESteamVRBone::EBone_MiddleFinger0:
//			OutTransform = RightHand.Middle_0;
//			break;
//
//		case ESteamVRBone::EBone_MiddleFinger1:
//			OutTransform = RightHand.Middle_1;
//			break;
//
//		case ESteamVRBone::EBone_MiddleFinger2:
//			OutTransform = RightHand.Middle_2;
//			break;
//
//		case ESteamVRBone::EBone_MiddleFinger3:
//			OutTransform = RightHand.Middle_3;
//			break;
//
//		case ESteamVRBone::EBone_MiddleFinger4:
//			OutTransform = RightHand.Middle_4;
//			break;
//
//		case ESteamVRBone::EBone_RingFinger0:
//			OutTransform = RightHand.Ring_0;
//			break;
//
//		case ESteamVRBone::EBone_RingFinger1:
//			OutTransform = RightHand.Ring_1;
//			break;
//
//		case ESteamVRBone::EBone_RingFinger2:
//			OutTransform = RightHand.Ring_2;
//			break;
//
//		case ESteamVRBone::EBone_RingFinger3:
//			OutTransform = RightHand.Ring_3;
//			break;
//
//		case ESteamVRBone::EBone_RingFinger4:
//			OutTransform = RightHand.Ring_4;
//			break;
//
//		case ESteamVRBone::EBone_PinkyFinger0:
//			OutTransform = RightHand.Pinky_0;
//			break;
//
//		case ESteamVRBone::EBone_PinkyFinger1:
//			OutTransform = RightHand.Pinky_1;
//			break;
//
//		case ESteamVRBone::EBone_PinkyFinger2:
//			OutTransform = RightHand.Pinky_2;
//			break;
//
//		case ESteamVRBone::EBone_PinkyFinger3:
//			OutTransform = RightHand.Pinky_3;
//			break;
//
//		case ESteamVRBone::EBone_PinkyFinger4:
//			OutTransform = RightHand.Pinky_4;
//			break;
//
//		default:
//			break;
//		}
//	}
//	else
//	{
//		switch ((ESteamVRBone)SteamVRBoneIndex)
//		{
//		case ESteamVRBone::EBone_Root:
//			//OutTransform = LeftHand.Root;
//			break;
//
//		case ESteamVRBone::EBone_Wrist:
//			OutTransform = LeftHand.Wrist;
//			if (HandSkeleton == EHandSkeleton::VR_SteamVRHandSkeleton)
//			{
//				OutTransform.SetLocation(FVector::ZeroVector);
//				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(180.f, 0.f, 45.f))));
//			}
//			else if (HandSkeleton == EHandSkeleton::VR_UE4HandSkeleton)
//			{
//				OutTransform.SetLocation(FVector::ZeroVector);
//				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(0.f, 0.f, -100.f))));
//			}
//			break;
//
//		case ESteamVRBone::EBone_Thumb1:
//			OutTransform = LeftHand.Thumb_0;
//			if (HandSkeleton != EHandSkeleton::VR_SteamVRHandSkeleton)
//			{
//				OutTransform = RightHand.Thumb_0;
//				OutTransform.SetLocation(FVector::ZeroVector);
//				OutTransform.GetRotation();
//				OutTransform.SetRotation(FQuat((FRotator(OutTransform.GetRotation()).Add(-45.f, -45.f, 90.f))));
//			}
//			break;
//
//		case ESteamVRBone::EBone_Thumb2:
//			OutTransform = LeftHand.Thumb_1;
//			break;
//
//		case ESteamVRBone::EBone_Thumb3:
//			OutTransform = LeftHand.Thumb_2;
//			break;
//
//		case ESteamVRBone::EBone_Thumb4:
//			OutTransform = LeftHand.Thumb_3;
//			break;
//
//		case ESteamVRBone::EBone_IndexFinger0:
//			OutTransform = LeftHand.Index_0;
//			break;
//
//		case ESteamVRBone::EBone_IndexFinger1:
//			OutTransform = LeftHand.Index_1;
//			break;
//
//		case ESteamVRBone::EBone_IndexFinger2:
//			OutTransform = LeftHand.Index_2;
//			break;
//
//		case ESteamVRBone::EBone_IndexFinger3:
//			OutTransform = LeftHand.Index_3;
//			break;
//
//		case ESteamVRBone::EBone_IndexFinger4:
//			OutTransform = LeftHand.Index_4;
//			break;
//
//		case ESteamVRBone::EBone_MiddleFinger0:
//			OutTransform = LeftHand.Middle_0;
//			break;
//
//		case ESteamVRBone::EBone_MiddleFinger1:
//			OutTransform = LeftHand.Middle_1;
//			break;
//
//		case ESteamVRBone::EBone_MiddleFinger2:
//			OutTransform = LeftHand.Middle_2;
//			break;
//
//		case ESteamVRBone::EBone_MiddleFinger3:
//			OutTransform = LeftHand.Middle_3;
//			break;
//
//		case ESteamVRBone::EBone_MiddleFinger4:
//			OutTransform = LeftHand.Middle_4;
//			break;
//
//		case ESteamVRBone::EBone_RingFinger0:
//			OutTransform = LeftHand.Ring_0;
//			break;
//
//		case ESteamVRBone::EBone_RingFinger1:
//			OutTransform = LeftHand.Ring_1;
//			break;
//
//		case ESteamVRBone::EBone_RingFinger2:
//			OutTransform = LeftHand.Ring_2;
//			break;
//
//		case ESteamVRBone::EBone_RingFinger3:
//			OutTransform = LeftHand.Ring_3;
//			break;
//
//		case ESteamVRBone::EBone_RingFinger4:
//			OutTransform = LeftHand.Ring_4;
//			break;
//
//		case ESteamVRBone::EBone_PinkyFinger0:
//			OutTransform = LeftHand.Pinky_0;
//			break;
//
//		case ESteamVRBone::EBone_PinkyFinger1:
//			OutTransform = LeftHand.Pinky_1;
//			break;
//
//		case ESteamVRBone::EBone_PinkyFinger2:
//			OutTransform = LeftHand.Pinky_2;
//			break;
//
//		case ESteamVRBone::EBone_PinkyFinger3:
//			OutTransform = LeftHand.Pinky_3;
//			break;
//
//		case ESteamVRBone::EBone_PinkyFinger4:
//			OutTransform = LeftHand.Pinky_4;
//			break;
//
//		default:
//			break;
//		}
//	}
//}


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
