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

#include "Common.h"
#include "Codec_DXTC.h"
#include "CompressonatorXCodec.h"
#include "dxtc_v11_compress.h"

CodecError CCodec_DXTC::CompressAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
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

CodecError CCodec_DXTC::CompressAlphaBlock_Fast(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
#ifdef _WIN64
    CompressAlphaBlock(alphaBlock, compressedBlock);
#else // !_WIN64
    DXTCV11CompressAlphaBlock(alphaBlock, compressedBlock);
#endif
    return CE_OK;
}

CodecError CCodec_DXTC::CompressAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2])
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

void CCodec_DXTC::EncodeAlphaBlock(CMP_DWORD compressedBlock[2], BYTE nEndpoints[2], BYTE nIndices[BLOCK_SIZE_4X4])
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


//
// This function decompresses a block
//
void CCodec_DXTC::DecompressAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
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

void CCodec_DXTC::DecompressAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    CODECFLOAT alpha[8];
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

#define EXPLICIT_ALPHA_PIXEL_MASK 0xf
#define EXPLICIT_ALPHA_PIXEL_BPP 4
CodecError CCodec_DXTC::CompressExplicitAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    DXTCV11CompressExplicitAlphaBlock(alphaBlock, compressedBlock);

    return CE_OK;
}

CodecError CCodec_DXTC::CompressExplicitAlphaBlock_Fast(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
#ifdef _WIN32
#ifndef DISABLE_TESTCODE
    DXTCV11CompressExplicitAlphaBlockMMX(alphaBlock, compressedBlock);
#else
    CompressExplicitAlphaBlock(alphaBlock, compressedBlock);
#endif
#else
    CompressExplicitAlphaBlock(alphaBlock, compressedBlock);
#endif

    return CE_OK;
}

CodecError CCodec_DXTC::CompressExplicitAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    compressedBlock[0] = compressedBlock[1] = 0;
    for(int i=0; i<16; i++)
    {
        int nBlock = i < 8 ? 0 : 1;
        CMP_BYTE cAlpha = CONVERT_FLOAT_TO_BYTE(alphaBlock[i]);
        cAlpha = (CMP_BYTE) ((cAlpha + ((cAlpha >> EXPLICIT_ALPHA_PIXEL_BPP) < 0x8 ? 7 : 8) - (cAlpha >> EXPLICIT_ALPHA_PIXEL_BPP)) >> EXPLICIT_ALPHA_PIXEL_BPP);
        if(cAlpha > EXPLICIT_ALPHA_PIXEL_MASK)
            cAlpha = EXPLICIT_ALPHA_PIXEL_MASK;
        compressedBlock[nBlock] |= (cAlpha << ((i % 8) * EXPLICIT_ALPHA_PIXEL_BPP));
    }

    return CE_OK;
}

//
// This function decompresses an explicit alpha block (DXT3)
//
void CCodec_DXTC::DecompressExplicitAlphaBlock(CMP_BYTE alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    for(int i=0;i<16;i++)
    {
        int nBlock = i < 8 ? 0 : 1;
        CMP_BYTE cAlpha = (CMP_BYTE) ((compressedBlock[nBlock] >> ((i % 8) * EXPLICIT_ALPHA_PIXEL_BPP)) & EXPLICIT_ALPHA_PIXEL_MASK);
        alphaBlock[i] = (CMP_BYTE) ((cAlpha << EXPLICIT_ALPHA_PIXEL_BPP) | cAlpha);
    }
}

void CCodec_DXTC::DecompressExplicitAlphaBlock(CODECFLOAT alphaBlock[BLOCK_SIZE_4X4], CMP_DWORD compressedBlock[2])
{
    for(int i=0;i<16;i++)
    {
        int nBlock = i < 8 ? 0 : 1;
        CMP_BYTE cAlpha = (CMP_BYTE) ((compressedBlock[nBlock] >> ((i % 8) * EXPLICIT_ALPHA_PIXEL_BPP)) & EXPLICIT_ALPHA_PIXEL_MASK);
        alphaBlock[i] = CONVERT_BYTE_TO_FLOAT((cAlpha << EXPLICIT_ALPHA_PIXEL_BPP) | cAlpha);
    }
}

void CCodec_DXTC::GetCompressedAlphaRamp(CMP_BYTE alpha[8], CMP_DWORD compressedBlock[2])
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

void CCodec_DXTC::GetCompressedAlphaRamp(CODECFLOAT alpha[8], CMP_DWORD compressedBlock[2])
{
    alpha[0] = CONVERT_BYTE_TO_FLOAT(compressedBlock[0] & 0xff);
    alpha[1] = CONVERT_BYTE_TO_FLOAT((compressedBlock[0]>>8) & 0xff);

    if (alpha[0] > alpha[1])
    {
        // 8-alpha block:  derive the other six alphas.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = (6 * alpha[0] + 1 * alpha[1]) / 7;    // bit code 010
        alpha[3] = (5 * alpha[0] + 2 * alpha[1]) / 7;    // bit code 011
        alpha[4] = (4 * alpha[0] + 3 * alpha[1]) / 7;    // bit code 100
        alpha[5] = (3 * alpha[0] + 4 * alpha[1]) / 7;    // bit code 101
        alpha[6] = (2 * alpha[0] + 5 * alpha[1]) / 7;    // bit code 110
        alpha[7] = (1 * alpha[0] + 6 * alpha[1]) / 7;    // bit code 111
    }
    else
    {
        // 6-alpha block.
        // Bit code 000 = alpha_0, 001 = alpha_1, others are interpolated.
        alpha[2] = (4 * alpha[0] + 1 * alpha[1]) / 5;  // Bit code 010
        alpha[3] = (3 * alpha[0] + 2 * alpha[1]) / 5;  // Bit code 011
        alpha[4] = (2 * alpha[0] + 3 * alpha[1]) / 5;  // Bit code 100
        alpha[5] = (1 * alpha[0] + 4 * alpha[1]) / 5;  // Bit code 101
        alpha[6] = 0;                                      // Bit code 110
        alpha[7] = 1.0f;                                   // Bit code 111
    }
}