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
//  File Name:   Codec_DXT5_Swizzled.cpp
//  Description: implementation of the CCodec_DXT5_Swizzled class
//
//////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4100)

#include "common.h"
#include "codec_dxt5_swizzled.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_DXT5_Swizzled::CCodec_DXT5_Swizzled(CodecType codecType) :
    CCodec_DXTC(codecType) {

}

CCodec_DXT5_Swizzled::~CCodec_DXT5_Swizzled() {

}

CodecError CCodec_DXT5_Swizzled::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
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

    for(CMP_DWORD j = 0; j < dwBlocksY; j++) {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++) {
            CMP_DWORD compressedBlock[4];
            if(bUseFixed) {
                CMP_BYTE srcBlock[BLOCK_SIZE_4X4X4];
                ReadBlock(bufferIn, i*4, j*4, srcBlock);
                CompressRGBABlock(srcBlock, compressedBlock, CalculateColourWeightings(srcBlock));
            } else {
                float srcBlock[BLOCK_SIZE_4X4X4];
                ReadBlock(bufferIn, i*4, j*4, srcBlock);
                CompressRGBABlock(srcBlock, compressedBlock, CalculateColourWeightings(srcBlock));
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

CodecError CCodec_DXT5_Swizzled::Compress_Fast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    CMP_DWORD compressedBlock[4];
    CMP_BYTE srcBlock[BLOCK_SIZE_4X4X4];
    for(CMP_DWORD j = 0; j < dwBlocksY; j++) {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++) {
            ReadBlock(bufferIn, i*4, j*4, srcBlock);
            CompressRGBABlock_Fast(srcBlock, compressedBlock);
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

CodecError CCodec_DXT5_Swizzled::Compress_SuperFast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    CMP_DWORD compressedBlock[4];
    CMP_BYTE srcBlock[BLOCK_SIZE_4X4X4];
    for(CMP_DWORD j = 0; j < dwBlocksY; j++) {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++) {
            ReadBlock(bufferIn, i*4, j*4, srcBlock);
            CompressRGBABlock_SuperFast(srcBlock, compressedBlock);
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

CodecError CCodec_DXT5_Swizzled::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const CMP_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const CMP_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);
    const CMP_DWORD dwBlocksXY = dwBlocksX*dwBlocksY;

    bool bUseFixed = (!bufferOut.IsFloat() && bufferOut.GetChannelDepth() == 8 && !m_bUseFloat);

    for(CMP_DWORD j = 0; j < dwBlocksY; j++) {
        for(CMP_DWORD i = 0; i < dwBlocksX; i++) {
            CMP_DWORD compressedBlock[4];
            bufferIn.ReadBlock(i*4, j*4, compressedBlock, 4);
            if(bUseFixed) {
                CMP_BYTE destBlock[BLOCK_SIZE_4X4X4];
                DecompressRGBABlock(destBlock, compressedBlock);
                WriteBlock(bufferOut, i*4, j*4, destBlock);
            } else {
                float destBlock[BLOCK_SIZE_4X4X4];
                DecompressRGBABlock(destBlock, compressedBlock);
                WriteBlock(bufferOut, i*4, j*4, destBlock);
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
