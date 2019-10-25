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

#include "openvr.h"
#include "GameFramework/InputSettings.h"
#include "InputCoreTypes.h"

using namespace vr;

#define CONTROLLERS_PER_PLAYER			2
#define GENERIC_TRACKER_PLAYER_NUM		0
#define TOUCHPAD_AXIS					0
#define TRIGGER_AXIS					1
#define KNUCKLES_TOTAL_HAND_GRIP_AXIS	2
#define KNUCKLES_UPPER_HAND_GRIP_AXIS	3
#define KNUCKLES_LOWER_HAND_GRIP_AXIS	4
#define STEAMVR_SKELETON_BONE_COUNT		31
#define DOT_45DEG						0.707f
#define TOUCHPAD_DEADZONE				0.0f

// Manifest constants
#define MAX_ACTION_SETS					25
#define CONTROLLER_BINDING_PATH			"SteamVRBindings"
#define ACTION_MANIFEST					"steamvr_manifest.json"
#define ACTION_MANIFEST_UE				"steamvr_actions.json"
#define APP_MANIFEST_FILE				"steamvr_ue_editor_app.json"
#define APP_MANIFEST_PREFIX				"application.generated.ue."

// Action paths
#define ACTION_SET						"/actions/main"
#define ACTION_PATH_IN					"/actions/main/in"
#define ACTION_PATH_CONTROLLER_LEFT		"/actions/main/in/controllerleft"
#define ACTION_PATH_CONTROLLER_RIGHT	"/actions/main/in/controllerright"
#define ACTION_PATH_SPECIAL_BACK_L		"/actions/main/in/special1"
#define ACTION_PATH_SPECIAL_BACK_R		"/actions/main/in/special2"
#define ACTION_PATH_SPECIAL_FRONT_L		"/actions/main/in/special3"
#define ACTION_PATH_SPECIAL_FRONT_R		"/actions/main/in/special4"
#define ACTION_PATH_SPECIAL_FRONTR_L	"/actions/main/in/special5"
#define ACTION_PATH_SPECIAL_FRONTR_R	"/actions/main/in/special6"
#define ACTION_PATH_SPECIAL_PISTOL_L	"/actions/main/in/special7"
#define ACTION_PATH_SPECIAL_PISTOL_R	"/actions/main/in/special8"
#define ACTION_PATH_SKELETON_LEFT		"/actions/main/in/skeletonleft"		
#define ACTION_PATH_SKELETON_RIGHT		"/actions/main/in/skeletonright"
#define ACTION_PATH_SKEL_HAND_LEFT		"/skeleton/hand/left"
#define ACTION_PATH_SKEL_HAND_RIGHT		"/skeleton/hand/right"
#define ACTION_PATH_OPEN_CONSOLE		"/actions/main/in/open_console"
#define ACTION_PATH_VIBRATE_LEFT		"/actions/main/out/vibrateleft"
#define ACTION_PATH_VIBRATE_RIGHT		"/actions/main/out/vibrateright"

// Input paths
#define ACTION_PATH_HEAD_PROXIMITY		"/user/head/proximity"
#define ACTION_PATH_CONT_RAW_LEFT		"/user/hand/left/pose/raw"
#define ACTION_PATH_CONT_RAW_RIGHT		"/user/hand/right/pose/raw"
#define ACTION_PATH_SPCL_BACK_LEFT		"/user/hand/left/pose/back"					// Special 1
#define ACTION_PATH_SPCL_BACK_RIGHT		"/user/hand/right/pose/back"				// Special 2
#define ACTION_PATH_SPCL_FRONT_LEFT		"/user/hand/left/pose/front"				// Special 3
#define ACTION_PATH_SPCL_FRONT_RIGHT	"/user/hand/right/pose/front"				// Special 4
#define ACTION_PATH_SPCL_FRONTR_LEFT	"/user/hand/left/pose/frontandrolled"		// Special 5
#define ACTION_PATH_SPCL_FRONTR_RIGHT	"/user/hand/right/pose/frontandrolled"		// Special 6
#define ACTION_PATH_SPCL_PISTOL_LEFT	"/user/hand/left/pose/pistolgrip"			// Special 7
#define ACTION_PATH_SPCL_PISTOL_RIGHT	"/user/hand/right/pose/pistolgrip"			// Special 8
#define ACTION_PATH_TRIGGER_LEFT		"/user/hand/left/input/trigger"
#define ACTION_PATH_TRIGGER_RIGHT		"/user/hand/right/input/trigger"
#define ACTION_PATH_BUMPER_LEFT			"/user/hand/left/input/bumper"
#define ACTION_PATH_BUMPER_RIGHT		"/user/hand/right/input/bumper"
#define ACTION_PATH_THUMBSTICK_LEFT		"/user/hand/left/input/thumbstick"
#define ACTION_PATH_THUMBSTICK_RIGHT	"/user/hand/right/input/thumbstick"
#define ACTION_PATH_TRACKPAD_LEFT		"/user/hand/left/input/trackpad"
#define ACTION_PATH_TRACKPAD_RIGHT		"/user/hand/right/input/trackpad"
#define ACTION_PATH_JOYSTICK_LEFT		"/user/hand/left/input/joystick"
#define ACTION_PATH_JOYSTICK_RIGHT		"/user/hand/right/input/joystick"
#define ACTION_PATH_GRIP_LEFT			"/user/hand/left/input/grip"
#define ACTION_PATH_GRIP_RIGHT			"/user/hand/right/input/grip"
#define ACTION_PATH_BTN_A_LEFT			"/user/hand/left/input/a"
#define ACTION_PATH_BTN_A_RIGHT			"/user/hand/right/input/a"
#define ACTION_PATH_BTN_B_LEFT			"/user/hand/left/input/b"
#define ACTION_PATH_BTN_B_RIGHT			"/user/hand/right/input/b"
#define ACTION_PATH_BTN_X_LEFT			"/user/hand/left/input/x"
#define ACTION_PATH_BTN_Y_LEFT			"/user/hand/left/input/y"
#define ACTION_PATH_USER_SKEL_LEFT		"/user/hand/left/input/skeleton/left"
#define ACTION_PATH_USER_SKEL_RIGHT		"/user/hand/right/input/skeleton/right"
#define ACTION_PATH_USER_VIB_LEFT		"/user/hand/left/output/haptic"
#define ACTION_PATH_USER_VIB_RIGHT		"/user/hand/right/output/haptic"
#define ACTION_PATH_PINCH_GRAB_LEFT		"/user/hand/left/input/pinch"
#define ACTION_PATH_PINCH_GRAB_RIGHT	"/user/hand/right/input/pinch"
#define ACTION_PATH_GRIP_GRAB_LEFT		"/user/hand/left/input/grip"
#define ACTION_PATH_GRIP_GRAB_RIGHT		"/user/hand/right/input/grip"
#define ACTION_PATH_APPMENU_LEFT        "/user/hand/left/input/application_menu"
#define ACTION_PATH_APPMENU_RIGHT        "/user/hand/right/input/application_menu"


namespace SteamVRInputDeviceConstants 
{
	/* The maximum number of Unreal controllers is 8, two motion controllers per Unreal controller */
	static const int32 MaxUnrealControllers = 4;

	/* Total number of supported tracked devices */
	static const int32 MaxControllers = k_unMaxTrackedDeviceCount;

	/* The maximum number of "Special" hand designations */
	static const int32 MaxSpecialDesignations = ((int32)EControllerHand::Special_9 - (int32)EControllerHand::Special_1) + 1;
}

/* Buttons available on a SteamVR controller */
struct ESteamVRInputButton
{
	enum Type
	{
		System = 0,
		ApplicationMenu = 1,
		Grip = 2,
		DPadLeft = 3,
		DPadUp = 4,
		DPadRight = 5,
		DPadDown = 6,
		ButtonA = 7,

		ProximitySensor = 31,

		Axis0 = 32,
		Axis1 = 33,
		Axis2 = 34,
		Axis3 = 35,
		Axis4 = 36,

		Touchpad = Axis0,
		Trigger =Axis1,
		DashboardBack =Grip,
		KnucklesA =Grip,
		KnucklesB =ApplicationMenu,
		KnucklesJoyStick =Axis3,
		TotalButtonCount = 64
	};
};

/* Available SteamVR Input Action Types */
enum EActionType
{
	Boolean,
	Vector1,
	Vector2,
	Vector3,
	Vibration,
	Pose,
	Skeleton,
	Invalid
};

struct FSteamVRAxisKeyMapping 
{
	FInputAxisKeyMapping InputAxisKeyMapping;
	bool bIsPartofVector2;
	bool bIsPartofVector3;
	FName XAxisName;
	FName YAxisName;
	FName ZAxisName;
	FName XAxisKey;
	FName YAxisKey;
	FName ZAxisKey;
	FString ActionName;
	FString ActionNameWithPath;
	FString ControllerName;

	FSteamVRAxisKeyMapping(FInputAxisKeyMapping& inInputAxisKeyMapping,  bool inIsPartofVector2, bool inIsPartofVector3)
		: InputAxisKeyMapping(inInputAxisKeyMapping)
		, bIsPartofVector2(inIsPartofVector2)
		, bIsPartofVector3(inIsPartofVector3)
	{
		XAxisName = NAME_None;
		YAxisName = NAME_None;
		ZAxisName = NAME_None;
		XAxisKey = NAME_None;
		YAxisKey = NAME_None;
		ZAxisKey = NAME_None;
		ActionName = "";
		ControllerName = "";
	}
};

struct FSteamVRInputKeyMapping
{
	FInputActionKeyMapping InputKeyMapping;
	FString ActionName;
	FString ActionNameWithPath;
	FString ControllerName;

	FSteamVRInputKeyMapping(FInputActionKeyMapping& inInputKeyMapping)
		: InputKeyMapping(inInputKeyMapping)
	{
		ActionName = "";
		ControllerName = "";
	}
};

const FString SActionTypes[] = {
			TEXT("boolean"),
			TEXT("vector1"),
			TEXT("vector2"),
			TEXT("vector3"),
			TEXT("vibration"),
			TEXT("pose"),
			TEXT("skeleton"),
			TEXT("")
};

struct FControllerType
{
	bool	bIsGenerated;
	FName	Name;
	FString	Description;
	FString KeyEquivalent;
	bool bIsActive;

	FControllerType() {}
	FControllerType(const FName& inName, const FString& inDescription, const FString& inKeyEquivalent)
		: bIsGenerated(false)
		, Name(inName)
		, Description(inDescription)
		, KeyEquivalent(inKeyEquivalent)
	{
		bIsActive = false;
	}
};

struct FActionPath
{
	FString	Path;

	FActionPath() {}
	FActionPath(const FString& inPath)
		: Path(inPath)
	{}
};

struct FActionSource
{
	FName			Mode;
	FString			Path;

	FActionSource() {}
	FActionSource(const FName& inMode, const FString& inPath)
		: Mode(inMode),
		Path(inPath)
	{}
};

struct FInputMapping
{
	FName	InputKey;			// The UE Key (e.g.Motion_Controller_Thumbstick_X)
	TArray<FString> Actions;	// Collection of SteamVR actions (e.g. Teleport, OpenConsole)

	FInputMapping() {}
};

struct FSteamVRInputActionSet
{
	int32		Priority;					// The priority of this action set relative to other action sets.	
	FName		Name;						// The name of the action set
	FString		RestrictedToDevicePath;		// Device path that this action set is active for
	FString		SecondaryActionSetPath;		// The path for the secondary action set

	VRActionSetHandle_t Handle;						// This action set's handle
	VRInputValueHandle_t RestrictedToDeviceHandle;	// Handle of a device path that this action set should be active for.  Use k_ulInvalidInputValueHandle to activate for all devices.
	VRActionSetHandle_t SecondaryActionSetHandle;	// Secondary action set handle, if RestrictedToDeviceHandle is k_ulInvalidInputValueHandle, this is ignored

	FSteamVRInputActionSet()
	{}

	FSteamVRInputActionSet(int32 InPriority, FName InName, VRActionSetHandle_t InHandle)
		: Priority (InPriority)
		, Name (InName)
		, Handle (InHandle)
	{
		RestrictedToDeviceHandle = k_ulInvalidInputValueHandle;	// All devices
		SecondaryActionSetHandle = k_ulInvalidActionSetHandle;	// Placeholder. Ignored due to k_ulInvalidValueHandle in RestrictedToDeviceHandle
	}
};

struct FSteamVRInputAction
{
	FString		Path;							// The path defined for the action (e.g. main/in/{ActionName})
	FName		Name;							// The SteamVR name of the action (e.g. Teleport, OpenConsole)
	EActionType	Type;							// The SteamVR input data type
	FName		KeyX;							// The UE Key in the X axis or float axis (e.g. Motion_Controller_Thumbstick_X)
	FName		KeyY;							// The UE Key in the Y axis
	FName		KeyZ;							// The UE Key in the Z axis
	FVector		Value;							// The 1D, 2D, 3D (analog) input value of this action
	FString		StringPath;						// The string value of this action (such as for Skeleton Paths where a bool or float axis is not appropriate)
	bool		bState;							// The bool (digital) value of this action
	bool		bRequirement;					// Whether or not this action is required

	VRActionHandle_t Handle;					// The handle to the SteamVR main Action Set
	VRInputValueHandle_t ActiveOrigin = 0;		// The input value handle of the origin of the latest input event
	EVRInputError LastError;					// A cache for the last Error for operations against this action (could also be "No Error")

	FString GetActionTypeName()
	{
		return SActionTypes[(int)Type];
	}

	FSteamVRInputAction(const FString& inPath, EActionType inType, bool inRequirement, const FName& inName, const FString& inStringPath)
		: Path(inPath)
		, Name(inName)
		, Type(inType)
		, KeyX()
		, KeyY()
		, KeyZ()
		, Value()
		, StringPath(inStringPath)
		, bRequirement(inRequirement)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const FName& inName, const FName& inKeyName, bool inState)
		: Path(inPath)
		, Name(inName)
		, Type(Boolean)
		, KeyX(inKeyName)
		, KeyY()
		, KeyZ()
		, Value()
		, bState(inState)
		, bRequirement(true)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const FName& inName, bool inRequirement, const FName& inKeyName, bool inState)
		: Path(inPath)
		, Name(inName)
		, Type(Boolean)
		, KeyX(inKeyName)
		, KeyY()
		, KeyZ()
		, Value()
		, bState(inState)
		, bRequirement(inRequirement)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const FName& inName, const FName& inKeyName, float inValue1D)
		: Path(inPath)
		, Name(inName)
		, Type(Vector1)
		, KeyX(inKeyName)
		, KeyY()
		, KeyZ()
		, Value(inValue1D, 0.f, 0.f)
		, bRequirement(true)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const FName& inName, const FName& inKeyName_X, const FName& inKeyName_Y, const FVector2D& inValue2D)
		: Path(inPath)
		, Name(inName)
		, Type(Vector2)
		, KeyX(inKeyName_X)
		, KeyY(inKeyName_Y)
		, KeyZ()
		, Value(inValue2D.X, inValue2D.Y, 0.f)
		, bRequirement(true)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const FName& inName, const FName& inKeyName_X, const FName& inKeyName_Y, const FName& inKeyName_Z, const FVector& inValue3D)
		: Path(inPath)
		, Name(inName)
		, Type(Vector3)
		, KeyX(inKeyName_X)
		, KeyY(inKeyName_Y)
		, KeyZ(inKeyName_Z)
		, Value(inValue3D.X, inValue3D.Y, inValue3D.Z)
		, bRequirement(true)
		, Handle()
		, LastError(VRInputError_None)
	{}

	FSteamVRInputAction(const FString& inPath, const EActionType& inActionType, const bool& inRequirement, const FName& inName)
		: Path(inPath)
		, Name(inName)
		, Type(inActionType)
		, KeyX()
		, KeyY()
		, KeyZ()
		, Value()
		, bRequirement(inRequirement)
		, Handle()
		, LastError(VRInputError_None)
	{}

};

struct FSteamVRInputState
{
	bool bIsAxis;
	bool bIsAxis2;
	bool bIsAxis3;
	bool bIsTrigger;
	bool bIsBumper;
	bool bIsThumbstick;
	bool bIsJoystick;
	bool bIsTrackpad;
	bool bIsDpadUp;
	bool bIsDpadDown;
	bool bIsDpadLeft;
	bool bIsDpadRight;
	bool bIsGrip;
	bool bIsCapSense;
	bool bIsLeft;
	bool bIsFaceButton1;
	bool bIsFaceButton2;
	bool bIsXButton;
	bool bIsYButton;
	bool bIsGripGrab;
	bool bIsPinchGrab;
	bool bIsPress;
	bool bIsAppMenu;
	bool bIsProximity;

	FSteamVRInputState() {}
};

struct FSteamVRTemporaryAction
{
	FKey UE4Key;
	FName ActionName;
	bool bIsY;

	FSteamVRTemporaryAction()
	{
		ActionName = NAME_None;
		UE4Key = EKeys::Invalid;
		bIsY = false;
	}

	FSteamVRTemporaryAction(const FKey& inUE4Key, const FName& inActionName)
		: UE4Key(inUE4Key)
		, ActionName(inActionName)
	{
		bIsY = false;
	}
};
