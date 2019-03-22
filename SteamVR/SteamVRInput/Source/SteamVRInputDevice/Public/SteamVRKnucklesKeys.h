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


#pragma once
#include "InputCoreTypes.h"

namespace KnucklesControllerKeyNames
{
	// Knuckles Finger Curls
	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_Index_Curl("Knuckles_Left_Finger_Index_Curl");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_Index_Curl("Knuckles_Right_Finger_Index_Curl");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_Middle_Curl("Knuckles_Left_Finger_Middle_Curl");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_Middle_Curl("Knuckles_Right_Finger_Middle_Curl");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_Ring_Curl("Knuckles_Left_Finger_Ring_Curl");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_Ring_Curl("Knuckles_Right_Finger_Ring_Curl");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_Pinky_Curl("Knuckles_Left_Finger_Pinky_Curl");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_Pinky_Curl("Knuckles_Right_Finger_Pinky_Curl");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_Thumb_Curl("Knuckles_Left_Finger_Thumb_Curl");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_Thumb_Curl("Knuckles_Right_Finger_Thumb_Curl");

	// Knuckles Finger Splays
	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_ThumbIndex_Splay("Knuckles_Left_Finger_ThumbIndex_Splay");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_ThumbIndex_Splay("Knuckles_Right_Finger_ThumbIndex_Splay");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_IndexMiddle_Splay("Knuckles_Left_Finger_IndexMiddle_Splay");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_IndexMiddle_Splay("Knuckles_Right_Finger_IndexMiddle_Splay");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_MiddleRing_Splay("Knuckles_Left_Finger_MiddleRing_Splay");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_MiddleRing_Splay("Knuckles_Right_Finger_MiddleRing_Splay");

	const FGamepadKeyNames::Type SteamVR_Knuckles_Left_Finger_RingPinky_Splay("Knuckles_Left_Finger_RingPinky_Splay");
	const FGamepadKeyNames::Type SteamVR_Knuckles_Right_Finger_RingPinky_Splay("Knuckles_Right_Finger_RingPinky_Splay");

}

namespace KnucklesControllerKeys
{
	// Knuckles CapSense
	const FKey SteamVR_Knuckles_Left_A_CapSense("Knuckles_Left_A_CapSense");
	const FKey SteamVR_Knuckles_Right_A_CapSense("Knuckles_Right_A_CapSense");
	const FKey SteamVR_Knuckles_Left_B_CapSense("Knuckles_Left_B_CapSense");
	const FKey SteamVR_Knuckles_Right_B_CapSense("Knuckles_Right_B_CapSense");

	const FKey SteamVR_Knuckles_Left_Trigger_CapSense("Knuckles_Left_Trigger_CapSense");
	const FKey SteamVR_Knuckles_Right_Trigger_CapSense("Knuckles_Right_Trigger_CapSense");

	const FKey SteamVR_Knuckles_Left_Thumbstick_CapSense("Knuckles_Left_Thumbstick_CapSense");
	const FKey SteamVR_Knuckles_Right_Thumbstick_CapSense("Knuckles_Right_Thumbstick_CapSense");

	const FKey SteamVR_Knuckles_Left_Trackpad_CapSense("Knuckles_Left_Trackpad_CapSense");
	const FKey SteamVR_Knuckles_Right_Trackpad_CapSense("Knuckles_Right_Trackpad_CapSense");

	const FKey SteamVR_Knuckles_Left_Trackpad_GripForce("Knuckles_Left_Trackpad_GripForce");
	const FKey SteamVR_Knuckles_Right_Trackpad_GripForce("Knuckles_Right_Trackpad_GripForce");

	// Knuckles Trackpad
	const FKey SteamVR_Knuckles_Left_Trackpad_X("Knuckles_Left_Trackpad_X");
	const FKey SteamVR_Knuckles_Right_Trackpad_X("Knuckles_Right_Trackpad_X");
	const FKey SteamVR_Knuckles_Left_Trackpad_Y("Knuckles_Left_Trackpad_Y");
	const FKey SteamVR_Knuckles_Right_Trackpad_Y("Knuckles_Right_Trackpad_Y");

	// Knuckles Curls
	const FKey SteamVR_Knuckles_Left_Finger_Index_Curl("Knuckles_Left_Finger_Index_Curl");
	const FKey SteamVR_Knuckles_Right_Finger_Index_Curl("Knuckles_Right_Finger_Index_Curl");

	const FKey SteamVR_Knuckles_Left_Finger_Middle_Curl("Knuckles_Left_Finger_Middle_Curl");
	const FKey SteamVR_Knuckles_Right_Finger_Middle_Curl("Knuckles_Right_Finger_Middle_Curl");

	const FKey SteamVR_Knuckles_Left_Finger_Ring_Curl("Knuckles_Left_Finger_Ring_Curl");
	const FKey SteamVR_Knuckles_Right_Finger_Ring_Curl("Knuckles_Right_Finger_Ring_Curl");

	const FKey SteamVR_Knuckles_Left_Finger_Pinky_Curl("Knuckles_Left_Finger_Pinky_Curl");
	const FKey SteamVR_Knuckles_Right_Finger_Pinky_Curl("Knuckles_Right_Finger_Pinky_Curl");

	const FKey SteamVR_Knuckles_Left_Finger_Thumb_Curl("Knuckles_Left_Finger_Thumb_Curl");
	const FKey SteamVR_Knuckles_Right_Finger_Thumb_Curl("Knuckles_Right_Finger_Thumb_Curl");

	// Knuckles Splays
	const FKey SteamVR_Knuckles_Left_Finger_ThumbIndex_Splay("Knuckles_Left_Finger_ThumbIndex_Splay");
	const FKey SteamVR_Knuckles_Right_Finger_ThumbIndex_Splay("Knuckles_Right_Finger_ThumbIndex_Splay");

	const FKey SteamVR_Knuckles_Left_Finger_IndexMiddle_Splay("Knuckles_Left_Finger_IndexMiddle_Splay");
	const FKey SteamVR_Knuckles_Right_Finger_IndexMiddle_Splay("Knuckles_Right_Finger_IndexMiddle_Splay");

	const FKey SteamVR_Knuckles_Left_Finger_MiddleRing_Splay("Knuckles_Left_Finger_MiddleRing_Splay");
	const FKey SteamVR_Knuckles_Right_Finger_MiddleRing_Splay("Knuckles_Right_Finger_MiddleRing_Splay");

	const FKey SteamVR_Knuckles_Left_Finger_RingPinky_Splay("Knuckles_Left_Finger_RingPinky_Splay");
	const FKey SteamVR_Knuckles_Right_Finger_RingPinky_Splay("Knuckles_Right_Finger_RingPinky_Splay");
}
