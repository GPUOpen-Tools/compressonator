//===============================================================================
// Copyright (c) 2007-2024  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   CodecBuffer_RGBA16.cpp
//  Description: implementation of the CCodecBuffer_RGBA16 class
//
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codecbuffer_rgba16.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

const int nChannelCount = 4;
const int nPixelSize    = nChannelCount * sizeof(CMP_WORD);

CCodecBuffer_RGBA16::CCodecBuffer_RGBA16(CMP_BYTE  nBlockWidth,
                                         CMP_BYTE  nBlockHeight,
                                         CMP_BYTE  nBlockDepth,
                                         CMP_DWORD dwWidth,
                                         CMP_DWORD dwHeight,
                                         CMP_DWORD dwPitch,
                                         CMP_BYTE* pData,
                                         CMP_DWORD dwDataSize)
    : CCodecBuffer(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData, dwDataSize)
{
    assert((m_dwPitch == 0) || (m_dwPitch >= GetWidth() * nPixelSize));
    if (m_dwPitch <= GetWidth() * nPixelSize)
        m_dwPitch = GetWidth() * nPixelSize;

    if (m_pData == NULL)
    {
        CMP_DWORD dwSize = m_dwPitch * GetHeight();
        m_pData          = (CMP_BYTE*)malloc(dwSize);
    }
}

CCodecBuffer_RGBA16::~CCodecBuffer_RGBA16()
{
}

void CCodecBuffer_RGBA16::Copy(CCodecBuffer& srcBuffer)
{
    if (GetWidth() != srcBuffer.GetWidth() || GetHeight() != srcBuffer.GetHeight())
        return;

    const CMP_DWORD dwBlocksX = ((GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((GetHeight() + 3) >> 2);

    for (CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for (CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            CMP_WORD block[BLOCK_SIZE_4X4X4];
            srcBuffer.ReadBlockRGBA(i * 4, j * 4, 4, 4, block);
            WriteBlockRGBA(i * 4, j * 4, 4, 4, block);
        }
    }
}
bool CCodecBuffer_RGBA16::ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[], CMP_DWORD dwChannelOffset)
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if (x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    CMP_DWORD i, j;
    for (j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_WORD* pData = (CMP_WORD*)(GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for (i = 0; i < dwWidth; i++)
        {
            wBlock[(j * w) + i] = pData[dwChannelOffset];
            pData += nChannelCount;
        }

        // Pad line with previous values if necessary
        if (i < w)
            PadLine(i, w, 1, &wBlock[j * w]);
    }

    // Pad block with previous values if necessary
    if (j < h)
        PadBlock(j, w, h, 1, wBlock);

    return true;
}

bool CCodecBuffer_RGBA16::WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[], CMP_DWORD dwChannelOffset)
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if (x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    for (CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_WORD* pData = (CMP_WORD*)(GetData() + ((y + j) * m_dwPitch) + (x * 4 * sizeof(CMP_WORD)));
        for (CMP_DWORD i = 0; i < dwWidth; i++)
        {
            *(pData + dwChannelOffset) = wBlock[(j * w) + i];
            pData += 4;
        }
    }
    return true;
}

bool CCodecBuffer_RGBA16::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
    return ReadBlock(x, y, w, h, wBlock, RGBA16_OFFSET_A);
}

bool CCodecBuffer_RGBA16::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
    return ReadBlock(x, y, w, h, wBlock, RGBA16_OFFSET_R);
}

bool CCodecBuffer_RGBA16::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
    return ReadBlock(x, y, w, h, wBlock, RGBA16_OFFSET_G);
}

bool CCodecBuffer_RGBA16::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
    return ReadBlock(x, y, w, h, wBlock, RGBA16_OFFSET_B);
}

bool CCodecBuffer_RGBA16::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
    return WriteBlock(x, y, w, h, wBlock, RGBA16_OFFSET_A);
}

bool CCodecBuffer_RGBA16::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
    return WriteBlock(x, y, w, h, wBlock, RGBA16_OFFSET_R);
}

bool CCodecBuffer_RGBA16::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
    return WriteBlock(x, y, w, h, wBlock, RGBA16_OFFSET_G);
}

bool CCodecBuffer_RGBA16::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
    return WriteBlock(x, y, w, h, wBlock, RGBA16_OFFSET_B);
}

#define GET_PIXEL(i, j) &wBlock[(((j * w) + i) * 4)]
bool CCodecBuffer_RGBA16::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % w == 0);
    assert(y % h == 0);

    if (x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    CMP_DWORD i, j;
    for (j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_WORD* pData = (CMP_WORD*)(GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for (i = 0; i < dwWidth; i++)
        {
            memcpy(GET_PIXEL(i, j), pData, nPixelSize);
            pData += 4;
        }

        // Pad line with previous values if necessary
        if (i < w)
            PadLine(i, w, 4, &wBlock[j * w * 4]);
    }

    // Pad block with previous values if necessary
    if (j < h)
        PadBlock(j, w, h, 4, wBlock);
    return true;
}

bool CCodecBuffer_RGBA16::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % w == 0);
    assert(y % h == 0);

    if (x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = cmp_minT(w, (GetWidth() - x));

    for (CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_WORD* pData = (CMP_WORD*)(GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        memcpy(pData, GET_PIXEL(0, j), dwWidth * nPixelSize);
    }

    return true;
}
