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

#include "common.h"
#include "compressonator.h"  // User shared: Keep priviate code out of this header
#include "compress.h"
#include "cmp_mips.h"
#include "debug.h"

#include <cassert>

using namespace CMP;

extern CodecType GetCodecType(CMP_FORMAT format);
extern CMP_ERROR GetError(CodecError err);
#ifdef ENABLE_MAKE_COMPATIBLE_API
extern bool      IsFloatFormat(CMP_FORMAT InFormat);
extern bool      IsCompressedFormat(CMP_FORMAT InFormat);
extern CMP_BYTE  FormatChannelBitSize(CMP_FORMAT InFormat);
extern CMP_ERROR Byte2HalfShort(CMP_HALFSHORT* hfsBlock, CMP_BYTE* cBlock, CMP_DWORD dwBlockSize);
extern CMP_ERROR Float2Byte(CMP_BYTE cBlock[], CMP_FLOAT* fBlock, CMP_Texture* srcTexture, CMP_FORMAT destFormat, const CMP_CompressOptions* pOptions);
extern CMP_ERROR RGBA_Word2Byte(CMP_BYTE cBlock[], CMP_WORD* wBlock, CMP_Texture* srcTexture);
extern CMP_ERROR SByte2Byte(CMP_BYTE cBlock[], CMP_SBYTE* sBlock, CMP_Texture* srcTexture);
extern CMP_ERROR Byte2SByte(CMP_SBYTE sBlock[], CMP_BYTE* cBlock, CMP_Texture* srcTexture);
#endif
extern CMP_ERROR CheckTexture(const CMP_Texture* pTexture, bool bSource);
extern CMP_ERROR CompressTexture(const CMP_Texture*         pSourceTexture,
                                 CMP_Texture*               pDestTexture,
                                 const CMP_CompressOptions* pOptions,
                                 CMP_Feedback_Proc          pFeedbackProc,
                                 CodecType                  destType);
extern CMP_ERROR ThreadedCompressTexture(const CMP_Texture*         pSourceTexture,
                                         CMP_Texture*               pDestTexture,
                                         const CMP_CompressOptions* pOptions,
                                         CMP_Feedback_Proc          pFeedbackProc,
                                         CodecType                  destType);

#ifdef _LOCAL_DEBUG
char DbgTracer::buff[MAX_DBGBUFF_SIZE];
char DbgTracer::PrintBuff[MAX_DBGPPRINTBUFF_SIZE];
#endif

CMP_DWORD CMP_API CMP_CalculateBufferSize(const CMP_Texture* pTexture)
{
#ifdef USE_DBGTRACE
    DbgTrace(("-------> pTexture [%x]", pTexture));
#endif

    assert(pTexture);
    if (pTexture == NULL)
        return 0;

    assert(pTexture->dwSize == sizeof(CMP_Texture));
    if (pTexture->dwSize != sizeof(CMP_Texture))
        return 0;

    assert(pTexture->dwWidth > 0);
    if (pTexture->dwWidth <= 0)
        return 0;

    assert(pTexture->dwHeight > 0);
    if (pTexture->dwHeight <= 0)
        return 0;

    // Check format range is valid
    assert(pTexture->format >= CMP_FORMAT_Unknown && pTexture->format <= CMP_FORMAT_MAX);
    if (pTexture->format < CMP_FORMAT_Unknown || pTexture->format > CMP_FORMAT_MAX)
        return 0;

    return CalcBufferSize(pTexture->format, pTexture->dwWidth, pTexture->dwHeight, pTexture->dwPitch, pTexture->nBlockWidth, pTexture->nBlockHeight);
}

CMP_DWORD CalcBufferSize(CMP_FORMAT format, CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight)
{
#ifdef USE_DBGTRACE
    DbgTrace(("format %d dwWidth %d dwHeight %d dwPitch %d", format, dwWidth, dwHeight, dwPitch));
#endif

    switch (format)
    {
    case CMP_FORMAT_RGBA_8888:
    case CMP_FORMAT_BGRA_8888:
    case CMP_FORMAT_ARGB_8888:
    case CMP_FORMAT_RGBA_8888_S:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * dwHeight));

    case CMP_FORMAT_ARGB_2101010:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * sizeof(CMP_WORD) * dwHeight));

    case CMP_FORMAT_BGR_888:
    case CMP_FORMAT_RGB_888:
    case CMP_FORMAT_RGB_888_S:
        return ((dwPitch) ? (dwPitch * dwHeight) : ((((dwWidth * 3) + 3) >> 2) * 4 * dwHeight));

    case CMP_FORMAT_RG_8:
    case CMP_FORMAT_RG_8_S:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 2 * dwHeight));

    case CMP_FORMAT_R_8:
    case CMP_FORMAT_R_8_S:
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
#endif  // ARGB_32_SUPPORT
    case CMP_FORMAT_ARGB_32F:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 4 * sizeof(float) * dwHeight));

#ifdef ARGB_32_SUPPORT
    case CMP_FORMAT_RG_32:
#endif  // ARGB_32_SUPPORT
    case CMP_FORMAT_RG_32F:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 2 * sizeof(float) * dwHeight));

#ifdef ARGB_32_SUPPORT
    case CMP_FORMAT_R_32:
#endif  // ARGB_32_SUPPORT
    case CMP_FORMAT_R_32F:
        return ((dwPitch) ? (dwPitch * dwHeight) : (dwWidth * 1 * sizeof(float) * dwHeight));

    default:
        return CalcBufferSize(GetCodecType(format), dwWidth, dwHeight, nBlockWidth, nBlockHeight);
    }
}

// This call is used to swizzle source data content.
// Use example: CMP_Map_Bytes(pData, dwWidth, dwHeight, { 2, 1, 0, 3 },4);
void CMP_Map_Bytes(BYTE* src, int width, int height, CMP_MAP_BYTES_SET map, CMP_BYTE offset)
{
    int  i, j;
    BYTE b[4];

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            if (offset == 4)
            {
                b[0]       = *src;
                b[1]       = *(src + 1);
                b[2]       = *(src + 2);
                b[3]       = *(src + 3);
                *(src)     = b[map.B0];
                *(src + 1) = b[map.B1];
                *(src + 2) = b[map.B2];
                *(src + 3) = b[map.B3];
                src        = src + 4;  // move to next set of bytes
            }
            else if (offset == 3)
            {
                b[0]       = *src;
                b[1]       = *(src + 1);
                b[2]       = *(src + 2);
                b[3]       = *(src + 3);
                *(src)     = b[map.B0];
                *(src + 1) = b[map.B1];
                *(src + 2) = b[map.B2];
                src        = src + 3;  // move to next set of bytes
            }
        }
    }
}

#ifndef USE_OLD_SWIZZLE

// For now this function will only handle a single case
// where the source data remains the same size and only RGBA channels
// are swizzled according output compressed formats,
// if source is compressed then no change is performed

void CMP_PrepareSourceForCMP_Destination(CMP_Texture* pTexture, CMP_FORMAT DestFormat)
{
    CMP_DWORD  dwWidth      = pTexture->dwWidth;
    CMP_DWORD  dwHeight     = pTexture->dwHeight;
    CMP_FORMAT newSrcFormat = pTexture->format;
    CMP_BYTE*  pData;

    pData = pTexture->pData;

    switch (newSrcFormat)
    {
    case CMP_FORMAT_BGRA_8888:
    {
        switch (DestFormat)
        {
        case CMP_FORMAT_ATI1N:
        case CMP_FORMAT_ATI2N:
        case CMP_FORMAT_ATI2N_XY:
        case CMP_FORMAT_ATI2N_DXT5:
        case CMP_FORMAT_ATC_RGB:
        case CMP_FORMAT_ATC_RGBA_Explicit:
        case CMP_FORMAT_ATC_RGBA_Interpolated:
        case CMP_FORMAT_BC1:
        case CMP_FORMAT_BC2:
        case CMP_FORMAT_BC3:
        case CMP_FORMAT_BC4:
        case CMP_FORMAT_BC4_S:
        case CMP_FORMAT_BC5:
        case CMP_FORMAT_BC5_S:
        case CMP_FORMAT_DXT1:
        case CMP_FORMAT_DXT3:
        case CMP_FORMAT_DXT5:
        case CMP_FORMAT_DXT5_xGBR:
        case CMP_FORMAT_DXT5_RxBG:
        case CMP_FORMAT_DXT5_RBxG:
        case CMP_FORMAT_DXT5_xRBG:
        case CMP_FORMAT_DXT5_RGxB:
        case CMP_FORMAT_DXT5_xGxR:
        {
            // The source format is correct for these codecs
            break;
        }
        case CMP_FORMAT_ASTC:
        case CMP_FORMAT_BC6H:
        case CMP_FORMAT_BC7:
        case CMP_FORMAT_GT:
        case CMP_FORMAT_ETC_RGB:
        case CMP_FORMAT_ETC2_RGB:
        case CMP_FORMAT_ETC2_SRGB:
        case CMP_FORMAT_ETC2_RGBA:
        case CMP_FORMAT_ETC2_RGBA1:
        case CMP_FORMAT_ETC2_SRGBA:
        case CMP_FORMAT_ETC2_SRGBA1:
        {
            newSrcFormat = CMP_FORMAT_RGBA_8888;
            CMP_Map_Bytes(pData, dwWidth, dwHeight, {2, 1, 0, 3}, 4);
            break;
        }
        default:
            break;
        }
        break;
    }
    case CMP_FORMAT_RGBA_8888:
    {
        switch (DestFormat)
        {
        case CMP_FORMAT_ATI1N:
        case CMP_FORMAT_ATI2N:
        case CMP_FORMAT_ATI2N_XY:
        case CMP_FORMAT_ATI2N_DXT5:
        case CMP_FORMAT_ATC_RGB:
        case CMP_FORMAT_ATC_RGBA_Explicit:
        case CMP_FORMAT_ATC_RGBA_Interpolated:
        case CMP_FORMAT_BC1:
        case CMP_FORMAT_BC2:
        case CMP_FORMAT_BC3:
        case CMP_FORMAT_BC4:
        case CMP_FORMAT_BC4_S:
        case CMP_FORMAT_BC5:
        case CMP_FORMAT_BC5_S:
        case CMP_FORMAT_DXT1:
        case CMP_FORMAT_DXT3:
        case CMP_FORMAT_DXT5:
        case CMP_FORMAT_DXT5_xGBR:
        case CMP_FORMAT_DXT5_RxBG:
        case CMP_FORMAT_DXT5_RBxG:
        case CMP_FORMAT_DXT5_xRBG:
        case CMP_FORMAT_DXT5_RGxB:
        case CMP_FORMAT_DXT5_xGxR:
        {
            newSrcFormat = CMP_FORMAT_BGRA_8888;
            CMP_Map_Bytes(pData, dwWidth, dwHeight, {2, 1, 0, 3}, 4);
            break;
        }
        case CMP_FORMAT_ASTC:
        case CMP_FORMAT_BC6H:
        case CMP_FORMAT_BC7:
        case CMP_FORMAT_ETC_RGB:
        case CMP_FORMAT_ETC2_RGB:
        case CMP_FORMAT_ETC2_SRGB:
        case CMP_FORMAT_ETC2_RGBA:
        case CMP_FORMAT_ETC2_RGBA1:
        case CMP_FORMAT_ETC2_SRGBA:
        case CMP_FORMAT_ETC2_SRGBA1:
        case CMP_FORMAT_GT:
        {
            // format is correct
        }
        default:
            break;
        }
        break;
    }
    case CMP_FORMAT_ARGB_8888:
    {
        switch (DestFormat)
        {
        case CMP_FORMAT_ATI1N:
        case CMP_FORMAT_ATI2N:
        case CMP_FORMAT_ATI2N_XY:
        case CMP_FORMAT_ATI2N_DXT5:
        case CMP_FORMAT_ATC_RGB:
        case CMP_FORMAT_ATC_RGBA_Explicit:
        case CMP_FORMAT_ATC_RGBA_Interpolated:
        case CMP_FORMAT_BC1:
        case CMP_FORMAT_BC2:
        case CMP_FORMAT_BC3:
        case CMP_FORMAT_BC4:
        case CMP_FORMAT_BC5:
        case CMP_FORMAT_DXT1:
        case CMP_FORMAT_DXT3:
        case CMP_FORMAT_DXT5:
        case CMP_FORMAT_DXT5_xGBR:
        case CMP_FORMAT_DXT5_RxBG:
        case CMP_FORMAT_DXT5_RBxG:
        case CMP_FORMAT_DXT5_xRBG:
        case CMP_FORMAT_DXT5_RGxB:
        case CMP_FORMAT_DXT5_xGxR:
        {
            newSrcFormat = CMP_FORMAT_BGRA_8888;
            CMP_Map_Bytes(pData, dwWidth, dwHeight, {3, 2, 1, 0}, 4);
            break;
        }
        case CMP_FORMAT_ASTC:
        case CMP_FORMAT_BC6H:
        case CMP_FORMAT_BC7:
        case CMP_FORMAT_ETC_RGB:
        case CMP_FORMAT_ETC2_RGB:
        case CMP_FORMAT_ETC2_SRGB:
        case CMP_FORMAT_ETC2_RGBA:
        case CMP_FORMAT_ETC2_RGBA1:
        case CMP_FORMAT_ETC2_SRGBA:
        case CMP_FORMAT_ETC2_SRGBA1:
        case CMP_FORMAT_GT:
        {
            newSrcFormat = CMP_FORMAT_RGBA_8888;
            CMP_Map_Bytes(pData, dwWidth, dwHeight, {1, 2, 3, 0}, 4);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }

    // Update Source format to new one
    pTexture->format = newSrcFormat;
}

// For now this function will only handle a single case
// where the source data remains the same size and only RGBA channels
// are swizzled according output compressed formats,
// if source is compressed then no change is performed

void CMP_PrepareCMPSourceForIMG_Destination(CMP_Texture* pDstTexture, CMP_FORMAT SrcFormat)
{
    CMP_DWORD  dwWidth      = pDstTexture->dwWidth;
    CMP_DWORD  dwHeight     = pDstTexture->dwHeight;
    CMP_FORMAT newDstFormat = pDstTexture->format;
    CMP_BYTE*  pData;

    pData = pDstTexture->pData;

    switch (SrcFormat)
    {
    // decompressed Data  is in the form BGRA
    case CMP_FORMAT_ATI1N:
    case CMP_FORMAT_ATI2N:
    case CMP_FORMAT_ATI2N_XY:
    case CMP_FORMAT_ATI2N_DXT5:
    case CMP_FORMAT_ATC_RGB:
    case CMP_FORMAT_ATC_RGBA_Explicit:
    case CMP_FORMAT_ATC_RGBA_Interpolated:
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_BC4_S:
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_BC5_S:
    case CMP_FORMAT_DXT1:
    case CMP_FORMAT_DXT3:
    case CMP_FORMAT_DXT5:
    case CMP_FORMAT_DXT5_xGBR:
    case CMP_FORMAT_DXT5_RxBG:
    case CMP_FORMAT_DXT5_RBxG:
    case CMP_FORMAT_DXT5_xRBG:
    case CMP_FORMAT_DXT5_RGxB:
    case CMP_FORMAT_DXT5_xGxR:
    {
        switch (newDstFormat)
        {
        case CMP_FORMAT_BGRA_8888:
            break;
        case CMP_FORMAT_RGBA_8888:
        {
            CMP_Map_Bytes(pData, dwWidth, dwHeight, {2, 1, 0, 3}, 4);
            break;
        }
        default:
            break;
        }
    }
    // decompressed Data  is in the form RGBA_8888
    case CMP_FORMAT_ASTC:
    case CMP_FORMAT_BC6H:
    case CMP_FORMAT_BC7:
    case CMP_FORMAT_GT:
    case CMP_FORMAT_ETC_RGB:
    case CMP_FORMAT_ETC2_RGB:
    case CMP_FORMAT_ETC2_SRGB:
    case CMP_FORMAT_ETC2_RGBA:
    case CMP_FORMAT_ETC2_RGBA1:
    case CMP_FORMAT_ETC2_SRGBA:
    case CMP_FORMAT_ETC2_SRGBA1:
    {
        switch (newDstFormat)
        {
        case CMP_FORMAT_RGBA_8888:
            break;
        case CMP_FORMAT_BGRA_8888:
        {
            CMP_Map_Bytes(pData, dwWidth, dwHeight, {2, 1, 0, 3}, 4);
            break;
        }
        default:
            break;
        }
    }
    }
}
#endif

CMP_ERROR CMP_API CMP_ConvertTexture(CMP_Texture*               pSourceTexture,
                                     CMP_Texture*               pDestTexture,
                                     const CMP_CompressOptions* pOptions,
                                     CMP_Feedback_Proc          pFeedbackProc)
{
#ifdef USE_DBGTRACE
    DbgTrace(("-------> pSourceTexture [%x] pDestTexture [%x] pOptions [%x]", pSourceTexture, pDestTexture, pOptions));
#endif
    CMP_ERROR tc_err = CheckTexture(pSourceTexture, true);
    if (tc_err != CMP_OK)
        return tc_err;

#ifdef ENABLE_MAKE_COMPATIBLE_API
    bool srcFloat  = IsFloatFormat(pSourceTexture->format);
    bool destFloat = IsFloatFormat(pDestTexture->format);

    bool newBuffer = false;
    if (srcFloat && !destFloat)
    {
        CMP_DWORD  size   = pSourceTexture->dwWidth * pSourceTexture->dwHeight;
        CMP_FLOAT* pfData = new CMP_FLOAT[pSourceTexture->dwDataSize];

        memcpy(pfData, pSourceTexture->pData, pSourceTexture->dwDataSize);

        CMP_BYTE* byteData = new CMP_BYTE[size * 4];

        Float2Byte(byteData, pfData, pSourceTexture, pDestTexture->format, pOptions);

        delete[] pfData;
        pSourceTexture->pData = byteData;

        pSourceTexture->format     = CMP_FORMAT_ARGB_8888;
        pSourceTexture->dwDataSize = size * 4;
        newBuffer                  = true;
    }

    else if (!srcFloat && destFloat)
    {
        CMP_DWORD      size        = pSourceTexture->dwWidth * pSourceTexture->dwHeight;
        CMP_BYTE*      pbData      = pSourceTexture->pData;
        CMP_HALFSHORT* hfloatSData = new CMP_HALFSHORT[size * 4];  // wxh*4 channels of type half
        Byte2HalfShort(hfloatSData, pbData, size * 4);
        pSourceTexture->pData      = (CMP_BYTE*)hfloatSData;
        pSourceTexture->format     = CMP_FORMAT_ARGB_16F;
        pSourceTexture->dwDataSize = size * 4 * sizeof(CMP_HALFSHORT);
        pSourceTexture->dwPitch    = pSourceTexture->dwWidth * 4 * sizeof(CMP_HALFSHORT);
        newBuffer                  = true;
    }
    else if (!srcFloat && !destFloat) 
    {
        // Both channels of not float, check for matching source & target formats.
        CMP_BYTE srcbitsize = FormatChannelBitSize(pSourceTexture->format);
        CMP_BYTE dstbitsize = FormatChannelBitSize(pDestTexture->format);
        // we are compressing not transcoding need to make src compatable
        if ((srcbitsize != dstbitsize) && (IsCompressedFormat(pDestTexture->format)))
        {
            CMP_DWORD size   = pSourceTexture->dwWidth * pSourceTexture->dwHeight;
            CMP_WORD* pwData = new CMP_WORD[pSourceTexture->dwDataSize];

            memcpy(pwData, pSourceTexture->pData, pSourceTexture->dwDataSize);

            CMP_BYTE* byteData = new CMP_BYTE[size * 4];

            RGBA_Word2Byte(byteData, pwData, pSourceTexture);

            delete[] pwData;
            pSourceTexture->pData = byteData;

            pSourceTexture->format     = CMP_FORMAT_ARGB_8888;
            pSourceTexture->dwDataSize = size * 4;
            newBuffer                  = true;
        }
        else
            // we are compressing make sure bit signs are the same
            if ((srcbitsize == 8) && (dstbitsize == 8) && (IsCompressedFormat(pDestTexture->format)) && (pSourceTexture->format == CMP_FORMAT_RGBA_8888_S))
        {
            if ((pDestTexture->format != CMP_FORMAT_BC4_S) && ((pDestTexture->format != CMP_FORMAT_BC5_S)))
            {
                //CMP_DWORD size     = pSourceTexture->dwWidth * pSourceTexture->dwHeight;
                CMP_BYTE* byteData = new CMP_BYTE[pSourceTexture->dwDataSize];

                SByte2Byte(byteData, (CMP_SBYTE*)pSourceTexture->pData, pSourceTexture);

                pSourceTexture->pData      = byteData;
                pSourceTexture->format     = CMP_FORMAT_ARGB_8888;
                pSourceTexture->dwDataSize = pSourceTexture->dwDataSize;
                newBuffer                  = true;
            }
        }
    }
#endif
    tc_err = CheckTexture(pDestTexture, false);
    if (tc_err != CMP_OK)
        return tc_err;

    if (pSourceTexture->dwWidth != pDestTexture->dwWidth || pSourceTexture->dwHeight != pDestTexture->dwHeight)
        return CMP_ERR_SIZE_MISMATCH;

    CodecType srcType = GetCodecType(pSourceTexture->format);
    assert(srcType != CT_Unknown);
    if (srcType == CT_Unknown)
        return CMP_ERR_UNSUPPORTED_SOURCE_FORMAT;

    CodecType destType = GetCodecType(pDestTexture->format);
    assert(destType != CT_Unknown);
    if (destType == CT_Unknown)
        return CMP_ERR_UNSUPPORTED_SOURCE_FORMAT;

    if (srcType == destType)
    {
        // Easy case ?
        if (pSourceTexture->format == pDestTexture->format && pSourceTexture->dwPitch == pDestTexture->dwPitch)
            memcpy(pDestTexture->pData, pSourceTexture->pData, CMP_CalculateBufferSize(pSourceTexture));
        else
        {
            CodecBufferType srcBufferType  = GetCodecBufferType(pSourceTexture->format);
            CodecBufferType destBufferType = GetCodecBufferType(pDestTexture->format);

            CCodecBuffer* pSrcBuffer = CreateCodecBuffer(srcBufferType,
                                                         pSourceTexture->nBlockWidth,
                                                         pSourceTexture->nBlockHeight,
                                                         pSourceTexture->nBlockDepth,
                                                         pSourceTexture->dwWidth,
                                                         pSourceTexture->dwHeight,
                                                         pSourceTexture->dwPitch,
                                                         pSourceTexture->pData,
                                                         pSourceTexture->dwDataSize);
            assert(pSrcBuffer);
            if (!pSrcBuffer)
                return CMP_ERR_GENERIC;

            CCodecBuffer* pDestBuffer = CreateCodecBuffer(destBufferType,
                                                          pDestTexture->nBlockWidth,
                                                          pDestTexture->nBlockHeight,
                                                          pDestTexture->nBlockDepth,
                                                          pDestTexture->dwWidth,
                                                          pDestTexture->dwHeight,
                                                          pDestTexture->dwPitch,
                                                          pDestTexture->pData,
                                                          pDestTexture->dwDataSize);
            assert(pDestBuffer);
            if (!pDestBuffer)
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
    else if (srcType == CT_None && destType != CT_None)
    {
#ifndef USE_OLD_SWIZZLE
        CMP_PrepareSourceForCMP_Destination(pSourceTexture, pDestTexture->format);
#endif

        CMP_BOOL bMultithread = true;
        if (!pOptions->bDisableMultiThreading && (pOptions->dwnumThreads == 1))
            bMultithread = false;

#ifdef THREADED_COMPRESS
        // Note:
        // BC7/BC6H has issues with this setting - we already set multithreading via numThreads so
        // this call is disabled for BC7/BC6H ASTC Codecs.
        // if the use has set DiableMultiThreading then numThreads will be set to 1 (regradless of its original value)
        if (((!pOptions || !pOptions->bDisableMultiThreading) && f_dwProcessorCount > 1) && (bMultithread) && (destType != CT_ASTC) && (destType != CT_BC7) &&
            (destType != CT_BC6H) && (destType != CT_BC6H_SF)
#ifdef USE_APC
            && (destType != CT_APC)
#endif
#ifdef USE_GTC
            && (destType != CT_GTC)
#endif
#ifdef USE_BASIS
            && (destType != CT_BASIS)
#endif
        )
        {
            tc_err = ThreadedCompressTexture(pSourceTexture, pDestTexture, pOptions, pFeedbackProc, destType);
#ifdef ENABLE_MAKE_COMPATIBLE_API
            if (pSourceTexture->pData && newBuffer)
            {
                free(pSourceTexture->pData);
                pSourceTexture->pData = NULL;
            }
#endif
            return tc_err;
        }
        else
#endif  // THREADED_COMPRESS
        {
            tc_err = CompressTexture(pSourceTexture, pDestTexture, pOptions, pFeedbackProc, destType);
#ifdef ENABLE_MAKE_COMPATIBLE_API
            if (pSourceTexture->pData && newBuffer)
            {
                free(pSourceTexture->pData);
                pSourceTexture->pData = NULL;
            }
#endif
            return tc_err;
        }
    }
    else if (srcType != CT_None && destType == CT_None)
    {
        // Decompressing

        CCodec* pCodec = CreateCodec(srcType);
        assert(pCodec);
        if (pCodec == NULL)
        {
#ifdef ENABLE_MAKE_COMPATIBLE_API
            if (pSourceTexture->pData && newBuffer)
            {
                free(pSourceTexture->pData);
                pSourceTexture->pData = NULL;
            }
#endif
            return CMP_ERR_UNABLE_TO_INIT_CODEC;
        }

        CodecBufferType destBufferType = GetCodecBufferType(pDestTexture->format);

        CCodecBuffer* pSrcBuffer = pCodec->CreateBuffer(pSourceTexture->nBlockWidth,
                                                        pSourceTexture->nBlockHeight,
                                                        pSourceTexture->nBlockDepth,
                                                        pSourceTexture->dwWidth,
                                                        pSourceTexture->dwHeight,
                                                        pSourceTexture->dwPitch,
                                                        pSourceTexture->pData,
                                                        pSourceTexture->dwDataSize);

        pDestTexture->nBlockWidth  = pSourceTexture->nBlockWidth;
        pDestTexture->nBlockHeight = pSourceTexture->nBlockHeight;
        pDestTexture->nBlockDepth  = pSourceTexture->nBlockDepth;

        CCodecBuffer* pDestBuffer = CreateCodecBuffer(destBufferType,
                                                      pDestTexture->nBlockWidth,
                                                      pDestTexture->nBlockHeight,
                                                      pDestTexture->nBlockDepth,
                                                      pDestTexture->dwWidth,
                                                      pDestTexture->dwHeight,
                                                      pDestTexture->dwPitch,
                                                      pDestTexture->pData,
                                                      pDestTexture->dwDataSize);

        // assert(pDestBuffer);
        // assert(pSrcBuffer);

        if (pSrcBuffer == NULL || pDestBuffer == NULL)
        {
            SAFE_DELETE(pCodec);
            SAFE_DELETE(pSrcBuffer);
            SAFE_DELETE(pDestBuffer);
#ifdef ENABLE_MAKE_COMPATIBLE_API
            if (pSourceTexture->pData && newBuffer)
            {
                free(pSourceTexture->pData);
                pSourceTexture->pData = NULL;
            }
#endif
            return CMP_ERR_GENERIC;
        }

        DISABLE_FP_EXCEPTIONS;

        pSrcBuffer->SetBlockHeight(pSourceTexture->nBlockHeight);
        pSrcBuffer->SetBlockWidth(pSourceTexture->nBlockWidth);
        pSrcBuffer->SetBlockDepth(pSourceTexture->nBlockDepth);
        pSrcBuffer->SetFormat(pSourceTexture->format);
        pSrcBuffer->SetTranscodeFormat(pSourceTexture->transcodeFormat);

        pDestBuffer->SetBlockHeight(pDestTexture->nBlockHeight);
        pDestBuffer->SetBlockWidth(pDestTexture->nBlockWidth);
        pDestBuffer->SetBlockDepth(pDestTexture->nBlockDepth);
        pDestBuffer->SetFormat(pDestTexture->format);

        CodecError err1 = pCodec->Decompress(*pSrcBuffer, *pDestBuffer, pFeedbackProc);
        RESTORE_FP_EXCEPTIONS;

#ifndef USE_OLD_SWIZZLE
        CMP_PrepareCMPSourceForIMG_Destination(pDestTexture, pSourceTexture->format);
#endif

        SAFE_DELETE(pCodec);
        SAFE_DELETE(pSrcBuffer);
        SAFE_DELETE(pDestBuffer);

#ifdef ENABLE_MAKE_COMPATIBLE_API
        if (pSourceTexture->pData && newBuffer)
        {
            free(pSourceTexture->pData);
            pSourceTexture->pData = NULL;
        }
#endif
        return GetError(err1);
    }
    else
    {  // Decompressing & then compressing
        // Decompressing
        CCodec* pCodecIn  = CreateCodec(srcType);
        CCodec* pCodecOut = CreateCodec(destType);
        assert(pCodecIn);
        assert(pCodecOut);

        if (pCodecIn == NULL || pCodecOut == NULL)
        {
            SAFE_DELETE(pCodecIn);
            SAFE_DELETE(pCodecOut);
#ifdef ENABLE_MAKE_COMPATIBLE_API
            if (pSourceTexture->pData && newBuffer)
            {
                free(pSourceTexture->pData);
                pSourceTexture->pData = NULL;
            }
#endif
            return CMP_ERR_UNABLE_TO_INIT_CODEC;
        }

        CCodecBuffer* pSrcBuffer  = pCodecIn->CreateBuffer(pSourceTexture->nBlockWidth,
                                                          pSourceTexture->nBlockHeight,
                                                          pSourceTexture->nBlockDepth,
                                                          pSourceTexture->dwWidth,
                                                          pSourceTexture->dwHeight,
                                                          pSourceTexture->dwPitch,
                                                          pSourceTexture->pData,
                                                          pSourceTexture->dwDataSize);
        CCodecBuffer* pTempBuffer = CreateCodecBuffer(
            CBT_RGBA32F, pDestTexture->nBlockWidth, pDestTexture->nBlockHeight, pDestTexture->nBlockDepth, pDestTexture->dwWidth, pDestTexture->dwHeight);
        CCodecBuffer* pDestBuffer = pCodecOut->CreateBuffer(pDestTexture->nBlockWidth,
                                                            pDestTexture->nBlockHeight,
                                                            pDestTexture->nBlockDepth,
                                                            pDestTexture->dwWidth,
                                                            pDestTexture->dwHeight,
                                                            pDestTexture->dwPitch,
                                                            pDestTexture->pData,
                                                            pDestTexture->dwDataSize);

        assert(pSrcBuffer);
        assert(pTempBuffer);
        assert(pDestBuffer);
        if (pSrcBuffer == NULL || pTempBuffer == NULL || pDestBuffer == NULL)
        {
            SAFE_DELETE(pCodecIn);
            SAFE_DELETE(pCodecOut);
            SAFE_DELETE(pSrcBuffer);
            SAFE_DELETE(pTempBuffer);
            SAFE_DELETE(pDestBuffer);
#ifdef ENABLE_MAKE_COMPATIBLE_API
            if (pSourceTexture->pData && newBuffer)
            {
                free(pSourceTexture->pData);
                pSourceTexture->pData = NULL;
            }
#endif
            return CMP_ERR_GENERIC;
        }

        DISABLE_FP_EXCEPTIONS;
        CodecError err2 = pCodecIn->Decompress(*pSrcBuffer, *pTempBuffer, pFeedbackProc);
        if (err2 == CE_OK)
        {
            err2 = pCodecOut->Compress(*pTempBuffer, *pDestBuffer, pFeedbackProc);
        }
        RESTORE_FP_EXCEPTIONS;

#ifdef ENABLE_MAKE_COMPATIBLE_API
        if (pSourceTexture->pData && newBuffer)
        {
            free(pSourceTexture->pData);
            pSourceTexture->pData = NULL;
        }
#endif
        return GetError(err2);
    }
}

CMP_ERROR CMP_API CMP_ConvertMipTexture(CMP_MipSet* p_MipSetIn, CMP_MipSet* p_MipSetOut, const CMP_CompressOptions* pOptions, CMP_Feedback_Proc pFeedbackProc)
{
    assert(p_MipSetIn);
    assert(p_MipSetOut);
    assert(pOptions);

    CMP_CMIPS CMips;

    // --------------------------------
    // Setup Compressed Mip Set Traget
    // --------------------------------
    //if (GetCodecType(pOptions->DestFormat) == CT_Unknown) return CMP_ERR_UNKNOWN_DESTINATION_FORMAT;

    // -------------
    // Output
    // -------------
    memset(p_MipSetOut, 0, sizeof(CMP_MipSet));
    p_MipSetOut->m_Flags   = MS_FLAG_Default;
    p_MipSetOut->m_format  = pOptions->DestFormat;
    p_MipSetOut->m_nHeight = p_MipSetIn->m_nHeight;
    p_MipSetOut->m_nWidth  = p_MipSetIn->m_nWidth;
    CMP_Format2FourCC(pOptions->DestFormat, p_MipSetOut);

    // Default compression block size if not set!
    p_MipSetIn->m_nBlockWidth  = (p_MipSetIn->m_nBlockWidth == 0) ? 4 : p_MipSetIn->m_nBlockWidth;
    p_MipSetIn->m_nBlockHeight = (p_MipSetIn->m_nBlockHeight == 0) ? 4 : p_MipSetIn->m_nBlockHeight;
    p_MipSetIn->m_nDepth       = (p_MipSetIn->m_nDepth < 1) ? 1 : p_MipSetIn->m_nDepth;

    // Allocate compression data
    p_MipSetOut->m_nMipLevels    = 1;  // this is overwriiten depending on input.
    p_MipSetOut->m_ChannelFormat = CF_Compressed;
    p_MipSetOut->m_nMaxMipLevels = p_MipSetIn->m_nMaxMipLevels;
    p_MipSetOut->m_nBlockWidth   = p_MipSetIn->m_nBlockWidth;
    p_MipSetOut->m_nBlockHeight  = p_MipSetIn->m_nBlockHeight;
    p_MipSetOut->m_nDepth        = p_MipSetIn->m_nDepth;
    p_MipSetOut->m_TextureType   = p_MipSetIn->m_TextureType;

    p_MipSetOut->m_nIterations = 0;  // tracks number of processed data miplevels

    //=====================================================
    // Case Uncompressed Source to Compressed Destination
    //=====================================================
    CMP_Texture srcTexture;
    srcTexture.dwSize = sizeof(srcTexture);

#ifdef USE_BASIS
    if (pOptions->DestFormat == CMP_FORMAT_BASIS)
    {
        p_MipSetOut->m_format          = CMP_FORMAT_BASIS;
        p_MipSetOut->m_transcodeFormat = CMP_FORMAT_BC1;
        p_MipSetOut->m_TextureType     = TT_2D;
        p_MipSetOut->m_TextureDataType = TDT_ARGB;
        p_MipSetOut->m_ChannelFormat   = CF_Compressed;
        p_MipSetOut->dwDataSize        = 0;  // will be set after compression
        p_MipSetOut->dwWidth           = 1;  // Container for 1 linear data buffer
        p_MipSetOut->dwHeight          = 1;  // Container for 1 linear data buffer
        p_MipSetOut->m_nDepth          = 1;

        if (!CMips.AllocateMipSet(p_MipSetOut, p_MipSetOut->m_ChannelFormat, TDT_ARGB, p_MipSetOut->m_TextureType, 1, 1, 1))
        {
            return CMP_ERR_MEM_ALLOC_FOR_MIPSET;
        }

        //=====================
        // Uncompressed source
        //======================
        CMP_MipLevel* pInMipLevel  = CMips.GetMipLevel(p_MipSetIn, 0, 0);
        srcTexture.dwPitch         = 0;
        srcTexture.nBlockWidth     = p_MipSetIn->m_nBlockWidth;
        srcTexture.nBlockHeight    = p_MipSetIn->m_nBlockHeight;
        srcTexture.nBlockDepth     = p_MipSetIn->m_nBlockDepth;
        srcTexture.format          = p_MipSetIn->m_format;
        srcTexture.dwWidth         = p_MipSetIn->m_nWidth;
        srcTexture.dwHeight        = p_MipSetIn->m_nHeight;
        srcTexture.transcodeFormat = p_MipSetIn->m_transcodeFormat;
        srcTexture.pData           = pInMipLevel->m_pbData;
        srcTexture.dwWidth         = pInMipLevel->m_nWidth;
        srcTexture.dwHeight        = pInMipLevel->m_nHeight;
        srcTexture.pData           = pInMipLevel->m_pbData;

        srcTexture.dwDataSize = CMP_CalculateBufferSize(&srcTexture);

        //========================
        // Compressed Destination
        //========================
        CMP_Texture destTexture;
        MipLevel*   pOutMipLevel  = CMips.GetMipLevel(p_MipSetOut, 0, 0);
        pOutMipLevel->m_pvec8Data = new CMP_VEC8();  // size = 0 before compression, will be set during processing
        p_MipSetOut->pData        = pOutMipLevel->m_pbData;

        destTexture.dwSize         = sizeof(destTexture);
        destTexture.dwWidth        = p_MipSetIn->m_nWidth;
        destTexture.dwHeight       = p_MipSetIn->m_nHeight;
        destTexture.dwPitch        = 0;
        destTexture.nBlockWidth    = p_MipSetIn->m_nBlockWidth;
        destTexture.nBlockHeight   = p_MipSetIn->m_nBlockHeight;
        destTexture.format         = pOptions->DestFormat;
        srcTexture.transcodeFormat = p_MipSetIn->m_transcodeFormat;
        destTexture.dwDataSize     = CMP_CalculateBufferSize(&srcTexture);  // This will be reset after compression
        destTexture.pData          = pOutMipLevel->m_pbData;

        //========================
        // Process ConvertTexture
        //========================
        CMP_ERROR cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, pOptions, pFeedbackProc);
        if (cmp_status != CMP_OK)
        {
            return cmp_status;
        }
    }
    else
#endif
    {
        if (!CMips.AllocateMipSet(p_MipSetOut,
                                  p_MipSetOut->m_ChannelFormat,
                                  TDT_ARGB,
                                  p_MipSetOut->m_TextureType,
                                  p_MipSetIn->m_nWidth,
                                  p_MipSetIn->m_nHeight,
                                  p_MipSetOut->m_nDepth))
        {
            return CMP_ERR_MEM_ALLOC_FOR_MIPSET;
        }

        p_MipSetOut->m_nMipLevels = p_MipSetIn->m_nMipLevels;

        for (int nMipLevel = 0; nMipLevel < p_MipSetIn->m_nMipLevels; nMipLevel++)
        {
            for (int nFaceOrSlice = 0; nFaceOrSlice < CMP_MaxFacesOrSlices(p_MipSetIn, nMipLevel); nFaceOrSlice++)
            {
                //=====================
                // Uncompressed source
                //======================
                CMP_MipLevel* pInMipLevel = CMips.GetMipLevel(p_MipSetIn, nMipLevel, nFaceOrSlice);
                srcTexture.dwPitch        = 0;
                srcTexture.nBlockWidth    = p_MipSetIn->m_nBlockWidth;
                srcTexture.nBlockHeight   = p_MipSetIn->m_nBlockHeight;
                srcTexture.nBlockDepth    = p_MipSetIn->m_nBlockDepth;
                srcTexture.format         = p_MipSetIn->m_format;
                srcTexture.dwWidth        = pInMipLevel->m_nWidth;
                srcTexture.dwHeight       = pInMipLevel->m_nHeight;
                srcTexture.pData          = pInMipLevel->m_pbData;
                srcTexture.dwDataSize     = CMP_CalculateBufferSize(&srcTexture);

                // Temporary settings
                p_MipSetIn->dwWidth    = pInMipLevel->m_nWidth;
                p_MipSetIn->dwHeight   = pInMipLevel->m_nHeight;
                p_MipSetIn->pData      = pInMipLevel->m_pbData;
                p_MipSetIn->dwDataSize = CMP_CalculateBufferSize(&srcTexture);

                //========================
                // Compressed Destination
                //========================
                CMP_Texture destTexture;
                destTexture.dwSize       = sizeof(destTexture);
                destTexture.dwWidth      = pInMipLevel->m_nWidth;
                destTexture.dwHeight     = pInMipLevel->m_nHeight;
                destTexture.dwPitch      = 0;
                destTexture.nBlockWidth  = p_MipSetIn->m_nBlockWidth;
                destTexture.nBlockHeight = p_MipSetIn->m_nBlockHeight;
                destTexture.format       = pOptions->DestFormat;
                destTexture.dwDataSize   = CMP_CalculateBufferSize(&destTexture);

                p_MipSetOut->m_format = pOptions->DestFormat;

                p_MipSetOut->dwDataSize = CMP_CalculateBufferSize(&destTexture);
                p_MipSetOut->dwWidth    = pInMipLevel->m_nWidth;
                p_MipSetOut->dwHeight   = pInMipLevel->m_nHeight;

                //--------------------------------------
                // Allocate MipSet for Block Compressors
                //--------------------------------------
                CMP_MipLevel* pOutMipLevel = CMips.GetMipLevel(p_MipSetOut, nMipLevel, nFaceOrSlice);
                if (!CMips.AllocateCompressedMipLevelData(pOutMipLevel, destTexture.dwWidth, destTexture.dwHeight, destTexture.dwDataSize))
                {
                    return CMP_ERR_MEM_ALLOC_FOR_MIPSET;
                }

                destTexture.pData  = pOutMipLevel->m_pbData;
                p_MipSetOut->pData = pOutMipLevel->m_pbData;

                //==========================
                // User suppiled Print Info
                //==========================
                if (pOptions->m_PrintInfoStr)
                {
                    char buff[256];
                    snprintf(buff,
                             sizeof(buff),
                             "\nSource Texture size = %d Bytes, width = %d px  height = %d px\n",
                             srcTexture.dwDataSize,
                             srcTexture.dwWidth,
                             srcTexture.dwHeight);
                    pOptions->m_PrintInfoStr(buff);
                    if (destTexture.dwDataSize > 0)
                    {
                        snprintf(buff,
                                 sizeof(buff),
                                 "Destination Texture size = %d Bytes   Resulting compression ratio = %2.2f:1\n",
                                 destTexture.dwDataSize,
                                 srcTexture.dwDataSize / (float)destTexture.dwDataSize);
                        pOptions->m_PrintInfoStr(buff);
                    }
                }

                //========================
                // Process ConvertTexture
                //========================
                CMP_ERROR cmp_status = CMP_ConvertTexture(&srcTexture, &destTexture, pOptions, pFeedbackProc);
                if (cmp_status != CMP_OK)
                {
                    return cmp_status;
                }
                else
                    p_MipSetOut->m_nIterations++;
            }
        }
    }
    if (pFeedbackProc)
        pFeedbackProc(100, NULL, NULL);

    return CMP_OK;
}
