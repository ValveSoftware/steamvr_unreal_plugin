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


#include "SteamVRSkeletonDefinition.h"

namespace SteamVRSkeleton
{
	// Define the index of each bone's parent
	static const int32 g_BoneParentMap[] = {
		-1,  // ESteamVRBone_Root = 0,
		0, 	 // ESteamVRBone_Wrist,
		1, 	 // ESteamVRBone_Thumb0,
		2, 	 // ESteamVRBone_Thumb1,
		3, 	 // ESteamVRBone_Thumb2,
		4, 	 // ESteamVRBone_Thumb3,
		1, 	 // ESteamVRBone_IndexFinger0,
		6, 	 // ESteamVRBone_IndexFinger1,
		7, 	 // ESteamVRBone_IndexFinger2,
		8, 	 // ESteamVRBone_IndexFinger3,
		9, 	 // ESteamVRBone_IndexFinger4,
		1, 	 // ESteamVRBone_MiddleFinger0,
		11,  // ESteamVRBone_MiddleFinger1,
		12,  // ESteamVRBone_MiddleFinger2,
		13,  // ESteamVRBone_MiddleFinger3,
		14,  // ESteamVRBone_MiddleFinger4,
		1, 	 // ESteamVRBone_RingFinger0,
		16,  // ESteamVRBone_RingFinger1,
		17,  // ESteamVRBone_RingFinger2,
		18,  // ESteamVRBone_RingFinger3,
		19,  // ESteamVRBone_RingFinger4,
		1, 	 // ESteamVRBone_PinkyFinger0,
		21,  // ESteamVRBone_PinkyFinger1,
		22,  // ESteamVRBone_PinkyFinger2,
		23,  // ESteamVRBone_PinkyFinger3,
		24,  // ESteamVRBone_PinkyFinger4,
		0, 	 // ESteamVRBone_Aux_Thumb,
		0, 	 // ESteamVRBone_Aux_IndexFinger,
		0, 	 // ESteamVRBone_Aux_MiddleFinger
		0, 	 // ESteamVRBone_Aux_RingFinger,
		0	 // ESteamVRBone_Aux_PinkyFinger,
	};

	// Ensure that the parent bone list matches the size of the list of bones
	static_assert( sizeof( g_BoneParentMap ) / sizeof(g_BoneParentMap[ 0 ] ) == (size_t)ESteamVRBone_Count, "Bone Parent map is not the right size");

	// Define the names of each bone
	static const FName g_BoneNames[] = {
		TEXT("Root"),
		TEXT("wrist"),
		TEXT("finger_thumb_0"),
		TEXT("finger_thumb_1"),
		TEXT("finger_thumb_2"),
		TEXT("finger_thumb_end"),
		TEXT("finger_index_meta"),
		TEXT("finger_index_0"),
		TEXT("finger_index_1"),
		TEXT("finger_index_2"),
		TEXT("finger_index_end"),
		TEXT("finger_middle_meta"),
		TEXT("finger_middle_0"),
		TEXT("finger_middle_1"),
		TEXT("finger_middle_2"),
		TEXT("finger_middle_end"),
		TEXT("finger_ring_meta"),
		TEXT("finger_ring_0"),
		TEXT("finger_ring_1"),
		TEXT("finger_ring_2"),
		TEXT("finger_ring_end"),
		TEXT("finger_pinky_meta"),
		TEXT("finger_pinky_0"),
		TEXT("finger_pinky_1"),
		TEXT("finger_pinky_2"),
		TEXT("finger_pinky_end"),
		TEXT("finger_thumb_aux"),
		TEXT("finger_index_aux"),
		TEXT("finger_middle_aux"),
		TEXT("finger_ring_aux"),
		TEXT("finger_pinky_aux")
	};

	static_assert( sizeof( g_BoneNames ) / sizeof( g_BoneNames[ 0 ] ) == (size_t)ESteamVRBone_Count, "Bone Name map is not the right size" );

	TArray<TArray<int32>> CreateChildList()
	{
		TArray<TArray<int32>> list;

		// Calculate the list of children based on the parent index array, for fast lookup later
		int32 nBoneCount = (int32)ESteamVRBone_Count;
		list.SetNum(nBoneCount);

		for (int32 boneIndex = 0; boneIndex < nBoneCount; ++boneIndex)
		{
			int32 nParentIndex = g_BoneParentMap[boneIndex];
			if (nParentIndex != -1)
			{
				list[nParentIndex].Add(boneIndex);
			}
		}

		return list;
	}

	static const TArray<TArray<int32>> kBoneChildList = CreateChildList();


	const FName& GetBoneName(int32 nBoneIndex)
	{
		check(nBoneIndex >= 0 && nBoneIndex < GetBoneCount());

		return g_BoneNames[nBoneIndex];
	}

	int32 GetParentIndex(int32 nBoneIndex)
	{
		check(nBoneIndex >= 0 && nBoneIndex < GetBoneCount());

		return g_BoneParentMap[nBoneIndex];
	}

	int32 GetChildCount(int32 nBoneIndex)
	{
		check(nBoneIndex >= 0 && nBoneIndex < GetBoneCount());

		return kBoneChildList[nBoneIndex].Num();
	}

	int32 GetChildIndex(int32 nBoneIndex, int32 nChildIndex)
	{
		check(nBoneIndex >= 0 && nBoneIndex < GetBoneCount());
		check(nChildIndex >= 0 && nChildIndex < kBoneChildList[nBoneIndex].Num());

		return kBoneChildList[nBoneIndex][nChildIndex];
	}
}
