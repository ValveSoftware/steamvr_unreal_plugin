#include "AnimGraphNode_SteamVRInputAnimPose.h"
#include "SteamVRInputDevice.h"

#define LOCTEXT_NAMESPACE "SteamVRInputAnimNode"

UAnimGraphNode_SteamVRInputAnimPose::UAnimGraphNode_SteamVRInputAnimPose(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// Node Color
FLinearColor UAnimGraphNode_SteamVRInputAnimPose::GetNodeTitleColor() const 
{ 
	return FLinearColor(0.f, 0.f, 255.f, 1.f);
}

// Node Category
FText UAnimGraphNode_SteamVRInputAnimPose::GetMenuCategory() const
{
	return LOCTEXT("NodeCategory", "SteamVR Input");
}

// Node Title
FText UAnimGraphNode_SteamVRInputAnimPose::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle", "SteamVR Skeletal Anim Pose");
}

// Node Tooltip
FText UAnimGraphNode_SteamVRInputAnimPose::GetTooltipText() const
{
	return LOCTEXT("NodeTooltip", "Retrieves the current pose from the SteamVR Skeletal Input API");
}

#undef LOCTEXT_NAMESPACE