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


#include "cmp_math_common.h"
#include "cmp_math_cpuid.h"

#ifndef ASPM_GPU

void cmp_cpuid(int cpuInfo[4], int function_id)
{
    // subfunction_id = 0
    #ifdef _WIN32
    __cpuidex(cpuInfo, function_id, 0);
#else
    // To Do
    //__cpuid_count(0, function_id, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
#endif
}

cmp_cpufeatures cmp_get_cpufeatures()
{
    unsigned int maxInfoType;
    cmp_cpufeatures cpu;
    int cpuInfo[4], i;

    // Clear the feature list
    for (i = 0; i<SSP_SSE_COUNT; ++i)
    {
        cpu.feature[i] = 0;
    }

#ifndef _LINUX
    cmp_cpuid(cpuInfo,0);
    int nIds = cpuInfo[0];

    if (nIds >= 0x00000001)
    {
        cmp_cpuid(cpuInfo, 0x00000001);
        cpu.feature[SSP_SSE3]   = (cpuInfo[2] & CMP_CPU_SSE3); 
        cpu.feature[SSP_SSSE3]  = (cpuInfo[2] & CMP_CPU_SSSE3);
        cpu.feature[SSP_SSE4_1] = (cpuInfo[2] & CMP_CPU_SSE41);
        cpu.feature[SSP_SSE4_2] = (cpuInfo[2] & CMP_CPU_SSE42);
        // (cpuInfo[2] & CMP_CPU_AES   );
        // (cpuInfo[2] & CMP_CPU_AVX   );

        // also check for fma4 features
        cpu.feature[SSP_FMA3] = (cpuInfo[2] & CMP_CPU_FMA3);

        // (cpuInfo[2] & CMP_CPU_RDRAND);
        cpu.feature[SSP_SSE]    = (cpuInfo[3] & CMP_CPU_SSE);
        cpu.feature[SSP_SSE2]   = (cpuInfo[3] & CMP_CPU_SSE2);
        // (cpuInfo[3] & CMP_CPU_MMX   );
    }

    //if (nIds >= 0x00000007)
    //{
    //    cpuid(cpuInfo,0x00000007);
    //    (cpuInfo[1] & CMP_CPU_AVX2       );
    //    (cpuInfo[1] & CMP_CPU_BMI1       );
    //    (cpuInfo[1] & CMP_CPU_BMI2       );
    //    (cpuInfo[1] & CMP_CPU_ADX        );
    //    (cpuInfo[1] & CMP_CPU_MPX        );
    //    (cpuInfo[1] & CMP_CPU_SHA        );
    //    (cpuInfo[1] & CMP_CPU_AVX512_F   );
    //    (cpuInfo[1] & CMP_CPU_AVX512_CD  );
    //    (cpuInfo[1] & CMP_CPU_AVX512_PF  );
    //    (cpuInfo[1] & CMP_CPU_AVX512_ER  );
    //    (cpuInfo[1] & CMP_CPU_AVX512_VL  );
    //    (cpuInfo[1] & CMP_CPU_AVX512_BW  );
    //    (cpuInfo[1] & CMP_CPU_AVX512_DQ  );
    //    (cpuInfo[1] & CMP_CPU_AVX512_IFMA);
    //    (cpuInfo[2] & CMP_CPU_AVX512_VBMI);
    //    (cpuInfo[2] & CMP_CPU_PREFETCHWT1);
    //
    //}

    cmp_cpuid(cpuInfo,0x80000000);
    maxInfoType = cpuInfo[0];
    if (maxInfoType >= 0x80000001)
    {
        cmp_cpuid(cpuInfo, 0x80000001);
        cpu.feature[SSP_SSE4a] = (cpuInfo[2] & CMP_CPU_SSE4a);
        cpu.feature[SSP_SSE5]  = (cpuInfo[2] & CMP_CPU_XOP);
        // (cpuInfo[3] & CMP_CPU_x64);
        // (cpuInfo[2] & CMP_CPU_ABM);
        // (cpuInfo[2] & CMP_CPU_FMA4);
        cpu.x64 = (cpuInfo[3] & CMP_CPU_x64) > 0;
    }
#endif

    return cpu;
}

void cmp_autodected_cpufeatures(CMP_MATH_BYTE set)
{
    // Determine which features are available
    cmp_cpufeatures cpu = cmp_get_cpufeatures();

    // Default: features always set to CPU 
    cmp_set_cpu_features();

    // User requested to use only CPU
    if ((set & CMP_MATH_USE_CPU) > 0) return;


#ifndef _LINUX
    // Auto detect CPU features to enable
    for (int i = 0; i<SSP_SSE_COUNT; i++)
    {
        if (cpu.feature[i] > 0)
        {
            switch (i)
            {
                // Enable SSE features
                case SSP_SSE2:
                case SSP_SSE:
                {
                    if ((set == CMP_MATH_USE_AUTO) || (set == CMP_MATH_USE_HPC))
                        cmp_set_sse2_features();
                    break;
                }
                case SSP_FMA3:
                {
                    if ((set == CMP_MATH_USE_AUTO) || (set == CMP_MATH_USE_HPC))
                        cmp_set_fma3_features();
                    break;
                }
            }
        }
    }
#endif

}

//}

#else
// ToDO: OpenCL supported code here
#endif // not def OpenCL
