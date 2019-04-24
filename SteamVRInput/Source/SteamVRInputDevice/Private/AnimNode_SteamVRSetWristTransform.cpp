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


#include "AnimNode_SteamVRSetWristTransform.h"
#include "ISteamVRInputDeviceModule.h"
#include "AnimationRuntime.h"
#include "Runtime/Engine/Public/Animation/AnimInstanceProxy.h"
#include "SteamVRInputDevice.h"
#include "UE4HandSkeletonDefinition.h"

FAnimNode_SteamVRSetWristTransform::FAnimNode_SteamVRSetWristTransform()
{
}

void FAnimNode_SteamVRSetWristTransform::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	// Initialize our poses
	ReferencePose.Initialize(Context);
	TargetPose.Initialize(Context);

}

void FAnimNode_SteamVRSetWristTransform::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{

}

void FAnimNode_SteamVRSetWristTransform::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	// Update our poses this frame
	ReferencePose.Update(Context);
	TargetPose.Update(Context);
}

void FAnimNode_SteamVRSetWristTransform::Evaluate_AnyThread(FPoseContext& Output)
{
	Output.ResetToRefPose();

	// Apply all the bones from the target to the output pose
	TargetPose.Evaluate(Output);

	// Setup buffer for the reference pose
	FPoseContext ReferenceContext = FPoseContext(Output.AnimInstanceProxy);
	ReferencePose.Evaluate(ReferenceContext);

	// Apply the appropriate root and/or wrist transforms to our output pose based on the skeleton being used
	if (HandSkeleton == EHandSkeleton::VR_SteamVRHandSkeleton)
	{
		// Since we are dealing with the SteamVR Hand Skeleton, we need to set both the root and wrist Bones
		Output.Pose[RootBoneIndex] = ReferenceContext.Pose[RootBoneIndex];
		Output.Pose[SteamVRWristBoneIndex] = ReferenceContext.Pose[SteamVRWristBoneIndex];
	}
	else
	{
		// For the UE4 stock hand skeleton, we only need the root bone (which is the wrist bone in this skeleton)
		Output.Pose[RootBoneIndex] = ReferenceContext.Pose[RootBoneIndex];
	}
}
