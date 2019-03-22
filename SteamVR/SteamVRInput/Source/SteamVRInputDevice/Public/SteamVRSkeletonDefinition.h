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

#include "CoreMinimal.h"

enum class ESteamVRBone : uint8
{
	EBone_Root = 0,
	EBone_Wrist = 1,
	EBone_Thumb0 = 2,
	EBone_Thumb1 = 2,
	EBone_Thumb2 = 3,
	EBone_Thumb3 = 4,
	EBone_Thumb4 = 5,
	EBone_IndexFinger0 = 6,
	EBone_IndexFinger1 = 7,
	EBone_IndexFinger2 = 8,
	EBone_IndexFinger3 = 9,
	EBone_IndexFinger4 = 10,
	EBone_MiddleFinger0 = 11,
	EBone_MiddleFinger1 = 12,
	EBone_MiddleFinger2 = 13,
	EBone_MiddleFinger3 = 14,
	EBone_MiddleFinger4 = 15,
	EBone_RingFinger0 = 16,
	EBone_RingFinger1 = 17,
	EBone_RingFinger2 = 18,
	EBone_RingFinger3 = 19,
	EBone_RingFinger4 = 20,
	EBone_PinkyFinger0 = 21,
	EBone_PinkyFinger1 = 22,
	EBone_PinkyFinger2 = 23,
	EBone_PinkyFinger3 = 24,
	EBone_PinkyFinger4 = 25,
	EBone_Aux_Thumb = 26,
	EBone_Aux_IndexFinger = 27,
	EBone_Aux_MiddleFinger = 28,
	EBone_Aux_RingFinger = 29,
	EBone_Aux_PinkyFinger = 30,
	EBone_Count = 31
};


//---------------------------------------------------------
// Convenience functions to access static data as defined 
// by the SteamVR Skeletal Input skeleton
//---------------------------------------------------------
namespace SteamVRSkeleton
{
	// Returns the number of bones in the skeleton
	inline int32	GetBoneCount() { return (int32)ESteamVRBone::EBone_Count; }

	// Returns the name of the bone at the given index
	const FName&	GetBoneName(int32 nBoneIndex);

	// Returns the index of the parent bone of the given bone.  Returns -1 if the bone does not have a parent
	int32			GetParentIndex(int32 nBoneIndex);

	// Returns the number of children of the given bone
	int32			GetChildCount(int32 nBoneIndex);

	// Returns the index of the nth child of the given bone
	int32			GetChildIndex(int32 nBoneIndex, int32 nChildIndex);
};
