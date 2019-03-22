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
