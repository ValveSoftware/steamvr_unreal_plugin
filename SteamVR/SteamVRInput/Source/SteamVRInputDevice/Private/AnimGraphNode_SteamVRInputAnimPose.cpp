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