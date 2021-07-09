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


#ifndef CMP_MATH_COMMON_H
#define CMP_MATH_COMMON_H

#include "common_def.h"

#ifndef ASPM_GPU

#ifdef _WIN32
#define CMP_Align(x)  __declspec(align(x))
#else
#define CMP_Align(x)  __attribute(aligned(x))
#endif

#define CMP_MATH_USE_AUTO   0
#define CMP_MATH_USE_CPU    1
#define CMP_MATH_USE_HPC    2
#define CMP_MATH_USE_GPU    4

typedef unsigned char CMP_MATH_BYTE;
typedef unsigned int  CMP_MATH_DWORD;

#ifdef CMP_USE_XMMINTRIN
#ifndef __linux__
extern void cmp_set_fma3_features();
extern void cmp_set_sse2_features();
#endif
#endif

extern void cmp_set_cpu_features();

//==================================
// internal interfaces
//==================================
extern float cpu_clampf(float value, float minval, float maxval);
extern float cpu_lerp2(CMP_Vec4uc C1, CMP_Vec4uc CA, CMP_Vec4uc CB, CMP_Vec4uc C2, CMP_MATH_BYTE *encode1, CMP_MATH_BYTE *encode2);
extern float cpu_maxf(float l1, float r1);
extern float cpu_minf(float l1, float r1);
extern float cpu_rsqf(float *f);
extern float cpu_sqrtf(float * pIn);


#ifndef __linux__
extern float sse_clampf(float value, float minval, float maxval);
extern float sse_lerp2(CMP_Vec4uc C1, CMP_Vec4uc CA, CMP_Vec4uc CB, CMP_Vec4uc C2, CMP_MATH_BYTE *encode1, CMP_MATH_BYTE *encode2);
extern float sse_maxf(float l1, float r1);
extern float sse_minf(float l1, float r1);
extern float sse_rsqf(float *v);
extern float sse_sqrtf(  float *pIn );
#endif

//==================================
// User interfaces
//==================================
void cmp_autodected_cpufeatures(CMP_MATH_BYTE set);

extern float(*cmp_clampf2 )(float value, float minval, float maxval);
extern float(*cmp_lerp2  )(CMP_Vec4uc C1, CMP_Vec4uc CA, CMP_Vec4uc CB, CMP_Vec4uc C2, CMP_MATH_BYTE *encode1, CMP_MATH_BYTE *encode2);
extern float(*cmp_maxf2   )(float l1, float r1);
extern float(*cmp_minf2   )(float l1, float r1);
extern float(*cmp_rsqf2   )(float *);
extern float(*cmp_sqrtf2  )(float *);

#endif

#endif
