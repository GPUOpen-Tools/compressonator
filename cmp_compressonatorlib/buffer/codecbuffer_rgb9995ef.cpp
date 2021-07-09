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
//  File Name:   CodecBuffer_R16F.cpp
//  Description: implementation of the CCodecBuffer_R16F class
//
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codecbuffer_rgb9995ef.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

const int nChannelCount = 4;
const int nPixelSize = nChannelCount * sizeof(char);

CCodecBuffer_RGB9995EF::CCodecBuffer_RGB9995EF(
    CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
    CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData,CMP_DWORD dwDataSize)
    : CCodecBuffer(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize) {
    assert((m_dwPitch == 0) || (m_dwPitch >= GetWidth() * nPixelSize));
    if(m_dwPitch <= GetWidth() * nPixelSize)
        m_dwPitch = GetWidth() * nPixelSize;

    if(m_pData == NULL) {
        CMP_DWORD dwSize = m_dwPitch * GetHeight();
        m_pData = (CMP_BYTE*) malloc(dwSize);
    }
}

CCodecBuffer_RGB9995EF::~CCodecBuffer_RGB9995EF() {
}

void CCodecBuffer_RGB9995EF::Copy(CCodecBuffer& srcBuffer) {
    if(GetWidth() != srcBuffer.GetWidth() || GetHeight() != srcBuffer.GetHeight())
        return;

    const CMP_DWORD dwBlocksX = ((GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((GetHeight() + 3) >> 2);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++) {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++) {
            float block[BLOCK_SIZE_4X4X4];
            srcBuffer.ReadBlockRGBA(i*4, j*4, 4, 4, block);
            WriteBlockRGBA(i*4, j*4, 4, 4, block);
        }
    }
}
bool CCodecBuffer_RGB9995EF::ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float block[], CMP_DWORD dwChannelIndex) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    CMP_DWORD i,j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_HALF* pData = (CMP_HALF*) (GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for(i = 0; i < dwWidth; i++) {
            block[(j * w) + i] = pData[dwChannelIndex];
            pData += nChannelCount;
        }

        // Pad line with previous values if necessary
        if(i < w)
            PadLine(i, w, 1, &block[j * w]);
    }

    // Pad block with previous values if necessary
    if(j < h)
        PadBlock(j, w, h, 1, block);

    return true;
}

bool CCodecBuffer_RGB9995EF::WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float block[], CMP_DWORD dwChannelIndex) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if (x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    for (CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_HALF* pData = (CMP_HALF*)(GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for (CMP_DWORD i = 0; i < dwWidth; i++) {
            pData[dwChannelIndex] = block[(j * w) + i];
            pData += nChannelCount;
        }
    }
    return true;
}

#pragma warning( push )
#pragma warning( disable : 4201)
typedef struct _R9G9B9E5 {
    union {
        struct {
            uint32_t rm : 9; // r-mantissa
            uint32_t gm : 9; // g-mantissa
            uint32_t bm : 9; // b-mantissa
            uint32_t e : 5; // shared exponent
        };
        uint32_t v;
    };

    operator uint32_t () const {
        return v;
    }

    _R9G9B9E5& operator= (const _R9G9B9E5& floatrgb9e5) {
        v = floatrgb9e5.v;
        return *this;
    }
    _R9G9B9E5& operator= (uint32_t Packed) {
        v = Packed;
        return *this;
    }
} R9G9B9E5;

#pragma warning( pop )

#define GET_PIXEL(i, j) &block[(((j * w) + i) * 4)]
bool CCodecBuffer_RGB9995EF::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float block[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % w == 0);
    assert(y % h == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));
    union {
        float f;
        int32_t i;
    } fi;
    float Scale = 0.0f;
    CMP_DWORD i, j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_DWORD* pData = (CMP_DWORD*)(GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        R9G9B9E5 pTemp;

        pTemp.rm = ((*pData) & 0x000001ff);
        pTemp.gm = ((*pData) & 0x0003fe00) >> 9;
        pTemp.bm = ((*pData) & 0x07fc0000) >> 18;
        pTemp.e = ((*pData) & 0xf8000000) >> 27;

        fi.i = 0x33800000 + (pTemp.e << 23);
        Scale = fi.f;
        for (i = 0; i < dwWidth; i++) {
            float* pDest = GET_PIXEL(i, j);
            *pDest++ = Scale * float(pTemp.rm);
            *pDest++ = Scale * float(pTemp.gm);
            *pDest++ = Scale * float(pTemp.bm);
            *pDest++ = 1.0f;
            //pData += nChannelCount;
        }
        //float* pData = (float*) (GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        //for(i = 0; i < dwWidth; i++)
        //{
        //    float* pDest = GET_PIXEL(i, j);
        //    *pDest++ = *pData++;
        //    *pDest++ = 0.0f;
        //    *pDest++ = 0.0f;
        //    *pDest++ = 1.0f;
        //}

        // Pad line with previous values if necessary
        if (i < w)
            PadLine(i, w, 1, &block[j * w]);
    }

    // Pad block with previous values if necessary
    if (j < h)
        PadBlock(j, w, h, 1, block);
    return true;
}

bool CCodecBuffer_RGB9995EF::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float block[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % w == 0);
    assert(y % h == 0);

    if (x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    for (CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++) {
        float* pData = (float*)(GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        memcpy(pData, GET_PIXEL(0, j), dwWidth * nPixelSize);
    }
    return true;
}
