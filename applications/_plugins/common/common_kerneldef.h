//===============================================================================
// Copyright (c) 2016-2018  Advanced Micro Devices, Inc. All rights reserved.
//===============================================================================
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
//
//  File Name:   Common_KernelDef.h
//
//////////////////////////////////////////////////////////////////////////////
#ifndef _COMMON_KERNELDEF2_H
#define _COMMON_KERNELDEF2_H

#include "common_def.h"

#ifndef ASPM_GPU

#include "cmp_math_common.h"
#include "stdio.h"
#include <math.h>
#include <algorithm>    // std::max

// defined when running code on cpu
#define __kernel
#define __global
#define __constant          const
#define __local             const

namespace CMP_Encoders {

//==========================================================
// Definitions used by ArchitectMF and Compressonator SDK
//==========================================================
typedef struct {
    CGU_UINT8  blockType;                                       // 0 = Single 1 = Dual
    CGU_UINT8  best_blockMode;                                  // Bock is Invalid if value is 99: No suitable mode was found
    CGU_UINT8  best_indices[MAX_SUBSETS][MAX_SUBSET_SIZE];

    // This can be union
    int       best_Endpoints[MAX_SUBSETS][2][MAX_DIMENSION_BIG];
    CGU_DWORD packedEndpoints[MAX_SUBSETS][2];

    // Specific to Dual
    CGU_UINT8  best_indexSelection;                             // either 0 or 1
    CGU_UINT8  best_rotation;                                   // either 0 or 2

    // Specific to Single
    CGU_UINT8   bestPartition;
} QUANTIZEBLOCK;


#define QUANTIZEBLOCK_ID "A896F2E4-0C7F-41F1-B074-22545FDE4808"
typedef struct {
    int         DataType = 0;  // 0 = 64 Bits, 1 = 128 Bits 2 = 512 bits
    int         w = 0;                          // Origin Image size
    int         h = 0;                          //
    int         x = 4;                          //Block Sizes: Width
    int         y = 4;                          //Block Sizes: Height
    int         curr_blockx = 0;                //Current block been processed
    int         curr_blocky = 0;                //Current block been processed
    int         compressed_size = 0;            //Track the current compressed block size
    int         source_size = 64;               //Usually fixed at 64 bytes max
    QUANTIZEBLOCK buffer;
} Quantizer_Buffer;

} // namespace CMP_Encoder
#endif

#endif  // _COMMON_KERNELDEF2_H
