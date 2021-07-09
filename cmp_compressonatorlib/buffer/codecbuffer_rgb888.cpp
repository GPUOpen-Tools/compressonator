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
//  File Name:   CodecBuffer_RGB888.cpp
//  Description: implementation of the CCodecBuffer_RGB888 class
//
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codecbuffer_rgb888.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

const int nChannelCount = 3;
const int nPixelSize = nChannelCount * sizeof(CMP_BYTE);

CCodecBuffer_RGB888::CCodecBuffer_RGB888(
    CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
    CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData,CMP_DWORD dwDataSize)
    : CCodecBuffer(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize) {
    CMP_DWORD dwMinPitch = dwWidth * nPixelSize;
    assert((m_dwPitch == 0) || (m_dwPitch >= dwMinPitch));

    if(m_dwPitch < dwMinPitch)
        m_dwPitch = dwMinPitch;

    if(m_pData == NULL) {
        CMP_DWORD dwSize = m_dwPitch * GetHeight();
        m_pData = (CMP_BYTE*) malloc(dwSize);
    }
}

CCodecBuffer_RGB888::~CCodecBuffer_RGB888() {

}

void CCodecBuffer_RGB888::Copy(CCodecBuffer& srcBuffer) {
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

bool CCodecBuffer_RGB888::ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[], CMP_DWORD dwChannelOffset) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    CMP_DWORD i, j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_BYTE* pSrcData = (CMP_BYTE*) (GetData() + ((y + j) * m_dwPitch) + (x * sizeof(nChannelCount))) + dwChannelOffset;
        for(i = 0; i < dwWidth; i++) {
            block[(j * w) + i] = *pSrcData;
            pSrcData += nChannelCount;
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

bool CCodecBuffer_RGB888::WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[], CMP_DWORD dwChannelOffset) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_BYTE* pDestData = (CMP_BYTE*) (GetData() + ((y + j) * m_dwPitch) + (x * sizeof(nChannelCount))) + dwChannelOffset;
        for(CMP_DWORD i = 0; i < dwWidth; i++) {
            *pDestData++ = block[(j * w) + i];
            pDestData += nChannelCount;
        }
    }
    return true;
}

bool CCodecBuffer_RGB888::ReadBlockA(CMP_DWORD /*x*/, CMP_DWORD /*y*/, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    memset(block, 255, w * h);
    return true;
}

bool CCodecBuffer_RGB888::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_R);
}

bool CCodecBuffer_RGB888::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_G);
}

bool CCodecBuffer_RGB888::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_B);
}

bool CCodecBuffer_RGB888::WriteBlockA(CMP_DWORD /*x*/, CMP_DWORD /*y*/, CMP_BYTE /*w*/, CMP_BYTE /*h*/, CMP_BYTE /*block*/[]) {
    return false;
}

bool CCodecBuffer_RGB888::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_R);
}

bool CCodecBuffer_RGB888::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_G);
}

bool CCodecBuffer_RGB888::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_B);
}

bool CCodecBuffer_RGB888::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % w == 0);
    assert(y % h == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    CMP_DWORD* pdwBlock = (CMP_DWORD*) block;
    CMP_DWORD i, j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_BYTE* pSrcData = (CMP_BYTE*) ((GetData() + ((y + j) * m_dwPitch) + (x * nChannelCount)));
        CMP_BYTE* pDestData = (CMP_BYTE*) &pdwBlock[(j * w)];
        for(i = 0; i < dwWidth; i++) {
            *pDestData++ = *pSrcData++;
            *pDestData++ = *pSrcData++;
            *pDestData++ = *pSrcData++;
            *pDestData++ = 0xff;
        }

        // Pad block with previous values if necessary
        if(i < w)
            PadLine(i, w, 4, (CMP_BYTE*) &pdwBlock[j * w]);
    }

    // Pad block with previous values if necessary
    if(j < h)
        PadBlock(j, w, h, 4, (CMP_BYTE*) pdwBlock);

    return true;
}

bool CCodecBuffer_RGB888::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % 4 == 0);
    assert(y % 4 == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD  dwWidth  = cmp_minT(w, (GetWidth() - x));
    CMP_DWORD* pdwBlock = (CMP_DWORD*) block;

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_BYTE* pSrcData = (CMP_BYTE*) &pdwBlock[(j * w)];
        CMP_BYTE* pDestData = (CMP_BYTE*) (GetData() + ((y + j) * m_dwPitch) + (x * sizeof(nChannelCount)));
        for(CMP_DWORD i = 0; i < dwWidth; i++) {
            *pDestData++ = *pSrcData++;
            *pDestData++ = *pSrcData++;
            *pDestData++ = *pSrcData++;
            pSrcData++;
        }
    }
    return true;
}


