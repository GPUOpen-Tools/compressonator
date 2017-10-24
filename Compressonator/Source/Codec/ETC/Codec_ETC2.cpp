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
//  File Name:   Codec_ETC.cpp
//  Description: implementation of the CCodec_ETC class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Codec_ETC2.h"
#include "Compressonator_tc.h"
#include "etcpack.h"
#include "CompressonatorXCodec.h"

using namespace CMP;

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_ETC2::CCodec_ETC2(CodecType codecType) :
CCodec_Block_4x4(codecType)
{
}

CCodec_ETC2::~CCodec_ETC2()
{
}

#define SWIZZLE_DWORD(i) ((((i >> 24) & BYTE_MASK)) | (((i >> 16) & BYTE_MASK) << 8) | (((i >> 8) & BYTE_MASK) << 16) | ((i & BYTE_MASK) << 24))


//-------------------------------------------------------------------------
// WRAPPER ADDED AROUND FUNCTION THAT ENCODES A SINGLE 4X4 BLOCK INTO ETC2
//-------------------------------------------------------------------------
void atiEncodeRGBBlockETC2(
    unsigned char *pPixels,         //Color888_t(* pPixels)[ 4 ] [ 4 ],
    unsigned int *pCompressed1,     // Return value
    unsigned int *pCompressed2,     // Return value
    bool          fast = true      // Mode to use fast or slow
)
{
    unsigned int compressed1, compressed2;       // Used to interface C's (unsigned int*) to C++'s &(unsigned int)
    unsigned char tmp[4 * 4 * 3];                // 

    if (fast)
    {
        compressBlockETC2FastPerceptual((uint8 *)pPixels, (uint8 *)tmp, 4, 4, 0, 0, compressed1, compressed2);
    }
    else
        compressBlockETC2ExhaustivePerceptual((uint8 *)pPixels, (uint8 *)tmp, 4, 4, 0, 0, compressed1, compressed2);

    *pCompressed1 = compressed1;
    *pCompressed2 = compressed2;
}

//-------------------------------------------------------------------------
// WRAPPER ADDED AROUND FUNCTION THAT DECODES A SINGLE 4X4 BLOCK INTO ETC
//-------------------------------------------------------------------------
void atiDecodeRGBBlockETC2(
    unsigned char *pPixels,        //Color888_t(* pPixels)[ 4 ] [ 4 ],
    unsigned int compressed1,
    unsigned int compressed2
    )
{
    decompressBlockETC2(compressed1, compressed2, (uint8*)pPixels, 4, 4, 0, 0);
}

CodecError CCodec_ETC2::CompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2])
{
    Color888_t srcRGB[4][4];
    for (int y = 0; y < 4; y++)
    {
        for (DWORD x = 0; x < 4; x++)
        {
            srcRGB[x][y].red   = rgbBlock[(x * 16) + (y * 4) + RGBA8888_CHANNEL_R];
            srcRGB[x][y].green = rgbBlock[(x * 16) + (y * 4) + RGBA8888_CHANNEL_G];
            srcRGB[x][y].blue  = rgbBlock[(x * 16) + (y * 4) + RGBA8888_CHANNEL_B];
        }
    }

    unsigned int uiCompressedBlockHi, uiCompressedBlockLo;
    atiEncodeRGBBlockETC2((unsigned char *)&srcRGB, &uiCompressedBlockHi, &uiCompressedBlockLo);

    compressedBlock[0] = SWIZZLE_DWORD(uiCompressedBlockHi);
    compressedBlock[1] = SWIZZLE_DWORD(uiCompressedBlockLo);

    return CE_OK;
}


void CCodec_ETC2::DecompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2])
{
    Color888_t destRGB[4][4];

    unsigned int uiCompressedBlockHi = SWIZZLE_DWORD(compressedBlock[0]);
    unsigned int uiCompressedBlockLo = SWIZZLE_DWORD(compressedBlock[1]);

    atiDecodeRGBBlockETC2((unsigned char *)&destRGB, uiCompressedBlockHi, uiCompressedBlockLo);

    for (DWORD y = 0; y < 4; y++)
    {
        for (DWORD x = 0; x < 4; x++)
        {
            // Swizzle output data!! this may need futher checking
            rgbBlock[(x * 16) + (y * 4) + RGBA8888_CHANNEL_R] = destRGB[x][y].red;
            rgbBlock[(x * 16) + (y * 4) + RGBA8888_CHANNEL_G] = destRGB[x][y].green;
            rgbBlock[(x * 16) + (y * 4) + RGBA8888_CHANNEL_B] = destRGB[x][y].blue;
        }
    }
}
