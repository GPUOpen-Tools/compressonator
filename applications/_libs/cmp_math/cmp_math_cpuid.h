//=====================================================================
// Copyright 2018 (c), Advanced Micro Devices, Inc. All rights reserved.
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



#ifndef CMP_MATH_CPUID_H
#define CMP_MATH_CPUID_H

#ifndef ASPM_GPU
//namespace CMP_MATH
//{

#define CMP_CPU_MMX          ((int)1 << 23)
#define CMP_CPU_SSE          ((int)1 << 25)      // 0x1000000
#define CMP_CPU_SSE2         ((int)1 << 26)      // 0x2000000
#define CMP_CPU_SSE3         ((int)1 <<  0)      // 0x0000001
#define CMP_CPU_SSSE3        ((int)1 <<  9)      // 0x0000100
#define CMP_CPU_SSE41        ((int)1 << 19)      // 0x0040000
#define CMP_CPU_SSE42        ((int)1 << 20)      // 0x0080000
#define CMP_CPU_AES          ((int)1 << 25)
#define CMP_CPU_AVX          ((int)1 << 28)
#define CMP_CPU_FMA3         ((int)1 << 12)
#define CMP_CPU_RDRAND       ((int)1 << 30)
#define CMP_CPU_AVX2         ((int)1 <<  5)
#define CMP_CPU_BMI1         ((int)1 <<  3)
#define CMP_CPU_BMI2         ((int)1 <<  8)
#define CMP_CPU_ADX          ((int)1 << 19)
#define CMP_CPU_MPX          ((int)1 << 14)
#define CMP_CPU_SHA          ((int)1 << 29)
#define CMP_CPU_PREFETCHWT1  ((int)1 <<  0)
#define CMP_CPU_AVX512_F     ((int)1 << 16)
#define CMP_CPU_AVX512_CD    ((int)1 << 28)
#define CMP_CPU_AVX512_PF    ((int)1 << 26)
#define CMP_CPU_AVX512_ER    ((int)1 << 27)
#define CMP_CPU_AVX512_VL    ((int)1 << 31)
#define CMP_CPU_AVX512_BW    ((int)1 << 30)
#define CMP_CPU_AVX512_DQ    ((int)1 << 17)
#define CMP_CPU_AVX512_IFMA  ((int)1 << 21)
#define CMP_CPU_AVX512_VBMI  ((int)1 <<  1)
#define CMP_CPU_x64          ((int)1 << 29)
#define CMP_CPU_ABM          ((int)1 <<  5)
#define CMP_CPU_SSE4a        ((int)1 <<  6)    // 0x00000040
#define CMP_CPU_FMA4         ((int)1 << 16)
#define CMP_CPU_XOP          ((int)1 << 11)    // 0x00000800

// List of features that are to be auto dectected and enabled
typedef enum {
    SSP_MMX,
    SSP_REF,
    SSP_SSE,
    SSP_SSE2,
    SSP_SSE3,
    SSP_SSSE3,
    SSP_SSE4a,
    SSP_SSE4_1,
    SSP_SSE4_2,
    SSP_SSE5,
    SSP_FMA3,
    SSP_SSE_COUNT
} cmp_cpu_feature;

typedef struct {
    int  feature[SSP_SSE_COUNT];
    bool x64;
} cmp_cpufeatures;

//}

void cmp_cpuid(int cpuInfo[4], int function_id);
cmp_cpufeatures cmp_get_cpufeatures();

#else
// ToDO: OpenCL supported code here
#endif // not def OpenCL


#endif
