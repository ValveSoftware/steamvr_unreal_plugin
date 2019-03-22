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


#include "SteamVRSkeletonDefinition.h"

namespace SteamVRSkeleton
{
	// Define the index of each bone's parent
	static const int32 g_BoneParentMap[] = {
		-1,  // EBone_Root = 0,
		0, 	 // EBone_Wrist,
		1, 	 // EBone_Thumb0,
		2, 	 // EBone_Thumb1,
		3, 	 // EBone_Thumb2,
		4, 	 // EBone_Thumb3,
		1, 	 // EBone_IndexFinger0,
		6, 	 // EBone_IndexFinger1,
		7, 	 // EBone_IndexFinger2,
		8, 	 // EBone_IndexFinger3,
		9, 	 // EBone_IndexFinger4,
		1, 	 // EBone_MiddleFinger0,
		11,  // EBone_MiddleFinger1,
		12,  // EBone_MiddleFinger2,
		13,  // EBone_MiddleFinger3,
		14,  // EBone_MiddleFinger4,
		1, 	 // EBone_RingFinger0,
		16,  // EBone_RingFinger1,
		17,  // EBone_RingFinger2,
		18,  // EBone_RingFinger3,
		19,  // EBone_RingFinger4,
		1, 	 // EBone_PinkyFinger0,
		21,  // EBone_PinkyFinger1,
		22,  // EBone_PinkyFinger2,
		23,  // EBone_PinkyFinger3,
		24,  // EBone_PinkyFinger4,
		0, 	 // EBone_Aux_Thumb,
		0, 	 // EBone_Aux_IndexFinger,
		0, 	 // EBone_Aux_MiddleFinger
		0, 	 // EBone_Aux_RingFinger,
		0	 // EBone_Aux_PinkyFinger,
	};

	// Ensure that the parent bone list matches the size of the list of bones
	static_assert( sizeof( g_BoneParentMap ) / sizeof(g_BoneParentMap[ 0 ] ) == (size_t)ESteamVRBone::EBone_Count, "Bone Parent map is not the right size");

	// Define the names of each bone
	static const FName g_BoneNames[] = {
		TEXT("Root"),
		TEXT("wrist_r"),
		TEXT("finger_thumb_0_r"),
		TEXT("finger_thumb_1_r"),
		TEXT("finger_thumb_2_r"),
		TEXT("finger_thumb_r_end"),
		TEXT("finger_index_meta_r"),
		TEXT("finger_index_0_r"),
		TEXT("finger_index_1_r"),
		TEXT("finger_index_2_r"),
		TEXT("finger_index_r_end"),
		TEXT("finger_middle_meta_r"),
		TEXT("finger_middle_0_r"),
		TEXT("finger_middle_1_r"),
		TEXT("finger_middle_2_r"),
		TEXT("finger_middle_r_end"),
		TEXT("finger_ring_meta_r"),
		TEXT("finger_ring_0_r"),
		TEXT("finger_ring_1_r"),
		TEXT("finger_ring_2_r"),
		TEXT("finger_ring_r_end"),
		TEXT("finger_pinky_meta_r"),
		TEXT("finger_pinky_0_r"),
		TEXT("finger_pinky_1_r"),
		TEXT("finger_pinky_2_r"),
		TEXT("finger_pinky_r_end"),
		TEXT("finger_thumb_r_aux"),
		TEXT("finger_index_r_aux"),
		TEXT("finger_middle_r_aux"),
		TEXT("finger_ring_r_aux"),
		TEXT("finger_pinky_r_aux")
	};

	static_assert( sizeof( g_BoneNames ) / sizeof( g_BoneNames[ 0 ] ) == (size_t)ESteamVRBone::EBone_Count, "Bone Name map is not the right size" );

	//-----------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------
	TArray< TArray<int32> > CreateChildList()
	{
		TArray< TArray<int32> > list;

		// Calculate the list of children based on the parent index array, for fast lookup later
		int32 nBoneCount = (int32)ESteamVRBone::EBone_Count;
		list.SetNum( nBoneCount );

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

	//-----------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------
	const FName& GetBoneName(int32 nBoneIndex)
	{
		check(nBoneIndex >= 0 && nBoneIndex < GetBoneCount());

		return g_BoneNames[nBoneIndex];
	}

	//-----------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------
	int32 GetParentIndex(int32 nBoneIndex)
	{
		check(nBoneIndex >= 0 && nBoneIndex < GetBoneCount());

		return g_BoneParentMap[nBoneIndex];
	}

	//-----------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------
	int32 GetChildCount(int32 nBoneIndex)
	{
		check(nBoneIndex >= 0 && nBoneIndex < GetBoneCount());

		return kBoneChildList[nBoneIndex].Num();
	}

	//-----------------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------
	int32 GetChildIndex(int32 nBoneIndex, int32 nChildIndex)
	{
		check(nBoneIndex >= 0 && nBoneIndex < GetBoneCount());
		check(nChildIndex >= 0 && nChildIndex < kBoneChildList[nBoneIndex].Num());

		return kBoneChildList[nBoneIndex][nChildIndex];
	}
}
