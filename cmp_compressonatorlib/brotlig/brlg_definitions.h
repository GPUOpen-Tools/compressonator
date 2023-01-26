//===============================================================================
// Copyright (c) 2022  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   BRLG_Definitions.h
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _BRLG_DEFINITIONS_H_
#define _BRLG_DEFINITIONS_H_

#include "common.h"

#define MAX_BRLG_THREADS 128

#define TRUE 1
#define FALSE 0

// TODO: these are all duplicated from "common_def.h" in CMP_Core, so perhaps figure out a way to combine this

#define COMPRESSED_BLOCK_SIZE   16
#define MAX_SUBSET_SIZE  16
#define MAX_DIMENSION_BIG 4

#ifndef min
#ifdef _WIN32
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#endif

// NOTE: This must be kept in sync with the "CMP_BRLGEncode" struct in brlg_encode_kernel.h
typedef struct {
    // Original data size
    int     m_srcSize;
    // Source block dimensions to compress at using GPU
    int     m_xdim;
    int     m_ydim;
    // Compression quality to apply during compression
    float   m_quality;
} BRLG_Encode;

#endif



