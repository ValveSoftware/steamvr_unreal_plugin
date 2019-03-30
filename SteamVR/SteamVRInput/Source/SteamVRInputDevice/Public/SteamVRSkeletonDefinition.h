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

#include "CoreMinimal.h"

/**
 * Enum for each bone in the SteamVR hand skeleton
*/
enum ESteamVRBone : int8
{
	ESteamVRBone_Root = 0,
	ESteamVRBone_Wrist = 1,
	ESteamVRBone_Thumb0 = 2,
	ESteamVRBone_Thumb1 = 2,
	ESteamVRBone_Thumb2 = 3,
	ESteamVRBone_Thumb3 = 4,
	ESteamVRBone_Thumb4 = 5,
	ESteamVRBone_IndexFinger0 = 6,
	ESteamVRBone_IndexFinger1 = 7,
	ESteamVRBone_IndexFinger2 = 8,
	ESteamVRBone_IndexFinger3 = 9,
	ESteamVRBone_IndexFinger4 = 10,
	ESteamVRBone_MiddleFinger0 = 11,
	ESteamVRBone_MiddleFinger1 = 12,
	ESteamVRBone_MiddleFinger2 = 13,
	ESteamVRBone_MiddleFinger3 = 14,
	ESteamVRBone_MiddleFinger4 = 15,
	ESteamVRBone_RingFinger0 = 16,
	ESteamVRBone_RingFinger1 = 17,
	ESteamVRBone_RingFinger2 = 18,
	ESteamVRBone_RingFinger3 = 19,
	ESteamVRBone_RingFinger4 = 20,
	ESteamVRBone_PinkyFinger0 = 21,
	ESteamVRBone_PinkyFinger1 = 22,
	ESteamVRBone_PinkyFinger2 = 23,
	ESteamVRBone_PinkyFinger3 = 24,
	ESteamVRBone_PinkyFinger4 = 25,
	ESteamVRBone_Aux_Thumb = 26,
	ESteamVRBone_Aux_IndexFinger = 27,
	ESteamVRBone_Aux_MiddleFinger = 28,
	ESteamVRBone_Aux_RingFinger = 29,
	ESteamVRBone_Aux_PinkyFinger = 30,
	ESteamVRBone_Count = 31
};


/**
 * Convenience functions to access static data as defined 
 * by the SteamVR Skeletal Input skeleton
*/
namespace SteamVRSkeleton
{
	/** Returns the number of bones in the skeleton */
	inline int32	GetBoneCount() { return ESteamVRBone_Count; }

	/** Returns the name of the bone at the given index */
	const FName&	GetBoneName(int32 nBoneIndex);

	/** Returns the index of the parent bone of the given bone.  Returns -1 if the bone does not have a parent */
	int32			GetParentIndex(int32 nBoneIndex);

	/** Returns the number of children of the given bone */
	int32			GetChildCount(int32 nBoneIndex);

	/** Returns the index of the nth child of the given bone */
	int32			GetChildIndex(int32 nBoneIndex, int32 nChildIndex);
};
