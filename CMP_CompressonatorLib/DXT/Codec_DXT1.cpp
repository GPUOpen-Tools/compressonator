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
//  File Name:   Codec_DXT1.cpp
//  Description: implementation of the CCodec_DXT1 class
//
//////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4100)

#include "Common.h"
#include "Compressonator.h"
#include "Codec_DXT1.h"

#ifdef TEST_CMP_CORE_DECODER
#include "CMP_Core.h"
#endif


//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_DXT1::CCodec_DXT1() :
CCodec_DXTC(CT_DXT1)
{
    m_bDXT1UseAlpha = false;
    m_nAlphaThreshold = 128;
}

CCodec_DXT1::~CCodec_DXT1()
{

}

bool CCodec_DXT1::SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue)
{
    if(strcmp(pszParamName, "DXT1UseAlpha") == 0)
        m_bDXT1UseAlpha = (std::stoi(sValue) > 0) ? true : false;
    else if(strcmp(pszParamName, "AlphaThreshold") == 0)
        m_nAlphaThreshold = (CMP_BYTE) (std::stoi(sValue) & 0xFF);
    else 
        return CCodec_DXTC::SetParameter(pszParamName, sValue);
    return true;
}

bool CCodec_DXT1::SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue)
{
    if(strcmp(pszParamName, "DXT1UseAlpha") == 0)
        m_bDXT1UseAlpha = dwValue ? true : false;
    else if(strcmp(pszParamName, "AlphaThreshold") == 0)
        m_nAlphaThreshold = (CMP_BYTE) dwValue;
    else
        return CCodec_DXTC::SetParameter(pszParamName, dwValue);
    return true;
}

bool CCodec_DXT1::GetParameter(const CMP_CHAR* pszParamName, CMP_DWORD& dwValue)
{
    if(strcmp(pszParamName, "DXT1UseAlpha") == 0)
        dwValue = m_bDXT1UseAlpha;
    else if(strcmp(pszParamName, "AlphaThreshold") == 0)
        dwValue = m_nAlphaThreshold;
    else
        return CCodec_DXTC::SetParameter(pszParamName, dwValue);
    return true;
}

CCodecBuffer* CCodec_DXT1::CreateBuffer(
                                        CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
                                        CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData,CMP_DWORD dwDataSize) const
{
    return CreateCodecBuffer(CBT_4x4Block_4BPP, nBlockWidth, nBlockHeight, nBlockDepth, dwWidth, dwHeight, dwPitch, pData,dwDataSize);
}

CodecError CCodec_DXT1::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
#ifndef _WIN64  //todo: add sse2 feature for win64
    if(m_nCompressionSpeed == CMP_Speed_SuperFast && m_bUseSSE2)
        return Compress_SuperFast(bufferIn, bufferOut, pFeedbackProc, pUser1, pUser2);
    else if((m_nCompressionSpeed == CMP_Speed_Fast || m_nCompressionSpeed == CMP_Speed_SuperFast) && m_bUseSSE)
        return Compress_Fast(bufferIn, bufferOut, pFeedbackProc, pUser1, pUser2);
#endif
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    bool bUseFixed = (!bufferIn.IsFloat() && bufferIn.GetChannelDepth() == 8 && !m_bUseFloat);

    float fAlphaThreshold = CONVERT_BYTE_TO_FLOAT(m_nAlphaThreshold);
    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            CMP_DWORD compressedBlock[2];
            if(bUseFixed)
            {
                CMP_BYTE srcBlock[BLOCK_SIZE_4X4X4];
                bufferIn.ReadBlockRGBA(i*4, j*4, 4, 4, srcBlock);
                CompressRGBBlock(srcBlock, compressedBlock, CalculateColourWeightings(srcBlock), true, m_bDXT1UseAlpha, m_nAlphaThreshold);
            }
            else
            {
                float srcBlock[BLOCK_SIZE_4X4X4];
                bufferIn.ReadBlockRGBA(i*4, j*4, 4, 4, srcBlock);
                CompressRGBBlock(srcBlock, compressedBlock, CalculateColourWeightings(srcBlock), true, m_bDXT1UseAlpha, fAlphaThreshold);
            }
            bufferOut.WriteBlock(i*4, j*4, compressedBlock, 2);
        }
        if(pFeedbackProc)
        {
            float fProgress = 100.f * (j * dwBlocksX) / (dwBlocksX * dwBlocksY);
            if(pFeedbackProc(fProgress, pUser1, pUser2))
                return CE_Aborted;
        }
    }

    return CE_OK;
}

CodecError CCodec_DXT1::Compress_Fast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    CMP_DWORD compressedBlock[2];
    CMP_BYTE srcBlock[BLOCK_SIZE_4X4X4];
    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            bufferIn.ReadBlockRGBA(i*4, j*4, 4, 4, srcBlock);
            CompressRGBBlock_Fast(srcBlock, compressedBlock);
            bufferOut.WriteBlock(i*4, j*4, compressedBlock, 2);
        }
        if(pFeedbackProc)
        {
            float fProgress = 100.f * (j * dwBlocksX) / (dwBlocksX * dwBlocksY);
            if(pFeedbackProc(fProgress, pUser1, pUser2))
                return CE_Aborted;
        }
    }

    return CE_OK;
}

CodecError CCodec_DXT1::Compress_SuperFast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    CMP_DWORD compressedBlock[2];
    CMP_BYTE srcBlock[BLOCK_SIZE_4X4X4];
    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            bufferIn.ReadBlockRGBA(i*4, j*4, 4, 4, srcBlock);
            CompressRGBBlock_SuperFast(srcBlock, compressedBlock);
            bufferOut.WriteBlock(i*4, j*4, compressedBlock, 2);
        }
        if(pFeedbackProc)
        {
            float fProgress = 100.f * (j * dwBlocksX) / (dwBlocksX * dwBlocksY);
            if(pFeedbackProc(fProgress, pUser1, pUser2))
                return CE_Aborted;
        }
    }
    return CE_OK;
}


CodecError CCodec_DXT1::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);
    const CMP_DWORD dwBlocksXY = dwBlocksX*dwBlocksY;

    bool bUseFixed = (!bufferOut.IsFloat() && bufferOut.GetChannelDepth() == 8 && !m_bUseFloat);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            CMP_DWORD compressedBlock[2];
            bufferIn.ReadBlock(i*4, j*4, compressedBlock, 2);
            if(bUseFixed)
            {
                CMP_BYTE destBlock[BLOCK_SIZE_4X4X4];
                #ifdef TEST_CMP_CORE_DECODER
                    DecompressBlockBC1((CMP_BYTE *)compressedBlock,destBlock);
                #else
                    DecompressRGBBlock(destBlock, compressedBlock, true);
                #endif
                bufferOut.WriteBlockRGBA(i*4, j*4, 4, 4, destBlock);
            }
            else
            {
                float destBlock[BLOCK_SIZE_4X4X4];
                DecompressRGBBlock(destBlock, compressedBlock, true);
                bufferOut.WriteBlockRGBA(i*4, j*4, 4, 4, destBlock);
            }
        }

        if (pFeedbackProc)
        {
            float fProgress = 100.f * (j * dwBlocksX) / dwBlocksXY;
            if (pFeedbackProc(fProgress, pUser1, pUser2))
            {
                return CE_Aborted;
            }
        }


    }

    return CE_OK;
}
