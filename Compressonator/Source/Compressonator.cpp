//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
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
//  File Name:   Compressonator.cpp
//  Description: A library to compress/decompress textures
//
//  Revisions
//  Feb 2016    -   Fix Parameter processing & Swizzle issue for DXTC Codecs
//  Jan 2016    -   Added ASTC support - 
//  Jan 2014    -   Completed support for BC6H and Command line options for new compressonator
//  Apr 2014    -   Refactored Library
//                  Code clean to support MSV 2010 and up
//////////////////////////////////////////////////////////////////////////////

#include "Compressonator.h"  // User shared: Keep priviate code out of this header
#include "Compress.h"
#include <assert.h>
#include "debug.h"

extern CodecType    GetCodecType(CMP_FORMAT format);
extern CMP_ERROR GetError(CodecError err);
extern CMP_ERROR CheckTexture(const CMP_Texture* pTexture, bool bSource);
extern CMP_ERROR CompressTexture(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture, const CMP_CompressOptions* pOptions, CMP_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2, CodecType destType);
extern CMP_ERROR ThreadedCompressTexture(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture, const CMP_CompressOptions* pOptions, CMP_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2, CodecType destType);

#ifdef _LOCAL_DEBUG
char    DbgTracer::buff[MAX_DBGBUFF_SIZE];
char    DbgTracer::PrintBuff[MAX_DBGPPRINTBUFF_SIZE];
#endif

CMP_DWORD CMP_API CMP_CalculateBufferSize(const CMP_Texture* pTexture)
{
#ifdef USE_DBGTRACE
    DbgTrace(("-------> pTexture [%x]",pTexture));
#endif

    assert(pTexture);
    if(pTexture == NULL)
        return 0;

    assert(pTexture->dwSize == sizeof(CMP_Texture));
    if(pTexture->dwSize != sizeof(CMP_Texture))
        return 0;

    assert(pTexture->dwWidth > 0);
    if(pTexture->dwWidth <= 0 )
        return 0;

    assert(pTexture->dwHeight > 0);
    if(pTexture->dwHeight <= 0 )
        return 0;

    assert(pTexture->format >= CMP_FORMAT_ARGB_8888 && pTexture->format <= CMP_FORMAT_MAX);
    if(pTexture->format < CMP_FORMAT_ARGB_8888 || pTexture->format > CMP_FORMAT_MAX)
        return 0;

    return CalcBufferSize(pTexture->format, pTexture->dwWidth, pTexture->dwHeight, pTexture->dwPitch);
}

CMP_DWORD CalcBufferSize(CMP_FORMAT format, CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch)
{
#ifdef USE_DBGTRACE
    DbgTrace(("format %d dwWidth %d dwHeight %d dwPitch %d",format, dwWidth, dwHeight, dwPitch));
#endif

    switch(format)
    {
        case CMP_FORMAT_ARGB_8888:
        case CMP_FORMAT_ARGB_2101010:
            return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * dwHeight));

        case CMP_FORMAT_RGB_888:
            return ((dwPitch) ? (dwPitch * dwHeight) : ((((dwWidth * 3) + 3) >> 2) * 4 * dwHeight));

        case CMP_FORMAT_RG_8:
            return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 2 * dwHeight));

        case CMP_FORMAT_R_8:
            return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * dwHeight));

        case CMP_FORMAT_ARGB_16:
        case CMP_FORMAT_ARGB_16F:
            return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * sizeof(CMP_WORD) * dwHeight));

        case CMP_FORMAT_RG_16:
        case CMP_FORMAT_RG_16F:
            return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * sizeof(CMP_WORD) * dwHeight));

        case CMP_FORMAT_R_16:
        case CMP_FORMAT_R_16F:
            return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * sizeof(CMP_WORD) * dwHeight));

#ifdef ARGB_32_SUPPORT
        case CMP_FORMAT_ARGB_32:
#endif // ARGB_32_SUPPORT
        case CMP_FORMAT_ARGB_32F:
            return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * sizeof(float) * dwHeight));

#ifdef ARGB_32_SUPPORT
        case CMP_FORMAT_RG_32:
#endif // ARGB_32_SUPPORT
        case CMP_FORMAT_RG_32F:
            return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 2 * sizeof(float) * dwHeight));

#ifdef ARGB_32_SUPPORT
        case CMP_FORMAT_R_32:
#endif // ARGB_32_SUPPORT
        case CMP_FORMAT_R_32F:
            return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 1 * sizeof(float) * dwHeight));

        default:
            return CalcBufferSize(GetCodecType(format), dwWidth, dwHeight);
    }
}

CMP_ERROR CMP_API CMP_ConvertTexture(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture, const CMP_CompressOptions* pOptions, CMP_Feedback_Proc pFeedbackProc, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
#ifdef USE_DBGTRACE
    DbgTrace(("-------> pSourceTexture [%x] pDestTexture [%x] pOptions [%x]",pSourceTexture, pDestTexture, pOptions));
#endif
    CMP_ERROR tc_err = CheckTexture(pSourceTexture, true);
    if(tc_err != CMP_OK)
        return tc_err;

#if defined(WIN32) || defined(_WIN64)
    assert(!IsBadReadPtr(pSourceTexture->pData, pSourceTexture->dwDataSize));
    assert(!IsBadWritePtr(pDestTexture->pData, pDestTexture->dwDataSize));
#endif // !WIN32 && !_WIN64

    tc_err = CheckTexture(pDestTexture, false);
    if(tc_err != CMP_OK)
        return tc_err;

    if(pSourceTexture->dwWidth != pDestTexture->dwWidth || pSourceTexture->dwHeight != pDestTexture->dwHeight)
        return CMP_ERR_SIZE_MISMATCH;

    CodecType srcType = GetCodecType(pSourceTexture->format);
    assert(srcType != CT_Unknown);
    if(srcType == CT_Unknown)
        return CMP_ERR_UNSUPPORTED_SOURCE_FORMAT;

    CodecType destType = GetCodecType(pDestTexture->format);
    assert(destType != CT_Unknown);
    if(destType == CT_Unknown)
        return CMP_ERR_UNSUPPORTED_SOURCE_FORMAT;

    if(srcType == destType)
    {
        // Easy case ?
        if(pSourceTexture->format == pDestTexture->format && pSourceTexture->dwPitch == pDestTexture->dwPitch)
            memcpy(pDestTexture->pData, pSourceTexture->pData, CMP_CalculateBufferSize(pSourceTexture));
        else
        {
            CodecBufferType srcBufferType = GetCodecBufferType(pSourceTexture->format);
            CodecBufferType destBufferType = GetCodecBufferType(pDestTexture->format);

            CCodecBuffer* pSrcBuffer = CreateCodecBuffer(srcBufferType, pSourceTexture->dwWidth, pSourceTexture->dwHeight, pSourceTexture->dwPitch, pSourceTexture->pData);
            assert(pSrcBuffer);
            if(!pSrcBuffer)
                return CMP_ERR_GENERIC;

            CCodecBuffer* pDestBuffer = CreateCodecBuffer(destBufferType, pDestTexture->dwWidth, pDestTexture->dwHeight, pDestTexture->dwPitch, pDestTexture->pData);
            assert(pDestBuffer);
            if(!pDestBuffer)
            {
                delete pSrcBuffer;
                return CMP_ERR_GENERIC;
            }

            DISABLE_FP_EXCEPTIONS;
            pDestBuffer->Copy(*pSrcBuffer);
            RESTORE_FP_EXCEPTIONS;

            delete pSrcBuffer;
            delete pDestBuffer;
        }

        return CMP_OK;
    }
    else if(srcType == CT_None && destType != CT_None)
    {
#ifdef THREADED_COMPRESS
        // Note: 
        // BC7/BC6H has issues with this setting - we already set multithreading via numThreads so
        // this call is disabled for BC7/BC6H ASTC Codecs.
        // if the use has set DiableMultiThreading then numThreads will be set to 1 (regradless of its original value)
        if(
            ((!pOptions || !pOptions->bDisableMultiThreading) && f_dwProcessorCount > 1) 
            && (destType != CT_ASTC)
            && (destType != CT_BC7)
            && (destType != CT_BC6H)
            && (destType != CT_GT)
            )
        {
            tc_err = ThreadedCompressTexture(pSourceTexture, pDestTexture, pOptions, pFeedbackProc, pUser1, pUser2, destType);
            return tc_err;
         }
        else
#endif // THREADED_COMPRESS
        {
            tc_err =  CompressTexture(pSourceTexture, pDestTexture, pOptions, pFeedbackProc, pUser1, pUser2, destType);
            return tc_err;
        }
    }
    else if(srcType != CT_None && destType == CT_None)
    {
        // Decompressing
        CCodec* pCodec = CreateCodec(srcType);
        assert(pCodec);
        if(pCodec == NULL)
            return CMP_ERR_UNABLE_TO_INIT_CODEC;

        CodecBufferType destBufferType = GetCodecBufferType(pDestTexture->format);

        CCodecBuffer* pSrcBuffer = pCodec->CreateBuffer(pSourceTexture->dwWidth, pSourceTexture->dwHeight, pSourceTexture->dwPitch, pSourceTexture->pData);
        CCodecBuffer* pDestBuffer = CreateCodecBuffer(destBufferType, pDestTexture->dwWidth, pDestTexture->dwHeight, pDestTexture->dwPitch, pDestTexture->pData);

        assert(pSrcBuffer);
        assert(pDestBuffer);
        if(pSrcBuffer == NULL || pDestBuffer == NULL)
        {
            SAFE_DELETE(pCodec);
            SAFE_DELETE(pSrcBuffer);
            SAFE_DELETE(pDestBuffer);
            return CMP_ERR_GENERIC;
        }

        DISABLE_FP_EXCEPTIONS;
        CodecError err1 = pCodec->Decompress(*pSrcBuffer, *pDestBuffer, pFeedbackProc, pUser1, pUser2);
        RESTORE_FP_EXCEPTIONS;

        SAFE_DELETE(pCodec);
        SAFE_DELETE(pSrcBuffer);
        SAFE_DELETE(pDestBuffer);

        return GetError(err1);
    }
    else // Decompressing & then compressing
    {
        // Decompressing
        CCodec* pCodecIn    = CreateCodec(srcType);
        CCodec* pCodecOut    = CreateCodec(destType);
        assert(pCodecIn);
        assert(pCodecOut);

        if(pCodecIn == NULL || pCodecOut == NULL )
        {
            SAFE_DELETE(pCodecIn);
            SAFE_DELETE(pCodecOut);
            return CMP_ERR_UNABLE_TO_INIT_CODEC;
        }

        CCodecBuffer* pSrcBuffer = pCodecIn->CreateBuffer(pSourceTexture->dwWidth, pSourceTexture->dwHeight, pSourceTexture->dwPitch, pSourceTexture->pData);
        CCodecBuffer* pTempBuffer = CreateCodecBuffer(CBT_RGBA32F, pDestTexture->dwWidth, pDestTexture->dwHeight);
        CCodecBuffer* pDestBuffer = pCodecOut->CreateBuffer(pDestTexture->dwWidth, pDestTexture->dwHeight, pDestTexture->dwPitch, pDestTexture->pData);

        assert(pSrcBuffer);
        assert(pTempBuffer);
        assert(pDestBuffer);
        if(pSrcBuffer == NULL || pTempBuffer == NULL || pDestBuffer == NULL)
        {
            SAFE_DELETE(pCodecIn);
            SAFE_DELETE(pCodecOut);
            SAFE_DELETE(pSrcBuffer);
            SAFE_DELETE(pTempBuffer);
            SAFE_DELETE(pDestBuffer);
            return CMP_ERR_GENERIC;
        }

        DISABLE_FP_EXCEPTIONS;
        CodecError err2 = pCodecIn->Decompress(*pSrcBuffer, *pTempBuffer, pFeedbackProc, pUser1, pUser2);
        if(err2 == CE_OK)
        {
            err2 = pCodecOut->Compress(*pTempBuffer, *pDestBuffer, pFeedbackProc, pUser1, pUser2);
        }
        RESTORE_FP_EXCEPTIONS;

        return GetError(err2);
    }
}
