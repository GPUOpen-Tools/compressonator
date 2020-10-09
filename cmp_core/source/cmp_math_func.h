//=====================================================================
// Copyright 2020 (c), Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//=====================================================================
#ifndef CMP_MATH_FUNC_H
#define CMP_MATH_FUNC_H


#include "common_def.h"

#include <cmath>

#ifndef ASPM_GPU

#include <cmath>
#include <algorithm>

//============================================================================
// Core API which have have GPU equivalents, defined here for HPC_CPU usage
//============================================================================

#include <algorithm>
using namespace std;

static CGU_INT QSortFCmp(const void *Elem1, const void *Elem2) {
    CGU_INT ret = 0;

    if (*(CGU_FLOAT *)Elem1 < *(CGU_FLOAT *)Elem2)
        ret = -1;
    else if (*(CGU_FLOAT *)Elem1 > *(CGU_FLOAT *)Elem2)
        ret = 1;
    return ret;
}

static int QSortIntCmp(const void *Elem1, const void *Elem2) {
    return (*(CGU_INT32 *)Elem1 - *(CGU_INT32 *)Elem2);
}

static CGU_FLOAT  dot(CMP_IN CGU_Vec3f Color,CMP_IN CGU_Vec3f Color2) {
    CGU_FLOAT  ColorDot;
    ColorDot = (Color.x * Color2.x) + (Color.y * Color2.y) + (Color.z * Color2.z);
    return ColorDot;
}

static CGU_FLOAT  dot(CMP_IN CGU_Vec2f Color,CMP_IN CGU_Vec2f Color2) {
    CGU_FLOAT  ColorDot;
    ColorDot = Color.x * Color2.x + Color.y * Color2.y;
    return ColorDot;
}

static CGU_Vec2f abs(CMP_IN CGU_Vec2f Color) {
    CGU_Vec2f  ColorAbs;
    ColorAbs.x = std::abs(Color.x);
    ColorAbs.y = std::abs(Color.y);
    return ColorAbs;
}

static CGU_Vec3f fabs(CMP_IN CGU_Vec3f Color) {
    CGU_Vec3f  ColorAbs;
    ColorAbs.x = std::abs(Color.x);
    ColorAbs.y = std::abs(Color.y);
    ColorAbs.z = std::abs(Color.z);
    return ColorAbs;
}

static CGU_Vec3f round(CMP_IN CGU_Vec3f Color) {
    CGU_Vec3f ColorRound;
    ColorRound.x = std::round(Color.x);
    ColorRound.y = std::round(Color.y);
    ColorRound.z = std::round(Color.z);
    return ColorRound;
}

static CGU_Vec2f round(CMP_IN CGU_Vec2f Color) {
    CGU_Vec2f ColorRound;
    ColorRound.x = std::round(Color.x);
    ColorRound.y = std::round(Color.y);
    return ColorRound;
}

static CGU_Vec3f ceil(CMP_IN CGU_Vec3f Color) {
    CGU_Vec3f ColorCeil;
    ColorCeil.x = std::ceil(Color.x);
    ColorCeil.y = std::ceil(Color.y);
    ColorCeil.z = std::ceil(Color.z);
    return ColorCeil;
}

static CGU_Vec3f floor(CMP_IN CGU_Vec3f Color) {
    CGU_Vec3f Colorfloor;
    Colorfloor.x = std::floor(Color.x);
    Colorfloor.y = std::floor(Color.y);
    Colorfloor.z = std::floor(Color.z);
    return Colorfloor;
}

static CGU_Vec3f saturate(CGU_Vec3f value) {
    if (value.x > 1.0f) value.x = 1.0f;
    else if (value.x < 0.0f) value.x = 0.0f;

    if (value.y > 1.0f) value.y = 1.0f;
    else if (value.y < 0.0f) value.y = 0.0f;

    if (value.z > 1.0f) value.z = 1.0f;
    else if (value.z < 0.0f) value.z = 0.0f;

    return value;
}

#endif

//============================================================================
// Core API which are shared between GPU & CPU
//============================================================================

#endif // Header Guard

