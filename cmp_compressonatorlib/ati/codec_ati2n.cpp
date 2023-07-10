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
//  File Name:   Codec_ATI2N.cpp
//  Description: implementation of the CCodec_ATI2N class
//
//////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4100)

#include "common.h"
#include "codec_ati2n.h"

#ifdef TEST_CMP_CORE_DECODER
#include "cmp_core.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_ATI2N::CCodec_ATI2N(CodecType codecType) :
    CCodec_DXTC(codecType) {
    m_codecType = codecType;
}

CCodec_ATI2N::~CCodec_ATI2N() {

}

CodecError CCodec_ATI2N::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
    if((m_nCompressionSpeed == CMP_Speed_Fast || m_nCompressionSpeed == CMP_Speed_SuperFast) && m_bUseSSE)
        return Compress_Fast(bufferIn, bufferOut, pFeedbackProc, pUser1, pUser2);

    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    const CMP_DWORD dwXOffset = 0;   //(GetType() == CT_ATI2N) ? 2 : 0;
    const CMP_DWORD dwYOffset = 2;   //(GetType() == CT_ATI2N) ? 0 : 2;

    bool bUseFixed = (!bufferIn.IsFloat() && bufferIn.GetChannelDepth() == 8 && !m_bUseFloat);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++) {
        CMP_DWORD compressedBlock[4];
        for(CMP_DWORD i = 0; i < dwBlocksX; i++) {
            if(bUseFixed) {
                CMP_BYTE cAlphaBlock[BLOCK_SIZE_4X4];

                if (bufferIn.m_bSwizzle)
                    bufferIn.ReadBlockB(i*4, j*4, 4, 4, cAlphaBlock);  // <=?? this is actually reading the red channel
                else
                    bufferIn.ReadBlockR(i * 4, j * 4, 4, 4, cAlphaBlock);

                CompressAlphaBlock(cAlphaBlock, &compressedBlock[dwXOffset]);

                bufferIn.ReadBlockG(i*4, j*4, 4, 4, cAlphaBlock);
                CompressAlphaBlock(cAlphaBlock, &compressedBlock[dwYOffset]);
            } else {
                float fAlphaBlock[BLOCK_SIZE_4X4];

                if (bufferIn.m_bSwizzle)
                    bufferIn.ReadBlockB(i * 4, j * 4, 4, 4, fAlphaBlock);
                else
                    bufferIn.ReadBlockR(i*4, j*4, 4, 4, fAlphaBlock);

                CompressAlphaBlock(fAlphaBlock, &compressedBlock[dwXOffset]);

                bufferIn.ReadBlockG(i*4, j*4, 4, 4, fAlphaBlock);
                CompressAlphaBlock(fAlphaBlock, &compressedBlock[dwYOffset]);
            }
            bufferOut.WriteBlock(i*4, j*4, compressedBlock, 4);
        }
        if(pFeedbackProc) {
            float fProgress = 100.f * (j * dwBlocksX) / (dwBlocksX * dwBlocksY);
            if(pFeedbackProc(fProgress, pUser1, pUser2))
                return CE_Aborted;
        }
    }

    return CE_OK;
}

CodecError CCodec_ATI2N::Compress_Fast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    const CMP_DWORD dwXOffset = 0; // (GetType() == CT_ATI2N) ? 2 : 0;
    const CMP_DWORD dwYOffset = 2; // (GetType() == CT_ATI2N) ? 0 : 2;

    for(CMP_DWORD j = 0; j < dwBlocksY; j++) {
        CMP_DWORD compressedBlock[4];
        for(CMP_DWORD i = 0; i < dwBlocksX; i++) {
            CMP_BYTE cAlphaBlock[BLOCK_SIZE_4X4];

            if (bufferIn.m_bSwizzle)
                bufferIn.ReadBlockB(i * 4, j * 4, 4, 4, cAlphaBlock);
            else
                bufferIn.ReadBlockR(i*4, j*4, 4, 4, cAlphaBlock);

            CompressAlphaBlock_Fast(cAlphaBlock, &compressedBlock[dwXOffset]);

            bufferIn.ReadBlockG(i*4, j*4, 4, 4, cAlphaBlock);
            CompressAlphaBlock_Fast(cAlphaBlock, &compressedBlock[dwYOffset]);

            bufferOut.WriteBlock(i*4, j*4, compressedBlock, 4);
        }
        if(pFeedbackProc) {
            float fProgress = 100.f * (j * dwBlocksX) / (dwBlocksX * dwBlocksY);
            if(pFeedbackProc(fProgress, pUser1, pUser2))
                return CE_Aborted;
        }
    }

    return CE_OK;
}

CodecError CCodec_ATI2N::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);
    const CMP_DWORD dwBlocksXY = dwBlocksX*dwBlocksY;

    const CMP_DWORD dwXOffset = (GetType() == CT_ATI2N) ? 2 : 0;
    const CMP_DWORD dwYOffset = (GetType() == CT_ATI2N) ? 0 : 2;


    bool bUseFixed = (!bufferOut.IsFloat() && bufferOut.GetChannelDepth() == 8 && !m_bUseFloat);

    CMP_BYTE alphaBlockA[BLOCK_SIZE_4X4];
    CMP_BYTE alphaBlockR[BLOCK_SIZE_4X4];
    CMP_BYTE alphaBlockG[BLOCK_SIZE_4X4];
    CMP_BYTE alphaBlockB[BLOCK_SIZE_4X4];
    memset(alphaBlockA, 255, sizeof(alphaBlockA));
    memset(alphaBlockB, 0, sizeof(alphaBlockB));

    float falphaBlockR[BLOCK_SIZE_4X4];
    float falphaBlockG[BLOCK_SIZE_4X4];
    float falphaBlockB[BLOCK_SIZE_4X4];
    float falphaBlockA[BLOCK_SIZE_4X4];
    memset(falphaBlockA, 255, sizeof(falphaBlockA));
    memset(falphaBlockB, 0, sizeof(falphaBlockB));

    CMP_DWORD compressedBlock[4];

    for(CMP_DWORD j = 0; j < dwBlocksY; j++) {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++) {
            bufferIn.ReadBlock(i*4, j*4, compressedBlock, 4);

            if(bUseFixed) {
#ifdef TEST_CMP_CORE_DECODER
                DecompressBlockBC5((CMP_BYTE *)&compressedBlock[dwXOffset],alphaBlockR,alphaBlockG);
                bufferOut.WriteBlockR(i * 4, j * 4, 4, 4, alphaBlockB);
                bufferOut.WriteBlockG(i * 4, j * 4, 4, 4, alphaBlockG);
                bufferOut.WriteBlockB(i * 4, j * 4, 4, 4, alphaBlockR);
                bufferOut.WriteBlockA(i * 4, j * 4, 4, 4, alphaBlockA);
#else
                DecompressAlphaBlock(alphaBlockR, &compressedBlock[dwXOffset]);
                DecompressAlphaBlock(alphaBlockG, &compressedBlock[dwYOffset]);

                // we need to write the blue and red channels like this because the internal representation of codec buffers is BGRA, and later in Compressonator
                // we directly access the byte data to get the final result
                bufferOut.WriteBlockR(i * 4, j * 4, 4, 4, alphaBlockB);
                bufferOut.WriteBlockG(i * 4, j * 4, 4, 4, alphaBlockG);
                bufferOut.WriteBlockB(i * 4, j * 4, 4, 4, alphaBlockR);
                bufferOut.WriteBlockA(i * 4, j * 4, 4, 4, alphaBlockA);
#endif
            } else {
                DecompressAlphaBlock(falphaBlockR, &compressedBlock[dwXOffset]);
                DecompressAlphaBlock(falphaBlockG, &compressedBlock[dwYOffset]);

                // we need to write the blue and red channels like this because the internal representation of codec buffers is BGRA, and later in Compressonator
                // we directly access the byte data to get the final result
                bufferOut.WriteBlockR(i * 4, j * 4, 4, 4, falphaBlockB);
                bufferOut.WriteBlockG(i * 4, j * 4, 4, 4, falphaBlockG);
                bufferOut.WriteBlockB(i * 4, j * 4, 4, 4, falphaBlockR);
                bufferOut.WriteBlockA(i * 4, j * 4, 4, 4, falphaBlockA);
            }
        }

        if (pFeedbackProc) {
            float fProgress = 100.f * (j * dwBlocksX) / dwBlocksXY;
            if (pFeedbackProc(fProgress, pUser1, pUser2)) {
                return CE_Aborted;
            }
        }

    }

    return CE_OK;
}

//====================== BC5 Signed Encoder 

CCodec_ATI2N_S::CCodec_ATI2N_S(CodecType codecType)
    : CCodec_DXTC(codecType)
{
    m_codecType = codecType;
}

CCodec_ATI2N_S::~CCodec_ATI2N_S()
{
}

CodecError CCodec_ATI2N_S::Compress(CCodecBuffer&       bufferIn,
                                  CCodecBuffer&       bufferOut,
                                  Codec_Feedback_Proc pFeedbackProc,
                                  CMP_DWORD_PTR       pUser1,
                                  CMP_DWORD_PTR       pUser2)
{

    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if (bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    const CMP_DWORD dwXOffset = 0; // (GetType() == CT_ATI2N) ? 2 : 0;
    const CMP_DWORD dwYOffset = 2; // (GetType() == CT_ATI2N) ? 0 : 2;

    // bool bUseFixed = (!bufferIn.IsFloat() && bufferIn.GetChannelDepth() == 8 && !m_bUseFloat);

    for (CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        CMP_DWORD compressedBlock[4];
        for (CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            if (bufferIn.GetFormat() == CMP_FORMAT_RGBA_8888_S)
            {

                CMP_SBYTE cAlphaBlock[BLOCK_SIZE_4X4];

                bufferIn.ReadBlockR(i * 4, j * 4, 4, 4, cAlphaBlock);

                // Convert to sbyte to float
                float AlphaBlockSNorm[BLOCK_SIZE_4X4];

                for (int ii = 0; ii < BLOCK_SIZE_4X4; ii++)
                {
                    AlphaBlockSNorm[ii] = (cAlphaBlock[ii] / 127.0f);
                }

                CompressAlphaBlockSNorm(AlphaBlockSNorm, &compressedBlock[dwXOffset]);

                bufferIn.ReadBlockG(i * 4, j * 4, 4, 4, cAlphaBlock);

                // conversion of ubyte to signed normalized <c>/(2^7 -1)
                for (int ii = 0; ii < BLOCK_SIZE_4X4; ii++)
                {
                    AlphaBlockSNorm[ii] = (cAlphaBlock[ii] / 127.0f);
                }

                CompressAlphaBlockSNorm(AlphaBlockSNorm, &compressedBlock[dwYOffset]);
            }
            else
            {
                CMP_BYTE cAlphaBlock[BLOCK_SIZE_4X4];

                if (bufferIn.m_bSwizzle)
                    bufferIn.ReadBlockB(i * 4, j * 4, 4, 4, cAlphaBlock);  // <=?? this is actually reading the red channel
                else
                    bufferIn.ReadBlockR(i * 4, j * 4, 4, 4, cAlphaBlock);

                // Convert to sbyte to float
                float AlphaBlockSNorm[BLOCK_SIZE_4X4];

                // Convert UINT -> SNORM
                for (int ii = 0; ii < BLOCK_SIZE_4X4; ii++)
                {
                    AlphaBlockSNorm[ii] = ((cAlphaBlock[ii] / 255.0f) * 2.0f - 1.0f);
                }


                CompressAlphaBlockSNorm(AlphaBlockSNorm, &compressedBlock[dwXOffset]);

                bufferIn.ReadBlockG(i * 4, j * 4, 4, 4, cAlphaBlock);

                // Convert UINT -> SNORM
                for (int ii = 0; ii < BLOCK_SIZE_4X4; ii++)
                {
                    AlphaBlockSNorm[ii] = ((cAlphaBlock[ii] / 255.0f) * 2.0f - 1.0f);
                }

                CompressAlphaBlockSNorm(AlphaBlockSNorm, &compressedBlock[dwYOffset]);
            }

            bufferOut.WriteBlock(i * 4, j * 4, compressedBlock, 4);
        }
        if (pFeedbackProc)
        {
            float fProgress = 100.f * (j * dwBlocksX) / (dwBlocksX * dwBlocksY);
            if (pFeedbackProc(fProgress, pUser1, pUser2))
                return CE_Aborted;
        }
    }

    return CE_OK;
}

CodecError CCodec_ATI2N_S::Decompress(CCodecBuffer&       bufferIn,
                                    CCodecBuffer&       bufferOut,
                                    Codec_Feedback_Proc pFeedbackProc,
                                    CMP_DWORD_PTR       pUser1,
                                    CMP_DWORD_PTR       pUser2)
{
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if (bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX  = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY  = ((bufferIn.GetHeight() + 3) >> 2);
    const CMP_DWORD dwBlocksXY = dwBlocksX * dwBlocksY;

    const CMP_DWORD dwXOffset = 0; // (GetType() == CT_ATI2N) ? 2 : 0;
    const CMP_DWORD dwYOffset = 2; // (GetType() == CT_ATI2N) ? 0 : 2;

    //bool bUseFixed = (!bufferOut.IsFloat() && bufferOut.GetChannelDepth() == 8 && !m_bUseFloat);

    CMP_SBYTE alphaBlockA[BLOCK_SIZE_4X4];
    CMP_SBYTE alphaBlockB[BLOCK_SIZE_4X4];

    memset(alphaBlockA, 127, sizeof(alphaBlockA));
    memset(alphaBlockB, 0, sizeof(alphaBlockB));

    CMP_SBYTE alphaBlockR[BLOCK_SIZE_4X4];
    CMP_SBYTE alphaBlockG[BLOCK_SIZE_4X4];
    CMP_DWORD compressedBlock[4];

    for (CMP_DWORD j = 0; j < dwBlocksY; j++)
    {
        for (CMP_DWORD i = 0; i < dwBlocksX; i++)
        {
            bufferIn.ReadBlock(i * 4, j * 4, compressedBlock, 4);

#ifdef TEST_CMP_CORE_DECODER
            DecompressBlockBC5((CMP_BYTE*)&compressedBlock[dwXOffset], alphaBlockR, alphaBlockG);
            // we need to write the blue and red channels like this because the internal representation of codec buffers is BGRA, and later in Compressonator
            // we directly access the byte data to get the final result
            bufferOut.WriteBlockR(i * 4, j * 4, 4, 4, alphaBlockB);
            bufferOut.WriteBlockG(i * 4, j * 4, 4, 4, alphaBlockG);
            bufferOut.WriteBlockB(i * 4, j * 4, 4, 4, alphaBlockR);
            bufferOut.WriteBlockA(i * 4, j * 4, 4, 4, alphaBlockA);
#else
            DecompressAlphaBlockInt8(alphaBlockR, &compressedBlock[dwXOffset]);
            DecompressAlphaBlockInt8(alphaBlockG, &compressedBlock[dwYOffset]);
            
            // we need to write the blue and red channels like this because the internal representation of codec buffers is BGRA, and later in Compressonator
            // we directly access the byte data to get the final result
            bufferOut.WriteBlockR(i * 4, j * 4, 4, 4, alphaBlockB);
            bufferOut.WriteBlockG(i * 4, j * 4, 4, 4, alphaBlockG);
            bufferOut.WriteBlockB(i * 4, j * 4, 4, 4, alphaBlockR);
            bufferOut.WriteBlockA(i * 4, j * 4, 4, 4, alphaBlockA);
#endif
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


