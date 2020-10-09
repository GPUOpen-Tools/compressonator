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
//  File Name:   Codec_ATI2N_DXT5.cpp
//  Description: implementation of the CCodec_ATI2N_DXT5 class
//
//////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4100)

#include "common.h"
#include "codec_ati2n_dxt5.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_ATI2N_DXT5::CCodec_ATI2N_DXT5() :
    CCodec_ATI2N(CT_ATI2N_DXT5) {

}

CCodec_ATI2N_DXT5::~CCodec_ATI2N_DXT5() {

}

CodecError CCodec_ATI2N_DXT5::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
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
                CMP_BYTE tempBlock[BLOCK_SIZE_4X4X4];
                bufferIn.ReadBlockRGBA(i*4, j*4, 4, 4, tempBlock);
                for(CMP_DWORD k = 0; k < BLOCK_SIZE_4X4; k++)
                    ((CMP_DWORD*) srcBlock)[k] = SWIZZLE_RGBA_xGxR(((CMP_DWORD*) tempBlock)[k]);
                CompressRGBABlock(srcBlock, compressedBlock);
            } else {
                float srcBlock[BLOCK_SIZE_4X4X4];
                float tempBlock[BLOCK_SIZE_4X4X4];
                bufferIn.ReadBlockRGBA(i*4, j*4, 4, 4, tempBlock);
                for(CMP_DWORD k = 0; k < BLOCK_SIZE_4X4; k++) {
                    srcBlock[(k * 4) + RGBA32F_OFFSET_R] = 0;
                    srcBlock[(k * 4) + RGBA32F_OFFSET_A] = tempBlock[(k* 4) + RGBA32F_OFFSET_R];
                    srcBlock[(k * 4) + RGBA32F_OFFSET_B] = 0;
                    srcBlock[(k * 4) + RGBA32F_OFFSET_G] = tempBlock[(k* 4) + RGBA32F_OFFSET_G];
                }
                CompressRGBABlock(srcBlock, compressedBlock);
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

CodecError CCodec_ATI2N_DXT5::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2) {
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
                CMP_BYTE tempBlock[BLOCK_SIZE_4X4X4];
                DecompressRGBABlock(tempBlock, compressedBlock);

                CMP_BYTE destBlock[BLOCK_SIZE_4X4X4];
                for(CMP_DWORD k = 0; k < BLOCK_SIZE_4X4; k++) {
                    // Bug Work Arround: This codec buffer is BGRA -> we expect data to be RGBA, the codec buffer is configured
                    // for BGRA and we want output as RGBA...
                    destBlock[(k * 4) + 0] = tempBlock[(k* 4) + 3];
                    destBlock[(k * 4) + 1] = tempBlock[(k* 4) + 1];
                    destBlock[(k * 4) + 2] = 0;
                    destBlock[(k * 4) + 3] = 0xff;
                }

                bufferOut.WriteBlockRGBA(i*4, j*4, 4, 4, destBlock);
            } else {
                float tempBlock[BLOCK_SIZE_4X4X4];
                DecompressRGBABlock(tempBlock, compressedBlock);

                float destBlock[BLOCK_SIZE_4X4X4];
                for(CMP_DWORD k = 0; k < BLOCK_SIZE_4X4; k++) {
                    destBlock[(k * 4) + RGBA32F_OFFSET_R] = tempBlock[(k* 4) + RGBA32F_OFFSET_A];
                    destBlock[(k * 4) + RGBA32F_OFFSET_G] = tempBlock[(k* 4) + RGBA32F_OFFSET_G];
                    destBlock[(k * 4) + RGBA32F_OFFSET_B] = 0.f;
                    destBlock[(k * 4) + RGBA32F_OFFSET_A] = 1.0;
                }

                bufferOut.WriteBlockRGBA(i*4, j*4, 4, 4, destBlock);
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
