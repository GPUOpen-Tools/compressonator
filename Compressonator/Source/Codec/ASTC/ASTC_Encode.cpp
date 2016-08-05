//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
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
// ASTC_Encode.cpp : A reference encoder for ASTC
//

#include <assert.h>
#include <float.h>
#include <math.h>
#include "ASTC_Encode.h"
#include "astc_codec_internals.h"
#include "softfloat.h"
#include "arm_stdint.h"

#ifdef ASTC_COMPDEBUGGER
#include "compclient.h"
#endif

#pragma warning(disable:4100)       // Dont show parameter warnings

extern int block_mode_histogram[2048];
void prepare_block_statistics(int xdim, int ydim, int zdim, const imageblock * blk, const error_weight_block * ewb, int *is_normal_map, float *lowest_correl);
int realign_weights(astc_decode_mode decode_mode, int xdim, int ydim, int zdim, const imageblock * blk, const error_weight_block * ewb, symbolic_compressed_block * scb, uint8_t * weight_set8, uint8_t * plane2_weight_set8);

#define SAFE_DELETE(x) if(x) { delete x; x = NULL;}
#define SAFE_DELETE_ARR(x) if (x) { delete[] x; x = NULL;}


double ASTCBlockEncoder::CompressBlock(
    astc_codec_image *input_image,
    uint8_t *bp,
    int xdim, 
    int ydim, 
    int zdim, 
    int x, 
    int y, 
    int z, 
    astc_decode_mode decode_mode,
    const error_weighting_params * ewp
    )
{

    swizzlepattern swz_encode = { 0, 1, 2, 3 };
    fetch_imageblock(input_image, &m_pb, xdim, ydim, zdim, x , y, z, swz_encode);
    compress_symbolic_block(input_image, decode_mode, xdim, ydim, zdim, ewp, &m_pb, &m_scb);
    m_pcb = symbolic_to_physical(xdim, ydim, zdim, &m_scb);
    *(physical_compressed_block *)bp = m_pcb;
    return 0.0;
}


