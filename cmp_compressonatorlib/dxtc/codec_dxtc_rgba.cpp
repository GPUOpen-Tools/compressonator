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
//  File Name:   Codec_DXTC.cpp
//  Description: implementation of the CCodec_DXTC class
//
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codec_dxtc.h"
#include "compressonatorxcodec.h"
#include "dxtc_v11_compress.h"

#include <algorithm>

#ifdef USE_CMP_CORE_API
#include "bcn_common_kernel.h"
#include "bcn_common_api.h"
#include "bc1_cmp.h"
#endif

using namespace CMP;

// #define PRINT_DECODE_INFO
using namespace std;

CodecError CCodec_DXTC::CompressRGBABlock(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4], CODECFLOAT* pfChannelWeights) {
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];
    for(CMP_DWORD i = 0; i < 16; i++)
        alphaBlock[i] = static_cast<CMP_BYTE>(((DWORD*)rgbaBlock)[i] >> RGBA8888_OFFSET_A);

    CodecError err = CompressAlphaBlock(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    if(err != CE_OK)
        return err;

    return CompressRGBBlock(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB], pfChannelWeights, false);
}

void CCodec_DXTC::DecompressRGBABlock(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]) {
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];

    DecompressAlphaBlock(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    DecompressRGBBlock(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB], false);

    for(CMP_DWORD i = 0; i < 16; i++)
        ((DWORD*)rgbaBlock)[i] = (alphaBlock[i] << RGBA8888_OFFSET_A) | (((DWORD*)rgbaBlock)[i] & ~(BYTE_MASK << RGBA8888_OFFSET_A));
}

CodecError CCodec_DXTC::CompressRGBABlock_ExplicitAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4], CODECFLOAT* pfChannelWeights) {
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];
    for(CMP_DWORD i = 0; i < 16; i++)
        alphaBlock[i] = static_cast<CMP_BYTE>(((DWORD*)rgbaBlock)[i] >> RGBA8888_OFFSET_A);

    CodecError err = CompressExplicitAlphaBlock(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    if(err != CE_OK)
        return err;

    return CompressRGBBlock(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB], pfChannelWeights, false);
}

void CCodec_DXTC::DecompressRGBABlock_ExplicitAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]) {
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];

    DecompressExplicitAlphaBlock(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    DecompressRGBBlock(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB], false);

    for(CMP_DWORD i = 0; i < 16; i++)
        ((DWORD*)rgbaBlock)[i] = (alphaBlock[i] << RGBA8888_OFFSET_A) | (((DWORD*)rgbaBlock)[i] & ~(BYTE_MASK << RGBA8888_OFFSET_A));
}

#define ConstructColour(r, g, b)  (((r) << 11) | ((g) << 5) | (b))

/*
Channel Bits
*/
#define RG 5
#define GG 6
#define BG 5

CodecError CCodec_DXTC::CompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2], CODECFLOAT* pfChannelWeights, bool bDXT1, bool bDXT1UseAlpha, CMP_BYTE nDXT1AlphaThreshold) {
    
    CMP_UNUSED(nDXT1AlphaThreshold);
    CMP_UNUSED(bDXT1UseAlpha);
    CMP_UNUSED(bDXT1);
    CMP_UNUSED(pfChannelWeights);

#ifdef USE_CMP_CORE_API
    CGU_Vec2ui cmpBlock;
    CGU_Vec4f src_imageNorm[BLOCK_SIZE_4X4];

    CGU_UINT32 jj=0;
    for (CGU_UINT32 ii = 0; ii < 16; ii++)
    {
        src_imageNorm[ii].b = rgbBlock[jj++] / 255.0f;
        src_imageNorm[ii].g = rgbBlock[jj++] / 255.0f;
        src_imageNorm[ii].r = rgbBlock[jj++] / 255.0f;
        src_imageNorm[ii].w = rgbBlock[jj++] / 255.0f;
    }

    cmpBlock = CompressBlockBC1_NORMALIZED(src_imageNorm,m_BC15Options);
    compressedBlock[0] = cmpBlock.x;
    compressedBlock[1] = cmpBlock.y;
#else // Use Legacy API
    /*
    ARGB Channel indexes
    */
    int RC = 2, GC = 1, BC = 0;

    if(bDXT1 && m_nCompressionSpeed == CMP_Speed_Normal) {
        CMP_BYTE nEndpoints[2][3][2];
        CMP_BYTE nIndices[2][BLOCK_SIZE_4X4];

        double fError3 = CompRGBBlock((DWORD*)rgbBlock,compressedBlock, BLOCK_SIZE_4X4, RG, GG, BG, nEndpoints[0], nIndices[0], 3, m_bUseSSE2, m_b3DRefinement, m_nRefinementSteps, pfChannelWeights, bDXT1UseAlpha, nDXT1AlphaThreshold);

        //printf(": %2d %2d %2d %2d %2d\n",nIndices[0][0],nIndices[0][1],nIndices[0][2],nIndices[0][3],nIndices[0][4]);

       // use case of small min max ranges
       if (compressedBlock[0] > 0) {
           return CE_OK;
       }

        CMP_DWORD compressedBlock1[2];
        double fError4 = (fError3 == 0.0) ? FLT_MAX : CompRGBBlock((DWORD*)rgbBlock, compressedBlock1, BLOCK_SIZE_4X4, RG, GG, BG, nEndpoints[1], nIndices[1], 4, m_bUseSSE2, m_b3DRefinement, m_nRefinementSteps, pfChannelWeights, bDXT1UseAlpha, nDXT1AlphaThreshold);

        //if (fError3 > 0.0f)
        //    printf(": %2d %2d %2d %2d %2d\n",nIndices[1][0],nIndices[1][1],nIndices[1][2],nIndices[1][3],nIndices[1][4]);

        unsigned int nMethod = (fError3 <= fError4) ? 0 : 1;

        unsigned int c0 = ConstructColour((nEndpoints[nMethod][RC][0] >> (8-RG)), (nEndpoints[nMethod][GC][0] >> (8-GG)), (nEndpoints[nMethod][BC][0] >> (8-BG)));
        unsigned int c1 = ConstructColour((nEndpoints[nMethod][RC][1] >> (8-RG)), (nEndpoints[nMethod][GC][1] >> (8-GG)), (nEndpoints[nMethod][BC][1] >> (8-BG)));
        if(nMethod == 1 && c0 <= c1 || nMethod == 0 && c0 > c1)
            compressedBlock[0] = c1 | (c0<<16);
        else
            compressedBlock[0] = c0 | (c1<<16);

        compressedBlock[1] = 0;
        for(int i=0; i<16; i++)
            compressedBlock[1] |= (nIndices[nMethod][i] << (2*i));

    } 
    else {
        CMP_BYTE nEndpoints[3][2];
        CMP_BYTE nIndices[BLOCK_SIZE_4X4];

        CompRGBBlock((DWORD*)rgbBlock,compressedBlock, BLOCK_SIZE_4X4, RG, GG, BG, nEndpoints, nIndices, 4, m_bUseSSE2, m_b3DRefinement, m_nRefinementSteps, pfChannelWeights, bDXT1UseAlpha, nDXT1AlphaThreshold);

        unsigned int c0 = ConstructColour((nEndpoints[RC][0] >> (8-RG)), (nEndpoints[GC][0] >> (8-GG)), (nEndpoints[BC][0] >> (8-BG)));
        unsigned int c1 = ConstructColour((nEndpoints[RC][1] >> (8-RG)), (nEndpoints[GC][1] >> (8-GG)), (nEndpoints[BC][1] >> (8-BG)));
        if(c0 <= c1)
            compressedBlock[0] = c1 | (c0<<16);
        else
            compressedBlock[0] = c0 | (c1<<16);

        compressedBlock[1] = 0;
        for(int i=0; i<16; i++)
            compressedBlock[1] |= (nIndices[i] << (2*i));
    }
#endif

    return CE_OK;
}

CodecError CCodec_DXTC::CompressRGBBlock_Fast(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]) {
    this->CompressRGBBlock(rgbBlock, compressedBlock);
    return CE_OK;
}

CodecError CCodec_DXTC::CompressRGBBlock_SuperFast(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2]) {
    CompressRGBBlock(rgbBlock, compressedBlock);
    return CE_OK;
}

CodecError CCodec_DXTC::CompressRGBABlock_Fast(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]) {
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];
    for(CMP_DWORD i = 0; i < 16; i++)
        alphaBlock[i] = static_cast<CMP_BYTE>(((DWORD*)rgbaBlock)[i] >> RGBA8888_OFFSET_A);

    CodecError err = CompressAlphaBlock_Fast(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    if(err != CE_OK)
        return err;

    return CompressRGBBlock_Fast(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB]);
}

CodecError CCodec_DXTC::CompressRGBABlock_SuperFast(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]) {
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];
    for(CMP_DWORD i = 0; i < 16; i++)
        alphaBlock[i] = static_cast<CMP_BYTE>(((DWORD*)rgbaBlock)[i] >> RGBA8888_OFFSET_A);

    CodecError err = CompressAlphaBlock_Fast(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    if(err != CE_OK)
        return err;

    return CompressRGBBlock_SuperFast(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB]);
}

CodecError CCodec_DXTC::CompressRGBABlock_ExplicitAlpha_Fast(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]) {
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];
    for(CMP_DWORD i = 0; i < 16; i++)
        alphaBlock[i] = static_cast<CMP_BYTE>(((DWORD*)rgbaBlock)[i] >> RGBA8888_OFFSET_A);

    CodecError err = CompressExplicitAlphaBlock_Fast(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    if(err != CE_OK)
        return err;

    return CompressRGBBlock_Fast(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB]);
}

CodecError CCodec_DXTC::CompressRGBABlock_ExplicitAlpha_SuperFast(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]) {
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];
    for(CMP_DWORD i = 0; i < 16; i++)
        alphaBlock[i] = static_cast<CMP_BYTE>(((DWORD*)rgbaBlock)[i] >> RGBA8888_OFFSET_A);

    CodecError err = CompressExplicitAlphaBlock_Fast(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    if(err != CE_OK)
        return err;

    return CompressRGBBlock_SuperFast(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB]);
}

CodecError CCodec_DXTC::CompressRGBABlock(CODECFLOAT rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4], CODECFLOAT* pfChannelWeights) {
    CODECFLOAT alphaBlock[BLOCK_SIZE_4X4];
    for(CMP_DWORD i = 0; i < 16; i++)
        alphaBlock[i] = rgbaBlock[(i * 4) + RGBA32F_OFFSET_A];

    CodecError err = CompressAlphaBlock(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    if(err != CE_OK)
        return err;

    return CompressRGBBlock(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB], pfChannelWeights, false);
}

CodecError CCodec_DXTC::CompressRGBABlock_ExplicitAlpha(CODECFLOAT rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4], CODECFLOAT* pfChannelWeights) {
    CODECFLOAT alphaBlock[BLOCK_SIZE_4X4];
    for(CMP_DWORD i = 0; i < 16; i++)
        alphaBlock[i] = rgbaBlock[(i * 4) + RGBA32F_OFFSET_A];

    CodecError err = CompressExplicitAlphaBlock(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    if(err != CE_OK)
        return err;

    return CompressRGBBlock(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB], pfChannelWeights, false);
}


// ToDo Remove this interface
CodecError CCodec_DXTC::CompressRGBBlock(CODECFLOAT rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2], CODECFLOAT* pfChannelWeights, bool bDXT1, bool bDXT1UseAlpha, CODECFLOAT fDXT1AlphaThreshold) {
    // Use Legacy API
#ifndef USE_CMP_CORE_API
     int RC = 2, GC = 1, BC = 0;
#endif

    if(bDXT1) {
        CMP_BYTE nEndpoints[2][3][2];
        CMP_BYTE nIndices[2][BLOCK_SIZE_4X4];

        double fError3 = CompRGBBlock(rgbBlock,compressedBlock, BLOCK_SIZE_4X4, RG, GG, BG, nEndpoints[0], nIndices[0], 3, m_bUseSSE2, m_b3DRefinement, m_nRefinementSteps, pfChannelWeights, bDXT1UseAlpha, fDXT1AlphaThreshold);

        CMP_DWORD compressedBlock1[2];
        double fError4 = (fError3 == 0.0) ? FLT_MAX : CompRGBBlock(rgbBlock,compressedBlock1, BLOCK_SIZE_4X4, RG, GG, BG, nEndpoints[1], nIndices[1], 4, m_bUseSSE2, m_b3DRefinement, m_nRefinementSteps, pfChannelWeights, bDXT1UseAlpha, fDXT1AlphaThreshold);

        unsigned int nMethod = (fError3 <= fError4) ? 0 : 1;
        unsigned int c0 = ConstructColour((nEndpoints[nMethod][RC][0] >> (8-RG)), (nEndpoints[nMethod][GC][0] >> (8-GG)), (nEndpoints[nMethod][BC][0] >> (8-BG)));
        unsigned int c1 = ConstructColour((nEndpoints[nMethod][RC][1] >> (8-RG)), (nEndpoints[nMethod][GC][1] >> (8-GG)), (nEndpoints[nMethod][BC][1] >> (8-BG)));
        if(nMethod == 1 && c0 <= c1 || nMethod == 0 && c0 > c1)
            compressedBlock[0] = c1 | (c0<<16);
        else
            compressedBlock[0] = c0 | (c1<<16);

        compressedBlock[1] = 0;
        for(int i=0; i<16; i++)
            compressedBlock[1] |= (nIndices[nMethod][i] << (2*i));
    } else {
        CMP_BYTE nEndpoints[3][2];
        CMP_BYTE nIndices[BLOCK_SIZE_4X4];

        CompRGBBlock(rgbBlock,compressedBlock, BLOCK_SIZE_4X4, RG, GG, BG, nEndpoints, nIndices, 4, m_bUseSSE2, m_b3DRefinement, m_nRefinementSteps, pfChannelWeights, bDXT1UseAlpha, fDXT1AlphaThreshold);

        unsigned int c0 = ConstructColour((nEndpoints[RC][0] >> (8-RG)), (nEndpoints[GC][0] >> (8-GG)), (nEndpoints[BC][0] >> (8-BG)));
        unsigned int c1 = ConstructColour((nEndpoints[RC][1] >> (8-RG)), (nEndpoints[GC][1] >> (8-GG)), (nEndpoints[BC][1] >> (8-BG)));
        if(c0 <= c1)
            compressedBlock[0] = c1 | (c0<<16);
        else
            compressedBlock[0] = c0 | (c1<<16);

        compressedBlock[1] = 0;
        for(int i=0; i<16; i++)
            compressedBlock[1] |= (nIndices[i] << (2*i));
    }

    return CE_OK;
}

void CCodec_DXTC::DecompressRGBABlock(CODECFLOAT rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]) {
    CODECFLOAT alphaBlock[BLOCK_SIZE_4X4];

    DecompressAlphaBlock(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    DecompressRGBBlock(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB], false);

    for(CMP_DWORD i = 0; i < 16; i++)
        rgbaBlock[(i * 4) + RGBA32F_OFFSET_A] = alphaBlock[i];
}

void CCodec_DXTC::DecompressRGBABlock_ExplicitAlpha(CODECFLOAT rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4]) {
    CODECFLOAT alphaBlock[BLOCK_SIZE_4X4];

    DecompressExplicitAlphaBlock(alphaBlock, &compressedBlock[DXTC_OFFSET_ALPHA]);
    DecompressRGBBlock(rgbaBlock, &compressedBlock[DXTC_OFFSET_RGB], false);

    for(CMP_DWORD i = 0; i < 16; i++)
        rgbaBlock[(i * 4) + RGBA32F_OFFSET_A] = alphaBlock[i];
}

// This function decompresses a DXT colour block
// The block is decompressed to 8 bits per channel
// The return data is rgbBlock[BLOCK_SIZE_4X4X4] = [RGBA,RGBA,...]
void CCodec_DXTC::DecompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2], bool bDXT1) {
    CMP_DWORD n0 = compressedBlock[0] & 0xffff;
    CMP_DWORD n1 = compressedBlock[0]>>16;
    CMP_DWORD r0;
    CMP_DWORD g0;
    CMP_DWORD b0;
    CMP_DWORD r1;
    CMP_DWORD g1;
    CMP_DWORD b1;

    // Decode RGB bits
    r0 = ((n0 & 0xf800) >> 8);
    g0 = ((n0 & 0x07e0) >> 3);
    b0 = ((n0 & 0x001f) << 3);

    r1 = ((n1 & 0xf800) >> 8);
    g1 = ((n1 & 0x07e0) >> 3);
    b1 = ((n1 & 0x001f) << 3);

    // Apply the lower bit replication to give full dynamic range
    r0 += (r0>>5);
    r1 += (r1>>5);
    g0 += (g0>>6);
    g1 += (g1>>6);
    b0 += (b0>>5);
    b1 += (b1>>5);

    // Save the pixel in ABGR
    CMP_DWORD c0 = 0xff000000 | (b0<<16) | (g0<<8) | r0;
    CMP_DWORD c1 = 0xff000000 | (b1<<16) | (g1<<8) | r1;

    if(!bDXT1 || n0 > n1) {
        CMP_DWORD c2 = 0xff000000 | (((2*b0+b1)/3)<<16) | (((2*g0+g1)/3)<<8) | (((2*r0+r1)/3));
        CMP_DWORD c3 = 0xff000000 | (((2*b1+b0)/3)<<16) | (((2*g1+g0)/3)<<8) | (((2*r1+r0)/3));

        for(int i=0; i<16; i++) {
            int index = (compressedBlock[1] >> (2 * i)) & 3;
            switch(index) {
            case 0:
                ((DWORD*)rgbBlock)[i] = c0;
                break;
            case 1:
                ((DWORD*)rgbBlock)[i] = c1;
                break;
            case 2:
                ((DWORD*)rgbBlock)[i] = c2;
                break;
            case 3:
                ((DWORD*)rgbBlock)[i] = c3;
                break;
            }
        }
    } else {
        // Transparent decode
        CMP_DWORD c2 = 0xff000000 | (((b0+b1)/2)<<16) | (((g0+g1)/2)<<8) | (((r0+r1)/2));

        for(int i=0; i<16; i++) {
            int index = (compressedBlock[1] >> (2 * i)) & 3;
            switch(index) {
            case 0:
                ((DWORD*)rgbBlock)[i] = c0;
                break;
            case 1:
                ((DWORD*)rgbBlock)[i] = c1;
                break;
            case 2:
                ((DWORD*)rgbBlock)[i] = c2;
                break;
            case 3:
                ((DWORD*)rgbBlock)[i] = 0x00000000;
                break;
            }
        }
    }
}

//
// This function decompresses a DXT colour block
// The block is decompressed to 8 bits per channel
//
void CCodec_DXTC::DecompressRGBBlock(CODECFLOAT rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2], bool bDXT1) {
    CMP_DWORD n0 = compressedBlock[0] & 0xffff;
    CMP_DWORD n1 = compressedBlock[0]>>16;
    CMP_DWORD r0;
    CMP_DWORD g0;
    CMP_DWORD b0;
    CMP_DWORD r1;
    CMP_DWORD g1;
    CMP_DWORD b1;

    r0 = ((n0 & 0xf800) >> 8);
    g0 = ((n0 & 0x07e0) >> 3);
    b0 = ((n0 & 0x001f) << 3);

    r1 = ((n1 & 0xf800) >> 8);
    g1 = ((n1 & 0x07e0) >> 3);
    b1 = ((n1 & 0x001f) << 3);

    // Apply the lower bit replication to give full dynamic range
    r0 += (r0>>5);
    r1 += (r1>>5);
    g0 += (g0>>6);
    g1 += (g1>>6);
    b0 += (b0>>5);
    b1 += (b1>>5);

#ifdef PRINT_DECODE_INFO
    FILE *gt_File_decode = fopen("decode_patterns.txt", "a");
#endif

    CODECFLOAT c0[4], c1[4], c2[4], c3[4];
    c0[RGBA32F_OFFSET_A] = 1.0;
    c0[RGBA32F_OFFSET_R] = CONVERT_BYTE_TO_FLOAT(r0);
    c0[RGBA32F_OFFSET_G] = CONVERT_BYTE_TO_FLOAT(g0);
    c0[RGBA32F_OFFSET_B] = CONVERT_BYTE_TO_FLOAT(b0);

    c1[RGBA32F_OFFSET_A] = 1.0;
    c1[RGBA32F_OFFSET_R] = CONVERT_BYTE_TO_FLOAT(r1);
    c1[RGBA32F_OFFSET_G] = CONVERT_BYTE_TO_FLOAT(g1);
    c1[RGBA32F_OFFSET_B] = CONVERT_BYTE_TO_FLOAT(b1);

    if(!bDXT1 || n0 > n1) {
#ifdef PRINT_DECODE_INFO
        fprintf(gt_File_decode, "BC1A              : C0(%3d,%3d,%3d) C1(%3d,%3d,%3d)                                 index = ",
                r0, g0, b0, r1, g1, b1
               );
#endif


        c2[RGBA32F_OFFSET_A] = 1.0;
        c2[RGBA32F_OFFSET_R] = ((2*c0[RGBA32F_OFFSET_R]+c1[RGBA32F_OFFSET_R])/3);
        c2[RGBA32F_OFFSET_G] = ((2*c0[RGBA32F_OFFSET_G]+c1[RGBA32F_OFFSET_G])/3);
        c2[RGBA32F_OFFSET_B] = ((2*c0[RGBA32F_OFFSET_B]+c1[RGBA32F_OFFSET_B])/3);

        c3[RGBA32F_OFFSET_A] = 1.0;
        c3[RGBA32F_OFFSET_R] = ((2*c1[RGBA32F_OFFSET_R]+c0[RGBA32F_OFFSET_R])/3);
        c3[RGBA32F_OFFSET_G] = ((2*c1[RGBA32F_OFFSET_G]+c0[RGBA32F_OFFSET_G])/3);
        c3[RGBA32F_OFFSET_B] = ((2*c1[RGBA32F_OFFSET_B]+c0[RGBA32F_OFFSET_B])/3);

        for(int i=0; i<16; i++) {

            int index = (compressedBlock[1] >> (2 * i)) & 3;
#ifdef PRINT_DECODE_INFO
            fprintf(gt_File_decode, "%2d,", index);
#endif
            switch(index) {
            case 0:
                memcpy(&rgbBlock[i*4], c0, 4 * sizeof(CODECFLOAT));
                break;
            case 1:
                memcpy(&rgbBlock[i*4], c1, 4 * sizeof(CODECFLOAT));
                break;
            case 2:
                memcpy(&rgbBlock[i*4], c2, 4 * sizeof(CODECFLOAT));
                break;
            case 3:
                memcpy(&rgbBlock[i*4], c3, 4 * sizeof(CODECFLOAT));
                break;
            }
        }
    } else {

#ifdef PRINT_DECODE_INFO
        fprintf(gt_File_decode, "BC1AT             : C0(%3d,%3d,%3d) C1(%3d,%3d,%3d)                                 index = ",
                r0, g0, b0, r1, g1, b1
               );
#endif

        // Transparent decode
        c2[RGBA32F_OFFSET_A] = 1.0;
        c2[RGBA32F_OFFSET_R] = ((c0[RGBA32F_OFFSET_R]+c1[RGBA32F_OFFSET_R])/2);
        c2[RGBA32F_OFFSET_G] = ((c0[RGBA32F_OFFSET_G]+c1[RGBA32F_OFFSET_G])/2);
        c2[RGBA32F_OFFSET_B] = ((c0[RGBA32F_OFFSET_B]+c1[RGBA32F_OFFSET_B])/2);

        c3[RGBA32F_OFFSET_A] = 0.0;
        c3[RGBA32F_OFFSET_R] = 0.0;
        c3[RGBA32F_OFFSET_G] = 0.0;
        c3[RGBA32F_OFFSET_B] = 0.0;

        for(int i=0; i<16; i++) {

            int index = (compressedBlock[1] >> (2 * i)) & 3;
#ifdef PRINT_DECODE_INFO
            fprintf(gt_File_decode, "%2d,", index);
#endif

            switch(index) {
            case 0:
                memcpy(&rgbBlock[i*4], c0, 4 * sizeof(CODECFLOAT));
                break;
            case 1:
                memcpy(&rgbBlock[i*4], c1, 4 * sizeof(CODECFLOAT));
                break;
            case 2:
                memcpy(&rgbBlock[i*4], c2, 4 * sizeof(CODECFLOAT));
                break;
            case 3:
                memcpy(&rgbBlock[i*4], c3, 4 * sizeof(CODECFLOAT));
                break;
            }
        }
    }
#ifdef PRINT_DECODE_INFO
    fprintf(gt_File_decode, "\n");
    fclose(gt_File_decode);
#endif
}

CODECFLOAT* CCodec_DXTC::CalculateColourWeightings(CMP_BYTE block[BLOCK_SIZE_4X4X4]) {
    if(!m_bUseChannelWeighting)
        return NULL;

    if(m_bUseAdaptiveWeighting) {
        float medianR = 0.0f, medianG = 0.0f, medianB = 0.0f;

        for(CMP_DWORD k=0; k<BLOCK_SIZE_4X4; k++) {
            CMP_DWORD R = (block[k] & 0xff0000) >> 16;
            CMP_DWORD G = (block[k] & 0xff00) >> 8;
            CMP_DWORD B = block[k] & 0xff;

            medianR += R;
            medianG += G;
            medianB += B;
        }

        medianR /= BLOCK_SIZE_4X4;
        medianG /= BLOCK_SIZE_4X4;
        medianB /= BLOCK_SIZE_4X4;

        // Now skew the colour weightings based on the gravity center of the block
        float largest = max(max(medianR, medianG), medianB);

        if(largest > 0) {
            medianR /= largest;
            medianG /= largest;
            medianB /= largest;
        } else
            medianR = medianG = medianB = 1.0f;

        // Scale weightings back up to 1.0f
        CODECFLOAT fWeightScale = 1.0f / (m_fBaseChannelWeights[0] + m_fBaseChannelWeights[1] + m_fBaseChannelWeights[2]);
        m_fChannelWeights[0] = m_fBaseChannelWeights[0] * fWeightScale;
        m_fChannelWeights[1] = m_fBaseChannelWeights[1] * fWeightScale;
        m_fChannelWeights[2] = m_fBaseChannelWeights[2] * fWeightScale;
        m_fChannelWeights[0] = ((m_fChannelWeights[0] * 3 * medianR) + m_fChannelWeights[0]) * 0.25f;
        m_fChannelWeights[1] = ((m_fChannelWeights[1] * 3 * medianG) + m_fChannelWeights[1]) * 0.25f;
        m_fChannelWeights[2] = ((m_fChannelWeights[2] * 3 * medianB) + m_fChannelWeights[2]) * 0.25f;
        fWeightScale = 1.0f / (m_fChannelWeights[0] + m_fChannelWeights[1] + m_fChannelWeights[2]);
        m_fChannelWeights[0] *= fWeightScale;
        m_fChannelWeights[1] *= fWeightScale;
        m_fChannelWeights[2] *= fWeightScale;
    }

    return m_fChannelWeights;
}

CODECFLOAT* CCodec_DXTC::CalculateColourWeightings(CODECFLOAT block[BLOCK_SIZE_4X4X4]) {
    if(!m_bUseChannelWeighting)
        return NULL;

    if(m_bUseAdaptiveWeighting) {
        float medianR = 0.0f, medianG = 0.0f, medianB = 0.0f;

        for(CMP_DWORD k=0; k<BLOCK_SIZE_4X4; k++) {
            *block++;
            medianB += *block++;
            medianG += *block++;
            medianR += *block++;
        }

        medianR /= BLOCK_SIZE_4X4;
        medianG /= BLOCK_SIZE_4X4;
        medianB /= BLOCK_SIZE_4X4;

        // Now skew the colour weightings based on the gravity center of the block
        float largest = max(max(medianR, medianG), medianB);

        if(largest > 0) {
            medianR /= largest;
            medianG /= largest;
            medianB /= largest;
        } else
            medianR = medianG = medianB = 1.0f;

        // Scale weightings back up to 1.0f
        CODECFLOAT fWeightScale = 1.0f / (m_fBaseChannelWeights[0] + m_fBaseChannelWeights[1] + m_fBaseChannelWeights[2]);
        m_fChannelWeights[0] *= m_fBaseChannelWeights[0] * fWeightScale;
        m_fChannelWeights[1] *= m_fBaseChannelWeights[1] * fWeightScale;
        m_fChannelWeights[2] *= m_fBaseChannelWeights[2] * fWeightScale;
        m_fChannelWeights[0] = ((m_fChannelWeights[0] * 3 * medianR) + m_fChannelWeights[0]) * 0.25f;
        m_fChannelWeights[1] = ((m_fChannelWeights[1] * 3 * medianG) + m_fChannelWeights[1]) * 0.25f;
        m_fChannelWeights[2] = ((m_fChannelWeights[2] * 3 * medianB) + m_fChannelWeights[2]) * 0.25f;
        fWeightScale = 1.0f / (m_fChannelWeights[0] + m_fChannelWeights[1] + m_fChannelWeights[2]);
        m_fChannelWeights[0] *= fWeightScale;
        m_fChannelWeights[1] *= fWeightScale;
        m_fChannelWeights[2] *= fWeightScale;
    }

    return m_fChannelWeights;
}