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
//  File Name:   Codec_ATC.cpp  
//  Description: implementation of the CCodec_ATC class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Codec_ATC.h"
#include "Compressonator_tc.h"
#include "CompressonatorXCodec.h"

using namespace CMP;

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_ATC::CCodec_ATC(CodecType codecType) :
CCodec_Block_4x4(codecType)
{

}

CCodec_ATC::~CCodec_ATC()
{

}

CodecError CCodec_ATC::CompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2])
{
    Color888_t srcRGB[4][4];
    unsigned int colorLow565or1555, colorHigh565;

    for(int y = 0; y < 4; y++)
    {
        for(DWORD x = 0; x < 4; x++)
        {
            srcRGB[x][y].red    = rgbBlock[(x*16) + (y*4) + ATC_RGBA8888_CHANNEL_R]; 
            srcRGB[x][y].green  = rgbBlock[(x*16) + (y*4) + ATC_RGBA8888_CHANNEL_G];
            srcRGB[x][y].blue   = rgbBlock[(x*16) + (y*4) + ATC_RGBA8888_CHANNEL_B];

        }
    }

    compressedBlock[1] = atiEncodeRGBBlockATITC(&srcRGB, &colorLow565or1555, &colorHigh565);
    compressedBlock[0] = (colorHigh565 << 16) | colorLow565or1555;

    return CE_OK;
}

void CCodec_ATC::DecompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2])
{
    Color888_t destRGB[4][4];

    unsigned int colorLow565or1555 = compressedBlock[0] & 0xffff;
    unsigned int colorHigh565 = (compressedBlock[0] >> 16) & 0xffff;

    atiDecodeRGBBlockATITC(&destRGB, compressedBlock[1], colorLow565or1555, colorHigh565);

    for(DWORD y = 0; y < 4; y++)
    {
        for(DWORD x = 0; x < 4; x++)
        {
            rgbBlock[(x*16) + (y*4) + ATC_RGBA8888_CHANNEL_R] = destRGB[x][y].red;
            rgbBlock[(x*16) + (y*4) + ATC_RGBA8888_CHANNEL_G] = destRGB[x][y].green;
            rgbBlock[(x*16) + (y*4) + ATC_RGBA8888_CHANNEL_B] = destRGB[x][y].blue;
        }
    }
}

CodecError CCodec_ATC::CompressRGBABlock_ExplicitAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4])
{
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];
    for(CMP_DWORD i = 0; i < 16; i++)
        alphaBlock[i] = static_cast<CMP_BYTE>(((DWORD*)rgbaBlock)[i] >> ATC_RGBA8888_OFFSET_A);

    CodecError err = CompressExplicitAlphaBlock(alphaBlock, &compressedBlock[ATC_OFFSET_ALPHA]);
    if(err != CE_OK)
        return err;

    return CompressRGBBlock(rgbaBlock, &compressedBlock[ATC_OFFSET_RGB]);
}

void CCodec_ATC::DecompressRGBABlock_ExplicitAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4])
{
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];

    DecompressExplicitAlphaBlock(alphaBlock, &compressedBlock[ATC_OFFSET_ALPHA]);
    DecompressRGBBlock(rgbaBlock, &compressedBlock[ATC_OFFSET_RGB]);

    for(CMP_DWORD i = 0; i < 16; i++)
        ((DWORD*)rgbaBlock)[i] = (alphaBlock[i] << ATC_RGBA8888_OFFSET_A) | (((DWORD*)rgbaBlock)[i] & ~(BYTE_MASK << ATC_RGBA8888_OFFSET_A));
}

CodecError CCodec_ATC::CompressRGBABlock_InterpolatedAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4])
{
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];
    for(CMP_DWORD i = 0; i < 16; i++)
        alphaBlock[i] = static_cast<CMP_BYTE>(((DWORD*)rgbaBlock)[i] >> ATC_RGBA8888_OFFSET_A);

    CodecError err = CompressInterpolatedAlphaBlock(alphaBlock, &compressedBlock[ATC_OFFSET_ALPHA]);
    if(err != CE_OK)
        return err;

    return CompressRGBBlock(rgbaBlock, &compressedBlock[ATC_OFFSET_RGB]);
}

void CCodec_ATC::DecompressRGBABlock_InterpolatedAlpha(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4])
{
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];

    DecompressInterpolatedAlphaBlock(alphaBlock, &compressedBlock[ATC_OFFSET_ALPHA]);
    DecompressRGBBlock(rgbaBlock, &compressedBlock[ATC_OFFSET_RGB]);

    for(CMP_DWORD i = 0; i < 16; i++)
        ((DWORD*)rgbaBlock)[i] = (alphaBlock[i] << ATC_RGBA8888_OFFSET_A) | (((DWORD*)rgbaBlock)[i] & ~(BYTE_MASK << ATC_RGBA8888_OFFSET_A));
}

#define EXPLICIT_ALPHA_PIXEL_MASK 0xf
#define EXPLICIT_ALPHA_PIXEL_BPP 4
CodecError CCodec_ATC::CompressExplicitAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    compressedBlock[0] = compressedBlock[1] = 0;
    for(int i=0; i<16; i++)
    {
        int nBlock = i < 8 ? 0 : 1;
        CMP_BYTE cAlpha = (CMP_BYTE) ((alphaBlock[i] + ((alphaBlock[i] >> EXPLICIT_ALPHA_PIXEL_BPP) < 0x8 ? 7 : 8) - (alphaBlock[i] >> EXPLICIT_ALPHA_PIXEL_BPP)) >> EXPLICIT_ALPHA_PIXEL_BPP);
        if(cAlpha > EXPLICIT_ALPHA_PIXEL_MASK)
            cAlpha = EXPLICIT_ALPHA_PIXEL_MASK;
        compressedBlock[nBlock] |= (cAlpha << ((i % 8) * EXPLICIT_ALPHA_PIXEL_BPP));
    }

    return CE_OK;
}

void CCodec_ATC::DecompressExplicitAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    for(int i=0;i<16;i++)
    {
        int nBlock = i < 8 ? 0 : 1;
        CMP_BYTE cAlpha = (CMP_BYTE) ((compressedBlock[nBlock] >> ((i % 8) * EXPLICIT_ALPHA_PIXEL_BPP)) & EXPLICIT_ALPHA_PIXEL_MASK);
        alphaBlock[i] = (CMP_BYTE) ((cAlpha << EXPLICIT_ALPHA_PIXEL_BPP) | cAlpha);
    }
}

CodecError CCodec_ATC::CompressInterpolatedAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    BYTE nEndpoints[2][2];
    BYTE nIndices[2][BLOCK_SIZE_4X4];
    float fError8 = CompBlock1X(alphaBlock, BLOCK_SIZE_4X4, nEndpoints[0], nIndices[0], 8, false, m_bUseSSE2, 8, 0, true);
    float fError6 = (fError8 == 0.f) ? FLT_MAX : CompBlock1X(alphaBlock, BLOCK_SIZE_4X4, nEndpoints[1], nIndices[1], 6, true, m_bUseSSE2, 8, 0, true);
    if(fError8 <= fError6)
        EncodeAlphaBlock(compressedBlock, nEndpoints[0], nIndices[0]);
    else
        EncodeAlphaBlock(compressedBlock, nEndpoints[1], nIndices[1]);

    return CE_OK;
}

void CCodec_ATC::EncodeAlphaBlock(CMP_DWORD compressedBlock[2], BYTE nEndpoints[2], BYTE nIndices[BLOCK_SIZE_4X4])
{
    compressedBlock[0] = ((int)nEndpoints[0]) | (((int)nEndpoints[1])<<8);
    compressedBlock[1] = 0;

    for(int i = 0; i < BLOCK_SIZE_4X4; i++)
    {
        if(i < 5)
            compressedBlock[0] |= (nIndices[i] & 0x7) << (16 + (i * 3));
        else if(i > 5)
            compressedBlock[1] |= (nIndices[i] & 0x7) << (2 + (i-6) * 3);
        else
        {
            compressedBlock[0] |= (nIndices[i] & 0x1) << 31;
            compressedBlock[1] |= (nIndices[i] & 0x6) >> 1;
        }
    }
}

void CCodec_ATC::DecompressInterpolatedAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    CMP_BYTE alpha[8];
    GetCompressedAlphaRamp(alpha, compressedBlock);

    for(int i = 0; i < BLOCK_SIZE_4X4; i++)
    {
        CMP_DWORD index;
        if(i < 5)
            index = (compressedBlock[0] & (0x7 << (16 + (i * 3)))) >> (16 + (i * 3));
        else if(i > 5)
            index = (compressedBlock[1] & (0x7 << (2 + (i-6)*3))) >> (2 + (i-6)*3);
        else
        {
            index = (compressedBlock[0] & 0x80000000) >> 31;
            index |= (compressedBlock[1] & 0x3) << 1;
        }

        alphaBlock[i] = alpha[index];
    }
}

void CCodec_ATC::GetCompressedAlphaRamp(CMP_BYTE alpha[8], CMP_DWORD compressedBlock[2])
{
    alpha[0] = (CMP_BYTE)(compressedBlock[0] & 0xff);
    alpha[1] = (CMP_BYTE)((compressedBlock[0]>>8) & 0xff);

    if (alpha[0] > alpha[1])
    {
        // 8-alpha block:  derive the other six alphas.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = static_cast<CMP_BYTE>((6 * alpha[0] + 1 * alpha[1] + 3) / 7);    // bit code 010
        alpha[3] = static_cast<CMP_BYTE>((5 * alpha[0] + 2 * alpha[1] + 3) / 7);    // bit code 011
        alpha[4] = static_cast<CMP_BYTE>((4 * alpha[0] + 3 * alpha[1] + 3) / 7);    // bit code 100
        alpha[5] = static_cast<CMP_BYTE>((3 * alpha[0] + 4 * alpha[1] + 3) / 7);    // bit code 101
        alpha[6] = static_cast<CMP_BYTE>((2 * alpha[0] + 5 * alpha[1] + 3) / 7);    // bit code 110
        alpha[7] = static_cast<CMP_BYTE>((1 * alpha[0] + 6 * alpha[1] + 3) / 7);    // bit code 111
    }
    else
    {
        // 6-alpha block.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = static_cast<CMP_BYTE>((4 * alpha[0] + 1 * alpha[1] + 2) / 5);  // Bit code 010
        alpha[3] = static_cast<CMP_BYTE>((3 * alpha[0] + 2 * alpha[1] + 2) / 5);  // Bit code 011
        alpha[4] = static_cast<CMP_BYTE>((2 * alpha[0] + 3 * alpha[1] + 2) / 5);  // Bit code 100
        alpha[5] = static_cast<CMP_BYTE>((1 * alpha[0] + 4 * alpha[1] + 2) / 5);  // Bit code 101
        alpha[6] = 0;                                      // Bit code 110
        alpha[7] = 255;                                    // Bit code 111
    }
}