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
//  File Name:   CodecBuffer_RGBA8888.cpp
//  Description: implementation of the CCodecBuffer_RGBA8888 class
//
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codecbuffer_rgba8888.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

const int nChannelCount = 4;
const int nPixelSize = nChannelCount * sizeof(CMP_BYTE);

CCodecBuffer_RGBA8888::CCodecBuffer_RGBA8888(
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

CCodecBuffer_RGBA8888::~CCodecBuffer_RGBA8888() {

}

void CCodecBuffer_RGBA8888::Copy(CCodecBuffer& srcBuffer) {
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

bool CCodecBuffer_RGBA8888::ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[], CMP_DWORD dwChannelOffset) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    CMP_DWORD i, j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * sizeof(CMP_DWORD)));
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

bool CCodecBuffer_RGBA8888::WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[], CMP_DWORD dwChannelOffset) {
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwChannelMask = ~((CMP_DWORD) BYTE_MASK << dwChannelOffset);
    CMP_DWORD dwWidth       = cmp_minT(w, (GetWidth() - x));

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * sizeof(CMP_DWORD)));
        for(CMP_DWORD i = 0; i < dwWidth; i++) {
            *pData = (*pData & dwChannelMask) | (((CMP_DWORD) block[(j * w) + i]) << dwChannelOffset);
            pData++;
        }
    }
    return true;
}

bool CCodecBuffer_RGBA8888::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_A);
}

bool CCodecBuffer_RGBA8888::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_R);
}

bool CCodecBuffer_RGBA8888::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_G);
}

bool CCodecBuffer_RGBA8888::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return ReadBlock(x, y, w, h, block, RGBA8888_OFFSET_B);
}

bool CCodecBuffer_RGBA8888::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_A);
}

bool CCodecBuffer_RGBA8888::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_R);
}

bool CCodecBuffer_RGBA8888::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_G);
}

bool CCodecBuffer_RGBA8888::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    return WriteBlock(x, y, w, h, block, RGBA8888_OFFSET_B);
}

bool CCodecBuffer_RGBA8888::ReadBlockRGBA(CMP_DWORD xw, CMP_DWORD yh, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    assert(xw < GetWidth());
    assert(yh < GetHeight());
    assert(xw % w == 0);
    assert(yh % h == 0);

    if (xw >= GetWidth() || yh >= GetHeight())
        return false;

    CMP_DWORD* pdwBlock = (CMP_DWORD*)block;
    if (w == 4 && h == 4 && (xw + w) <= GetWidth() && (yh + h) <= GetHeight()) {
        // Fastpath for the key case to alleviate the drag this code puts on the really fast DXTC
        CMP_DWORD* pData = (CMP_DWORD*)(GetData() + (yh * m_dwPitch) + (xw * sizeof(CMP_DWORD)));

        // The source is RGBA8888 and the codec reqiures a BGRA8888
        if (m_bSwizzle) {
            pdwBlock[0] = SWIZZLE_RGBA_BGRA(pData[0]);
            pdwBlock[1] = SWIZZLE_RGBA_BGRA(pData[1]);
            pdwBlock[2] = SWIZZLE_RGBA_BGRA(pData[2]);
            pdwBlock[3] = SWIZZLE_RGBA_BGRA(pData[3]);
            pData += (m_dwPitch >> 2);
            pdwBlock[4] = SWIZZLE_RGBA_BGRA(pData[0]);
            pdwBlock[5] = SWIZZLE_RGBA_BGRA(pData[1]);
            pdwBlock[6] = SWIZZLE_RGBA_BGRA(pData[2]);
            pdwBlock[7] = SWIZZLE_RGBA_BGRA(pData[3]);
            pData += (m_dwPitch >> 2);
            pdwBlock[8] = SWIZZLE_RGBA_BGRA(pData[0]);
            pdwBlock[9] = SWIZZLE_RGBA_BGRA(pData[1]);
            pdwBlock[10] = SWIZZLE_RGBA_BGRA(pData[2]);
            pdwBlock[11] = SWIZZLE_RGBA_BGRA(pData[3]);
            pData += (m_dwPitch >> 2);
            pdwBlock[12] = SWIZZLE_RGBA_BGRA(pData[0]);
            pdwBlock[13] = SWIZZLE_RGBA_BGRA(pData[1]);
            pdwBlock[14] = SWIZZLE_RGBA_BGRA(pData[2]);
            pdwBlock[15] = SWIZZLE_RGBA_BGRA(pData[3]);
        } else {
            pdwBlock[0] = pData[0];
            pdwBlock[1] = pData[1];
            pdwBlock[2] = pData[2];
            pdwBlock[3] = pData[3];
            pData += (m_dwPitch >> 2);
            pdwBlock[4] = pData[0];
            pdwBlock[5] = pData[1];
            pdwBlock[6] = pData[2];
            pdwBlock[7] = pData[3];
            pData += (m_dwPitch >> 2);
            pdwBlock[8] = pData[0];
            pdwBlock[9] = pData[1];
            pdwBlock[10] = pData[2];
            pdwBlock[11] = pData[3];
            pData += (m_dwPitch >> 2);
            pdwBlock[12] = pData[0];
            pdwBlock[13] = pData[1];
            pdwBlock[14] = pData[2];
            pdwBlock[15] = pData[3];
        }
    } else {
        CMP_DWORD  minWidth = cmp_minT(w, (GetWidth() - xw));
        CMP_DWORD srcOffset;
        CMP_DWORD iw, jh;
        CMP_BYTE  *srcData = GetData();
        CMP_DWORD *pdwData;

        for (jh = 0; jh < h && (yh + jh) < GetHeight(); jh++) {
            srcOffset = ((yh + jh) * m_dwPitch) + (xw * 4);
            pdwData = (CMP_DWORD*)(srcData + srcOffset);
            if (m_bSwizzle) {
                for (iw = 0; iw < minWidth; iw++) {
                    pdwBlock[(jh * w) + iw] = SWIZZLE_RGBA_BGRA(pdwData[iw]);
                }
            } else {
                for (iw = 0; iw < minWidth; iw++) {
                    pdwBlock[(jh * w) + iw] = pdwData[iw];
                }
            }

            // Pad block with previous values if necessary
            if (iw < w)
                PadLine(iw, w, 4, (CMP_BYTE*)&pdwBlock[jh * w]);
        }

        // Pad block with previous values if necessary
        if (jh < h)
            PadBlock(jh, w, h, 4, (CMP_BYTE*)pdwBlock);
    }

    return true;
}

bool CCodecBuffer_RGBA8888::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]) {
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % 4 == 0);
    assert(y % 4 == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD  dwWidth  = cmp_minT(w, (GetWidth() - x));
    CMP_DWORD* pdwBlock = (CMP_DWORD*) block;

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++) {
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * sizeof(CMP_DWORD)));
        for(CMP_DWORD i = 0; i < dwWidth; i++)
            *pData++ = pdwBlock[(j * w) + i];
    }
    return true;
}


