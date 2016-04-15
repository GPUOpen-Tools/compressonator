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
//  File Name:   CodecBuffer_Block.cpp
//  Description: implementation of the CCodecBuffer_Block class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "CodecBuffer_Block.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodecBuffer_Block::CCodecBuffer_Block(CodecBufferType nCodecBufferType, CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData)
: CCodecBuffer(dwWidth, dwHeight, dwPitch, pData), m_nCodecBufferType(nCodecBufferType)
{
    switch(nCodecBufferType)
    {
        case CBT_4x4Block_2BPP:
            m_dwBlockWidth = 4; m_dwBlockHeight = 4; m_dwBlockBPP = 2; break;
        case CBT_4x4Block_4BPP:
            m_dwBlockWidth = 4; m_dwBlockHeight = 4; m_dwBlockBPP = 4; break;
        case CBT_4x4Block_8BPP:
            m_dwBlockWidth = 4; m_dwBlockHeight = 4; m_dwBlockBPP = 8; break;
        case CBT_4x4Block_16BPP:
            m_dwBlockWidth = 4; m_dwBlockHeight = 4; m_dwBlockBPP = 16; break;
        case CBT_4x4Block_32BPP:
            m_dwBlockWidth = 4; m_dwBlockHeight = 4; m_dwBlockBPP = 32; break;
        case CBT_8x8Block_2BPP:
            m_dwBlockWidth = 8; m_dwBlockHeight = 8; m_dwBlockBPP = 2; break;
        case CBT_8x8Block_4BPP:
            m_dwBlockWidth = 8; m_dwBlockHeight = 8; m_dwBlockBPP = 4; break;
        case CBT_8x8Block_8BPP:
            m_dwBlockWidth = 8; m_dwBlockHeight = 8; m_dwBlockBPP = 8; break;
        case CBT_8x8Block_16BPP:
            m_dwBlockWidth = 8; m_dwBlockHeight = 8; m_dwBlockBPP = 16; break;
        case CBT_8x8Block_32BPP:
            m_dwBlockWidth = 8; m_dwBlockHeight = 8; m_dwBlockBPP = 32; break;
        default:
            assert(0);
    }

    CMP_DWORD dwBlocksX = ((GetWidth() + m_dwBlockWidth - 1) / m_dwBlockWidth);
    CMP_DWORD dwBlocksY = ((GetHeight() + m_dwBlockHeight - 1) / m_dwBlockHeight);
    CMP_DWORD dwBlocks = dwBlocksX * dwBlocksY;
    m_dwBlockSize = m_dwBlockWidth * m_dwBlockHeight * m_dwBlockBPP / 8;
    m_dwPitch = dwBlocksX * m_dwBlockSize;

    if(m_pData == NULL)
    {
        CMP_DWORD dwSize = dwBlocks * m_dwBlockSize;
        m_pData = (CMP_BYTE*) malloc(dwSize);
    }
}

CCodecBuffer_Block::~CCodecBuffer_Block()
{

}

bool CCodecBuffer_Block::ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_DWORD* pBlock, CMP_DWORD dwBlockSize)
{
    assert(pBlock);
    assert(dwBlockSize == m_dwBlockSize / sizeof(CMP_DWORD));
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % m_dwBlockWidth == 0);
    assert(y % m_dwBlockHeight == 0);

    if(!pBlock || dwBlockSize != (m_dwBlockSize / sizeof(CMP_DWORD)) || x >= GetWidth() || y >= GetHeight())
        return false;

    memcpy(pBlock, GetData() + ((y / m_dwBlockHeight) * m_dwPitch) + ((x / m_dwBlockWidth) * dwBlockSize * sizeof(CMP_DWORD)), dwBlockSize * sizeof(CMP_DWORD));

    return true;
}

bool CCodecBuffer_Block::WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_DWORD* pBlock, CMP_DWORD dwBlockSize)
{
    assert(pBlock);
    assert(dwBlockSize == m_dwBlockSize / sizeof(CMP_DWORD));
    assert(x < GetWidth());
    assert(y < GetHeight());
    assert(x % m_dwBlockWidth == 0);
    assert(y % m_dwBlockHeight == 0);

    if(!pBlock || dwBlockSize != m_dwBlockSize / sizeof(CMP_DWORD) || x >= GetWidth() || y >= GetHeight())
        return false;

    memcpy(GetData() + ((y / m_dwBlockHeight) * m_dwPitch) + ((x / m_dwBlockWidth) * dwBlockSize * sizeof(CMP_DWORD)), pBlock, dwBlockSize * sizeof(CMP_DWORD));

    return true;
}
