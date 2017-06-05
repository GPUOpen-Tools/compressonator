//===============================================================================
// Copyright (c) 2007-2017  Advanced Micro Devices, Inc. All rights reserved.
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
#include "ASTC_Definitions.h"
#include "softfloat.h"

#include "ASTC_Encode_Kernel.h"
#include "ASTC_Host.h"

#ifdef ASTC_COMPDEBUGGER
#include "compclient.h"
#endif

#pragma warning(disable:4100)       // Dont show parameter warnings

void prepare_block_statistics(int xdim, int ydim, int zdim, const imageblock * blk, const error_weight_block * ewb, int *is_normal_map, float *lowest_correl);
int realign_weights(astc_decode_mode decode_mode, int xdim, int ydim, int zdim, const imageblock * blk, const error_weight_block * ewb, symbolic_compressed_block * scb, uint8_t * weight_set8, uint8_t * plane2_weight_set8);

#define SAFE_DELETE(x) if(x) { delete x; x = NULL;}
#define SAFE_DELETE_ARR(x) if (x) { delete[] x; x = NULL;}

double ASTCBlockEncoder::CompressBlock_kernel(
    ASTC_Encoder::astc_codec_image *input_image,
    uint8_t *bp,
    int x,
    int y,
    int z,
    ASTC_Encoder::ASTC_Encode  *ASTCEncode
)
{
    //ASTC_Encoder::imageblock m_pb;
    ASTC_Encoder::symbolic_compressed_block  scb;

    //ASTC_Encoder::CGU_UINT pixelcount = ASTCEncode->m_ydim * ASTCEncode->m_xdim;
    //ASTC_Encoder::fetch_imageblock(input_image, &pb, pixelcount, ASTCEncode);

    fetch_imageblock_cpu(
        (const astc_codec_image_cpu *)input_image,
        (imageblock_cpu *) &m_pb,
        ASTCEncode->m_xdim,
        ASTCEncode->m_ydim, 
        ASTCEncode->m_zdim, 
        x, 
        y, 
        z
        );


    ASTC_Encoder::compress_symbolic_block((ASTC_Encoder::imageblock *)&m_pb, &scb, ASTCEncode);
    ASTC_Encoder::physical_compressed_block   pcb;
    pcb = ASTC_Encoder::symbolic_to_physical(&scb, ASTCEncode);

    *(ASTC_Encoder::physical_compressed_block *)bp = pcb;

    return 0.0;
}

