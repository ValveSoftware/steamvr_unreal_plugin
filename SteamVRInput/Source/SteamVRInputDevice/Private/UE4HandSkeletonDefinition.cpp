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


#include "UE4HandSkeletonDefinition.h"

namespace UE4HandSkeleton
{
	// Define the index of each bone's parent
	static const int32 g_BoneParentMap[] = {
		-1,						// EUE4HandBone_Wrist
		EUE4HandBone_Wrist, 	// EUE4HandBone_Index_01
		EUE4HandBone_Index_01, 	// EUE4HandBone_Index_02
		EUE4HandBone_Index_02, 	// EUE4HandBone_Index_03
		EUE4HandBone_Wrist, 	// EUE4HandBone_Middle_01
		EUE4HandBone_Middle_01, // EUE4HandBone_Middle_02
		EUE4HandBone_Middle_02, // EUE4HandBone_Middle_03
		EUE4HandBone_Wrist, 	// EUE4HandBone_Pinky_01
		EUE4HandBone_Pinky_01, 	// EUE4HandBone_Pinky_02
		EUE4HandBone_Pinky_02, 	// EUE4HandBone_Pinky_03
		EUE4HandBone_Wrist, 	// EUE4HandBone_Ring_01
		EUE4HandBone_Ring_01, 	// EUE4HandBone_Ring_02
		EUE4HandBone_Ring_02,	// EUE4HandBone_Ring_03
		EUE4HandBone_Wrist,		// EUE4HandBone_Thumb_01
		EUE4HandBone_Thumb_01,  // EUE4HandBone_Thumb_02
		EUE4HandBone_Thumb_02,  // EUE4HandBone_Thumb_03
	};

	// Ensure that the parent bone list matches the size of the list of bones
	static_assert( sizeof(g_BoneParentMap) / sizeof(g_BoneParentMap[0]) == ( size_t )EUE4HandBone_Count, "Bone Parent map is not the right size" );

	// Define the names of each bone
	static const FName g_BoneNames[] = {
		TEXT("hand"),
		TEXT("index_01"),
		TEXT("index_02"),
		TEXT("index_03"),
		TEXT("middle_01"),
		TEXT("middle_02"),
		TEXT("middle_03"),
		TEXT("pinky_01"),
		TEXT("pinky_02"),
		TEXT("pinky_03"),
		TEXT("ring_01"),
		TEXT("ring_02"),
		TEXT("ring_03"),
		TEXT("thumb_01"),
		TEXT("thumb_02"),
		TEXT("thumb_03")
	};

	static_assert( sizeof(g_BoneNames) / sizeof(g_BoneNames[0]) == ( size_t )EUE4HandBone_Count, "Bone Name map is not the right size" );

	TArray<TArray<int32>> CreateChildList()
	{
		TArray<TArray<int32>> list;

		// Calculate the list of children based on the parent index array, for fast lookup later
		int32 nBoneCount = ( int32 )EUE4HandBone_Count;
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
