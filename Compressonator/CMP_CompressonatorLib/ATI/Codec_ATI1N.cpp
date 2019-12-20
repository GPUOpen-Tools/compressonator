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
//  File Name:   Codec_ATI1N.cpp  
//  Description: implementation of the CCodec_ATI1N class
//
//////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4100)

#include "Common.h"
#include "Codec_ATI1N.h"

#ifdef TEST_CMP_CORE_DECODER
#include "CMP_Core.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_ATI1N::CCodec_ATI1N(CodecType codecType) :
CCodec_DXTC(codecType)
{

}

CCodec_ATI1N::~CCodec_ATI1N()
{

}

CCodecBuffer* CCodec_ATI1N::CreateBuffer(
    CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
    CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData,
    CMP_DWORD dwDataSize) const
{
    return CreateCodecBuffer(CBT_4x4Block_4BPP, 4,4,1,dwWidth, dwHeight, dwPitch, pData);
}

CodecError CCodec_ATI1N::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    if((m_nCompressionSpeed == CMP_Speed_Fast || m_nCompressionSpeed == CMP_Speed_SuperFast) && m_bUseSSE)
        return Compress_Fast(bufferIn, bufferOut, pFeedbackProc, pUser1, pUser2);

    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    bool bUseFixed = (!bufferIn.IsFloat() && bufferIn.GetChannelDepth() == 8 && !m_bUseFloat);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            CMP_DWORD compressedBlock[2];
            if(bUseFixed)
            {
                CMP_BYTE cAlphaBlock[BLOCK_SIZE_4X4];
                if (bufferIn.m_bSwizzle) // Our input red is in the blue offset position of the source buffer
                    bufferIn.ReadBlockB(i * 4, j * 4, 4, 4, cAlphaBlock);
                else
                    bufferIn.ReadBlockR(i*4, j*4, 4, 4, cAlphaBlock);
                CompressAlphaBlock(cAlphaBlock, compressedBlock);
            }
            else
            {
                float fAlphaBlock[BLOCK_SIZE_4X4];
                if (bufferIn.m_bSwizzle)
                    bufferIn.ReadBlockB(i*4, j*4, 4, 4, fAlphaBlock);
                else
                    bufferIn.ReadBlockR(i*4, j*4, 4, 4, fAlphaBlock);
                CompressAlphaBlock(fAlphaBlock, compressedBlock);

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

CodecError CCodec_ATI1N::Compress_Fast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            CMP_DWORD compressedBlock[2];
            CMP_BYTE cAlphaBlock[BLOCK_SIZE_4X4];

            if (bufferIn.m_bSwizzle)
                bufferIn.ReadBlockB(i * 4, j * 4, 4, 4, cAlphaBlock);
            else
                bufferIn.ReadBlockR(i*4, j*4, 4, 4, cAlphaBlock);
            CompressAlphaBlock_Fast(cAlphaBlock, compressedBlock);
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

CodecError CCodec_ATI1N::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);
    const CMP_DWORD dwBlocksXY = dwBlocksX*dwBlocksY;

    bool bUseFixed = (!bufferOut.IsFloat() && bufferOut.GetChannelDepth() == 8 && !m_bUseFloat);

    // Init alpha channel 
    CMP_BYTE  alpha[BLOCK_SIZE_4X4];
    CMP_FLOAT alphaF[BLOCK_SIZE_4X4];
    for (CMP_BYTE i=0; i< BLOCK_SIZE_4X4; i++)
    {
        alpha[i]  = 0xFF;
        alphaF[i] = FLT_MAX;
    }

    for(CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            CMP_DWORD compressedBlock[2];
            bufferIn.ReadBlock(i*4, j*4, compressedBlock, 2);
        
            if(bUseFixed)
            {
                #ifdef TEST_CMP_CORE_DECODER
                    CMP_BYTE ATI1NBlock[BLOCK_SIZE_4X4];
                    DecompressBlockBC4((CMP_BYTE *)compressedBlock,ATI1NBlock);
                    bufferOut.WriteBlockR(i*4, j*4, 4, 4, ATI1NBlock);
                    bufferOut.WriteBlockG(i*4, j*4, 4, 4, ATI1NBlock);
                    bufferOut.WriteBlockB(i*4, j*4, 4, 4, ATI1NBlock);
                    bufferOut.WriteBlockA(i*4, j*4, 4, 4, alpha);
                #else
                    CMP_BYTE ATI1NBlock[BLOCK_SIZE_4X4];
                    DecompressAlphaBlock(ATI1NBlock, compressedBlock);
                    bufferOut.WriteBlockR(i*4, j*4, 4, 4, ATI1NBlock);
                    bufferOut.WriteBlockG(i*4, j*4, 4, 4, ATI1NBlock);
                    bufferOut.WriteBlockB(i*4, j*4, 4, 4, ATI1NBlock);
                    bufferOut.WriteBlockA(i*4, j*4, 4, 4, alpha);
                #endif
            }
            else
            {
                float ATI1NBlock[BLOCK_SIZE_4X4];
                DecompressAlphaBlock(ATI1NBlock, compressedBlock);
                bufferOut.WriteBlockR(i*4, j*4, 4, 4, ATI1NBlock);
                bufferOut.WriteBlockG(i*4, j*4, 4, 4, ATI1NBlock);
                bufferOut.WriteBlockB(i*4, j*4, 4, 4, ATI1NBlock);
                bufferOut.WriteBlockA(i*4, j*4, 4, 4, alphaF);
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
