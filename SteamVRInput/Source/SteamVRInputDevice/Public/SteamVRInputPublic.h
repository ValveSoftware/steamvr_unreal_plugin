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


struct FSteamVRSkeletonTransform_t
{
	FTransform Wrist;

	FTransform Thumb_Metacarpal;
	FTransform Thumb_Proximal;
	FTransform Thumb_Middle;
	FTransform Thumb_Distal;
	FTransform Thumb_Tip;
	FTransform Thumb_Aux;

	FTransform Index_Metacarpal;
	FTransform Index_Proximal;
	FTransform Index_Middle;
	FTransform Index_Distal;
	FTransform Index_Tip;
	FTransform Index_Aux;

	FTransform Middle_Metacarpal;
	FTransform Middle_Proximal;
	FTransform Middle_Middle;
	FTransform Middle_Distal;
	FTransform Middle_Tip;
	FTransform Middle_Aux;

	FTransform Ring_Metacarpal;
	FTransform Ring_Proximal;
	FTransform Ring_Middle;
	FTransform Ring_Distal;
	FTransform Ring_Tip;
	FTransform Ring_Aux;

	FTransform Pinky_Metacarpal;
	FTransform Pinky_Proximal;
	FTransform Pinky_Middle;
	FTransform Pinky_Distal;
	FTransform Pinky_Tip;
	FTransform Pinky_Aux;
};
