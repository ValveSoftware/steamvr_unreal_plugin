#include "AnimNode_SteamVRInputAnimPose.h"
#include "ISteamVRInputDeviceModule.h"
#include "AnimInstanceProxy.h"
#include "SteamVRInputDevice.h"

FAnimNode_SteamVRInputAnimPose::FAnimNode_SteamVRInputAnimPose()
	: SteamVRTransform()
{
}

void FAnimNode_SteamVRInputAnimPose::Initialize(const FAnimationInitializeContext& Context)
{
	// Initialize Base Pose
	BasePose.Initialize(Context);
}

void FAnimNode_SteamVRInputAnimPose::CacheBones(const FAnimationCacheBonesContext & Context)
{
	BasePose.CacheBones(Context);
}

void FAnimNode_SteamVRInputAnimPose::Update(const FAnimationUpdateContext & Context)
{
	EvaluateGraphExposedInputs.Execute(Context);

	// ...

	BasePose.Update(Context);
}

void FAnimNode_SteamVRInputAnimPose::Evaluate(FPoseContext& Output)
{
	BasePose.Evaluate(Output);
	
	// ...
}