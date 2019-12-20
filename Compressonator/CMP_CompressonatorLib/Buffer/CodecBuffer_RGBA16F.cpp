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
//  File Name:   CodecBuffer_RGBA16F.cpp
//  Description: implementation of the CCodecBuffer_RGBA16F class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "CodecBuffer_RGBA16F.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

#define RGBA16F_nChannelCount  4
#define RGBA16F_nPixelSize     8

CCodecBuffer_RGBA16F::CCodecBuffer_RGBA16F(
                                            CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
                                            CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData,CMP_DWORD dwDataSize)
                                            : CCodecBuffer(nBlockWidth, nBlockHeight, nBlockDepth,dwWidth, dwHeight, dwPitch, pData,dwDataSize)
{
    if (m_dwPitch == 0) printf("");
    int gw= GetWidth() * RGBA16F_nPixelSize;
    //assert((m_dwPitch == 0) || (m_dwPitch >= GetWidth() * RGBA16F_nPixelSize));
    if(m_dwPitch <= GetWidth() * RGBA16F_nPixelSize)
        m_dwPitch = GetWidth() * RGBA16F_nPixelSize;

    if(m_pData == NULL)
    {
        CMP_DWORD dwSize = m_dwPitch * GetHeight();
        m_pData = (CMP_BYTE*) malloc(dwSize);
    }
}

CCodecBuffer_RGBA16F::~CCodecBuffer_RGBA16F()
{
}

void CCodecBuffer_RGBA16F::Copy(CCodecBuffer& srcBuffer)
{
    if(GetWidth() != srcBuffer.GetWidth() || GetHeight() != srcBuffer.GetHeight())
        return;

    const CMP_DWORD dwBlocksX = ((GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((GetHeight() + 3) >> 2);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            CMP_HALF block[BLOCK_SIZE_4X4X4];
            srcBuffer.ReadBlockRGBA(i*4, j*4, 4, 4, block);
            WriteBlockRGBA(i*4, j*4, 4, 4, block);
        }
    }
}
bool CCodecBuffer_RGBA16F::ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[], CMP_DWORD dwChannelIndex)
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = min(w, (GetWidth() - x));

    CMP_DWORD i,j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_HALF* pData = (CMP_HALF*) (GetData() + ((y + j) * m_dwPitch) + (x * RGBA16F_nPixelSize));
        for(i = 0; i < dwWidth; i++)
        {
            block[(j * w) + i] = pData[dwChannelIndex];
            pData += RGBA16F_nChannelCount;
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

bool CCodecBuffer_RGBA16F::WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[], CMP_DWORD dwChannelIndex)
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = min(w, (GetWidth() - x));

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_HALF* pData = (CMP_HALF*) (GetData() + ((y + j) * m_dwPitch) + (x * RGBA16F_nPixelSize));
        for(CMP_DWORD i = 0; i < dwWidth; i++)
        {
            pData[dwChannelIndex] = block[(j * w) + i]; 
            pData += RGBA16F_nChannelCount;
        }
    }
    return true;
}

bool CCodecBuffer_RGBA16F::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[])
{
    return ReadBlock(x, y, w, h, block, CHANNEL_INDEX_A);
}

bool CCodecBuffer_RGBA16F::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[])
{
    return ReadBlock(x, y, w, h, block, CHANNEL_INDEX_R);
}

bool CCodecBuffer_RGBA16F::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[])
{
    return ReadBlock(x, y, w, h, block, CHANNEL_INDEX_G);
}

bool CCodecBuffer_RGBA16F::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[])
{
    return ReadBlock(x, y, w, h, block, CHANNEL_INDEX_B);
}

bool CCodecBuffer_RGBA16F::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[])
{
    return WriteBlock(x, y, w, h, block, CHANNEL_INDEX_A);
}

bool CCodecBuffer_RGBA16F::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[])
{
    return WriteBlock(x, y, w, h, block, CHANNEL_INDEX_R);
}

bool CCodecBuffer_RGBA16F::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[])
{
    return WriteBlock(x, y, w, h, block, CHANNEL_INDEX_G);
}

bool CCodecBuffer_RGBA16F::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[])
{
    return WriteBlock(x, y, w, h, block, CHANNEL_INDEX_B);
}

#define GET_PIXEL(i, j) &block[(((j * w) + i) * RGBA16F_nChannelCount)]
bool CCodecBuffer_RGBA16F::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % w == 0);
    assert(y % h == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = min(w, (GetWidth() - x));

    CMP_DWORD i,j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_HALF* pData = (CMP_HALF*) (GetData() + ((y + j) * m_dwPitch) + (x * RGBA16F_nPixelSize));
        for(i = 0; i < dwWidth; i++)
        {
            memcpy(GET_PIXEL(i, j), pData, RGBA16F_nPixelSize);    
            pData += RGBA16F_nChannelCount;
        }

        // Pad line with previous values if necessary
        if(i < w)
            PadLine(i, w, 4, &block[j * w * RGBA16F_nChannelCount]);
    }

    // Pad block with previous values if necessary
    if(j < h)
        PadBlock(j, w, h, 4, block);
    return true;
}

bool CCodecBuffer_RGBA16F::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF block[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % w == 0);
    assert(y % h == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = min(w, (GetWidth() - x));

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_HALF* pData = (CMP_HALF*) (GetData() + ((y + j) * m_dwPitch) + (x * RGBA16F_nPixelSize));
        memcpy(pData, GET_PIXEL(0, j), dwWidth * RGBA16F_nPixelSize);
    }
    return true;
}
