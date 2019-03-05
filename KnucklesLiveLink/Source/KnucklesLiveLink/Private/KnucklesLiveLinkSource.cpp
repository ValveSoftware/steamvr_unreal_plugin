#include "KnucklesLiveLinkSource.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "HAL/FileManagerGeneric.h"
#include "ILiveLinkClient.h"

#define LOCTEXT_NAMESPACE "KnucklesLiveLinkSource"
#define KNUCKLESBONE(keyenum, name, parent) BoneKeypoints.Add(keyenum); BoneNames.Add(name); BoneParents.Add(parent);

DEFINE_LOG_CATEGORY_STATIC(LogKnucklesLivelinkSource, Log, All);

FKnucklesLiveLinkSource::FKnucklesLiveLinkSource()
{
	// Tick delegate
	TickDelegate = FTickerDelegate::CreateRaw(this, &FKnucklesLiveLinkSource::Tick);
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(TickDelegate);

	// Live link params
	SourceStatus = LOCTEXT("SourceStatus_Active", "Active");
	SourceType = LOCTEXT("KnucklesLiveLinkSourceType", "Knuckles LiveLink");
	SourceMachineName = LOCTEXT("KnucklesLiveLinkSourceMachineName", "localhost");

	// Initialize OpenVR
	EVRInitError SteamVRError = VRInitError_None;
	SteamVRSystem = VR_Init(&SteamVRError, VRApplication_Scene);

	if (SteamVRError != VRInitError_None)
	{
		SteamVRSystem = NULL;
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] Unable to init SteamVR runtime : %s"), *FString(VR_GetVRInitErrorAsEnglishDescription(SteamVRError)));
		bSteamVRPresent = false;
		return;
	}
	else
	{
		UE_LOG(LogKnucklesLivelinkSource, Display, TEXT("[KNUCKLES LIVELINK] SteamVR runtime initialised with status: %s"), *FString(VR_GetVRInitErrorAsEnglishDescription(SteamVRError)));

		for (unsigned int id = 0; id < k_unMaxTrackedDeviceCount; ++id)
		{
			ETrackedDeviceClass trackedDeviceClass = SteamVRSystem->GetTrackedDeviceClass(id);
			char buf[32];

			if (trackedDeviceClass != ETrackedDeviceClass::TrackedDeviceClass_Invalid)
			{
				uint32 StringBytes = SteamVRSystem->GetStringTrackedDeviceProperty(id, ETrackedDeviceProperty::Prop_ModelNumber_String, buf, sizeof(buf));
				FString stringCache = *FString(UTF8_TO_TCHAR(buf));
				UE_LOG(LogKnucklesLivelinkSource, Display, TEXT("[KNUCKLES LIVELINK] Found the following device: [%i] %s"), id, *stringCache);
			}
		}

		// Set action manifest path
		const FString ActionManifestPath = FFileManagerGeneric::Get().ConvertToAbsolutePathForExternalAppForRead(*(FPaths::GeneratedConfigDir() / TEXT("steamvr_manifest.json")));
		UE_LOG(LogKnucklesLivelinkSource, Display, TEXT("[KNUCKLES LIVELINK] Action Manifest Path is [%s]  "), *ActionManifestPath);

		EVRInputError inputError = VRInput()->SetActionManifestPath(TCHAR_TO_UTF8(*ActionManifestPath));
		GetInputError(inputError, FString(TEXT("Setting Action Manifest Path Result")));

		// TODO: Action set path from manifest instead of a hardcode
		// Set Action Handles for Skeletal Data - REQUIRED!
		inputError = VRInput()->GetActionHandle(TCHAR_TO_UTF8(*FString(TEXT("/actions/main/in/skeletonleft"))), &SteamVRSkeletonLeft);
		GetInputError(inputError, FString(TEXT("Retrieveng Skeletal Action Handle (Left) Result")));

		inputError = VRInput()->GetActionHandle(TCHAR_TO_UTF8(*FString(TEXT("/actions/main/in/skeletonright"))), &SteamVRSkeletonRight);
		if (inputError != vr::VRInputError_None)
		GetInputError(inputError, FString(TEXT("Retrieveng Skeletal Action Handle (Left) Result")));

		// Set Action Set Handles
		inputError = VRInput()->GetActionSetHandle("/actions/main", &SteamVRActionSetDefault);
		GetInputError(inputError, FString(TEXT("Retrieveng Skeletal Action Set Handle Result")));

		// Get Input Source
		inputError = VRInput()->GetInputSourceHandle(TCHAR_TO_UTF8(*FString(TEXT("/user/hand/left"))), &SteamActiveOriginLeft);
		GetInputError(inputError, FString(TEXT("Retrieveng Input Source Handle (Left) Result: ")));

		inputError = VRInput()->GetInputSourceHandle(TCHAR_TO_UTF8(*FString(TEXT("/user/hand/right"))), &SteamActiveOriginRight);
		if (inputError == EVRInputError::VRInputError_None)
		GetInputError(inputError, FString(TEXT("Retrieveng Input Source Handle (Right) Result")));

		// Set SteamVR Status
		bSteamVRPresent = true;
	}

	// Setup Knuckles Bones
	KNUCKLESBONE(EKnucklesBone::EBone_Root, FName(TEXT("Root")), -1);
	KNUCKLESBONE(EKnucklesBone::EBone_Wrist, FName(TEXT("wrist_r")), 0);

	KNUCKLESBONE(EKnucklesBone::EBone_Thumb0, FName(TEXT("finger_thumb_0_r")), 1);
	KNUCKLESBONE(EKnucklesBone::EBone_Thumb1, FName(TEXT("finger_thumb_1_r")), 2);
	KNUCKLESBONE(EKnucklesBone::EBone_Thumb2, FName(TEXT("finger_thumb_2_r")), 3);
	KNUCKLESBONE(EKnucklesBone::EBone_Thumb3, FName(TEXT("finger_thumb_r_end")), 4);

	KNUCKLESBONE(EKnucklesBone::EBone_IndexFinger0, FName(TEXT("finger_index_meta_r")), 1);
	KNUCKLESBONE(EKnucklesBone::EBone_IndexFinger1, FName(TEXT("finger_index_0_r")), 6);
	KNUCKLESBONE(EKnucklesBone::EBone_IndexFinger2, FName(TEXT("finger_index_1_r")), 7);
	KNUCKLESBONE(EKnucklesBone::EBone_IndexFinger3, FName(TEXT("finger_index_2_r")), 8);
	KNUCKLESBONE(EKnucklesBone::EBone_IndexFinger4, FName(TEXT("finger_index_r_end")), 9);

	KNUCKLESBONE(EKnucklesBone::EBone_MiddleFinger0, FName(TEXT("finger_middle_meta_r")), 1);
	KNUCKLESBONE(EKnucklesBone::EBone_MiddleFinger1, FName(TEXT("finger_middle_0_r")), 11);
	KNUCKLESBONE(EKnucklesBone::EBone_MiddleFinger2, FName(TEXT("finger_middle_1_r")), 12);
	KNUCKLESBONE(EKnucklesBone::EBone_MiddleFinger3, FName(TEXT("finger_middle_2_r")), 13);
	KNUCKLESBONE(EKnucklesBone::EBone_MiddleFinger4, FName(TEXT("finger_middle_r_end")), 14);

	KNUCKLESBONE(EKnucklesBone::EBone_RingFinger0, FName(TEXT("finger_ring_meta_r")), 1);
	KNUCKLESBONE(EKnucklesBone::EBone_RingFinger1, FName(TEXT("finger_ring_0_r")), 16);
	KNUCKLESBONE(EKnucklesBone::EBone_RingFinger2, FName(TEXT("finger_ring_1_r")), 17);
	KNUCKLESBONE(EKnucklesBone::EBone_RingFinger3, FName(TEXT("finger_ring_2_r")), 18);
	KNUCKLESBONE(EKnucklesBone::EBone_RingFinger4, FName(TEXT("finger_ring_r_end")), 19);

	KNUCKLESBONE(EKnucklesBone::EBone_PinkyFinger0, FName(TEXT("finger_pinky_meta_r")), 1);
	KNUCKLESBONE(EKnucklesBone::EBone_PinkyFinger1, FName(TEXT("finger_pinky_0_r")), 21);
	KNUCKLESBONE(EKnucklesBone::EBone_PinkyFinger2, FName(TEXT("finger_pinky_1_r")), 22);
	KNUCKLESBONE(EKnucklesBone::EBone_PinkyFinger3, FName(TEXT("finger_pinky_2_r")), 23);
	KNUCKLESBONE(EKnucklesBone::EBone_PinkyFinger4, FName(TEXT("finger_pinky_r_end")), 24);

	KNUCKLESBONE(EKnucklesBone::EBone_Aux_Thumb, FName(TEXT("finger_thumb_r_aux")), 0);
	KNUCKLESBONE(EKnucklesBone::EBone_Aux_IndexFinger, FName(TEXT("finger_index_r_aux")), 0);
	KNUCKLESBONE(EKnucklesBone::EBone_Aux_MiddleFinger, FName(TEXT("finger_middle_r_aux")), 0);
	KNUCKLESBONE(EKnucklesBone::EBone_Aux_RingFinger, FName(TEXT("finger_ring_r_aux")), 0);
	KNUCKLESBONE(EKnucklesBone::EBone_Aux_PinkyFinger, FName(TEXT("finger_pinky_r_aux")), 0);

	// Left Hand
	KNUCKLESBONE(EKnucklesBone::EBone_Wrist, FName(TEXT("wrist_l")), 0);

	KNUCKLESBONE(EKnucklesBone::EBone_Thumb0, FName(TEXT("finger_thumb_0_l")), 31);
	KNUCKLESBONE(EKnucklesBone::EBone_Thumb1, FName(TEXT("finger_thumb_1_l")), 32);
	KNUCKLESBONE(EKnucklesBone::EBone_Thumb2, FName(TEXT("finger_thumb_2_l")), 33);
	KNUCKLESBONE(EKnucklesBone::EBone_Thumb3, FName(TEXT("finger_thumb_l_end")), 34);

	KNUCKLESBONE(EKnucklesBone::EBone_IndexFinger0, FName(TEXT("finger_index_meta_l")), 31);
	KNUCKLESBONE(EKnucklesBone::EBone_IndexFinger1, FName(TEXT("finger_index_0_l")), 36);
	KNUCKLESBONE(EKnucklesBone::EBone_IndexFinger2, FName(TEXT("finger_index_1_l")), 37);
	KNUCKLESBONE(EKnucklesBone::EBone_IndexFinger3, FName(TEXT("finger_index_2_l")), 38);
	KNUCKLESBONE(EKnucklesBone::EBone_IndexFinger4, FName(TEXT("finger_index_l_end")), 39);

	KNUCKLESBONE(EKnucklesBone::EBone_MiddleFinger0, FName(TEXT("finger_middle_meta_l")), 31);
	KNUCKLESBONE(EKnucklesBone::EBone_MiddleFinger1, FName(TEXT("finger_middle_0_l")), 41);
	KNUCKLESBONE(EKnucklesBone::EBone_MiddleFinger2, FName(TEXT("finger_middle_1_l")), 42);
	KNUCKLESBONE(EKnucklesBone::EBone_MiddleFinger3, FName(TEXT("finger_middle_2_l")), 43);
	KNUCKLESBONE(EKnucklesBone::EBone_MiddleFinger4, FName(TEXT("finger_middle_l_end")), 44);

	KNUCKLESBONE(EKnucklesBone::EBone_RingFinger0, FName(TEXT("finger_ring_meta_l")), 31);
	KNUCKLESBONE(EKnucklesBone::EBone_RingFinger1, FName(TEXT("finger_ring_0_l")), 46);
	KNUCKLESBONE(EKnucklesBone::EBone_RingFinger2, FName(TEXT("finger_ring_1_l")), 47);
	KNUCKLESBONE(EKnucklesBone::EBone_RingFinger3, FName(TEXT("finger_ring_2_l")), 48);
	KNUCKLESBONE(EKnucklesBone::EBone_RingFinger4, FName(TEXT("finger_ring_l_end")), 49);

	KNUCKLESBONE(EKnucklesBone::EBone_PinkyFinger0, FName(TEXT("finger_pinky_meta_l")), 31);
	KNUCKLESBONE(EKnucklesBone::EBone_PinkyFinger1, FName(TEXT("finger_pinky_0_l")), 51);
	KNUCKLESBONE(EKnucklesBone::EBone_PinkyFinger2, FName(TEXT("finger_pinky_1_l")), 52);
	KNUCKLESBONE(EKnucklesBone::EBone_PinkyFinger3, FName(TEXT("finger_pinky_2_l")), 53);
	KNUCKLESBONE(EKnucklesBone::EBone_PinkyFinger4, FName(TEXT("finger_pinky_l_end")), 54);

	KNUCKLESBONE(EKnucklesBone::EBone_Aux_Thumb, FName(TEXT("finger_thumb_l_aux")), 0);
	KNUCKLESBONE(EKnucklesBone::EBone_Aux_IndexFinger, FName(TEXT("finger_index_l_aux")), 0);
	KNUCKLESBONE(EKnucklesBone::EBone_Aux_MiddleFinger, FName(TEXT("finger_middle_l_aux")), 0);
	KNUCKLESBONE(EKnucklesBone::EBone_Aux_RingFinger, FName(TEXT("finger_ring_l_aux")), 0);
	KNUCKLESBONE(EKnucklesBone::EBone_Aux_PinkyFinger, FName(TEXT("finger_pinky_l_aux")), 0);

	// Set Live link bone data
	LiveLinkRefSkeleton.SetBoneNames(BoneNames);
	LiveLinkRefSkeleton.SetBoneParents(BoneParents);
}

FKnucklesLiveLinkSource::~FKnucklesLiveLinkSource()
{
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
}

bool FKnucklesLiveLinkSource::Tick(float DeltaTime)
{
	if (bSteamVRPresent)
	{
		UpdateLiveLink();
	}

	return true;
}

void FKnucklesLiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	LiveLinkClient = InClient;
	LiveLinkSourceGuid = InSourceGuid;
}

bool FKnucklesLiveLinkSource::IsSourceStillValid()
{
	return LiveLinkClient != nullptr;
}

bool FKnucklesLiveLinkSource::RequestSourceShutdown()
{
	LiveLinkClient = nullptr;
	LiveLinkSourceGuid.Invalidate();
	return true;
}

void FKnucklesLiveLinkSource::UpdateLiveLink()
{
	// Check if we're running in the Game Thread
	check(IsInGameThread());

	if (LiveLinkClient)
	{
		if (!bIsInitialized)
		{
			LiveLinkClient->PushSubjectSkeleton(LiveLinkSourceGuid, SubjectNameKnuckles, LiveLinkRefSkeleton);
			bIsInitialized = true;
		}

		// Update action set
		VRActiveActionSet_t actionSet[1] = { { 0 } };
		actionSet[0].ulActionSet = SteamVRActionSetDefault;
		actionSet[0].ulSecondaryActionSet = k_ulInvalidInputValueHandle;
		actionSet[0].ulRestrictedToDevice = k_ulInvalidInputValueHandle;

		EVRInputError inputError = vr::VRInput()->UpdateActionState(actionSet, sizeof(actionSet[0]), 1);
		CheckForSkeletalController();

		if (inputError == EVRInputError::VRInputError_None)
		{
			// Get Motion Range
			MotionRangeLeft = bRangeWithControllerL ? VRSkeletalMotionRange_WithController : VRSkeletalMotionRange_WithoutController;
			MotionRangeRight = bRangeWithControllerR ? VRSkeletalMotionRange_WithController : VRSkeletalMotionRange_WithoutController;

			// TODO: Check for active data first!
			// Get Skeletal Bone Data
			if (bLeftKnucklesPresent)
			{
				VRInput()->GetSkeletalBoneData(SteamVRSkeletonLeft, VRSkeletalTransformSpace_Parent, MotionRangeLeft, vrBoneTransformLeft, BoneCount);
			}
			else
			{
				VRInput()->GetSkeletalReferenceTransforms(SteamVRSkeletonLeft, VRSkeletalTransformSpace_Parent, EVRSkeletalReferencePose::VRSkeletalReferencePose_BindPose, vrBoneTransformLeft, BoneCount);
			}

			if (bRightKnucklesPresent)
			{
				VRInput()->GetSkeletalBoneData(SteamVRSkeletonRight, VRSkeletalTransformSpace_Parent, MotionRangeRight, vrBoneTransformRight, BoneCount);
			}
			else
			{
				VRInput()->GetSkeletalReferenceTransforms(SteamVRSkeletonRight, VRSkeletalTransformSpace_Parent, EVRSkeletalReferencePose::VRSkeletalReferencePose_BindPose, vrBoneTransformRight, BoneCount);
			}

			TArray<FTransform> BoneTransforms;
			TArray<FTransform> UEBoneTransformsLeft;
			TArray<FTransform> UEBoneTransformsRight;

			// Root (Right)
			BoneTransforms.Add(FTransform(
				FRotator(0.f, 0.f, 0.f),
				FVector::ZeroVector,
				FVector(100.f, 100.f, 100.f)));
			
			// Wrist (Right)
			BoneTransforms.Add(FTransform(
				FRotator(0.f, 0.f, 0.f),
				FVector(-0.000160f, 0.000032f, -0.000626f),
				FVector(1.f, 1.f, 1.f)));

			// Thumb (Right)
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[2], FVector(0.017914f, -0.029178f, 0.025298f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[3], FVector(-0.040406f, 0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[4], FVector(-0.032517f, 0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[5], FVector(-0.030464f, -0.000000f, 0.000000f), true));

			// Index (Right)
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[6], FVector(0.001557f, -0.021073f, 0.014787f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[7], FVector(-0.073798f, 0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[8], FVector(-0.043287f, -0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[9], FVector(-0.028275f, 0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[10], FVector(-0.022821f, 0.000000f, 0.000000f), true));

			// Middle (Right)
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[11], FVector(-0.002177f, -0.007120f, .016319f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[12], FVector(-0.070886f, -0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[13], FVector(-0.043108f, 0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[14], FVector(-0.033266f, -0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[15], FVector(-0.025892f, -0.000000f, -0.000000f), true));

			// Ring (Right)
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[16], FVector(-0.000513f, 0.006545, 0.016348f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[17], FVector(-0.065975f, 0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[18], FVector(-0.040331f, 0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[19], FVector(-0.028489f, -0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[20], FVector(-0.022430f, -0.000000f, -0.000000f), true));

			// Pinky (Right)
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[21], FVector(0.002478f, 0.018981f, 0.015214f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[22], FVector(-0.062856f, -0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[23], FVector(-0.029874f, 0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[24], FVector(-0.017979f, -0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[25], FVector(-0.018018f, 0.000000f, 0.000000f), true));

			// Aux (Right) - For completion
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[26]));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[27]));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[28]));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[29]));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformRight[30]));


			// Wrist (Left)
			BoneTransforms.Add(FTransform(
				FRotator(0.f, 0.f, 0.f),
				FVector(0.000160f, 0.000032f, -0.000626f),
				FVector(1.f, 1.f, 1.f)));

			// Thumb (Left)
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[2], FVector(-0.017914f, -0.029178f, 0.025298f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[3], FVector(0.040406f, 0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[4], FVector(0.032517f, 0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[5], FVector(0.030464f, -0.000000f, 0.000000f), true));

			// Index (Left)
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[6], FVector(-0.001557f, -0.021073f, 0.014787f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[7], FVector(0.073798f, 0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[8], FVector(0.043287f, -0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[9], FVector(0.028275f, 0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[10], FVector(0.022821f, 0.000000f, 0.000000f), true));

			// Middle (Left)
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[11], FVector(0.002177f, -0.007120f, .016319f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[12], FVector(0.070886f, -0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[13], FVector(0.043108f, 0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[14], FVector(0.033266f, -0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[15], FVector(0.025892f, -0.000000f, -0.000000f), true));

			// Ring (Left)
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[16], FVector(0.000513f, 0.006545, 0.016348f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[17], FVector(0.065975f, 0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[18], FVector(0.040331f, 0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[19], FVector(0.028489f, -0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[20], FVector(0.022430f, -0.000000f, -0.000000f), true));

			// Pinky (Left)
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[21], FVector(-0.002478f, 0.018981f, 0.015214f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[22], FVector(0.062856f, -0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[23], FVector(0.029874f, 0.000000f, 0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[24], FVector(0.017979f, -0.000000f, -0.000000f), true));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[25], FVector(0.018018f, 0.000000f, 0.000000f), true));

			// Aux (Left) - For completion
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[26]));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[27]));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[28]));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[29]));
			BoneTransforms.Add(GetUEBoneTransform(vrBoneTransformLeft[30]));

			// Setup Frame Data
			if (bWithKnucklesAnim)
			{
				static FLiveLinkFrameData LiveLinkKnucklesFrame;

				LiveLinkKnucklesFrame.Transforms = BoneTransforms;
				LiveLinkKnucklesFrame.WorldTime = FPlatformTime::Seconds();

				LiveLinkClient->PushSubjectData(LiveLinkSourceGuid, SubjectNameKnuckles, LiveLinkKnucklesFrame);
			}

		}
		else
		{
			GetInputError(inputError, FString(TEXT("[KNUCKLES LIVELINK] Action State Update Frame Result")));
		}
	}

}

void FKnucklesLiveLinkSource::CheckForSkeletalController()
{
	KnucklesControllerIdLeft = -1;
	bLeftKnucklesPresent = false;

	KnucklesControllerIdRight = -1;
	bRightKnucklesPresent = false;

	for (unsigned int id = 0; id < k_unMaxTrackedDeviceCount; ++id)
	{
		// Check if this is a controller
		ETrackedDeviceClass trackedDeviceClass = SteamVRSystem->GetTrackedDeviceClass(id);

		if (SteamVRSystem && trackedDeviceClass == ETrackedDeviceClass::TrackedDeviceClass_Controller)
		{
			//uint32 StringBytes = SteamVRSystem->GetStringTrackedDeviceProperty(id, ETrackedDeviceProperty::Prop_ModelNumber_String, buf, sizeof(buf));
			//FString stringCache = *FString(UTF8_TO_TCHAR(buf));
			//UE_LOG(LogKnucklesLivelinkSource, Display, TEXT("[KNUCKLES LIVELINK] Found the following device: [%i] %s"), id, *stringCache);

			// Check if Knuckles is present and active
			EDeviceActivityLevel DeviceActivityLevel = SteamVRSystem->GetTrackedDeviceActivityLevel(id);

			if (DeviceActivityLevel != k_EDeviceActivityLevel_Unknown)
			{
				if (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(id) == ETrackedControllerRole::TrackedControllerRole_LeftHand)
				{
					EVRInputError inputError = VRInput()->GetSkeletalTrackingLevel(SteamVRSkeletonLeft, &vrSkeletalTrackingLevel);
					//UE_LOG(LogKnucklesLivelinkSource, Warning, TEXT("[KNUCKLES LIVELINK] Left Skeletal Tracking Level: %i"), vrSkeletalTrackingLevel);

					if (inputError == VRInputError_None &&
						vrSkeletalTrackingLevel == VRSkeletalTracking_Partial)
					{
						KnucklesControllerIdLeft = id;
						bLeftKnucklesPresent = true;
						//UE_LOG(LogKnucklesLivelinkSource, Warning, TEXT("[KNUCKLES LIVELINK] Knuckles Left found and is ACTIVE"));
					}

				}
				else if (SteamVRSystem->GetControllerRoleForTrackedDeviceIndex(id) == ETrackedControllerRole::TrackedControllerRole_RightHand)
				{
					EVRInputError inputError = VRInput()->GetSkeletalTrackingLevel(SteamVRSkeletonRight, &vrSkeletalTrackingLevel);
					//UE_LOG(LogKnucklesLivelinkSource, Warning, TEXT("[KNUCKLES LIVELINK] Right Skeletal Tracking Level: %i"), vrSkeletalTrackingLevel);

					if (inputError == VRInputError_None &&
						vrSkeletalTrackingLevel == VRSkeletalTracking_Partial)
					{
						KnucklesControllerIdRight = id;
						bRightKnucklesPresent = true;
						//UE_LOG(LogKnucklesLivelinkSource, Warning, TEXT("[KNUCKLES LIVELINK] Knuckles Right found and is ACTIVE"));
					}
				}
			}
		}
	}
}

FTransform FKnucklesLiveLinkSource::GetUEBoneTransform(VRBoneTransform_t SteamBoneTransform, FVector OverrideVector, bool bOverrideVector)
{
	FTransform RetTransform;

	// Calculate UE Bone Transform
	if (bOverrideVector)
	{
		RetTransform = FTransform(FQuat(SteamBoneTransform.orientation.x,
			-SteamBoneTransform.orientation.y,
			SteamBoneTransform.orientation.z,
			-SteamBoneTransform.orientation.w),
			OverrideVector);
	}
	else
	{
		RetTransform = FTransform(FQuat(SteamBoneTransform.orientation.x,
			-SteamBoneTransform.orientation.y,
			SteamBoneTransform.orientation.z,
			-SteamBoneTransform.orientation.w),
			FVector(SteamBoneTransform.position.v[2] * -1.f,
				SteamBoneTransform.position.v[0],
				SteamBoneTransform.position.v[1]));

	}

	// Ensure Rotations are Normalized
	if (!RetTransform.GetRotation().IsNormalized())
	{
		RetTransform.GetRotation().Normalize();
	}

	return RetTransform;
}

void FKnucklesLiveLinkSource::GetInputError(EVRInputError InputError, FString InputAction)
{
	// TODO: Refactor strings
	switch (InputError)
	{
	case vr::VRInputError_None:
		UE_LOG(LogKnucklesLivelinkSource, Display, TEXT("[KNUCKLES LIVELINK] %s: Success"), *InputAction);
		break;
	case vr::VRInputError_NameNotFound:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Name Not Found"), *InputAction);
		break;
	case vr::VRInputError_WrongType:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Wrong Type"), *InputAction);
		break;
	case vr::VRInputError_InvalidHandle:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Invalid Handle"), *InputAction);
		break;
	case vr::VRInputError_InvalidParam:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Invalid Param"), *InputAction);
		break;
	case vr::VRInputError_NoSteam:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: No Steam"), *InputAction);
		break;
	case vr::VRInputError_MaxCapacityReached:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s:  Max Capacity Reached"), *InputAction);
		break;
	case vr::VRInputError_IPCError:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: IPC Error"), *InputAction);
		break;
	case vr::VRInputError_NoActiveActionSet:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: No Active Action Set"), *InputAction);
		break;
	case vr::VRInputError_InvalidDevice:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Invalid Device"), *InputAction);
		break;
	case vr::VRInputError_InvalidSkeleton:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Invalid Skeleton"), *InputAction);
		break;
	case vr::VRInputError_InvalidBoneCount:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Invalid Bone Count"), *InputAction);
		break;
	case vr::VRInputError_InvalidCompressedData:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Invalid Compressed Data"), *InputAction);
		break;
	case vr::VRInputError_NoData:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: No Data"), *InputAction);
		break;
	case vr::VRInputError_BufferTooSmall:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Buffer Too Small"), *InputAction);
		break;
	case vr::VRInputError_MismatchedActionManifest:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Mismatched Action Manifest"), *InputAction);
		break;
	case vr::VRInputError_MissingSkeletonData:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Missing Skeleton Data"), *InputAction);
		break;
	default:
		UE_LOG(LogKnucklesLivelinkSource, Error, TEXT("[KNUCKLES LIVELINK] %s: Unknown Error"), *InputAction);
		break;
	}

	return;
}

#undef LOCTEXT_NAMESPACE
