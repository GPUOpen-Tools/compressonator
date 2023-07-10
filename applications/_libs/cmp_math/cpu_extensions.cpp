//=====================================================================
// Copyright 2023 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#ifdef _WIN32
#include <intrin.h>
#endif

#include "cpu_extensions.h"
#include "cmp_math_common.h"

void GetCPUID(int outInfo[4], int functionID) {
    // subfunction_id = 0
#ifdef _WIN32
    __cpuidex(outInfo, functionID, 0); // defined in intrin.h
#else
    // To Do
    //__cpuid_count(0, function_id, outInfo[0], outInfo[1], outInfo[2], outInfo[3]);
#endif
}

bool IsAvailableSSE4(CPUExtensions extensions)
{
    return extensions.extensionList[EXTENSION_SSE42] > 0;
}

bool IsAvailableAVX2(CPUExtensions extensions)
{
    return extensions.extensionList[EXTENSION_AVX2] > 0;
}

bool IsAvailableAVX512(CPUExtensions extensions)
{
    return ((extensions.extensionList[EXTENSION_AVX512_F]      || 
            extensions.extensionList[EXTENSION_AVX512_CD]      ||
            extensions.extensionList[EXTENSION_AVX512_PF]      ||
            extensions.extensionList[EXTENSION_AVX512_ER]      ||
            extensions.extensionList[EXTENSION_AVX512_VL]      ||
            extensions.extensionList[EXTENSION_AVX512_BW]      ||
            extensions.extensionList[EXTENSION_AVX512_DQ]      ||
            extensions.extensionList[EXTENSION_AVX512_IFMA]    ||
            extensions.extensionList[EXTENSION_AVX512_VBMI]) > 0);
}

CPUExtensions GetCPUExtensions()
{
    CPUExtensions result = {};

    int cpuInfo[4];

#ifndef __linux__

    GetCPUID(cpuInfo, 0);

    int nIds = cpuInfo[0];

    if (nIds >= 0x00000001) {
        GetCPUID(cpuInfo, 0x00000001);

        result.extensionList[EXTENSION_SSE3] = (cpuInfo[2] & CMP_CPU_SSE3_MASK);
        result.extensionList[EXTENSION_SSSE3] = (cpuInfo[2] & CMP_CPU_SSSE3_MASK);
        result.extensionList[EXTENSION_SSE41] = (cpuInfo[2] & CMP_CPU_SSE41_MASK);
        result.extensionList[EXTENSION_SSE42] = (cpuInfo[2] & CMP_CPU_SSE42_MASK);
        result.extensionList[EXTENSION_AES] = (cpuInfo[2] & CMP_CPU_AES_MASK);
        result.extensionList[EXTENSION_AVX] = (cpuInfo[2] & CMP_CPU_AVX_MASK);

        // also check for fma4 features
        result.extensionList[EXTENSION_FMA3] = (cpuInfo[2] & CMP_CPU_FMA3_MASK);

        // (cpuInfo[2] & CMP_CPU_RDRAND);
        result.extensionList[EXTENSION_SSE] = (cpuInfo[3] & CMP_CPU_SSE_MASK);
        result.extensionList[EXTENSION_SSE2] = (cpuInfo[3] & CMP_CPU_SSE2_MASK);
        result.extensionList[EXTENSION_MMX] = (cpuInfo[3] & CMP_CPU_MMX_MASK);
    }

    if (nIds >= 0x00000007)
    {
        GetCPUID(cpuInfo, 0x00000007);

        result.extensionList[EXTENSION_AVX2] = (cpuInfo[1] & CMP_CPU_AVX2_MASK);
        result.extensionList[EXTENSION_BMI1] = (cpuInfo[1] & CMP_CPU_BMI1_MASK);
        result.extensionList[EXTENSION_BMI2] = (cpuInfo[1] & CMP_CPU_BMI2_MASK);
        result.extensionList[EXTENSION_ADX] = (cpuInfo[1] & CMP_CPU_ADX_MASK);
        result.extensionList[EXTENSION_MPX] = (cpuInfo[1] & CMP_CPU_MPX_MASK);
        result.extensionList[EXTENSION_SHA] = (cpuInfo[1] & CMP_CPU_SHA_MASK);
        result.extensionList[EXTENSION_AVX512_F] = (cpuInfo[1] & CMP_CPU_AVX512_F_MASK);
        result.extensionList[EXTENSION_AVX512_CD] = (cpuInfo[1] & CMP_CPU_AVX512_CD_MASK);
        result.extensionList[EXTENSION_AVX512_PF] = (cpuInfo[1] & CMP_CPU_AVX512_PF_MASK);
        result.extensionList[EXTENSION_AVX512_ER] = (cpuInfo[1] & CMP_CPU_AVX512_ER_MASK);
        result.extensionList[EXTENSION_AVX512_VL] = (cpuInfo[1] & CMP_CPU_AVX512_VL_MASK);
        result.extensionList[EXTENSION_AVX512_BW] = (cpuInfo[1] & CMP_CPU_AVX512_BW_MASK);
        result.extensionList[EXTENSION_AVX512_DQ] = (cpuInfo[1] & CMP_CPU_AVX512_DQ_MASK);
        result.extensionList[EXTENSION_AVX512_IFMA] = (cpuInfo[1] & CMP_CPU_AVX512_IFMA_MASK);
        result.extensionList[EXTENSION_AVX512_VBMI] = (cpuInfo[2] & CMP_CPU_AVX512_VBMI_MASK);
        result.extensionList[EXTENSION_PREFETCHWT1] = (cpuInfo[2] & CMP_CPU_PREFETCHWT1_MASK);
    
    }

    GetCPUID(cpuInfo, 0x80000000);

    if ((unsigned int)cpuInfo[0] >= 0x80000001) {
        GetCPUID(cpuInfo, 0x80000001);

        result.extensionList[EXTENSION_SSE4a] = (cpuInfo[2] & CMP_CPU_SSE4a_MASK);
        result.extensionList[EXTENSION_XOP] = (cpuInfo[2] & CMP_CPU_XOP_MASK);
        result.extensionList[EXTENSION_x64] = (cpuInfo[3] & CMP_CPU_x64_MASK);
        result.extensionList[EXTENSION_ABM] = (cpuInfo[2] & CMP_CPU_ABM_MASK);
        result.extensionList[EXTENSION_FMA4] = (cpuInfo[2] & CMP_CPU_FMA4_MASK);
    }

#endif

    return result;
}

void printinfo(char *info)
{
#if defined(_DEBUG) && defined(CMP_VERBOSE)
    printf("%s",info);
#endif
}

void cmp_autodetected_cpufeatures(CMP_MATH_BYTE set) {
    // Determine which features are available
    CPUExtensions cpu = GetCPUExtensions();

    // Default: features always set to CPU
    cmp_set_cpu_features();

    // User requested to use only CPU
    if ((set & CMP_MATH_USE_CPU) > 0) return;

#ifdef CMP_USE_XMMINTRIN
#ifndef __linux__
    // Auto detect CPU features to enable
    for (int i = 0; i < EXTENSION_COUNT; i++)
    {
        if (cpu.extensionList[i] > 0)
        {
            switch (i) {
                // Enable SSE features
                case EXTENSION_SSE2: {
                    printinfo("SSE2 Enabled\n");
                    if ((set == CMP_MATH_USE_AUTO) || (set == CMP_MATH_USE_HPC))
                        cmp_set_sse2_features();
                    break;
                    }
                case EXTENSION_FMA3: {
                    printinfo("FMA3 Enabled\n");
                    if ((set == CMP_MATH_USE_AUTO) || (set == CMP_MATH_USE_HPC))
                        cmp_set_fma3_features();
                    break;
                    }
                case EXTENSION_AVX512_F   :
                    printinfo("EXTENSION_AVX512_F    Enabled\n");
                    break;
                case EXTENSION_AVX512_CD:
                    printinfo("EXTENSION_AVX512_CD   Enabled\n");
                    break;
                 case EXTENSION_AVX512_PF:
                        printinfo("EXTENSION_AVX512_PF   Enabled\n");
                    break;
                case EXTENSION_AVX512_ER:
                    printinfo("EXTENSION_AVX512_ER   Enabled\n");
                    break;
                case EXTENSION_AVX512_VL:
                    printinfo("EXTENSION_AVX512_VL   Enabled\n");
                    break;
                case EXTENSION_AVX512_BW:
                    printinfo("EXTENSION_AVX512_BW   Enabled\n");
                    break;
                case EXTENSION_AVX512_DQ:
                    printinfo("EXTENSION_AVX512_DQ   Enabled\n");
                    break;
                case EXTENSION_AVX512_IFMA:
                    printinfo("EXTENSION_AVX512_IFMA Enabled\n");
                    break;
                case EXTENSION_AVX512_VBMI:
                    printinfo("EXTENSION_AVX512_VBMI Enabled\n");
                    break;
                default:
                    //printinfo("enum index %d Enabled\n",i);
                    break;
            }
        }
    }
#endif
#endif

}