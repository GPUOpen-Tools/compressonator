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

#include "common.h"  // napatel

#ifdef SUPPORT_ETC_ALPHA

#include "etcpack.h"
#include "codec_etc_rgba_explicit.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_ETC_RGBA_Explicit::CCodec_ETC_RGBA_Explicit() :
    CCodec_ETC(CT_ETC_RGBA_Explicit) {

}

CCodec_ETC_RGBA_Explicit::~CCodec_ETC_RGBA_Explicit() {

}

CodecError CCodec_ETC_RGBA_Explicit::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2) {
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    readCompressParams();

    const AMD_TC_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const AMD_TC_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    AMD_TC_BYTE srcBlock[BLOCK_SIZE_4X4X4];
    AMD_TC_DWORD compressedBlock[4];
    for(AMD_TC_DWORD j = 0; j < dwBlocksY; j++) {
        for(AMD_TC_DWORD i = 0; i < dwBlocksX; i++) {
            bufferIn.ReadBlockRGBA(i*4, j*4, 4, 4, srcBlock);
            CompressRGBABlock_ExplicitAlpha(srcBlock, compressedBlock);
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

CodecError CCodec_ETC_RGBA_Explicit::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut) {
    assert(bufferIn.GetWidth() == bufferOut.GetWidth());
    assert(bufferIn.GetHeight() == bufferOut.GetHeight());

    if(bufferIn.GetWidth() != bufferOut.GetWidth() || bufferIn.GetHeight() != bufferOut.GetHeight())
        return CE_Unknown;

    const AMD_TC_DWORD dwBlocksX = ((bufferIn.GetWidth() + 3) >> 2);
    const AMD_TC_DWORD dwBlocksY = ((bufferIn.GetHeight() + 3) >> 2);

    AMD_TC_DWORD compressedBlock[4];
    AMD_TC_BYTE destBlock[BLOCK_SIZE_4X4X4];
    for(AMD_TC_DWORD j = 0; j < dwBlocksY; j++) {
        for(AMD_TC_DWORD i = 0; i < dwBlocksX; i++) {
            bufferIn.ReadBlock(i*4, j*4, compressedBlock, 4);
            DecompressRGBABlock_ExplicitAlpha(destBlock, compressedBlock);
            bufferOut.WriteBlockRGBA(i*4, j*4, 4, 4, destBlock);
        }
    }

    return CE_OK;
}

#endif // SUPPORT_ETC_ALPHA
