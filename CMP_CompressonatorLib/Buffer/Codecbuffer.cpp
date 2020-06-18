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
//  File Name:   CodecBuffer.cpp
//  Description: implementation of the CCodecBuffer class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "CodecBuffer.h"
#include "CodecBuffer_RGBA8888.h"
#include "CodecBuffer_RGB888.h"
#include "CodecBuffer_RG8.h"
#include "CodecBuffer_R8.h"
#include "CodecBuffer_RGBA2101010.h"
#include "CodecBuffer_RGBA16.h"
#include "CodecBuffer_RG16.h"
#include "CodecBuffer_R16.h"
#include "CodecBuffer_RGBA32.h"
#include "CodecBuffer_RG32.h"
#include "CodecBuffer_R32.h"
#include "CodecBuffer_RGBA16F.h"
#include "CodecBuffer_RG16F.h"
#include "CodecBuffer_R16F.h"
#include "CodecBuffer_RGBA32F.h"
#include "CodecBuffer_RG32F.h"
#include "CodecBuffer_R32F.h"
#include "CodecBuffer_Block.h"
#include "CodecBuffer_RGB9995EF.h"


CCodecBuffer* CreateCodecBuffer(CodecBufferType nCodecBufferType, 
                                CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
                                CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData,
                                CMP_DWORD dwDataSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(("nCodecBufferType %d dwWidth %d dwHeight %d dwPitch %d pData [%x]",nCodecBufferType,dwWidth, dwHeight, dwPitch, pData));  
#endif
    switch(nCodecBufferType)
    {
        case CBT_RGBA8888:
        case CBT_BGRA8888:
        case CBT_ARGB8888:
            return new CCodecBuffer_RGBA8888(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RGB888:
            return new CCodecBuffer_RGB888(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RG8:
            return new CCodecBuffer_RG8(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_R8:
            return new CCodecBuffer_R8(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RGBA2101010:
            return new CCodecBuffer_RGBA2101010(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RGBA16:
            return new CCodecBuffer_RGBA16(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RG16:
            return new CCodecBuffer_RG16(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_R16:
            return new CCodecBuffer_R16(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RGBA16F:
            return new CCodecBuffer_RGBA16F(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RG16F:
            return new CCodecBuffer_RG16F(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_R16F:
            return new CCodecBuffer_R16F(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RGBA32:
            return new CCodecBuffer_RGBA32(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RG32:
            return new CCodecBuffer_RG32(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_R32:
            return new CCodecBuffer_R32(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RGBA32F:
            return new CCodecBuffer_RGBA32F(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RG32F:
            return new CCodecBuffer_RG32F(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_R32F:
            return new CCodecBuffer_R32F(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_RGBE32F:
            return new CCodecBuffer_RGB9995EF(nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_4x4Block_2BPP:
        case CBT_4x4Block_4BPP:
        case CBT_4x4Block_8BPP:
        case CBT_4x4Block_16BPP:
            return new CCodecBuffer_Block(nCodecBufferType, nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
        case CBT_8x8Block_2BPP:
        case CBT_8x8Block_4BPP:
        case CBT_8x8Block_8BPP:
        case CBT_8x8Block_16BPP:
            return new CCodecBuffer_Block(nCodecBufferType, nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);

        case CBT_Unknown:
        default:
            assert(0);
            return NULL;
    }
}

CodecBufferType GetCodecBufferType(CMP_FORMAT format)
{
    CodecBufferType CBT_type;
#ifdef USE_DBGTRACE
    DbgTrace(("IN : CMP_FORMAT %d",format)); 
#endif
    // ToDo Expand the CBT data types listed below so that CMP_FORMAT maps to a single CBT_ type
    switch(format)
    {
        case CMP_FORMAT_ARGB_32F:
        case CMP_FORMAT_ABGR_32F:
        case CMP_FORMAT_RGBA_32F:
        case CMP_FORMAT_BGRA_32F:   
                                    CBT_type = CBT_RGBA32F;
                                    break;
        case CMP_FORMAT_RG_32F: 
                                    CBT_type = CBT_RG32F;
                                    break;
        case CMP_FORMAT_R_32F: 
                                    CBT_type = CBT_R32F;
                                    break;
        case CMP_FORMAT_ARGB_16F:
        case CMP_FORMAT_ABGR_16F:
        case CMP_FORMAT_RGBA_16F:
        case CMP_FORMAT_BGRA_16F:
                                    CBT_type =  CBT_RGBA16F;
                                    break;
        case CMP_FORMAT_RGBE_32F:
                                    CBT_type = CBT_RGBE32F;
                                    break;
        case CMP_FORMAT_RG_16F: 
                                    CBT_type =  CBT_RG16F;
                                    break;
        case CMP_FORMAT_R_16F: 
                                    CBT_type =  CBT_R16F;
                                    break;
        case CMP_FORMAT_ARGB_16:
        case CMP_FORMAT_ABGR_16:
        case CMP_FORMAT_RGBA_16:
        case CMP_FORMAT_BGRA_16:
                                    CBT_type =  CBT_RGBA16;
                                    break;
        case CMP_FORMAT_RG_16: 
                                    CBT_type =  CBT_RG16;
                                    break;
        case CMP_FORMAT_R_16: 
                                    CBT_type = CBT_R16;
                                    break;

#ifdef ARGB_32_SUPPORT
        case TI_TC_FORMAT_ARGB_32: 
                                    CBT_type =  CBT_RGBA32;
                                    break;
        case TI_TC_FORMAT_RG_32: 
                                    CBT_type =  CBT_RG32;
                                    break;
        case TI_TC_FORMAT_R_32: 
                                    CBT_type = CBT_R32;
                                    break;
#endif // ARGB_32_SUPPORT
        case CMP_FORMAT_ARGB_2101010: 
                                    CBT_type = CBT_RGBA2101010;
                                    break;
        case CMP_FORMAT_ARGB_8888:  // Need to expand on this format
        case CMP_FORMAT_BGRA_8888:  // Need to expand on this format
        case CMP_FORMAT_RGBA_8888:
                                    CBT_type = CBT_RGBA8888;
                                    break;
        case CMP_FORMAT_BGR_888:    // Need to expand on this format
        case CMP_FORMAT_RGB_888: 
                                    CBT_type = CBT_RGB888;
                                    break;
        case CMP_FORMAT_RG_8: 
                                    CBT_type = CBT_RG8;
                                    break;
        case CMP_FORMAT_R_8: 
                                    CBT_type = CBT_R8;
                                    break;
        default: 
                                    CBT_type = CBT_Unknown;
                                    break;
    }
#ifdef USE_DBGTRACE
    DbgTrace(("OUT: %d",CBT_type));
#endif
    return CBT_type;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCodecBuffer::CCodecBuffer(
                            CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
                            CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData,CMP_DWORD dwDataSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(("dwWidth %d,dwHeight %d,dwPitch %d pData [%x]",dwWidth,dwHeight,dwPitch,pData));  
#endif
    m_dwWidth  = dwWidth;
    m_dwHeight = dwHeight;
    m_dwPitch  = dwPitch;

    m_nBlockWidth = nBlockWidth <4?4:nBlockWidth;
    m_nBlockHeight= nBlockHeight<4?4:nBlockHeight;
    m_nBlockDepth = nBlockDepth <1?1:nBlockDepth;

    m_pData             = pData;
    m_bUserAllocedData  = (pData != NULL);
    m_DataSize          = dwDataSize;

    m_bPerformingConversion = false;
    m_bSwizzle = false;
}

CCodecBuffer::~CCodecBuffer()
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    if(m_pData && !m_bUserAllocedData)
    {
        free(m_pData);
        m_pData = NULL;
    }
}

void CCodecBuffer::Copy(CCodecBuffer& srcBuffer)
{
#ifdef USE_DBGTRACE
    DbgTrace(("srcBuffer [%x]",srcBuffer)); 
#endif
    if(GetWidth() != srcBuffer.GetWidth() || GetHeight() != srcBuffer.GetHeight())
        return;

    const CMP_DWORD dwBlocksX = ((GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((GetHeight() + 3) >> 2);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            float block[BLOCK_SIZE_4X4X4];
            srcBuffer.ReadBlockRGBA(i*4, j*4, 4, 4, block);
            WriteBlockRGBA(i*4, j*4, 4, 4, block);
        }
    }
}

#define MAX_BLOCK_WIDTH 8
#define MAX_BLOCK_HEIGHT 8
#define MAX_BLOCK MAX_BLOCK_WIDTH*MAX_BLOCK_HEIGHT

#define ATTEMPT_BLOCK_READ(b, c, t) \
{ \
    t block[MAX_BLOCK]; \
    if(ReadBlock##c(x, y, w, h, block)) \
    { \
        ConvertBlock(b, block, w * h); \
        m_bPerformingConversion = false; \
        return true; \
    } \
}

bool CCodecBuffer::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(cBlock, R, double);
        ATTEMPT_BLOCK_READ(cBlock, R, float);
        ATTEMPT_BLOCK_READ(cBlock, R, CMP_HALF);
        ATTEMPT_BLOCK_READ(cBlock, R, CMP_DWORD);
        ATTEMPT_BLOCK_READ(cBlock, R, CMP_WORD);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(cBlock, G, double);
        ATTEMPT_BLOCK_READ(cBlock, G, float);
        ATTEMPT_BLOCK_READ(cBlock, G, CMP_HALF);
        ATTEMPT_BLOCK_READ(cBlock, G, CMP_DWORD);
        ATTEMPT_BLOCK_READ(cBlock, G, CMP_WORD);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(cBlock, B, double);
        ATTEMPT_BLOCK_READ(cBlock, B, float);
        ATTEMPT_BLOCK_READ(cBlock, B, CMP_HALF);
        ATTEMPT_BLOCK_READ(cBlock, B, CMP_DWORD);
        ATTEMPT_BLOCK_READ(cBlock, B, CMP_WORD);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(cBlock, A, double);
        ATTEMPT_BLOCK_READ(cBlock, A, float);
        ATTEMPT_BLOCK_READ(cBlock, A, CMP_HALF);
        ATTEMPT_BLOCK_READ(cBlock, A, CMP_DWORD);
        ATTEMPT_BLOCK_READ(cBlock, A, CMP_WORD);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif 
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(wBlock, R, double);
        ATTEMPT_BLOCK_READ(wBlock, R, float);
        ATTEMPT_BLOCK_READ(wBlock, R, CMP_HALF);
        ATTEMPT_BLOCK_READ(wBlock, R, CMP_DWORD);
        ATTEMPT_BLOCK_READ(wBlock, R, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(wBlock, G, double);
        ATTEMPT_BLOCK_READ(wBlock, G, float);
        ATTEMPT_BLOCK_READ(wBlock, G, CMP_HALF);
        ATTEMPT_BLOCK_READ(wBlock, G, CMP_DWORD);
        ATTEMPT_BLOCK_READ(wBlock, G, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif 
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(wBlock, B, double);
        ATTEMPT_BLOCK_READ(wBlock, B, float);
        ATTEMPT_BLOCK_READ(wBlock, B, CMP_HALF);
        ATTEMPT_BLOCK_READ(wBlock, B, CMP_DWORD);
        ATTEMPT_BLOCK_READ(wBlock, B, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(wBlock, A, double);
        ATTEMPT_BLOCK_READ(wBlock, A, float);
        ATTEMPT_BLOCK_READ(wBlock, A, CMP_HALF);
        ATTEMPT_BLOCK_READ(wBlock, A, CMP_DWORD);
        ATTEMPT_BLOCK_READ(wBlock, A, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif 
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(dwBlock, R, double);
        ATTEMPT_BLOCK_READ(dwBlock, R, float);
        ATTEMPT_BLOCK_READ(dwBlock, R, CMP_HALF);
        ATTEMPT_BLOCK_READ(dwBlock, R, CMP_WORD);
        ATTEMPT_BLOCK_READ(dwBlock, R, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(dwBlock, G, double);
        ATTEMPT_BLOCK_READ(dwBlock, G, float);
        ATTEMPT_BLOCK_READ(dwBlock, G, CMP_HALF);
        ATTEMPT_BLOCK_READ(dwBlock, G, CMP_WORD);
        ATTEMPT_BLOCK_READ(dwBlock, G, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(dwBlock, B, double);
        ATTEMPT_BLOCK_READ(dwBlock, B, float);
        ATTEMPT_BLOCK_READ(dwBlock, B, CMP_HALF);
        ATTEMPT_BLOCK_READ(dwBlock, B, CMP_WORD);
        ATTEMPT_BLOCK_READ(dwBlock, B, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(dwBlock, A, double);
        ATTEMPT_BLOCK_READ(dwBlock, A, float);
        ATTEMPT_BLOCK_READ(dwBlock, A, CMP_HALF);
        ATTEMPT_BLOCK_READ(dwBlock, A, CMP_WORD);
        ATTEMPT_BLOCK_READ(dwBlock, A, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(hBlock, R, double);
        ATTEMPT_BLOCK_READ(hBlock, R, float);
        ATTEMPT_BLOCK_READ(hBlock, R, CMP_DWORD);
        ATTEMPT_BLOCK_READ(hBlock, R, CMP_WORD);
        ATTEMPT_BLOCK_READ(hBlock, R, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(hBlock, G, double);
        ATTEMPT_BLOCK_READ(hBlock, G, float);
        ATTEMPT_BLOCK_READ(hBlock, G, CMP_DWORD);
        ATTEMPT_BLOCK_READ(hBlock, G, CMP_WORD);
        ATTEMPT_BLOCK_READ(hBlock, G, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(hBlock, B, double);
        ATTEMPT_BLOCK_READ(hBlock, B, float);
        ATTEMPT_BLOCK_READ(hBlock, B, CMP_DWORD);
        ATTEMPT_BLOCK_READ(hBlock, B, CMP_WORD);
        ATTEMPT_BLOCK_READ(hBlock, B, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(hBlock, A, double);
        ATTEMPT_BLOCK_READ(hBlock, A, float);
        ATTEMPT_BLOCK_READ(hBlock, A, CMP_DWORD);
        ATTEMPT_BLOCK_READ(hBlock, A, CMP_WORD);
        ATTEMPT_BLOCK_READ(hBlock, A, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(fBlock, R, double);
        ATTEMPT_BLOCK_READ(fBlock, R, CMP_HALF);
        ATTEMPT_BLOCK_READ(fBlock, R, CMP_DWORD);
        ATTEMPT_BLOCK_READ(fBlock, R, CMP_WORD);
        ATTEMPT_BLOCK_READ(fBlock, R, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(fBlock, G, double);
        ATTEMPT_BLOCK_READ(fBlock, G, CMP_HALF);
        ATTEMPT_BLOCK_READ(fBlock, G, CMP_DWORD);
        ATTEMPT_BLOCK_READ(fBlock, G, CMP_WORD);
        ATTEMPT_BLOCK_READ(fBlock, G, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(fBlock, B, double);
        ATTEMPT_BLOCK_READ(fBlock, B, CMP_HALF);
        ATTEMPT_BLOCK_READ(fBlock, B, CMP_DWORD);
        ATTEMPT_BLOCK_READ(fBlock, B, CMP_WORD);
        ATTEMPT_BLOCK_READ(fBlock, B, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(fBlock, A, double);
        ATTEMPT_BLOCK_READ(fBlock, A, CMP_HALF);
        ATTEMPT_BLOCK_READ(fBlock, A, CMP_DWORD);
        ATTEMPT_BLOCK_READ(fBlock, A, CMP_WORD);
        ATTEMPT_BLOCK_READ(fBlock, A, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(dBlock, R, float);
        ATTEMPT_BLOCK_READ(dBlock, R, CMP_HALF);
        ATTEMPT_BLOCK_READ(dBlock, R, CMP_DWORD);
        ATTEMPT_BLOCK_READ(dBlock, R, CMP_WORD);
        ATTEMPT_BLOCK_READ(dBlock, R, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(dBlock, G, float);
        ATTEMPT_BLOCK_READ(dBlock, G, CMP_HALF);
        ATTEMPT_BLOCK_READ(dBlock, G, CMP_DWORD);
        ATTEMPT_BLOCK_READ(dBlock, G, CMP_WORD);
        ATTEMPT_BLOCK_READ(dBlock, G, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(dBlock, B, float);
        ATTEMPT_BLOCK_READ(dBlock, B, CMP_HALF);
        ATTEMPT_BLOCK_READ(dBlock, B, CMP_DWORD);
        ATTEMPT_BLOCK_READ(dBlock, B, CMP_WORD);
        ATTEMPT_BLOCK_READ(dBlock, B, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_READ(dBlock, A, float);
        ATTEMPT_BLOCK_READ(dBlock, A, CMP_HALF);
        ATTEMPT_BLOCK_READ(dBlock, A, CMP_DWORD);
        ATTEMPT_BLOCK_READ(dBlock, A, CMP_WORD);
        ATTEMPT_BLOCK_READ(dBlock, A, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

#define ATTEMPT_BLOCK_WRITE(b, c, t) \
{ \
    t block[MAX_BLOCK]; \
    ConvertBlock(block, b, w * h); \
    if(WriteBlock##c(x, y, w, h, block)) \
    { \
        m_bPerformingConversion = false; \
        return true; \
    } \
}

bool CCodecBuffer::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(cBlock, R, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(cBlock, R, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(cBlock, R, double);
        ATTEMPT_BLOCK_WRITE(cBlock, R, float);
        ATTEMPT_BLOCK_WRITE(cBlock, R, CMP_HALF);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(cBlock, G, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(cBlock, G, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(cBlock, G, double);
        ATTEMPT_BLOCK_WRITE(cBlock, G, float);
        ATTEMPT_BLOCK_WRITE(cBlock, G, CMP_HALF);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(cBlock, B, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(cBlock, B, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(cBlock, B, double);
        ATTEMPT_BLOCK_WRITE(cBlock, B, float);
        ATTEMPT_BLOCK_WRITE(cBlock, B, CMP_HALF);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(cBlock, A, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(cBlock, A, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(cBlock, A, double);
        ATTEMPT_BLOCK_WRITE(cBlock, A, float);
        ATTEMPT_BLOCK_WRITE(cBlock, A, CMP_HALF);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(wBlock, R, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(wBlock, R, double);
        ATTEMPT_BLOCK_WRITE(wBlock, R, float);
        ATTEMPT_BLOCK_WRITE(wBlock, R, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(wBlock, R, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(wBlock, G, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(wBlock, G, double);
        ATTEMPT_BLOCK_WRITE(wBlock, G, float);
        ATTEMPT_BLOCK_WRITE(wBlock, G, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(wBlock, G, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(wBlock, B, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(wBlock, B, double);
        ATTEMPT_BLOCK_WRITE(wBlock, B, float);
        ATTEMPT_BLOCK_WRITE(wBlock, B, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(wBlock, B, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(wBlock, A, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(wBlock, A, double);
        ATTEMPT_BLOCK_WRITE(wBlock, A, float);
        ATTEMPT_BLOCK_WRITE(wBlock, A, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(wBlock, A, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(dwBlock, R, double);
        ATTEMPT_BLOCK_WRITE(dwBlock, R, float);
        ATTEMPT_BLOCK_WRITE(dwBlock, R, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(dwBlock, R, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(dwBlock, R, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(dwBlock, G, double);
        ATTEMPT_BLOCK_WRITE(dwBlock, G, float);
        ATTEMPT_BLOCK_WRITE(dwBlock, G, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(dwBlock, G, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(dwBlock, G, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(dwBlock, B, double);
        ATTEMPT_BLOCK_WRITE(dwBlock, B, float);
        ATTEMPT_BLOCK_WRITE(dwBlock, B, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(dwBlock, B, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(dwBlock, B, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(dwBlock, A, double);
        ATTEMPT_BLOCK_WRITE(dwBlock, A, float);
        ATTEMPT_BLOCK_WRITE(dwBlock, A, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(dwBlock, A, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(dwBlock, A, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(hBlock, R, double);
        ATTEMPT_BLOCK_WRITE(hBlock, R, float);
        ATTEMPT_BLOCK_WRITE(hBlock, R, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(hBlock, R, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(hBlock, R, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(hBlock, G, double);
        ATTEMPT_BLOCK_WRITE(hBlock, G, float);
        ATTEMPT_BLOCK_WRITE(hBlock, G, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(hBlock, G, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(hBlock, G, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(hBlock, B, double);
        ATTEMPT_BLOCK_WRITE(hBlock, B, float);
        ATTEMPT_BLOCK_WRITE(hBlock, B, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(hBlock, B, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(hBlock, B, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(hBlock, A, double);
        ATTEMPT_BLOCK_WRITE(hBlock, A, float);
        ATTEMPT_BLOCK_WRITE(hBlock, A, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(hBlock, A, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(hBlock, A, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(fBlock, R, double);
        ATTEMPT_BLOCK_WRITE(fBlock, R, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(fBlock, R, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(fBlock, R, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(fBlock, R, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(fBlock, G, double);
        ATTEMPT_BLOCK_WRITE(fBlock, G, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(fBlock, G, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(fBlock, G, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(fBlock, G, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(fBlock, B, double);
        ATTEMPT_BLOCK_WRITE(fBlock, B, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(fBlock, B, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(fBlock, B, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(fBlock, B, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(fBlock, A, double);
        ATTEMPT_BLOCK_WRITE(fBlock, A, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(fBlock, A, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(fBlock, A, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(fBlock, A, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(dBlock, R, float);
        ATTEMPT_BLOCK_WRITE(dBlock, R, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(dBlock, R, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(dBlock, R, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(dBlock, R, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(dBlock, G, float);
        ATTEMPT_BLOCK_WRITE(dBlock, G, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(dBlock, G, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(dBlock, G, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(dBlock, G, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(dBlock, B, float);
        ATTEMPT_BLOCK_WRITE(dBlock, B, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(dBlock, B, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(dBlock, B, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(dBlock, B, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        ATTEMPT_BLOCK_WRITE(dBlock, A, float);
        ATTEMPT_BLOCK_WRITE(dBlock, A, CMP_HALF);
        ATTEMPT_BLOCK_WRITE(dBlock, A, CMP_DWORD);
        ATTEMPT_BLOCK_WRITE(dBlock, A, CMP_WORD);
        ATTEMPT_BLOCK_WRITE(dBlock, A, CMP_BYTE);

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        CMP_DWORD dwBlock[MAX_BLOCK*4];
        ConvertBlock(dwBlock, cBlock, w*h*4);
        SwizzleBlock(dwBlock, w*h);
        if(WriteBlockRGBA(x, y, w, h, dwBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_WORD wBlock[MAX_BLOCK*4];
        ConvertBlock(wBlock, cBlock, w*h*4);
        SwizzleBlock(wBlock, w*h);
        if(WriteBlockRGBA(x, y, w, h, wBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        double dBlock[MAX_BLOCK*4];
        ConvertBlock(dBlock, cBlock, w*h*4);
        SwizzleBlock(dBlock, w*h);
        if(WriteBlockRGBA(x, y, w, h, dBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        float fBlock[MAX_BLOCK*4];
        ConvertBlock(fBlock, cBlock, w*h*4);
        SwizzleBlock(fBlock, w*h);
        if(WriteBlockRGBA(x, y, w, h, fBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_HALF hBlock[MAX_BLOCK*4];
        ConvertBlock(hBlock, cBlock, w*h*4);
        SwizzleBlock(hBlock, w*h);
        if(WriteBlockRGBA(x, y, w, h, hBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE cBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        double dBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, dBlock))
        {
            SwizzleBlock(dBlock, w*h);
            ConvertBlock(cBlock, dBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        float fBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, fBlock))
        {
            SwizzleBlock(fBlock, w*h);
            ConvertBlock(cBlock, fBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_HALF hBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, hBlock))
        {
            SwizzleBlock(hBlock, w*h);
            ConvertBlock(cBlock, hBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_DWORD dwBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, dwBlock))
        {
            SwizzleBlock(dwBlock, w*h);
            ConvertBlock(cBlock, dwBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_WORD wBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, wBlock))
        {
            SwizzleBlock(wBlock, w*h);
            ConvertBlock(cBlock, wBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        double dBlock[MAX_BLOCK*4];
        ConvertBlock(dBlock, dwBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, dBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        float fBlock[MAX_BLOCK*4];
        ConvertBlock(fBlock, dwBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, fBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_HALF hBlock[MAX_BLOCK*4];
        ConvertBlock(hBlock, dwBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, hBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_WORD wBlock[MAX_BLOCK];
        SwizzleBlock(dwBlock, w*h);
        ConvertBlock(wBlock, dwBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, wBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_DWORD dwBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        double dBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, dBlock))
        {
            ConvertBlock(dwBlock, dBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        float fBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, fBlock))
        {
            ConvertBlock(dwBlock, fBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_HALF hBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, hBlock))
        {
            ConvertBlock(dwBlock, hBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        unsigned char cBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, cBlock))
        {
            ConvertBlock(dwBlock, cBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_WORD wBlock[MAX_BLOCK];
        if(ReadBlockRGBA(x, y, w, h, dwBlock))
        {
            ConvertBlock(dwBlock, wBlock, w*h*4);
            SwizzleBlock(dwBlock, w*h);
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        double dBlock[MAX_BLOCK*4];
        ConvertBlock(dBlock, wBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, dBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        float fBlock[MAX_BLOCK*4];
        ConvertBlock(fBlock, wBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, fBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_HALF hBlock[MAX_BLOCK*4];
        ConvertBlock(hBlock, wBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, hBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_DWORD dwBlock[MAX_BLOCK];
        SwizzleBlock(wBlock, w*h);
        ConvertBlock((CMP_BYTE*) dwBlock, wBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, dwBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_WORD wBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        double dBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, dBlock))
        {
            ConvertBlock(wBlock, dBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        float fBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, fBlock))
        {
            ConvertBlock(wBlock, fBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_HALF hBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, hBlock))
        {
            ConvertBlock(wBlock, hBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_DWORD dwBlock[MAX_BLOCK];
        if(ReadBlockRGBA(x, y, w, h, dwBlock))
        {
            ConvertBlock(wBlock, (CMP_BYTE*) dwBlock, w*h*4);
            SwizzleBlock(wBlock, w*h);
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        double dBlock[MAX_BLOCK*4];
        ConvertBlock(dBlock, hBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, dBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        float fBlock[MAX_BLOCK*4];
        ConvertBlock(fBlock, hBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, fBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_WORD wBlock[MAX_BLOCK*4];
        ConvertBlock(wBlock, hBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, wBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_DWORD dwBlock[MAX_BLOCK];
        SwizzleBlock(hBlock, w*h);
        ConvertBlock((CMP_BYTE*) dwBlock, hBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, dwBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_HALF hBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        double dBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, dBlock))
        {
            ConvertBlock(hBlock, dBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        float fBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, fBlock))
        {
            ConvertBlock(hBlock, fBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_WORD wBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, wBlock))
        {
            ConvertBlock(hBlock, wBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_DWORD dwBlock[MAX_BLOCK];
        if(ReadBlockRGBA(x, y, w, h, dwBlock))
        {
            ConvertBlock(hBlock, (CMP_BYTE*) dwBlock, w*h*4);
            SwizzleBlock(hBlock, w*h);
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        double dBlock[MAX_BLOCK*4];
        ConvertBlock(dBlock, fBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, dBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_HALF hBlock[MAX_BLOCK*4];
        ConvertBlock(hBlock, fBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, hBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_WORD wBlock[MAX_BLOCK*4];
        ConvertBlock(wBlock, fBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, wBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_DWORD dwBlock[MAX_BLOCK];
        SwizzleBlock(fBlock, w*h);
        ConvertBlock((CMP_BYTE*) dwBlock, fBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, dwBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, float fBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        double dBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, dBlock))
        {
            ConvertBlock(fBlock, dBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_HALF hBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, hBlock))
        {
            ConvertBlock(fBlock, hBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_WORD wBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, wBlock))
        {
            ConvertBlock(fBlock, wBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_DWORD dwBlock[MAX_BLOCK];
        if(ReadBlockRGBA(x, y, w, h, dwBlock))
        {
            ConvertBlock(fBlock, (CMP_BYTE*) dwBlock, w*h*4);
            SwizzleBlock(fBlock, w*h);
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        float fBlock[MAX_BLOCK*4];
        ConvertBlock(fBlock, dBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, fBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_HALF hBlock[MAX_BLOCK*4];
        ConvertBlock(hBlock, dBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, hBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_WORD wBlock[MAX_BLOCK*4];
        ConvertBlock(wBlock, dBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, wBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        CMP_DWORD dwBlock[MAX_BLOCK];
        SwizzleBlock(dBlock, w*h);
        ConvertBlock((CMP_BYTE*) dwBlock, dBlock, w*h*4);
        if(WriteBlockRGBA(x, y, w, h, dwBlock))
        {
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, double dBlock[])
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    // Ok, so we don't support this format
    // So we try other formats to find one that is supported

    if(m_bPerformingConversion)
    {
        return false;
    }
    else
    {
        m_bPerformingConversion = true;

        float fBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, fBlock))
        {
            ConvertBlock(dBlock, fBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_HALF hBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, hBlock))
        {
            ConvertBlock(dBlock, hBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_WORD wBlock[MAX_BLOCK*4];
        if(ReadBlockRGBA(x, y, w, h, wBlock))
        {
            ConvertBlock(dBlock, wBlock, w*h*4);
            m_bPerformingConversion = false;
            return true;
        }

        CMP_DWORD dwBlock[MAX_BLOCK];
        if(ReadBlockRGBA(x, y, w, h, dwBlock))
        {
            ConvertBlock(dBlock, (CMP_BYTE*) dwBlock, w*h*4);
            SwizzleBlock(dBlock, w*h);
            m_bPerformingConversion = false;
            return true;
        }

        assert(0);
        m_bPerformingConversion = false;
        return false;
    }
}

bool CCodecBuffer::WriteBlock(CMP_DWORD /*x*/, CMP_DWORD /*y*/, CMP_DWORD* /*pBlock*/, CMP_DWORD /*dwBlockSize*/)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    assert(0);
    return false;
}

bool CCodecBuffer::ReadBlock(CMP_DWORD /*x*/, CMP_DWORD /*y*/, CMP_DWORD* /*pBlock*/, CMP_DWORD /*dwBlockSize*/)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(0);
    return false;
}

void CCodecBuffer::ConvertBlock(double dBlock[], float fBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    assert(dBlock);
    assert(fBlock);
    assert(dwBlockSize);
    if(dBlock && fBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            dBlock[i] = fBlock[i];
    }
}

void CCodecBuffer::ConvertBlock(double dBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dBlock);
    assert(hBlock);
    assert(dwBlockSize);
    if(dBlock && hBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            dBlock[i] = hBlock[i];
    }
}

void CCodecBuffer::ConvertBlock(double dBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dBlock);
    assert(dwBlock);
    assert(dwBlockSize);
    if(dBlock && dwBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            dBlock[i] = CONVERT_DWORD_TO_FLOAT(dwBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(double dBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dBlock);
    assert(wBlock);
    assert(dwBlockSize);
    if(dBlock && wBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            dBlock[i] = CONVERT_WORD_TO_FLOAT(wBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(double dBlock[], CMP_BYTE cBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dBlock);
    assert(cBlock);
    assert(dwBlockSize);
    if(dBlock && cBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            dBlock[i] = CONVERT_BYTE_TO_FLOAT(cBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(float fBlock[], double dBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(fBlock);
    assert(dBlock);
    assert(dwBlockSize);
    if(fBlock && dBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            fBlock[i] = (float) dBlock[i];
    }
}

void CCodecBuffer::ConvertBlock(float fBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(fBlock);
    assert(hBlock);
    assert(dwBlockSize);
    if(fBlock && hBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            fBlock[i] = (float) hBlock[i];
    }
}

void CCodecBuffer::ConvertBlock(float fBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(fBlock);
    assert(dwBlock);
    assert(dwBlockSize);
    if(fBlock && dwBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            fBlock[i] = CONVERT_DWORD_TO_FLOAT(dwBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(float fBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(fBlock);
    assert(wBlock);
    assert(dwBlockSize);
    if(fBlock && wBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            fBlock[i] = CONVERT_WORD_TO_FLOAT(wBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(float fBlock[], CMP_BYTE cBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(fBlock);
    assert(cBlock);
    assert(dwBlockSize);
    if(fBlock && cBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            fBlock[i] = CONVERT_BYTE_TO_FLOAT(cBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_HALF hBlock[], double dBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(hBlock);
    assert(dBlock);
    assert(dwBlockSize);
    if(hBlock && dBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            hBlock[i] = (float) dBlock[i];
    }
}

void CCodecBuffer::ConvertBlock(CMP_HALF hBlock[], float fBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(hBlock);
    assert(fBlock);
    assert(dwBlockSize);
    if(hBlock && fBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            hBlock[i] = (float) fBlock[i];
    }
}

void CCodecBuffer::ConvertBlock(CMP_HALF hBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(hBlock);
    assert(dwBlock);
    assert(dwBlockSize);
    if(hBlock && dwBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            hBlock[i] = CONVERT_DWORD_TO_FLOAT(dwBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_HALF hBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(hBlock);
    assert(wBlock);
    assert(dwBlockSize);
    if(hBlock && wBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            hBlock[i] = CONVERT_WORD_TO_FLOAT(wBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_HALF hBlock[], CMP_BYTE cBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(hBlock);
    assert(cBlock);
    assert(dwBlockSize);
    if(hBlock && cBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            hBlock[i] = CONVERT_BYTE_TO_FLOAT(cBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_DWORD dwBlock[], double dBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dwBlock);
    assert(dBlock);
    assert(dwBlockSize);
    if(dwBlock && dBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            dwBlock[i] = CONVERT_FLOAT_TO_DWORD(dBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_DWORD dwBlock[], float fBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dwBlock);
    assert(fBlock);
    assert(dwBlockSize);
    if(dwBlock && fBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            dwBlock[i] = CONVERT_FLOAT_TO_DWORD(fBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_DWORD dwBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dwBlock);
    assert(hBlock);
    assert(dwBlockSize);
    if(dwBlock && hBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            dwBlock[i] = CONVERT_FLOAT_TO_DWORD(hBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_DWORD dwBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dwBlock);
    assert(wBlock);
    assert(dwBlockSize);
    if(dwBlock && wBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            dwBlock[i] = CONVERT_WORD_TO_DWORD(wBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_DWORD dwBlock[], CMP_BYTE cBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dwBlock);
    assert(cBlock);
    assert(dwBlockSize);
    if(dwBlock && cBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            dwBlock[i] = CONVERT_BYTE_TO_DWORD(cBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_WORD wBlock[], double dBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(wBlock);
    assert(dBlock);
    assert(dwBlockSize);
    if(wBlock && dBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            wBlock[i] = CONVERT_FLOAT_TO_WORD(dBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_WORD wBlock[], float fBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(wBlock);
    assert(fBlock);
    assert(dwBlockSize);
    if(wBlock && fBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            wBlock[i] = CONVERT_FLOAT_TO_WORD(fBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_WORD wBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(wBlock);
    assert(hBlock);
    assert(dwBlockSize);
    if(wBlock && hBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            wBlock[i] = CONVERT_FLOAT_TO_WORD(hBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_WORD wBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(wBlock);
    assert(dwBlock);
    assert(dwBlockSize);
    if(wBlock && dwBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            wBlock[i] = CONVERT_DWORD_TO_WORD(dwBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_WORD wBlock[], CMP_BYTE cBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(wBlock);
    assert(cBlock);
    assert(dwBlockSize);
    if(wBlock && cBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            wBlock[i] = CONVERT_BYTE_TO_WORD(cBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_BYTE cBlock[], double dBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(cBlock);
    assert(dBlock);
    assert(dwBlockSize);
    if(cBlock && dBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            cBlock[i] = CONVERT_FLOAT_TO_BYTE(dBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_BYTE cBlock[], float fBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(cBlock);
    assert(fBlock);
    assert(dwBlockSize);
    if(cBlock && fBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            cBlock[i] = CONVERT_FLOAT_TO_BYTE(fBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_BYTE cBlock[], CMP_HALF hBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(cBlock);
    assert(hBlock);
    assert(dwBlockSize);
    if(cBlock && hBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            cBlock[i] = CONVERT_FLOAT_TO_BYTE(hBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_BYTE cBlock[], CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(cBlock);
    assert(dwBlock);
    assert(dwBlockSize);
    if(cBlock && dwBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            cBlock[i] = CONVERT_DWORD_TO_BYTE(dwBlock[i]);
    }
}

void CCodecBuffer::ConvertBlock(CMP_BYTE cBlock[], CMP_WORD wBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(cBlock);
    assert(wBlock);
    assert(dwBlockSize);
    if(cBlock && wBlock && dwBlockSize)
    {
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            cBlock[i] = CONVERT_WORD_TO_BYTE(wBlock[i]);
    }
}

void CCodecBuffer::SwizzleBlock(double dBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dBlock);
    assert(dwBlockSize);
    if(dBlock && dwBlockSize)
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            SWAP_DOUBLES(dBlock[(i* 4)], dBlock[(i* 4) + 2]);
}

void CCodecBuffer::SwizzleBlock(float fBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(fBlock);
    assert(dwBlockSize);
    if(fBlock && dwBlockSize)
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            SWAP_FLOATS(fBlock[(i* 4)], fBlock[(i* 4) + 2]);
}

void CCodecBuffer::SwizzleBlock(CMP_HALF hBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(hBlock);
    assert(dwBlockSize);
    if(hBlock && dwBlockSize)
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            SWAP_HALFS(hBlock[(i* 4)], hBlock[(i* 4) + 2]);
}

void CCodecBuffer::SwizzleBlock(CMP_DWORD dwBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(dwBlock);
    assert(dwBlockSize);
    if(dwBlock && dwBlockSize)
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            SWAP_DWORDS(dwBlock[(i* 4)], dwBlock[(i* 4) + 2]);
}

void CCodecBuffer::SwizzleBlock(CMP_WORD wBlock[], CMP_DWORD dwBlockSize)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif

    assert(wBlock);
    assert(dwBlockSize);
    if(wBlock && dwBlockSize)
        for(CMP_DWORD i = 0; i < dwBlockSize; i++)
            SWAP_WORDS(wBlock[(i* 4)], wBlock[(i* 4) + 2]);
}
