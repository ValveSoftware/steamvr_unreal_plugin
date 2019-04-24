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
