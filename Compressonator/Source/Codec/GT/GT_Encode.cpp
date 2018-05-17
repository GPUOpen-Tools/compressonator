//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//===============================================================================
//
// INTERNAL IP - NOT FOR DISTRIBUTION
//
//  GT_Encode.cpp : A reference encoder for GT
// 


#include "Common.h"
#include "GT_Definitions.h"
#include "GT_Encode.h"
#include "debug.h"

// SSE Implementation
#include <immintrin.h>

double GTBlockEncoder::CompressBlock(CMP_BYTE in[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG], CMP_BYTE out[COMPRESSED_BLOCK_SIZE])
{
    // Reserved for new code
    return (0);
}

void  GTCompressBlockSSE(CMP_DWORD *block_32, CMP_DWORD *block_dxtc)
{

}
