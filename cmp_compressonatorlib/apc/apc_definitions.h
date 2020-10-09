//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
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
//=====================================================================


#ifndef _APC_DEFINITIONS_H_
#define _APC_DEFINITIONS_H_

#include "common.h"

#define TRUE 1
#define FALSE 0

#define MAX_GT_THREADS 128

// Size of a compressed block in bytes
#define COMPRESSED_BLOCK_SIZE   16

#define MAX_SUBSET_SIZE  16

#define MAX_DIMENSION_BIG 4

#ifndef min
#ifdef _WIN32
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#endif

typedef struct {
    // Original image size
    int     m_srcWidth;
    int     m_srcHeight;
    // Source block dimensions to compress
    int     m_xdim;
    int     m_ydim;
    int     m_zdim;
    // Compression quality to apply during compression
    float   m_quality;
} APC_Encode;


typedef union {
    struct {
        unsigned int blockTypeBit : 1;
    } TwoPartitionMode;

    struct {
        unsigned int blockTypeBit : 1;
    } ThreePartitionMode;

    unsigned int dwRawData[4];
} APC6_BLOCK;


#endif



