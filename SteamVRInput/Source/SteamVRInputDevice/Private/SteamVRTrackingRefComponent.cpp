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

#include "SteamVRTrackingRefComponent.h"
#include "../../OpenVRSDK/headers/openvr.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "SteamVRInput.h"

using namespace vr;
DEFINE_LOG_CATEGORY_STATIC(LogSteamVRTrackingRefComponent, Log, All);

USteamVRTrackingReferences::USteamVRTrackingReferences()
{
	PrimaryComponentTick.bCanEverTick = false;
}


bool USteamVRTrackingReferences::ShowTrackingReferences(UStaticMesh* TrackingReferenceMesh)
{
	if (!TrackingReferenceMesh->IsValidLowLevel())
	{
		// TODO: Set default mesh to SteamVR provided render model. Must be backwards compatible to UE4.15
		UE_LOG(LogSteamVRTrackingRefComponent, Error, TEXT("[TRACKING REFERENCE] No reference mesh found/defined!"));
		return false;
	}

	if (VRSystem() && VRCompositor())
	{
		// Remove any existing tracking references in-world
		HideTrackingReferences();

		// Owner must be the player pawn 
		FVector HMDOffset = GetOwner()->GetActorLocation();
		// UE_LOG(LogSteamVRTrackingRefComponent, Warning, TEXT("[TRACKING REF] HMD Offset recorded as: %s"), *HMDOffset.ToString());

		// Find all SteamVR Tracking References
		for (unsigned int id = 0; id < k_unMaxTrackedDeviceCount; ++id)
		{
			ETrackedDeviceClass trackedDeviceClass = VRSystem()->GetTrackedDeviceClass(id);

			if (trackedDeviceClass == ETrackedDeviceClass::TrackedDeviceClass_TrackingReference)
			{
				// Extraneous call, used for Debugging
				//char buf[k_unMaxPropertyStringSize];
				//uint32 StringBytes = VRSystem()->GetStringTrackedDeviceProperty(id, ETrackedDeviceProperty::Prop_ModelNumber_String, buf, sizeof(buf));
				//FString stringCache = *FString(UTF8_TO_TCHAR(buf));
				//UE_LOG(LogSteamVRTrackingRefComponent, Warning, TEXT("[TRACKING REFERENCE] Found the following tracking device: [%i] %s"), id, *stringCache);

				TrackedDevicePose_t TrackedDevicePose = { 0 };
				VRCompositor()->GetLastPoseForTrackedDeviceIndex(id, &TrackedDevicePose, NULL);

				// Get SteamVR Transform Matrix for this tracking reference
				HmdMatrix34_t Matrix = TrackedDevicePose.mDeviceToAbsoluteTracking;

				// Transform SteamVR Pose to Unreal Pose
				FMatrix Pose = FMatrix(
					FPlane(Matrix.m[0][0], Matrix.m[1][0], Matrix.m[2][0], 0.0f),
					FPlane(Matrix.m[0][1], Matrix.m[1][1], Matrix.m[2][1], 0.0f),
					FPlane(Matrix.m[0][2], Matrix.m[1][2], Matrix.m[2][2], 0.0f),
					FPlane(Matrix.m[0][3], Matrix.m[1][3], Matrix.m[2][3], 1.0f)
				);

				// Transform SteamVR Rotation Quaternion to a UE FRotator
				FQuat OrientationQuat;
				FQuat OrientationPose(Pose);
				OrientationQuat.X = -OrientationPose.Z;
				OrientationQuat.Y = OrientationPose.X;
				OrientationQuat.Z = OrientationPose.Y;
				OrientationQuat.W = -OrientationPose.W;

				FVector PositionPose = ((FVector(-Pose.M[3][2], Pose.M[3][0], Pose.M[3][1])) * 100.f + HMDOffset);
				FVector Position = PositionPose;

				//OutOrientation = BaseOrientation.Inverse() * OutOrientation;
				OrientationQuat.Normalize();
				FRotator Orientation = OrientationQuat.Rotator();
				Orientation.Normalize();

				// Create new static mesh component and attach to actor
				UStaticMeshComponent* TrackingRefStaticMeshComponent;
				TrackingRefStaticMeshComponent = NewObject<UStaticMeshComponent>(GetOwner());
				TrackingRefStaticMeshComponent->RegisterComponentWithWorld(GetWorld());
				TrackingRefStaticMeshComponent->SetSimulatePhysics(false);
				TrackingRefStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				TrackingRefStaticMeshComponent->SetWorldTransform(FTransform(Orientation, Position, TrackingReferenceScale));
				TrackingRefStaticMeshComponent->SetMobility(EComponentMobility::Movable);
				TrackingRefStaticMeshComponent->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

				//Set Mesh
				TrackingRefStaticMeshComponent->SetStaticMesh(TrackingReferenceMesh);
				TrackingRefStaticMeshComponent->SetVisibility(true);

				// Add to list of in-world tracking references 
				TrackingReferences.Add(TrackingRefStaticMeshComponent);
			}
		}
	}

	return false;
}

void USteamVRTrackingReferences::HideTrackingReferences()
{
	// Check if need to clear up any existing Tracking Reference in-world
	if (TrackingReferences.Num()> 0)
	{
		// Manually destroy each held tracking reference in array
		for (UStaticMeshComponent* TrackingReference : TrackingReferences)
		{
			if (TrackingReference->IsValidLowLevel())
			{
				TrackingReference->DestroyComponent();
			}
		}

		// Clear up array
		TrackingReferences.Empty();
	}
}

void USteamVRTrackingReferences::BeginPlay()
{
	Super::BeginPlay();
}
