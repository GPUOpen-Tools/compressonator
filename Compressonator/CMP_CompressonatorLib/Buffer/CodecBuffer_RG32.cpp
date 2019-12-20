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
//  File Name:   CodecBuffer_RG32.cpp
//  Description: implementation of the CCodecBuffer_RG32 class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "CodecBuffer_RG32.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

const int nChannelCount = 2;
const int nPixelSize = nChannelCount * sizeof(CMP_DWORD);

CCodecBuffer_RG32::CCodecBuffer_RG32(
    CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
    CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData,CMP_DWORD dwDataSize)
    : CCodecBuffer(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize)
{
    assert((m_dwPitch == 0) || (m_dwPitch >= GetWidth() * nPixelSize));
    if(m_dwPitch <= GetWidth() * nPixelSize)
        m_dwPitch = GetWidth() * nPixelSize;

    if(m_pData == NULL)
    {
        CMP_DWORD dwSize = m_dwPitch * GetHeight();
        m_pData = (CMP_BYTE*) malloc(dwSize);
    }
}

CCodecBuffer_RG32::~CCodecBuffer_RG32()
{

}

void CCodecBuffer_RG32::Copy(CCodecBuffer& srcBuffer)
{
    if(GetWidth() != srcBuffer.GetWidth() || GetHeight() != srcBuffer.GetHeight())
        return;

    const CMP_DWORD dwBlocksX = ((GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((GetHeight() + 3) >> 2);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            CMP_DWORD block[BLOCK_SIZE_4X4X4];
            srcBuffer.ReadBlockRGBA(i*4, j*4, 4, 4, block);
            WriteBlockRGBA(i*4, j*4, 4, 4, block);
        }
    }
}
bool CCodecBuffer_RG32::ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[], CMP_DWORD dwChannelOffset)
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = min(w, (GetWidth() - x));

    CMP_DWORD i,j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for(i = 0; i < dwWidth; i++)
        {
            dwBlock[(j * w) + i] = *(pData + dwChannelOffset);
            pData += nChannelCount;
        }

        // Pad line with previous values if necessary
        if(i < w)
            PadLine(i, w, 1, &dwBlock[j * w]);
    }

    // Pad block with previous values if necessary
    if(j < h)
        PadBlock(j, w, h, 1, dwBlock);

    return true;
}

bool CCodecBuffer_RG32::WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[], CMP_DWORD dwChannelOffset)
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = min(w, (GetWidth() - x));

    for(CMP_DWORD j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * nChannelCount * sizeof(CMP_DWORD)));
        for(CMP_DWORD i = 0; i < dwWidth; i++)
        {
            *(pData + dwChannelOffset) = dwBlock[(j * w) + i]; 
            pData += nChannelCount;
        }
    }
    return true;
}

bool CCodecBuffer_RG32::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    memset(dwBlock, 0, w*h*sizeof(CMP_DWORD));

    return true;
}

#define RG32_OFFSET_R 1
#define RG32_OFFSET_G 0

bool CCodecBuffer_RG32::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
    return ReadBlock(x, y, w, h, dwBlock, RG32_OFFSET_R);
}

bool CCodecBuffer_RG32::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
    return ReadBlock(x, y, w, h, dwBlock, RG32_OFFSET_G);
}

bool CCodecBuffer_RG32::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    memset(dwBlock, 0, w*h*sizeof(CMP_DWORD));

    return true;
}

bool CCodecBuffer_RG32::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE /*w*/, CMP_BYTE /*h*/, CMP_DWORD /*dwBlock*/[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    return true;
}

bool CCodecBuffer_RG32::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
    return WriteBlock(x, y, w, h, dwBlock, RG32_OFFSET_R);
}

bool CCodecBuffer_RG32::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
    return WriteBlock(x, y, w, h, dwBlock, RG32_OFFSET_G);
}

bool CCodecBuffer_RG32::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE /*w*/, CMP_BYTE /*h*/, CMP_DWORD /*dwBlock*/[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    return true;
}

#define GET_PIXEL(i, j) &dwBlock[(((j * w) + i) * 4)]
bool CCodecBuffer_RG32::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % w == 0);
    assert(y % h == 0);

    if(x >= GetWidth() || y >= GetHeight())
        return false;

    CMP_DWORD dwWidth = min(w, (GetWidth() - x));

    CMP_DWORD i, j;
    for(j = 0; j < h && (y + j) < GetHeight(); j++)
    {
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for(i = 0; i < dwWidth; i++)
        {
            CMP_DWORD* pDest = GET_PIXEL(i, j);
            memcpy(GET_PIXEL(i, j), pData, nPixelSize);    
            pData += nChannelCount; pDest += 2;
            *pDest++ = 0;
            *pDest++ = 0xffffffff;
        }

        // Pad line with previous values if necessary
        if(i < w)
            PadLine(i, w, 4, &dwBlock[j * w * 4]);
    }

    // Pad block with previous values if necessary
    if(j < h)
        PadBlock(j, w, h, 4, dwBlock);
    return true;
}

bool CCodecBuffer_RG32::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
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
        CMP_DWORD* pData = (CMP_DWORD*) (GetData() + ((y + j) * m_dwPitch) + (x * nPixelSize));
        for(CMP_DWORD i = 0; i < dwWidth; i++)
        {
            memcpy(pData, GET_PIXEL(i, j), nPixelSize);
            pData += nChannelCount;
        }
    }

    return true;
}

