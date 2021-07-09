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
//  File Name:   CodecBuffer_RG8.cpp
//  Description: implementation of the CCodecBuffer_RG8 class
//
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codecbuffer_rg8s.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

const int nChannelCount = 2;
const int nPixelSize = nChannelCount * sizeof(CMP_BYTE);

CCodecBuffer_RG8S::CCodecBuffer_RG8S(
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

CCodecBuffer_RG8S::~CCodecBuffer_RG8S() {

}

void CCodecBuffer_RG8S::Copy(CCodecBuffer& srcBuffer) {
    if(GetWidth() != srcBuffer.GetWidth() || GetHeight() != srcBuffer.GetHeight())
        return;

    const CMP_DWORD dwBlocksX = ((GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((GetHeight() + 3) >> 2);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++) {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++) {
            CMP_BYTE block[BLOCK_SIZE_4X4X4];
            srcBuffer.ReadBlockRGBA(i*4, j*4, 4, 4, block);
            WriteBlockRGBA(i*4, j*4, 4, 4, block);
        }
    }
}

bool CCodecBuffer_RG8S::ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[], CMP_DWORD dwChannelOffset) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    CMP_DWORD i, j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_WORD* pData = (CMP_WORD*) (GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for(i = 0; i < dwWidth; i++)
            block[(j * w) + i] = static_cast<CMP_BYTE>(((*pData++) >> dwChannelOffset) & BYTE_MASK);

        // Pad line with previous values if necessary
        if(i < w)
            PadLine(i, w, 1, &block[j * w]);
    }

    // Pad block with previous values if necessary
    if(j < h)
        PadBlock(j, w, h, 1, block);

    return true;
}

bool CCodecBuffer_RG8S::WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[], CMP_DWORD dwChannelOffset) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwChannelMask = ~((CMP_DWORD) BYTE_MASK << dwChannelOffset);
    CMP_DWORD dwWidth       = cmp_minT(w, (GetWidth() - x));

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_WORD* pData = (CMP_WORD*) (GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for(CMP_DWORD i = 0; i < dwWidth; i++) {
            *pData = (CMP_WORD) ((*pData & dwChannelMask) | (((CMP_WORD) block[(j * w) + i]) << dwChannelOffset));
            pData++;
        }
    }
    return true;
}

#define RG8_CHANNEL_R 1
#define RG8_CHANNEL_G 0

#define RG8_OFFSET_R (RG8_CHANNEL_R * 8)
#define RG8_OFFSET_G (RG8_CHANNEL_G * 8)

bool CCodecBuffer_RG8S::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    memset(block, 0, w*h*nPixelSize);

    return true;
}

bool CCodecBuffer_RG8S::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return ReadBlock(x, y, w, h, block, RG8_OFFSET_R);
}

bool CCodecBuffer_RG8S::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return ReadBlock(x, y, w, h, block, RG8_OFFSET_G);
}

bool CCodecBuffer_RG8S::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    memset(block, 0, w*h*nPixelSize);

    return true;
}

bool CCodecBuffer_RG8S::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE /*w*/, CMP_BYTE /*h*/, CMP_BYTE /*block*/[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    return true;
}

bool CCodecBuffer_RG8S::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return WriteBlock(x, y, w, h, block, RG8_OFFSET_R);
}

bool CCodecBuffer_RG8S::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return WriteBlock(x, y, w, h, block, RG8_OFFSET_G);
}

bool CCodecBuffer_RG8S::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE /*w*/, CMP_BYTE /*h*/, CMP_BYTE /*block*/[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    return true;
}

#define GET_PIXEL(i, j) &block[(((j * w) + i) * 4)]
bool CCodecBuffer_RG8S::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % w == 0);
    assert(y % h == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    CMP_DWORD i, j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_BYTE* pData = (CMP_BYTE*) (GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for(i = 0; i < dwWidth; i++) {
            CMP_BYTE* pDest = GET_PIXEL(i, j);
            *pDest++ = 0;
            memcpy(pDest, pData, nPixelSize);
            pData += nChannelCount;
            pDest += 2;
            *pDest++ = 0xff;
        }

        // Pad block with previous values if necessary
        if(i < w)
            PadLine(i, w, 4, &block[j * w * 4]);
    }

    // Pad block with previous values if necessary
    if(j < h)
        PadBlock(j, w, h, 4, block);

    return true;
}

bool CCodecBuffer_RG8S::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % 4 == 0);
    assert(y % 4 == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_BYTE* pData = (CMP_BYTE*) (GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for(CMP_DWORD i = 0; i < dwWidth; i++) {
            memcpy(pData, GET_PIXEL(i, j)+RG8_CHANNEL_G, nPixelSize);
            pData += nChannelCount;
        }
    }
    return true;
}


