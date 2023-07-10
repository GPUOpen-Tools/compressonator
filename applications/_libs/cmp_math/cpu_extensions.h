//=====================================================================
// Copyright 2018-2023 (c), Advanced Micro Devices, Inc. All rights reserved.
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



#ifndef CPU_EXTENSION_DETECTION_H_
#define CPU_EXTENSION_DETECTION_H_

#define CMP_CPU_MMX_MASK          ((int)1 << 23)
#define CMP_CPU_SSE_MASK          ((int)1 << 25)      // 0x1000000
#define CMP_CPU_SSE2_MASK         ((int)1 << 26)      // 0x2000000
#define CMP_CPU_SSE3_MASK         ((int)1 <<  0)      // 0x0000001
#define CMP_CPU_SSSE3_MASK        ((int)1 <<  9)      // 0x0000100
#define CMP_CPU_SSE41_MASK        ((int)1 << 19)      // 0x0040000
#define CMP_CPU_SSE42_MASK        ((int)1 << 20)      // 0x0080000
#define CMP_CPU_AES_MASK          ((int)1 << 25)
#define CMP_CPU_AVX_MASK          ((int)1 << 28)
#define CMP_CPU_FMA3_MASK         ((int)1 << 12)
#define CMP_CPU_RDRAND_MASK       ((int)1 << 30)
#define CMP_CPU_AVX2_MASK         ((int)1 <<  5)
#define CMP_CPU_BMI1_MASK         ((int)1 <<  3)
#define CMP_CPU_BMI2_MASK         ((int)1 <<  8)
#define CMP_CPU_ADX_MASK          ((int)1 << 19)
#define CMP_CPU_MPX_MASK          ((int)1 << 14)
#define CMP_CPU_SHA_MASK          ((int)1 << 29)
#define CMP_CPU_PREFETCHWT1_MASK  ((int)1 <<  0)
#define CMP_CPU_AVX512_F_MASK     ((int)1 << 16)
#define CMP_CPU_AVX512_CD_MASK    ((int)1 << 28)
#define CMP_CPU_AVX512_PF_MASK    ((int)1 << 26)
#define CMP_CPU_AVX512_ER_MASK    ((int)1 << 27)
#define CMP_CPU_AVX512_VL_MASK    ((int)1 << 31)
#define CMP_CPU_AVX512_BW_MASK    ((int)1 << 30)
#define CMP_CPU_AVX512_DQ_MASK    ((int)1 << 17)
#define CMP_CPU_AVX512_IFMA_MASK  ((int)1 << 21)
#define CMP_CPU_AVX512_VBMI_MASK  ((int)1 <<  1)
#define CMP_CPU_x64_MASK          ((int)1 << 29)
#define CMP_CPU_ABM_MASK          ((int)1 <<  5)
#define CMP_CPU_SSE4a_MASK        ((int)1 <<  6)    // 0x00000040
#define CMP_CPU_FMA4_MASK         ((int)1 << 16)
#define CMP_CPU_XOP_MASK          ((int)1 << 11)    // 0x00000800

// List of possible instruction set extensions that might be supported by a CPU
typedef enum
{
    // used as a default "off" value
    EXTENSION_NONE = 0,

    EXTENSION_MMX,
    EXTENSION_SSE,
    EXTENSION_SSE2,
    EXTENSION_SSE3,
    EXTENSION_SSSE3,
    EXTENSION_SSE41,
    EXTENSION_SSE42,
    EXTENSION_AES,
    EXTENSION_AVX,
    EXTENSION_FMA3,
    EXTENSION_RDRAND,
    EXTENSION_AVX2,
    EXTENSION_BMI1,
    EXTENSION_BMI2,
    EXTENSION_ADX,
    EXTENSION_MPX,
    EXTENSION_SHA,
    EXTENSION_PREFETCHWT1,
    EXTENSION_AVX512_F,
    EXTENSION_AVX512_CD,
    EXTENSION_AVX512_PF,
    EXTENSION_AVX512_ER,
    EXTENSION_AVX512_VL,
    EXTENSION_AVX512_BW,
    EXTENSION_AVX512_DQ,
    EXTENSION_AVX512_IFMA,
    EXTENSION_AVX512_VBMI,
    EXTENSION_x64,
    EXTENSION_ABM,
    EXTENSION_SSE4a,
    EXTENSION_FMA4,
    EXTENSION_XOP,

    EXTENSION_COUNT
} CPU_EXTENSIONS;

typedef struct
{
    int extensionList[EXTENSION_COUNT];
} CPUExtensions;

// A wrapper around the cpuid instruction
void GetCPUID(int outInfo[4], int functionID);

// Fill out the CPUExtensions struct with all the instruction extensions that are supported on the current CPU
CPUExtensions GetCPUExtensions();

// Return whether certain instruction sets are available to use on the current CPU
bool IsAvailableSSE4(CPUExtensions extensions);
bool IsAvailableAVX2(CPUExtensions extensions);
bool IsAvailableAVX512(CPUExtensions extensions);

#endif
