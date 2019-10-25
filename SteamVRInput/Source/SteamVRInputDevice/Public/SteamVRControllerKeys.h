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

namespace GenericKeys
{
	const FKey SteamVR_MotionController_None("SteamVR_MotionController_None");
	const FKey SteamVR_HMD_Proximity("SteamVR_HMD_Proximity");
}

namespace IndexControllerKeys
{
	// Button A
	const FKey SteamVR_Valve_Index_Controller_A_Press_Left("SteamVR_Valve_Index_Controller_A_Press_Left");
	const FKey SteamVR_Valve_Index_Controller_A_Press_Right("SteamVR_Valve_Index_Controller_A_Press_Right");
	const FKey SteamVR_Valve_Index_Controller_A_Touch_Left("SteamVR_Valve_Index_Controller_A_Touch_Left");
	const FKey SteamVR_Valve_Index_Controller_A_Touch_Right("SteamVR_Valve_Index_Controller_A_Touch_Right");

	// Button B
	const FKey SteamVR_Valve_Index_Controller_B_Press_Left("SteamVR_Valve_Index_Controller_B_Press_Left");
	const FKey SteamVR_Valve_Index_Controller_B_Touch_Left("SteamVR_Valve_Index_Controller_B_Touch_Left");
	const FKey SteamVR_Valve_Index_Controller_B_Press_Right("SteamVR_Valve_Index_Controller_B_Press_Right");
	const FKey SteamVR_Valve_Index_Controller_B_Touch_Right("SteamVR_Valve_Index_Controller_B_Touch_Right");

	// Thumbstick
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_Press_Left("SteamVR_Valve_Index_Controller_Thumbstick_Press_Left");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_Touch_Left("SteamVR_Valve_Index_Controller_Thumbstick_Touch_Left");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_Press_Right("SteamVR_Valve_Index_Controller_Thumbstick_Press_Right");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_Touch_Right("SteamVR_Valve_Index_Controller_Thumbstick_Touch_Right");

	const FKey SteamVR_Valve_Index_Controller_Thumbstick_X_Left("SteamVR_Valve_Index_Controller_Thumbstick_X_Left");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_Y_Left("SteamVR_Valve_Index_Controller_Thumbstick_Y_Left");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_X_Right("SteamVR_Valve_Index_Controller_Thumbstick_X_Right");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_Y_Right("SteamVR_Valve_Index_Controller_Thumbstick_Y_Right");

	// Thumbstick Directions
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_Up_Left("SteamVR_Valve_Index_Controller_Thumbstick_Up_Left");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_Down_Left("SteamVR_Valve_Index_Controller_Thumbstick_Down_Left");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_L_Left("SteamVR_Valve_Index_Controller_Thumbstick_L_Left");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_R_Left("SteamVR_Valve_Index_Controller_Thumbstick_R_Left");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_Up_Right("SteamVR_Valve_Index_Controller_Thumbstick_Up_Right");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_Down_Right("SteamVR_Valve_Index_Controller_Thumbstick_Down_Right");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_L_Right("SteamVR_Valve_Index_Controller_Thumbstick_L_Right");
	const FKey SteamVR_Valve_Index_Controller_Thumbstick_R_Right("SteamVR_Valve_Index_Controller_Thumbstick_R_Right");

	// Grip
	const FKey SteamVR_Valve_Index_Controller_Grip_Press_Left("SteamVR_Valve_Index_Controller_Grip_Press_Left");
	const FKey SteamVR_Valve_Index_Controller_Grip_Touch_Left("SteamVR_Valve_Index_Controller_Grip_Touch_Left");
	const FKey SteamVR_Valve_Index_Controller_Grip_CapSense_Left("SteamVR_Valve_Index_Controller_Grip_CapSense_Left");
	const FKey SteamVR_Valve_Index_Controller_GripForce_Axis_Left("SteamVR_Valve_Index_Controller_GripForce_Axis_Left");
	const FKey SteamVR_Valve_Index_Controller_Grip_Press_Right("SteamVR_Valve_Index_Controller_Grip_Press_Right");
	const FKey SteamVR_Valve_Index_Controller_Grip_Touch_Right("SteamVR_Valve_Index_Controller_Grip_Touch_Right");
	const FKey SteamVR_Valve_Index_Controller_Grip_CapSense_Right("SteamVR_Valve_Index_Controller_Grip_CapSense_Right");
	const FKey SteamVR_Valve_Index_Controller_GripForce_Axis_Right("SteamVR_Valve_Index_Controller_GripForce_Axis_Right");

	// Trigger
	const FKey SteamVR_Valve_Index_Controller_Trigger_Click_Left("SteamVR_Valve_Index_Controller_Trigger_Click_Left");
	const FKey SteamVR_Valve_Index_Controller_Trigger_Press_Left("SteamVR_Valve_Index_Controller_Trigger_Press_Left");
	const FKey SteamVR_Valve_Index_Controller_Trigger_Touch_Left("SteamVR_Valve_Index_Controller_Trigger_Touch_Left");
	const FKey SteamVR_Valve_Index_Controller_Trigger_Pull_Left("SteamVR_Valve_Index_Controller_Trigger_Pull_Left");
	const FKey SteamVR_Valve_Index_Controller_Trigger_Click_Right("SteamVR_Valve_Index_Controller_Trigger_Click_Right");
	const FKey SteamVR_Valve_Index_Controller_Trigger_Press_Right("SteamVR_Valve_Index_Controller_Trigger_Press_Right");
	const FKey SteamVR_Valve_Index_Controller_Trigger_Touch_Right("SteamVR_Valve_Index_Controller_Trigger_Touch_Right");
	const FKey SteamVR_Valve_Index_Controller_Trigger_Pull_Right("SteamVR_Valve_Index_Controller_Trigger_Pull_Right");

	// Trackpad
	const FKey SteamVR_Valve_Index_Controller_Trackpad_Press_Left("SteamVR_Valve_Index_Controller_Trackpad_Press_Left");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_Touch_Left("SteamVR_Valve_Index_Controller_Trackpad_Touch_Left");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_X_Left("SteamVR_Valve_Index_Controller_Trackpad_X_Left");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_Y_Left("SteamVR_Valve_Index_Controller_Trackpad_Y_Left");
	const FKey SteamVR_Valve_Index_Controller_TrackpadForce_Axis_Left("SteamVR_Valve_Index_Controller_TrackpadForce_Axis_Left");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_Press_Right("SteamVR_Valve_Index_Controller_Trackpad_Press_Right");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_Touch_Right("SteamVR_Valve_Index_Controller_Trackpad_Touch_Right");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_X_Right("SteamVR_Valve_Index_Controller_Trackpad_X_Right");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_Y_Right("SteamVR_Valve_Index_Controller_Trackpad_Y_Right");
	const FKey SteamVR_Valve_Index_Controller_TrackpadForce_Axis_Right("SteamVR_Valve_Index_Controller_TrackpadForce_Axis_Right");

	// Trackpad Directions
	const FKey SteamVR_Valve_Index_Controller_Trackpad_Up_Left("SteamVR_Valve_Index_Controller_Trackpad_Up_Left");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_Down_Left("SteamVR_Valve_Index_Controller_Trackpad_Down_Left");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_L_Left("SteamVR_Valve_Index_Controller_Trackpad_L_Left");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_R_Left("SteamVR_Valve_Index_Controller_Trackpad_R_Left");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_Up_Right("SteamVR_Valve_Index_Controller_Trackpad_Up_Right");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_Down_Right("SteamVR_Valve_Index_Controller_Trackpad_Down_Right");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_L_Right("SteamVR_Valve_Index_Controller_Trackpad_L_Right");
	const FKey SteamVR_Valve_Index_Controller_Trackpad_R_Right("SteamVR_Valve_Index_Controller_Trackpad_R_Right");

	// Special
	const FKey SteamVR_Valve_Index_Controller_Grip_Grab_Left("SteamVR_Valve_Index_Controller_Grip_Grab_Left");
	const FKey SteamVR_Valve_Index_Controller_Pinch_Grab_Left("SteamVR_Valve_Index_Controller_Pinch_Grab_Left");
	const FKey SteamVR_Valve_Index_Controller_Grip_Grab_Right("SteamVR_Valve_Index_Controller_Grip_Grab_Right");
	const FKey SteamVR_Valve_Index_Controller_Pinch_Grab_Right("SteamVR_Valve_Index_Controller_Pinch_Grab_Right");
}

namespace ViveControllerKeys
{
	// Application
	const FKey SteamVR_Vive_Controller_Application_Press_Left("SteamVR_Vive_Controller_Application_Press_Left");
	const FKey SteamVR_Vive_Controller_Application_Press_Right("SteamVR_Vive_Controller_Application_Press_Right");

	// Grip
	const FKey SteamVR_Vive_Controller_Grip_Press_Left("SteamVR_Vive_Controller_Grip_Press_Left");
	const FKey SteamVR_Vive_Controller_Grip_Press_Right("SteamVR_Vive_Controller_Grip_Press_Right");

	// Trigger
	const FKey SteamVR_Vive_Controller_Trigger_Click_Left("SteamVR_Vive_Controller_Trigger_Click_Left");
	const FKey SteamVR_Vive_Controller_Trigger_Press_Left("SteamVR_Vive_Controller_Trigger_Press_Left");
	const FKey SteamVR_Vive_Controller_Trigger_Pull_Left("SteamVR_Vive_Controller_Trigger_Pull_Left");

	const FKey SteamVR_Vive_Controller_Trigger_Click_Right("SteamVR_Vive_Controller_Trigger_Click_Right");
	const FKey SteamVR_Vive_Controller_Trigger_Press_Right("SteamVR_Vive_Controller_Trigger_Press_Right");
	const FKey SteamVR_Vive_Controller_Trigger_Pull_Right("SteamVR_Vive_Controller_Trigger_Pull_Right");

	// Trackpad
	const FKey SteamVR_Vive_Controller_Trackpad_Press_Left("SteamVR_Vive_Controller_Trackpad_Press_Left");
	const FKey SteamVR_Vive_Controller_Trackpad_Touch_Left("SteamVR_Vive_Controller_Trackpad_Touch_Left");
	const FKey SteamVR_Vive_Controller_Trackpad_X_Left("SteamVR_Vive_Controller_Trackpad_X_Left");
	const FKey SteamVR_Vive_Controller_Trackpad_Y_Left("SteamVR_Vive_Controller_Trackpad_Y_Left");

	const FKey SteamVR_Vive_Controller_Trackpad_Press_Right("SteamVR_Vive_Controller_Trackpad_Press_Right");
	const FKey SteamVR_Vive_Controller_Trackpad_Touch_Right("SteamVR_Vive_Controller_Trackpad_Touch_Right");
	const FKey SteamVR_Vive_Controller_Trackpad_X_Right("SteamVR_Vive_Controller_Trackpad_X_Right");
	const FKey SteamVR_Vive_Controller_Trackpad_Y_Right("SteamVR_Vive_Controller_Trackpad_Y_Right");

	// Trackpad Directions
	const FKey SteamVR_Vive_Controller_Trackpad_Up_Left("SteamVR_Vive_Controller_Trackpad_Up_Left");
	const FKey SteamVR_Vive_Controller_Trackpad_Down_Left("SteamVR_Vive_Controller_Trackpad_Down_Left");
	const FKey SteamVR_Vive_Controller_Trackpad_L_Left("SteamVR_Vive_Controller_Trackpad_L_Left");
	const FKey SteamVR_Vive_Controller_Trackpad_R_Left("SteamVR_Vive_Controller_Trackpad_R_Left");
	const FKey SteamVR_Vive_Controller_Trackpad_Up_Right("SteamVR_Vive_Controller_Trackpad_Up_Right");
	const FKey SteamVR_Vive_Controller_Trackpad_Down_Right("SteamVR_Vive_Controller_Trackpad_Down_Right");
	const FKey SteamVR_Vive_Controller_Trackpad_L_Right("SteamVR_Vive_Controller_Trackpad_L_Right");
	const FKey SteamVR_Vive_Controller_Trackpad_R_Right("SteamVR_Vive_Controller_Trackpad_R_Right");
}

namespace CosmosControllerKeys
{
	// Application
	const FKey SteamVR_HTC_Cosmos_Application_Press_Left("SteamVR_HTC_Cosmos_Application_Press_Left");
	const FKey SteamVR_HTC_Cosmos_Application_Press_Right("SteamVR_HTC_Cosmos_Application_Press_Right");

	// Button A
	const FKey SteamVR_HTC_Cosmos_A_Press("SteamVR_HTC_Cosmos_A_Press");
	const FKey SteamVR_HTC_Cosmos_A_Touch("SteamVR_HTC_Cosmos_A_Touch");

	// Button B
	const FKey SteamVR_HTC_Cosmos_B_Press("SteamVR_HTC_Cosmos_B_Press");
	const FKey SteamVR_HTC_Cosmos_B_Touch("SteamVR_HTC_Cosmos_B_Touch");

	// Button X
	const FKey SteamVR_HTC_Cosmos_X_Press("SteamVR_HTC_Cosmos_X_Press");
	const FKey SteamVR_HTC_Cosmos_X_Touch("SteamVR_HTC_Cosmos_X_Touch");

	// Button Y
	const FKey SteamVR_HTC_Cosmos_Y_Press("SteamVR_HTC_Cosmos_Y_Press");
	const FKey SteamVR_HTC_Cosmos_Y_Touch("SteamVR_HTC_Cosmos_Y_Touch");

	// Grip
	const FKey SteamVR_HTC_Cosmos_Grip_Click_Left("SteamVR_HTC_Cosmos_Grip_Click_Left");
	const FKey SteamVR_HTC_Cosmos_Grip_Click_Right("SteamVR_HTC_Cosmos_Grip_Click_Right");

	// @HTC to finalize
	//const FKey SteamVR_HTC_Cosmos_Grip_Press_Left("SteamVR_HTC_Cosmos_Grip_Press_Left");
	//const FKey SteamVR_HTC_Cosmos_Grip_Press_Right("SteamVR_HTC_Cosmos_Grip_Press_Right");

	//const FKey SteamVR_HTC_Cosmos_Grip_Touch_Left("SteamVR_HTC_Cosmos_Grip_Touch_Left");
	//const FKey SteamVR_HTC_Cosmos_Grip_Touch_Right("SteamVR_HTC_Cosmos_Grip_Touch_Right");

	const FKey SteamVR_HTC_Cosmos_Grip_Pull_Left("SteamVR_HTC_Cosmos_Grip_Pull_Left");
	const FKey SteamVR_HTC_Cosmos_Grip_Pull_Right("SteamVR_HTC_Cosmos_Grip_Pull_Right");

	// Bumper (L1/R1)
	const FKey SteamVR_HTC_Cosmos_Bumper_Click_Left("SteamVR_HTC_Cosmos_Bumper_Click_Left");
	const FKey SteamVR_HTC_Cosmos_Bumper_Click_Right("SteamVR_HTC_Cosmos_Bumper_Click_Right");

	// Trigger (L2/R2)
	const FKey SteamVR_HTC_Cosmos_Trigger_Click_Left("SteamVR_HTC_Cosmos_Trigger_Click_Left");
	const FKey SteamVR_HTC_Cosmos_Trigger_Press_Left("SteamVR_HTC_Cosmos_Trigger_Press_Left");
	const FKey SteamVR_HTC_Cosmos_Trigger_Touch_Left("SteamVR_HTC_Cosmos_Trigger_Touch_Left");
	const FKey SteamVR_HTC_Cosmos_Trigger_Pull_Left("SteamVR_HTC_Cosmos_Trigger_Pull_Left");

	const FKey SteamVR_HTC_Cosmos_Trigger_Click_Right("SteamVR_HTC_Cosmos_Trigger_Click_Right");
	const FKey SteamVR_HTC_Cosmos_Trigger_Press_Right("SteamVR_HTC_Cosmos_Trigger_Press_Right");
	const FKey SteamVR_HTC_Cosmos_Trigger_Touch_Right("SteamVR_HTC_Cosmos_Trigger_Touch_Right");
	const FKey SteamVR_HTC_Cosmos_Trigger_Pull_Right("SteamVR_HTC_Cosmos_Trigger_Pull_Right");

	// Joystick
	const FKey SteamVR_HTC_Cosmos_Joystick_Press_Left("SteamVR_HTC_Cosmos_Joystick_Press_Left");
	const FKey SteamVR_HTC_Cosmos_Joystick_Touch_Left("SteamVR_HTC_Cosmos_Joystick_Touch_Left");
	const FKey SteamVR_HTC_Cosmos_Joystick_X_Left("SteamVR_HTC_Cosmos_Joystick_X_Left");
	const FKey SteamVR_HTC_Cosmos_Joystick_Y_Left("SteamVR_HTC_Cosmos_Joystick_Y_Left");

	const FKey SteamVR_HTC_Cosmos_Joystick_Press_Right("SteamVR_HTC_Cosmos_Joystick_Press_Right");
	const FKey SteamVR_HTC_Cosmos_Joystick_Touch_Right("SteamVR_HTC_Cosmos_Joystick_Touch_Right");
	const FKey SteamVR_HTC_Cosmos_Joystick_X_Right("SteamVR_HTC_Cosmos_Joystick_X_Right");
	const FKey SteamVR_HTC_Cosmos_Joystick_Y_Right("SteamVR_HTC_Cosmos_Joystick_Y_Right");

	// Joystick Directions
	const FKey SteamVR_HTC_Cosmos_Joystick_Up_Left("SteamVR_HTC_Cosmos_Joystick_Up_Left");
	const FKey SteamVR_HTC_Cosmos_Joystick_Down_Left("SteamVR_HTC_Cosmos_Joystick_Down_Left");
	const FKey SteamVR_HTC_Cosmos_Joystick_L_Left("SteamVR_HTC_Cosmos_Joystick_L_Left");
	const FKey SteamVR_HTC_Cosmos_Joystick_R_Left("SteamVR_HTC_Cosmos_Joystick_R_Left");
	const FKey SteamVR_HTC_Cosmos_Joystick_Up_Right("SteamVR_HTC_Cosmos_Joystick_Up_Right");
	const FKey SteamVR_HTC_Cosmos_Joystick_Down_Right("SteamVR_HTC_Cosmos_Joystick_Down_Right");
	const FKey SteamVR_HTC_Cosmos_Joystick_L_Right("SteamVR_HTC_Cosmos_Joystick_L_Right");
	const FKey SteamVR_HTC_Cosmos_Joystick_R_Right("SteamVR_HTC_Cosmos_Joystick_R_Right");

}

namespace OculusTouchKeys
{
	// Button A
	const FKey SteamVR_Oculus_Touch_A_Press("SteamVR_Oculus_Touch_A_Press");
	const FKey SteamVR_Oculus_Touch_A_Touch("SteamVR_Oculus_Touch_A_Touch");

	// Button B
	const FKey SteamVR_Oculus_Touch_B_Press("SteamVR_Oculus_Touch_B_Press");
	const FKey SteamVR_Oculus_Touch_B_Touch("SteamVR_Oculus_Touch_B_Touch");

	// Button X
	const FKey SteamVR_Oculus_Touch_X_Press("SteamVR_Oculus_Touch_X_Press");
	const FKey SteamVR_Oculus_Touch_X_Touch("SteamVR_Oculus_Touch_X_Touch");

	// Button Y
	const FKey SteamVR_Oculus_Touch_Y_Press("SteamVR_Oculus_Touch_Y_Press");
	const FKey SteamVR_Oculus_Touch_Y_Touch("SteamVR_Oculus_Touch_Y_Touch");

	// Joystick
	const FKey SteamVR_Oculus_Touch_Joystick_Press_Left("SteamVR_Oculus_Touch_Joystick_Press_Left");
	const FKey SteamVR_Oculus_Touch_Joystick_Touch_Left("SteamVR_Oculus_Touch_Joystick_Touch_Left");
	const FKey SteamVR_Oculus_Touch_Joystick_Press_Right("SteamVR_Oculus_Touch_Joystick_Press_Right");
	const FKey SteamVR_Oculus_Touch_Joystick_Touch_Right("SteamVR_Oculus_Touch_Joystick_Touch_Right");

	const FKey SteamVR_Oculus_Touch_Joystick_X_Left("SteamVR_Oculus_Touch_Joystick_X_Left");
	const FKey SteamVR_Oculus_Touch_Joystick_Y_Left("SteamVR_Oculus_Touch_Joystick_Y_Left");
	const FKey SteamVR_Oculus_Touch_Joystick_X_Right("SteamVR_Oculus_Touch_Joystick_X_Right");
	const FKey SteamVR_Oculus_Touch_Joystick_Y_Right("SteamVR_Oculus_Touch_Joystick_Y_Right");

	// Joystick Directions
	const FKey SteamVR_Oculus_Touch_Joystick_Up_Left("SteamVR_Oculus_Touch_Joystick_Up_Left");
	const FKey SteamVR_Oculus_Touch_Joystick_Down_Left("SteamVR_Oculus_Touch_Joystick_Down_Left");
	const FKey SteamVR_Oculus_Touch_Joystick_L_Left("SteamVR_Oculus_Touch_Joystick_L_Left");
	const FKey SteamVR_Oculus_Touch_Joystick_R_Left("SteamVR_Oculus_Touch_Joystick_R_Left");

	const FKey SteamVR_Oculus_Touch_Joystick_Up_Right("SteamVR_Oculus_Touch_Joystick_Up_Right");
	const FKey SteamVR_Oculus_Touch_Joystick_Down_Right("SteamVR_Oculus_Touch_Joystick_Down_Right");
	const FKey SteamVR_Oculus_Touch_Joystick_L_Right("SteamVR_Oculus_Touch_Joystick_L_Right");
	const FKey SteamVR_Oculus_Touch_Joystick_R_Right("SteamVR_Oculus_Touch_Joystick_R_Right");

	// Grip
	const FKey SteamVR_Oculus_Touch_Grip_Press_Left("SteamVR_Oculus_Touch_Grip_Press_Left");
	const FKey SteamVR_Oculus_Touch_Grip_Touch_Left("SteamVR_Oculus_Touch_Grip_Touch_Left");
	const FKey SteamVR_Oculus_Touch_Grip_Pull_Left("SteamVR_Oculus_Touch_Grip_Pull_Left");

	const FKey SteamVR_Oculus_Touch_Grip_Press_Right("SteamVR_Oculus_Touch_Grip_Press_Right");
	const FKey SteamVR_Oculus_Touch_Grip_Touch_Right("SteamVR_Oculus_Touch_Grip_Touch_Right");
	const FKey SteamVR_Oculus_Touch_Grip_Pull_Right("SteamVR_Oculus_Touch_Grip_Pull_Right");

	// Trigger
	const FKey SteamVR_Oculus_Touch_Trigger_Press_Left("SteamVR_Oculus_Touch_Trigger_Press_Left");
	const FKey SteamVR_Oculus_Touch_Trigger_Touch_Left("SteamVR_Oculus_Touch_Trigger_Touch_Left");
	const FKey SteamVR_Oculus_Touch_Trigger_Pull_Left("SteamVR_Oculus_Touch_Trigger_Pull_Left");

	const FKey SteamVR_Oculus_Touch_Trigger_Press_Right("SteamVR_Oculus_Touch_Trigger_Press_Right");
	const FKey SteamVR_Oculus_Touch_Trigger_Touch_Right("SteamVR_Oculus_Touch_Trigger_Touch_Right");
	const FKey SteamVR_Oculus_Touch_Trigger_Pull_Right("SteamVR_Oculus_Touch_Trigger_Pull_Right");
}

namespace WindowsMRKeys
{
	// Application
	const FKey SteamVR_Windows_MR_Controller_Application_Press_Left("SteamVR_Windows_MR_Controller_Application_Press_Left");
	const FKey SteamVR_Windows_MR_Controller_Application_Press_Right("SteamVR_Windows_MR_Controller_Application_Press_Right");

	// Joystick
	const FKey SteamVR_Windows_MR_Controller_Joystick_Press_Left("SteamVR_Windows_MR_Controller_Joystick_Press_Left");
	const FKey SteamVR_Windows_MR_Controller_Joystick_Press_Right("SteamVR_Windows_MR_Controller_Joystick_Press_Right");

	const FKey SteamVR_Windows_MR_Controller_Joystick_X_Left("SteamVR_Windows_MR_Controller_Joystick_X_Left");
	const FKey SteamVR_Windows_MR_Controller_Joystick_Y_Left("SteamVR_Windows_MR_Controller_Joystick_Y_Left");
	const FKey SteamVR_Windows_MR_Controller_Joystick_X_Right("SteamVR_Windows_MR_Controller_Joystick_X_Right");
	const FKey SteamVR_Windows_MR_Controller_Joystick_Y_Right("SteamVR_Windows_MR_Controller_Joystick_Y_Right");

	// Joystick Directions
	const FKey SteamVR_Windows_MR_Controller_Joystick_Up_Left("SteamVR_Windows_MR_Controller_Joystick_Up_Left");
	const FKey SteamVR_Windows_MR_Controller_Joystick_Down_Left("SteamVR_Windows_MR_Controller_Joystick_Down_Left");
	const FKey SteamVR_Windows_MR_Controller_Joystick_L_Left("SteamVR_Windows_MR_Controller_Joystick_L_Left");
	const FKey SteamVR_Windows_MR_Controller_Joystick_R_Left("SteamVR_Windows_MR_Controller_Joystick_R_Left");

	const FKey SteamVR_Windows_MR_Controller_Joystick_Up_Right("SteamVR_Windows_MR_Controller_Joystick_Up_Right");
	const FKey SteamVR_Windows_MR_Controller_Joystick_Down_Right("SteamVR_Windows_MR_Controller_Joystick_Down_Right");
	const FKey SteamVR_Windows_MR_Controller_Joystick_L_Right("SteamVR_Windows_MR_Controller_Joystick_L_Right");
	const FKey SteamVR_Windows_MR_Controller_Joystick_R_Right("SteamVR_Windows_MR_Controller_Joystick_R_Right");

	// Grip
	const FKey SteamVR_Windows_MR_Controller_Grip_Press_Left("SteamVR_Windows_MR_Controller_Grip_Press_Left");
	const FKey SteamVR_Windows_MR_Controller_Grip_Press_Right("SteamVR_Windows_MR_Controller_Grip_Press_Right");

	// Trigger
	const FKey SteamVR_Windows_MR_Controller_Trigger_Press_Left("SteamVR_Windows_MR_Controller_Trigger_Press_Left");
	const FKey SteamVR_Windows_MR_Controller_Trigger_Pull_Left("SteamVR_Windows_MR_Controller_Trigger_Pull_Left");

	const FKey SteamVR_Windows_MR_Controller_Trigger_Press_Right("SteamVR_Windows_MR_Controller_Trigger_Press_Right");
	const FKey SteamVR_Windows_MR_Controller_Trigger_Pull_Right("SteamVR_Windows_MR_Controller_Trigger_Pull_Right");

	// Trackpad
	const FKey SteamVR_Windows_MR_Controller_Trackpad_Press_Left("SteamVR_Windows_MR_Controller_Trackpad_Press_Left");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_Touch_Left("SteamVR_Windows_MR_Controller_Trackpad_Touch_Left");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_X_Left("SteamVR_Windows_MR_Controller_Trackpad_X_Left");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_Y_Left("SteamVR_Windows_MR_Controller_Trackpad_Y_Left");

	const FKey SteamVR_Windows_MR_Controller_Trackpad_Press_Right("SteamVR_Windows_MR_Controller_Trackpad_Press_Right");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_Touch_Right("SteamVR_Windows_MR_Controller_Trackpad_Touch_Right");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_X_Right("SteamVR_Windows_MR_Controller_Trackpad_X_Right");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_Y_Right("SteamVR_Windows_MR_Controller_Trackpad_Y_Right");

	// Trackpad Directions
	const FKey SteamVR_Windows_MR_Controller_Trackpad_Up_Left("SteamVR_Windows_MR_Controller_Trackpad_Up_Left");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_Down_Left("SteamVR_Windows_MR_Controller_Trackpad_Down_Left");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_Up_Right("SteamVR_Windows_MR_Controller_Trackpad_Up_Right");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_Down_Right("SteamVR_Windows_MR_Controller_Trackpad_Down_Right");

	const FKey SteamVR_Windows_MR_Controller_Trackpad_L_Left("SteamVR_Windows_MR_Controller_Trackpad_L_Left");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_R_Left("SteamVR_Windows_MR_Controller_Trackpad_R_Left");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_L_Right("SteamVR_Windows_MR_Controller_Trackpad_L_Right");
	const FKey SteamVR_Windows_MR_Controller_Trackpad_R_Right("SteamVR_Windows_MR_Controller_Trackpad_R_Right");
}
