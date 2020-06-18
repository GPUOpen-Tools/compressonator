//===============================================================================
// Copyright (c) 2007-2018  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   Codec_ETC2.cpp  
//  Description: implementation of the CCodec_ETC2 class
//

#include "Codec_ETC2.h"

#pragma warning( push )
#pragma warning(disable:4244)

using namespace CMP;

bool g_alphaTableInitialized = false;


//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

void CCodec_ETC2::setupAlphaTable()
{
    if (!g_alphaTableInitialized)
    {
#ifdef USE_ETCPACK
        setupAlphaTableAndValtab();
#else
        cmp_setupAlphaTable();
#endif
        g_alphaTableInitialized = true;
    }
}

CCodec_ETC2::CCodec_ETC2(CodecType codecType) :
CCodec_Block_4x4(codecType)
{
#ifdef USE_ETCPACK
    readCompressParams();
    switch (codecType)
    {
    case  CT_ETC2_RGB:
    case  CT_ETC2_SRGB:
        format = ETC2PACKAGE_RGB_NO_MIPMAPS;
      break;
    case  CT_ETC2_RGBA1:
    case  CT_ETC2_SRGBA1:
        setupAlphaTable();
        format = ETC2PACKAGE_RGBA1_NO_MIPMAPS;
        break;
    case  CT_ETC2_RGBA:
    case  CT_ETC2_SRGBA:
        setupAlphaTable();
        format = ETC2PACKAGE_RGBA_NO_MIPMAPS;
        break;
    default:
        format = ETC2PACKAGE_RGB_NO_MIPMAPS;
        break;
}
#else
    switch (codecType)
    {
    case  CT_ETC2_RGBA1:
    case  CT_ETC2_SRGBA1:
    case  CT_ETC2_RGBA:
    case  CT_ETC2_SRGBA:
        setupAlphaTable();
        break;
    }
#endif
}

CCodec_ETC2::~CCodec_ETC2()
{

}

//=============
// ETC2 RGB
//=============

CodecError CCodec_ETC2::CompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2])
{
    Color888_t srcRGB[4][4];

    for(int y = 0; y < 4; y++)
    {
        for(DWORD x = 0; x < 4; x++)
        {
            srcRGB[x][y].red    = rgbBlock[(x*16) + (y*4) + RGBA8888_CHANNEL_R];
            srcRGB[x][y].green  = rgbBlock[(x*16) + (y*4) + RGBA8888_CHANNEL_G];
            srcRGB[x][y].blue   = rgbBlock[(x*16) + (y*4) + RGBA8888_CHANNEL_B];
        }
    }


    unsigned int uiCompressedBlockHi, uiCompressedBlockLo;
    unsigned char tmp[4 * 4 * 3];                // Decompressed image

#ifdef USE_ETCPACK
    if (m_fast)
    {
        compressBlockETC2FastPerceptual((uint8 *)&srcRGB, (uint8 *)tmp, 4, 4, 0, 0, uiCompressedBlockHi, uiCompressedBlockLo);
    }
    else
        compressBlockETC2ExhaustivePerceptual((uint8 *)&srcRGB, (uint8 *)tmp, 4, 4, 0, 0, uiCompressedBlockHi, uiCompressedBlockLo);
#else
    cmp_compressBlockETC2FastPerceptual((uint8 *)&srcRGB, (uint8 *)tmp, uiCompressedBlockHi, uiCompressedBlockLo);
#endif

    compressedBlock[0] = SWIZZLE_DWORD(uiCompressedBlockHi);  // Required format for GPU
    compressedBlock[1] = SWIZZLE_DWORD(uiCompressedBlockLo);  // Required format for GPU

    return CE_OK;
}

void CCodec_ETC2::DecompressRGBBlock(CMP_BYTE rgbBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2])
{

    unsigned int uiCompressedBlockHi = SWIZZLE_DWORD(compressedBlock[0]);
    unsigned int uiCompressedBlockLo = SWIZZLE_DWORD(compressedBlock[1]);

#ifdef USE_ETCPACK
    Color888_t destRGB[4][4];
    decompressBlockETC2(uiCompressedBlockHi, uiCompressedBlockLo, (uint8*)destRGB, 4, 4, 0, 0);
    for(DWORD y = 0; y < 4; y++)
    {
        for(DWORD x = 0; x < 4; x++)
        {
            rgbBlock[(x*16) + (y*4) + RGBA8888_CHANNEL_R] = destRGB[x][y].red;
            rgbBlock[(x*16) + (y*4) + RGBA8888_CHANNEL_G] = destRGB[x][y].green;
            rgbBlock[(x*16) + (y*4) + RGBA8888_CHANNEL_B] = destRGB[x][y].blue;
        }
    }
#else
    union
    {
        CMP_DWORD cmp[2];
        uint64    block;
    }data;
    
    union
    {
        Color888_t rgb;
        uint32     blockRGBA;
    } pixels;

    data.cmp[0] = uiCompressedBlockLo;
    data.cmp[1] = uiCompressedBlockHi;

    CMP_BYTE pixelpos;
    for (uint32 y = 0; y < 4; y++)
        for (uint32 x = 0; x < 4; x++)
        {
             cmp_decompressETC2Pixel(data.block,(uint32 &)pixels.rgb, x, y);
             pixelpos = (y * 16) + (x * 4);
             rgbBlock[pixelpos + RGBA8888_CHANNEL_R] = pixels.rgb.red;
             rgbBlock[pixelpos + RGBA8888_CHANNEL_G] = pixels.rgb.green;
             rgbBlock[pixelpos + RGBA8888_CHANNEL_B] = pixels.rgb.blue;
        }
#endif

}

//=============
// ETC2 RGBA
//=============

CodecError CCodec_ETC2::CompressRGBABlock(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4])
{
    CMP_BYTE alphaBlock[BLOCK_SIZE_4X4];
    CMP_BYTE ii = 0;
    for (DWORD y = 0; y < 4; y++)
    {
        for (DWORD x = 0; x < 4; x++)
        {
            alphaBlock[ii++] = rgbaBlock[(x * 4) + (y * 16) + RGBA8888_CHANNEL_A];
        }
    }

    union
    {
        uint8       alphadata[8];
        CMP_DWORD   CompressedAlphaBlock[2];
    } data;

#ifdef USE_ETCPACK
    if (m_fast)
        compressBlockAlphaFast(alphaBlock, 0, 0, 4, 4, data.alphadata);
    else
        compressBlockAlphaSlow(alphaBlock, 0, 0, 4, 4, data.alphadata);
#else
    cmp_compressBlockAlphaFast(alphaBlock, data.alphadata);
#endif

    compressedBlock[0] = data.CompressedAlphaBlock[0];
    compressedBlock[1] = data.CompressedAlphaBlock[1];

    return CompressRGBBlock(rgbaBlock, &compressedBlock[ATC_OFFSET_RGB]);
}

void CCodec_ETC2::DecompressRGBABlock(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[4])
{

    union
    {
        uint8       alphadata[8];
        CMP_DWORD   CompressedAlphaBlock[2];
    } data;

    uint8_t alphaimg[BLOCK_SIZE_4X4];
 

    data.CompressedAlphaBlock[0]     = compressedBlock[0];
    data.CompressedAlphaBlock[1]     = compressedBlock[1];

#ifdef USE_ETCPACK
    Color888_t destRGB[4][4];
    decompressBlockAlpha(data.alphadata, alphaimg, 4, 4, 0, 0);
    unsigned int uiCompressedBlockHi = SWIZZLE_DWORD(compressedBlock[2]);
    unsigned int uiCompressedBlockLo = SWIZZLE_DWORD(compressedBlock[3]);
    decompressBlockETC2(uiCompressedBlockHi, uiCompressedBlockLo, (uint8*)destRGB, 4, 4, 0, 0);
    CMP_BYTE ii = 0;
    for (CMP_BYTE y = 0; y < 4; y++)
    {
        for (CMP_BYTE x = 0; x < 4; x++)
        {
            rgbaBlock[(x * 16) + (y * 4) + RGBA8888_CHANNEL_R] = destRGB[x][y].red;
            rgbaBlock[(x * 16) + (y * 4) + RGBA8888_CHANNEL_G] = destRGB[x][y].green;
            rgbaBlock[(x * 16) + (y * 4) + RGBA8888_CHANNEL_B] = destRGB[x][y].blue;
            rgbaBlock[(x * 4) + (y * 16) + RGBA8888_CHANNEL_A] = alphaimg[ii];
            ii++;
        }
    }
#else
    cmp_decompressRGBABlockAlpha(data.alphadata, alphaimg);
    //cmp_decompressBlockETC2c(uiCompressedBlockHi, uiCompressedBlockLo, (uint8*)destRGB);
    DecompressRGBBlock(rgbaBlock, &compressedBlock[2]);
    CMP_BYTE ii = 0;
    for (CMP_BYTE y = 0; y < 4; y++)
    {
        for (CMP_BYTE x = 0; x < 4; x++)
        {
            rgbaBlock[(x * 4) + (y * 16) + RGBA8888_CHANNEL_A] = alphaimg[ii];
            ii++;
        }
    }
#endif


}

//=============
// ETC2 RGBA1
//=============

CodecError CCodec_ETC2::CompressRGBA1Block(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2])
{
    Color888_t srcRGB[4][4];
    unsigned int uiCompressedBlockHi, uiCompressedBlockLo;
    unsigned char tmp[4 * 4 * 3];                // Decompressed image

    // Swizzle RGBA input to match required BGRA input
    int pixelPos;
    unsigned char srcAlpha[16];
    int ii=0;
    for (int y = 0; y < 4; y++)
    {
        for (DWORD x = 0; x < 4; x++)
        {
            pixelPos = (x * 16) + (y * 4);
            srcRGB[x][y].red    = rgbaBlock[pixelPos + RGBA8888_CHANNEL_R];
            srcRGB[x][y].green  = rgbaBlock[pixelPos + RGBA8888_CHANNEL_G];
            srcRGB[x][y].blue   = rgbaBlock[pixelPos + RGBA8888_CHANNEL_B];
            srcAlpha[ii++]      = rgbaBlock[(x*4)  + (y*16) + RGBA8888_CHANNEL_A] > 128?255:0;
        }
    }

#ifdef USE_ETCPACK
    compressBlockETC2Fast((uint8 *)&srcRGB, (uint8 *)srcAlpha, (uint8 *)tmp, 4, 4, 0, 0, uiCompressedBlockHi, uiCompressedBlockLo);
#else
    cmp_compressBlockETC2Fast((uint8 *)&srcRGB, (uint8 *)srcAlpha, (uint8 *)tmp, uiCompressedBlockHi, uiCompressedBlockLo);
#endif
    compressedBlock[0] = SWIZZLE_DWORD(uiCompressedBlockHi);
    compressedBlock[1] = SWIZZLE_DWORD(uiCompressedBlockLo);

    return CE_OK;
}

void CCodec_ETC2::DecompressRGBA1Block(CMP_BYTE rgbaBlock[BLOCK_SIZE_4X4X4], CMP_DWORD compressedBlock[2])
{
    Color888_t destRGB[4][4] = {0};


    unsigned int uiCompressedBlockHi = SWIZZLE_DWORD(compressedBlock[0]);
    unsigned int uiCompressedBlockLo = SWIZZLE_DWORD(compressedBlock[1]);

#ifdef USE_ETCPACK
    uint8 dstAlpha[BLOCK_SIZE_4X4];

    decompressBlockETC21BitAlpha(uiCompressedBlockHi, uiCompressedBlockLo, (uint8*)destRGB, dstAlpha, 4, 4, 0, 0);

    int ii = 0;
    int pixelPos;
    for (DWORD y = 0; y < 4; y++)
    {
        for (DWORD x = 0; x < 4; x++)
        {
            pixelPos = (x * 4) + (y * 16);
            rgbaBlock[pixelPos + RGBA8888_CHANNEL_R] = destRGB[y][x].red;
            rgbaBlock[pixelPos + RGBA8888_CHANNEL_G] = destRGB[y][x].green;
            rgbaBlock[pixelPos + RGBA8888_CHANNEL_B] = destRGB[y][x].blue;
            rgbaBlock[pixelPos + RGBA8888_CHANNEL_A] = dstAlpha[ii++];
        }
    }
#else
    // cmp_decompressBlockETC21BitAlphaC(uiCompressedBlockHi, uiCompressedBlockLo, (uint8*)destRGB, dstAlpha);
    
    union
    {
        CMP_DWORD cmp[2];
        uint64    block;
    }data;

    union
    {
        uint8      rgba[4];
        uint32     blockRGBA;
    } pixel;

    data.cmp[0] = uiCompressedBlockLo;
    data.cmp[1] = uiCompressedBlockHi;

    CMP_BYTE pixelpos;
    for (uint32 y = 0; y < 4; y++)
        for (uint32 x = 0; x < 4; x++)
        {
            cmp_decompressETC21BitAlpha(data.block, (uint32 &)pixel.blockRGBA, x, y);
            pixelpos = (y * 16) + (x * 4);
            rgbaBlock[pixelpos + RGBA8888_CHANNEL_R] = pixel.rgba[0];
            rgbaBlock[pixelpos + RGBA8888_CHANNEL_G] = pixel.rgba[1];
            rgbaBlock[pixelpos + RGBA8888_CHANNEL_B] = pixel.rgba[2];
            rgbaBlock[pixelpos + RGBA8888_CHANNEL_A] = pixel.rgba[3];
        }

#endif

#pragma warning( pop )
}
