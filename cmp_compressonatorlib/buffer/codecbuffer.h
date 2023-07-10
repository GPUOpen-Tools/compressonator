//===============================================================================
// Copyright (c) 2007-2023  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   CodecBuffer.h
//  Description: interface for the CCodecBuffer class
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _CODECBUFFER_H_INCLUDED_
#define _CODECBUFFER_H_INCLUDED_

#include "common.h"
#include "compressonator.h"
#include "mathmacros.h"



typedef enum _CodecBufferType {
    CBT_Unknown = 0,
    CBT_RGBA8888,
    CBT_BGRA8888,       // Reserved for future work
    CBT_ARGB8888,       // Reserved for future work
    CBT_RGB888,
    CBT_BGR888,         // Reserved for future work
    CBT_RG8,
    CBT_R8,
    CBT_RGBA8888S,
    CBT_RGB888S,
    CBT_RG8S,
    CBT_R8S,
    CBT_RGBA2101010,
    CBT_RGBA1010102,
    CBT_RGBA16,
    CBT_RG16,
    CBT_R16,
    CBT_RGBA32,
    CBT_RG32,
    CBT_R32,
    CBT_RGBA16F,
    CBT_RGBE32F,
    CBT_RG16F,
    CBT_R16F,
    CBT_RGBA32F,
    CBT_RG32F,
    CBT_R32F,
    CBT_4x4Block_2BPP,
    CBT_4x4Block_4BPP,
    CBT_4x4Block_8BPP,
    CBT_4x4Block_16BPP,
    CBT_4x4Block_32BPP,
    CBT_8x8Block_2BPP,
    CBT_8x8Block_4BPP,
    CBT_8x8Block_8BPP,
    CBT_8x8Block_16BPP,
    CBT_8x8Block_32BPP,
} CodecBufferType;

#define CHANNEL_SIZE_ARGB       4
#define BLOCK_SIZE_4            4

#define BLOCK_SIZE_4X4          16
#define BLOCK_SIZE_4X4X4        64

#define BLOCK_SIZE_8X8          64
#define BLOCK_SIZE_8X8X4        256

// This is also used in code for ETC ATC it needs to be fixed
// and only used in codebuffer IO, The prefix description is not correct
// it should read BGRA8888_...
// Codecbuffer.cpp has CCodecBuffer* CreateCodecBuffer() mixes up these into one create buffer
// new CodecBuffer for BGRA.. should be added.
#define RGBA8888_CHANNEL_R      2
#define RGBA8888_CHANNEL_G      1
#define RGBA8888_CHANNEL_B      0
#define RGBA8888_CHANNEL_A      3

#define RGBA8888_OFFSET_A (RGBA8888_CHANNEL_A * 8)
#define RGBA8888_OFFSET_R (RGBA8888_CHANNEL_R * 8)
#define RGBA8888_OFFSET_G (RGBA8888_CHANNEL_G * 8)
#define RGBA8888_OFFSET_B (RGBA8888_CHANNEL_B * 8)

// Patch to correctly use RGBA src and decomp
#define ATC_RGBA8888_CHANNEL_R      0
#define ATC_RGBA8888_CHANNEL_G      1
#define ATC_RGBA8888_CHANNEL_B      2
#define ATC_RGBA8888_CHANNEL_A      3

#define ATC_RGBA8888_OFFSET_A (ATC_RGBA8888_CHANNEL_A * 8)
#define ATC_RGBA8888_OFFSET_R (ATC_RGBA8888_CHANNEL_R * 8)
#define ATC_RGBA8888_OFFSET_G (ATC_RGBA8888_CHANNEL_G * 8)
#define ATC_RGBA8888_OFFSET_B (ATC_RGBA8888_CHANNEL_B * 8)

#define RGBA16_OFFSET_R 0
#define RGBA16_OFFSET_G 1
#define RGBA16_OFFSET_B 2
#define RGBA16_OFFSET_A 3

#define RGBA16F_OFFSET_R 0
#define RGBA16F_OFFSET_G 1
#define RGBA16F_OFFSET_B 2
#define RGBA16F_OFFSET_A 3

#define RGBA32_OFFSET_R 0
#define RGBA32_OFFSET_G 1
#define RGBA32_OFFSET_B 2
#define RGBA32_OFFSET_A 3

#define RGBA32F_OFFSET_R 0
#define RGBA32F_OFFSET_G 1
#define RGBA32F_OFFSET_B 2
#define RGBA32F_OFFSET_A 3

#define BYTE_MASK       0x00ff
#define WORD_MASK       0xffff

#define MAKE_RGBA8888(r, g, b, a) ((r << RGBA8888_OFFSET_R) | (g << RGBA8888_OFFSET_G) | (b << RGBA8888_OFFSET_B) | (a << RGBA8888_OFFSET_A))
#define GET_R(i) ((i >> RGBA8888_OFFSET_R) & BYTE_MASK)
#define GET_G(i) ((i >> RGBA8888_OFFSET_G) & BYTE_MASK)
#define GET_B(i) ((i >> RGBA8888_OFFSET_B) & BYTE_MASK)
#define GET_A(i) ((i >> RGBA8888_OFFSET_A) & BYTE_MASK)

// Note: CMP_DWORD AABBGGRR for RGBA8888 CMP_BYTE[4];
//                                RED   to Blue           BLUE  to Red               GREEN & ALPHA
#define SWIZZLE_RGBA_BGRA(dsw) ((dsw << 16) & 0x00FF0000)|((dsw >> 16) & 0x000000FF)|(dsw&0xFF00FF00)

#define SWIZZLE_RGBA_RBxG(dsw) ((((dsw >> RGBA8888_OFFSET_R) & BYTE_MASK) << RGBA8888_OFFSET_R) | (((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_A) | (((dsw >> RGBA8888_OFFSET_B) & BYTE_MASK) << RGBA8888_OFFSET_G))
#define SWIZZLE_RBxG_RGBA(dsw) ((((dsw >> RGBA8888_OFFSET_R) & BYTE_MASK) << RGBA8888_OFFSET_R) | (((dsw >> RGBA8888_OFFSET_A) & BYTE_MASK) << RGBA8888_OFFSET_G) | (((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_B) | (BYTE_MASK << RGBA8888_OFFSET_A))
#define SWIZZLE_RGBA_RGxB(dsw) ((((dsw >> RGBA8888_OFFSET_R) & BYTE_MASK) << RGBA8888_OFFSET_R) | (((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_G) | (((dsw >> RGBA8888_OFFSET_B) & BYTE_MASK) << RGBA8888_OFFSET_A))
#define SWIZZLE_RGxB_RGBA(dsw) ((((dsw >> RGBA8888_OFFSET_R) & BYTE_MASK) << RGBA8888_OFFSET_R) | (((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_G) | (((dsw >> RGBA8888_OFFSET_A) & BYTE_MASK) << RGBA8888_OFFSET_B) | (BYTE_MASK << RGBA8888_OFFSET_A))
#define SWIZZLE_RGBA_RxBG(dsw) ((((dsw >> RGBA8888_OFFSET_R) & BYTE_MASK) << RGBA8888_OFFSET_R) | (((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_A) | (((dsw >> RGBA8888_OFFSET_B) & BYTE_MASK) << RGBA8888_OFFSET_B))
#define SWIZZLE_RxBG_RGBA(dsw) ((((dsw >> RGBA8888_OFFSET_R) & BYTE_MASK) << RGBA8888_OFFSET_R) | (((dsw >> RGBA8888_OFFSET_A) & BYTE_MASK) << RGBA8888_OFFSET_G) | (((dsw >> RGBA8888_OFFSET_B) & BYTE_MASK) << RGBA8888_OFFSET_B) | (BYTE_MASK << RGBA8888_OFFSET_A))
#define SWIZZLE_RGBA_xGBR(dsw) ((((dsw >> RGBA8888_OFFSET_R) & BYTE_MASK) << RGBA8888_OFFSET_A) | (((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_G) | (((dsw >> RGBA8888_OFFSET_B) & BYTE_MASK) << RGBA8888_OFFSET_B))
#define SWIZZLE_xGBR_RGBA(dsw) ((((dsw >> RGBA8888_OFFSET_A) & BYTE_MASK) << RGBA8888_OFFSET_R) | (((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_G) | (((dsw >> RGBA8888_OFFSET_B) & BYTE_MASK) << RGBA8888_OFFSET_B) | (BYTE_MASK << RGBA8888_OFFSET_A))
#define SWIZZLE_RGBA_xRBG(dsw) ((((dsw >> RGBA8888_OFFSET_R) & BYTE_MASK) << RGBA8888_OFFSET_G) | (((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_A) | (((dsw >> RGBA8888_OFFSET_B) & BYTE_MASK) << RGBA8888_OFFSET_B))
#define SWIZZLE_xRBG_RGBA(dsw) ((((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_R) | (((dsw >> RGBA8888_OFFSET_A) & BYTE_MASK) << RGBA8888_OFFSET_G) | (((dsw >> RGBA8888_OFFSET_B) & BYTE_MASK) << RGBA8888_OFFSET_B) | (BYTE_MASK << RGBA8888_OFFSET_A))
#define SWIZZLE_xGxR_RGBA(dsw) ((((dsw >> RGBA8888_OFFSET_A) & BYTE_MASK) << RGBA8888_OFFSET_R) | (((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_G) | (BYTE_MASK << RGBA8888_OFFSET_A))
#define SWIZZLE_RGBA_xGxR(dsw) ((((dsw >> RGBA8888_OFFSET_R) & BYTE_MASK) << RGBA8888_OFFSET_A) | (((dsw >> RGBA8888_OFFSET_G) & BYTE_MASK) << RGBA8888_OFFSET_G))

#define SBYTE_MAXVAL 127
#define BYTE_MAXVAL 255
#define WORD_MAXVAL 65535
#define DWORD_MAXVAL 4294967295

// The general goal of these conversion macros is to go between types in a simple and easily reversible way (if possible)
// Larger types -> smaller types will potentially loose data, but if the smaller type can hold the entire value that should be preserved
// Smaller types -> larger types will typically not modify anything, we simply perform an up-cast

// Float Converions
#define CONVERT_FLOAT_TO_BYTE(f) static_cast<CMP_BYTE>(((float)(f)*BYTE_MAXVAL) + 0.5f)
#define CONVERT_FLOAT_TO_SBYTE(f) static_cast<CMP_SBYTE>(((float)(f)*SBYTE_MAXVAL) + 0.5f)
#define CONVERT_FLOAT_TO_WORD(f) static_cast<CMP_WORD>(((float)(f)*WORD_MAXVAL) + 0.5f)
#define CONVERT_FLOAT_TO_DWORD(f) static_cast<CMP_DWORD>(((float)(f)*DWORD_MAXVAL) + 0.5f)

// DWORD Conversions
#define CONVERT_DWORD_TO_BYTE(dw) static_cast<CMP_BYTE>(dw)
#define CONVERT_DWORD_TO_SBYTE(dw) static_cast<CMP_SBYTE>(dw)
#define CONVERT_DWORD_TO_WORD(dw) static_cast<CMP_WORD>(dw)
#define CONVERT_DWORD_TO_FLOAT(dw) ((dw) / (float)DWORD_MAXVAL)

// Byte Conversions
#define CONVERT_BYTE_TO_WORD(b) static_cast<CMP_WORD>(b)
#define CONVERT_BYTE_TO_DWORD(b) static_cast<CMP_DWORD>(b)
#define CONVERT_BYTE_TO_FLOAT(b) ((b) / (float)BYTE_MAXVAL)

// Signed Byte Conversions
#define CONVERT_SBYTE_TO_WORD(b) static_cast<CMP_WORD>(b)
#define CONVERT_SBYTE_TO_DWORD(b) static_cast<CMP_DWORD>(b)
#define CONVERT_SBYTE_TO_FLOAT(b) ((b) / (float)SBYTE_MAXVAL)

// WORD Conversions
#define CONVERT_WORD_TO_BYTE(w) static_cast<CMP_BYTE>(w)
#define CONVERT_WORD_TO_SBYTE(w) static_cast<CMP_SBYTE>(w)
#define CONVERT_WORD_TO_DWORD(w) static_cast<CMP_DWORD>(w)
#define CONVERT_WORD_TO_FLOAT(w) ((w) / (float)WORD_MAXVAL)

#define CONVERT_10BIT_TO_WORD(b) (((static_cast<CMP_WORD>(b)) << 6) | static_cast<CMP_WORD>(b) >> 2)
#define CONVERT_2BIT_TO_WORD(b) ((static_cast<CMP_WORD>(b)) | ((static_cast<CMP_WORD>(b)) << 2) | ((static_cast<CMP_WORD>(b)) << 4) | ((static_cast<CMP_WORD>(b)) << 6) | ((static_cast<CMP_WORD>(b)) << 8) | ((static_cast<CMP_WORD>(b)) << 10) | ((static_cast<CMP_WORD>(b)) << 12) | ((static_cast<CMP_WORD>(b)) << 14))

#define SWAP_DWORDS(a, b) {CMP_DWORD dwTemp = a; a = b; b = dwTemp;}
#define SWAP_WORDS(a, b) {CMP_WORD wTemp = a; a = b; b = wTemp;}
#define SWAP_BYTES(a, b) {CMP_BYTE temp = a; a = b; b = temp;}
#define SWAP_SBYTES(a, b) {CMP_SBYTE temp = a; a = b; b = temp;}
#define SWAP_HALFS(a, b) {CMP_HALF fTemp = a; a = b; b = fTemp;}
#define SWAP_FLOATS(a, b) {float fTemp = a; a = b; b = fTemp;}
#define SWAP_DOUBLES(a, b) {double dTemp = a; a = b; b = dTemp;}

template <typename T> void PadLine(CMP_DWORD startIndex, CMP_DWORD endIndex, CMP_BYTE repeatCount, T block[])
{
    CMP_DWORD padWidth = endIndex - startIndex;

    if(padWidth > startIndex)
    {
        PadLine(startIndex, endIndex >> 1, repeatCount, block);

        startIndex = endIndex >> 1;
        padWidth = endIndex - startIndex;
    }

    memcpy(&block[startIndex*repeatCount], &block[0], padWidth*repeatCount*sizeof(T));
}

template <typename T> void PadBlock(CMP_DWORD j, CMP_BYTE w, CMP_BYTE h, CMP_BYTE c, T block[]) {
    CMP_DWORD dwPadHeight = h - j;
    if(dwPadHeight > j) {
        PadBlock(j, w, h >> 1, c, block);
        j = h >> 1;
        dwPadHeight = h - j;
    }
    memcpy(&block[j*w*c], &block[0], dwPadHeight * w * c * sizeof(T));
}

class CCodecBuffer {
  public:

    CCodecBuffer(
        CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
        CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch = 0,
        CMP_BYTE* pData = 0,
        CMP_DWORD dwDataSize = 0);
    virtual ~CCodecBuffer();

    virtual void Copy(CCodecBuffer& srcBuffer);

    virtual CodecBufferType GetBufferType() const {
        return CBT_Unknown;
    };
    virtual CMP_DWORD GetChannelDepth() const = 0;
    virtual CMP_DWORD GetChannelCount() const = 0;
    virtual bool IsFloat() const = 0;

    inline const CMP_DWORD GetWidth()  const {
        return m_dwWidth;
    };
    inline const CMP_DWORD GetHeight() const {
        return m_dwHeight;
    };
    inline const CMP_DWORD GetPitch()  const {
        return m_dwPitch;
    };

    inline const void SetPitch(CMP_DWORD dwPitch) {
        m_dwPitch = dwPitch;
    };

    inline const void SetFormat(CMP_FORMAT dwFormat) {
        m_dwFormat = dwFormat;
    };

    inline const void SetTranscodeFormat(CMP_FORMAT Format) {
        m_dwTranscodeFormat = Format;
    };

    inline const void SetDataSize(CMP_DWORD dwDataSize) {
        m_DataSize = dwDataSize;
    };

    inline const CMP_FORMAT GetFormat() const {
        return m_dwFormat;
    };
    inline const CMP_FORMAT GetTranscodeFormat() const {
        return m_dwTranscodeFormat;
    };

    inline const CMP_BYTE GetBlockWidth()  const {
        return m_nBlockWidth;
    };
    inline const CMP_BYTE GetBlockHeight() const {
        return m_nBlockHeight;
    };
    inline const CMP_BYTE GetBlockDepth()  const {
        return m_nBlockDepth;
    };

    inline const void SetBlockWidth(CMP_BYTE BlockWidth)      {
        m_nBlockWidth  = BlockWidth;
    };
    inline const void SetBlockHeight(CMP_BYTE BlockHeight)    {
        m_nBlockHeight = BlockHeight;
    };
    inline const void SetBlockDepth(CMP_BYTE BlockDepth)      {
        m_nBlockDepth  = BlockDepth;
    };



    virtual bool ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[]);
    virtual bool ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[]);
    virtual bool ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[]);
    virtual bool ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[]);

    virtual bool ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_SBYTE cBlock[]);
    virtual bool ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_SBYTE cBlock[]);
    virtual bool ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_SBYTE cBlock[]);
    virtual bool ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_SBYTE cBlock[]);


    virtual bool ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wblock[]);
    virtual bool ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wblock[]);
    virtual bool ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wblock[]);
    virtual bool ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wblock[]);

    virtual bool ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwblock[]);
    virtual bool ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwblock[]);
    virtual bool ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwblock[]);
    virtual bool ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwblock[]);

    virtual bool ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[]);
    virtual bool ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[]);
    virtual bool ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[]);
    virtual bool ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[]);

    virtual bool ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[]);
    virtual bool ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[]);
    virtual bool ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[]);
    virtual bool ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[]);

    virtual bool ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[]);
    virtual bool ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[]);
    virtual bool ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[]);
    virtual bool ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[]);

    virtual bool WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[]);
    virtual bool WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[]);
    virtual bool WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[]);
    virtual bool WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[]);

    virtual bool WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_SBYTE cBlock[]);
    virtual bool WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_SBYTE cBlock[]);
    virtual bool WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_SBYTE cBlock[]);
    virtual bool WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_SBYTE cBlock[]);

    virtual bool WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wblock[]);
    virtual bool WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wblock[]);
    virtual bool WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wblock[]);
    virtual bool WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wblock[]);

    virtual bool WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwblock[]);
    virtual bool WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwblock[]);
    virtual bool WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwblock[]);
    virtual bool WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwblock[]);

    virtual bool WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[]);
    virtual bool WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[]);
    virtual bool WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[]);
    virtual bool WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[]);

    virtual bool WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[]);
    virtual bool WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[]);
    virtual bool WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[]);
    virtual bool WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[]);

    virtual bool WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[]);
    virtual bool WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[]);
    virtual bool WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[]);
    virtual bool WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[]);

    virtual bool ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_SBYTE cBlock[]);
    virtual bool WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_SBYTE cBlock[]);

    virtual bool ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[]);
    virtual bool WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[]);

    virtual bool ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[]);
    virtual bool WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[]);

    virtual bool ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[]);
    virtual bool WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[]);

    virtual bool ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[]);
    virtual bool WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[]);

    virtual bool ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[]);
    virtual bool WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[]);

    virtual bool ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[]);
    virtual bool WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[]);

    virtual bool ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_DWORD* pBlock, CMP_DWORD dwBlockSize);
    virtual bool WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_DWORD* pBlock, CMP_DWORD dwBlockSize);

    inline CMP_BYTE* GetData() const {
        return m_pData;
    };
    inline CMP_DWORD GetDataSize() const {
        return m_DataSize;
    };

    bool m_bSwizzle;

  protected:

    // Converts data from a source type  to a destination type
    void ConvertBlock(double dBlock[], float fBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(double dBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(double dBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(double dBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(double dBlock[], CMP_BYTE cBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(double dBlock[], CMP_SBYTE cBlock[], CMP_DWORD dwBlockSize);

    void ConvertBlock(float fBlock[], double dBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(float fBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(float fBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(float fBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(float fBlock[], CMP_BYTE cBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(float fBlock[], CMP_SBYTE cBlock[], CMP_DWORD dwBlockSize);

    void ConvertBlock(CMP_HALF hBlock[], double dBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_HALF hBlock[], float fBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_HALF hBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_HALF hBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_HALF hBlock[], CMP_BYTE cBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_HALF hBlock[], CMP_SBYTE cBlock[], CMP_DWORD dwBlockSize);

    void ConvertBlock(CMP_DWORD dwBlock[], double dBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_DWORD dwBlock[], float fBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_DWORD dwBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_DWORD dwBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_DWORD dwBlock[], CMP_BYTE cBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_DWORD dwBlock[], CMP_SBYTE cBlock[], CMP_DWORD dwBlockSize);

    void ConvertBlock(CMP_WORD wBlock[], double dBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_WORD wBlock[], float fBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_WORD wBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_WORD wBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_WORD wBlock[], CMP_BYTE cBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_WORD wBlock[], CMP_SBYTE cBlock[], CMP_DWORD dwBlockSize);

    void ConvertBlock(CMP_BYTE cBlock[], double dBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_BYTE cBlock[], float fBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_BYTE cBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_BYTE cBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_BYTE cBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize);

    // Signed Conversions
    void ConvertBlock(CMP_BYTE cBlock[], CMP_SBYTE wBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_SBYTE cBlock[], double dBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_SBYTE cBlock[], float fBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_SBYTE cBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_SBYTE cBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_SBYTE cBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize);
    void ConvertBlock(CMP_SBYTE cBlock[], CMP_BYTE wBlock[], CMP_DWORD dwBlockSize);

    // NOTE: these are never used currently, so they are a candidate for removal
    void SwizzleBlock(double dBlock[], CMP_DWORD dwBlockSize);
    void SwizzleBlock(float fBlock[], CMP_DWORD dwBlockSize);
    void SwizzleBlock(CMP_HALF hBlock[], CMP_DWORD dwBlockSize);
    void SwizzleBlock(CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize);
    void SwizzleBlock(CMP_WORD wBlock[], CMP_DWORD dwBlockSize);
    void SwizzleBlock(CMP_BYTE bBlock[], CMP_DWORD dwBlockSize);
    void SwizzleBlock(CMP_SBYTE sbBlock[], CMP_DWORD dwBlockSize);

    CMP_DWORD m_dwWidth;        // Final Image Width
    CMP_DWORD m_dwHeight;       // Final Image Height
    CMP_DWORD m_dwDepth;        // Final Image Depth
    CMP_DWORD m_dwPitch;
    CMP_FORMAT m_dwFormat;
    CMP_FORMAT m_dwTranscodeFormat;

    CMP_BYTE m_nBlockWidth;     // DeCompression Block Sizes (Default is 4x4x1)
    CMP_BYTE m_nBlockHeight;    //
    CMP_BYTE m_nBlockDepth;     //

    bool m_bUserAllocedData;
    CMP_BYTE* m_pData;
    CMP_DWORD m_DataSize;

    bool m_bPerformingConversion;
};

CCodecBuffer*   CreateCodecBuffer(CodecBufferType nCodecBufferType,
                                  CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
                                  CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch = 0, CMP_BYTE* pData = 0,
                                  CMP_DWORD dwDataSize = 0);

CodecBufferType GetCodecBufferType(CMP_FORMAT format);

#endif // !defined(_CODECBUFFER_H_INCLUDED_)
