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
//  File Name:   Compress.cpp
//  Description: A library to compress/decompress textures
//
//  Revisions
//  Apr 2014    -    Refactored Library
//                   Code clean to support MSV 2010 and up
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Compressonator.h"
#include "Compress.h"
#include <assert.h>
#include <algorithm>

#ifdef _WIN32
#include "windows.h"
#include "sysinfoapi.h"
#endif

CMP_INT CMP_GetNumberOfProcessors()
{
#ifndef _WIN32
//    return sysconf(_SC_NPROCESSORS_ONLN);
    return std::thread::hardware_concurrency();
#else
    // Figure out how many cores there are on this machine
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return (sysinfo.dwNumberOfProcessors);
#endif
}


CodecType GetCodecType(CMP_FORMAT format)
{
    switch(format)
    {
        case CMP_FORMAT_ARGB_2101010:            return CT_None;
        case CMP_FORMAT_RGBA_8888:               return CT_None;
        case CMP_FORMAT_BGRA_8888:               return CT_None;
        case CMP_FORMAT_ARGB_8888:               return CT_None;
        case CMP_FORMAT_BGR_888:                 return CT_None;
        case CMP_FORMAT_RGB_888:                 return CT_None;
        case CMP_FORMAT_RG_8:                    return CT_None;
        case CMP_FORMAT_R_8:                     return CT_None;
        case CMP_FORMAT_ARGB_16:                 return CT_None;
        case CMP_FORMAT_RG_16:                   return CT_None;
        case CMP_FORMAT_R_16:                    return CT_None;
        case CMP_FORMAT_ARGB_16F:                return CT_None;
        case CMP_FORMAT_RG_16F:                  return CT_None;
        case CMP_FORMAT_R_16F:                   return CT_None;
        case CMP_FORMAT_ARGB_32F:                return CT_None;
        case CMP_FORMAT_RG_32F:                  return CT_None;
        case CMP_FORMAT_R_32F:                   return CT_None;
        case CMP_FORMAT_RGBE_32F:                return CT_None;
#ifdef ARGB_32_SUPPORT
        case CMP_FORMAT_ARGB_32:                 return CT_None;
        case CMP_FORMAT_RG_32:                   return CT_None;
        case CMP_FORMAT_R_32:                    return CT_None;
#endif  // ARGB_32_SUPPORT
        case CMP_FORMAT_DXT1:                    return CT_DXT1;
        case CMP_FORMAT_DXT3:                    return CT_DXT3;
        case CMP_FORMAT_DXT5:                    return CT_DXT5;
        case CMP_FORMAT_DXT5_xGBR:               return CT_DXT5_xGBR;
        case CMP_FORMAT_DXT5_RxBG:               return CT_DXT5_RxBG;
        case CMP_FORMAT_DXT5_RBxG:               return CT_DXT5_RBxG;
        case CMP_FORMAT_DXT5_xRBG:               return CT_DXT5_xRBG;
        case CMP_FORMAT_DXT5_RGxB:               return CT_DXT5_RGxB;
        case CMP_FORMAT_DXT5_xGxR:               return CT_DXT5_xGxR;
        case CMP_FORMAT_ATI1N:                   return CT_ATI1N;
        case CMP_FORMAT_ATI2N:                   return CT_ATI2N;
        case CMP_FORMAT_ATI2N_XY:                return CT_ATI2N_XY;
        case CMP_FORMAT_ATI2N_DXT5:              return CT_ATI2N_DXT5;
        case CMP_FORMAT_BC1:                     return CT_DXT1;
        case CMP_FORMAT_BC2:                     return CT_DXT3;
        case CMP_FORMAT_BC3:                     return CT_DXT5;
        case CMP_FORMAT_BC4:                     return CT_ATI1N;
        case CMP_FORMAT_BC5:                     return CT_ATI2N_XY;    // Red & Green channels
        case CMP_FORMAT_BC6H:                    return CT_BC6H; 
        case CMP_FORMAT_BC6H_SF:                 return CT_BC6H_SF;
        case CMP_FORMAT_BC7:                     return CT_BC7;
        case CMP_FORMAT_ASTC:                    return CT_ASTC;
        case CMP_FORMAT_ATC_RGB:                 return CT_ATC_RGB;
        case CMP_FORMAT_ATC_RGBA_Explicit:       return CT_ATC_RGBA_Explicit;
        case CMP_FORMAT_ATC_RGBA_Interpolated:   return CT_ATC_RGBA_Interpolated;
        case CMP_FORMAT_ETC_RGB:                 return CT_ETC_RGB;
        case CMP_FORMAT_ETC2_RGB:                return CT_ETC2_RGB;
        case CMP_FORMAT_ETC2_SRGB:               return CT_ETC2_SRGB;
        case CMP_FORMAT_ETC2_RGBA:               return CT_ETC2_RGBA;
        case CMP_FORMAT_ETC2_RGBA1:              return CT_ETC2_RGBA1;
        case CMP_FORMAT_ETC2_SRGBA:              return CT_ETC2_SRGBA;
        case CMP_FORMAT_ETC2_SRGBA1:             return CT_ETC2_SRGBA1;
        case CMP_FORMAT_GTC:                     return CT_GTC;
#ifdef USE_BASIS
        case CMP_FORMAT_BASIS:                  return CT_BASIS;
#endif
        default:                                return CT_Unknown;
    }
}
#ifdef ENABLE_MAKE_COMPATIBLE_API
bool IsFloatFormat(CMP_FORMAT InFormat)
{
    switch (InFormat)
    {
    case CMP_FORMAT_ARGB_16F:
    case CMP_FORMAT_ABGR_16F:
    case CMP_FORMAT_RGBA_16F:
    case CMP_FORMAT_BGRA_16F:
    case CMP_FORMAT_RG_16F:
    case CMP_FORMAT_R_16F:
    case CMP_FORMAT_ARGB_32F:
    case CMP_FORMAT_ABGR_32F:
    case CMP_FORMAT_RGBA_32F:
    case CMP_FORMAT_BGRA_32F:
    case CMP_FORMAT_RGB_32F:
    case CMP_FORMAT_BGR_32F:
    case CMP_FORMAT_RG_32F:
    case CMP_FORMAT_R_32F:
    case CMP_FORMAT_BC6H:
    case CMP_FORMAT_BC6H_SF:
    case CMP_FORMAT_RGBE_32F:
    {
        return true;
    }
    break;
    default:
        break;
    }

    return false;
}

bool NeedSwizzle(CMP_FORMAT destformat)
{
    // determin of the swizzle flag needs to be turned on!
    switch (destformat)
    {
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_ATI1N:        // same as BC4    
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_ATI2N:       // Green & Red Channels
    case CMP_FORMAT_ATI2N_XY:    // same as ATI2N  with XY = Red & Green channels
    case CMP_FORMAT_ATI2N_DXT5:  // same as BC5
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:        // same as BC1     
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_DXT3:        // same as BC2     
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_DXT5:        // same as BC3     
    case CMP_FORMAT_ATC_RGB:
    case CMP_FORMAT_ATC_RGBA_Explicit:
    case CMP_FORMAT_ATC_RGBA_Interpolated:
        return true;
        break;
    default:
        break;
    }

    return false;
}

inline float clamp(float a, float l, float h)
{
    return (a < l) ? l : ((a > h) ? h : a);
}

inline float knee(double x, double f)
{
    return float(log(x * f + 1.f) / f);
}

float findKneeValue(float x, float y)
{
    float f0 = 0;
    float f1 = 1.f;

    while (knee(x, f1) > y) {
        f0 = f1;
        f1 = f1 * 2.f;
    }

    for (int i = 0; i < 30; ++i) {
        const float f2 = (f0 + f1) / 2.f;
        const float y2 = knee(x, f2);

        if (y2 < y) {
            f1 = f2;
        }
        else {
            f0 = f2;
        }
    }

    return (f0 + f1) / 2.f;
}

CMP_ERROR Byte2HalfShort(CMP_HALFSHORT* hfsBlock, CMP_BYTE* cBlock, CMP_DWORD dwcBlockSize)
{
    assert(hfsBlock);
    assert(cBlock);
    assert(dwcBlockSize);
    if (hfsBlock && cBlock && dwcBlockSize)
    {
        for (CMP_DWORD i = 0; i < dwcBlockSize; i++)
        {
            hfsBlock[i] = CMP_HALF(float(cBlock[i] / 255.0f)).bits();
        }
    }

    return CMP_OK;
}


CMP_ERROR Float2Byte(CMP_BYTE cBlock[], CMP_FLOAT* fBlock, CMP_Texture* srcTexture, CMP_FORMAT destFormat, const CMP_CompressOptions* pOptions)
{
    assert(cBlock);
    assert(fBlock);
    assert(&srcTexture);

    if (cBlock && fBlock)
    {
        CMP_HALF* hfData = (CMP_HALF*)fBlock;
        float r = 0, g = 0, b = 0, a = 0;

        float kl = powf(2.f, pOptions->fInputKneeLow);
        float f = findKneeValue(powf(2.f, pOptions->fInputKneeHigh) - kl, powf(2.f, 3.5f) - kl);
        float luminance3f = powf(2, -3.5);         // always assume max intensity is 1 and 3.5f darker for scale later
        float invGamma = 1/ pOptions->fInputGamma; //for gamma correction
        float scale = (float)255.0 * powf(luminance3f, invGamma);
        int i = 0;
        bool needSwizzle = false; // NeedSwizzle(destFormat); Bug Fix This is no longer needed for <=BC5 and rel
        for (unsigned int y = 0; y < srcTexture->dwHeight; y++) {
            for (unsigned int x = 0; x < srcTexture->dwWidth; x++) {
                if (srcTexture->format == CMP_FORMAT_ARGB_16F) {
                    if (needSwizzle) {
                        b = (float)(*hfData);
                        hfData++;
                        g = (float)(*hfData);
                        hfData++;
                        r = (float)(*hfData);
                        hfData++;
                        a = (float)(*hfData);
                        hfData++;
                    }
                    else 
                    {
                        r = (float)(*hfData);
                        hfData++;
                        g = (float)(*hfData);
                        hfData++;
                        b = (float)(*hfData);
                        hfData++;
                        a = (float)(*hfData);
                        hfData++;
                    }
                }
                else if (srcTexture->format == CMP_FORMAT_ARGB_32F) {
                    if (needSwizzle) {
                        b = (float)(*fBlock);
                        fBlock++;
                        g = (float)(*fBlock);
                        fBlock++;
                        r = (float)(*fBlock);
                        fBlock++;
                        a = (float)(*fBlock);
                        fBlock++;
                    }
                    else
                    {
                        r = (float)(*fBlock);
                        fBlock++;
                        g = (float)(*fBlock);
                        fBlock++;
                        b = (float)(*fBlock);
                        fBlock++;
                        a = (float)(*fBlock);
                        fBlock++;
                    }
                }

                CMP_BYTE r_b, g_b, b_b, a_b;


                //  1) Compensate for fogging by subtracting defog
                //     from the raw pixel values.
                // We assume a defog of 0
                if (pOptions->fInputDefog > 0.0)
                {
                    r = r - pOptions->fInputDefog;
                    g = g - pOptions->fInputDefog;
                    b = b - pOptions->fInputDefog;
                    a = a - pOptions->fInputDefog;
                }

                //  2) Multiply the defogged pixel values by
                //     2^(exposure + 2.47393).
                const float exposeScale = powf(2, pOptions->fInputExposure + 2.47393f);
                r = r * exposeScale;
                g = g * exposeScale;
                b = b * exposeScale;
                a = a * exposeScale;

                //  3) Values that are now 1.0 are called "middle gray".
                //     If defog and exposure are both set to 0.0, then
                //     middle gray corresponds to a raw pixel value of 0.18.
                //     In step 6, middle gray values will be mapped to an
                //     intensity 3.5 f-stops below the display's maximum
                //     intensity.

                //  4) Apply a knee function.  The knee function has two
                //     parameters, kneeLow and kneeHigh.  Pixel values
                //     below 2^kneeLow are not changed by the knee
                //     function.  Pixel values above kneeLow are lowered
                //     according to a logarithmic curve, such that the
                //     value 2^kneeHigh is mapped to 2^3.5.  (In step 6,
                //     this value will be mapped to the the display's
                //     maximum intensity.)
                if (r > kl) {
                    r = kl + knee(r - kl, f);
                }
                if (g > kl) {
                    g = kl + knee(g - kl, f);
                }
                if (b > kl) {
                    b = kl + knee(b - kl, f);
                }
                if (a > kl) {
                    a = kl + knee(a - kl, f);
                }

                //  5) Gamma-correct the pixel values, according to the
                //     screen's gamma.  (We assume that the gamma curve
                //     is a simple power function.)
                r = powf(r, invGamma);
                g = powf(g, invGamma);
                b = powf(b, invGamma);
                a = powf(a, pOptions->fInputGamma);

                //  6) Scale the values such that middle gray pixels are
                //     mapped to a frame buffer value that is 3.5 f-stops
                //     below the display's maximum intensity. 
                r *= scale;
                g *= scale;
                b *= scale;
                a *= scale;

                r_b = (CMP_BYTE)clamp(r, 0.f, 255.f);
                g_b = (CMP_BYTE)clamp(g, 0.f, 255.f);
                b_b = (CMP_BYTE)clamp(b, 0.f, 255.f);
                a_b = (CMP_BYTE)clamp(a, 0.f, 255.f);
                cBlock[i] = r_b;
                i++;
                cBlock[i] = g_b;
                i++;
                cBlock[i] = b_b;
                i++;
                cBlock[i] = a_b;
                i++;
            }

        }

    }

    return CMP_OK;
}
#endif
CMP_ERROR GetError(CodecError err)
{
    switch(err)
    {
        case CE_OK: return CMP_OK;
        case CE_Aborted: return CMP_ABORTED;
        case CE_Unknown: return CMP_ERR_GENERIC;
        default: return CMP_ERR_GENERIC;
    }
}

CMP_ERROR CheckTexture(const CMP_Texture* pTexture, bool bSource)
{
    assert(pTexture);
    if(pTexture == NULL)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    assert(pTexture->dwSize == sizeof(CMP_Texture));
    if(pTexture->dwSize != sizeof(CMP_Texture))
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    assert(pTexture->dwWidth > 0);
    if(pTexture->dwWidth <= 0 )
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    assert(pTexture->dwHeight > 0);
    if(pTexture->dwHeight <= 0 )
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    assert(pTexture->format >= CMP_FORMAT_ARGB_8888 && pTexture->format <= CMP_FORMAT_MAX);
    if(pTexture->format < CMP_FORMAT_ARGB_8888 || pTexture->format > CMP_FORMAT_MAX)
        return (bSource ? CMP_ERR_UNSUPPORTED_SOURCE_FORMAT : CMP_ERR_UNSUPPORTED_DEST_FORMAT);

    assert((pTexture->format != CMP_FORMAT_ARGB_8888 && pTexture->format != CMP_FORMAT_ARGB_2101010)
        || pTexture->dwPitch == 0 || pTexture->dwPitch >= (pTexture->dwWidth*4));
    if((pTexture->format == CMP_FORMAT_ARGB_8888 || pTexture->format == CMP_FORMAT_ARGB_2101010)
        && pTexture->dwPitch != 0 && pTexture->dwPitch < (pTexture->dwWidth*4))
        return (bSource ? CMP_ERR_UNSUPPORTED_SOURCE_FORMAT : CMP_ERR_UNSUPPORTED_DEST_FORMAT);

    assert(pTexture->pData);
    if(pTexture->pData == NULL)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    CMP_DWORD dwDataSize = CMP_CalculateBufferSize(pTexture);
    assert(pTexture->dwDataSize >= dwDataSize);
    if(pTexture->dwDataSize < dwDataSize)
        return (bSource ? CMP_ERR_INVALID_SOURCE_TEXTURE : CMP_ERR_INVALID_DEST_TEXTURE);

    return CMP_OK;
}

CMP_ERROR CompressTexture(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture, const CMP_CompressOptions* pOptions, CMP_Feedback_Proc pFeedbackProc, CodecType destType)
{
    // Compressing
    CCodec* pCodec = CreateCodec(destType);
    assert(pCodec);
    if(pCodec == NULL)
        return CMP_ERR_UNABLE_TO_INIT_CODEC;

    CMP_BOOL swizzleSrcBuffer = false;

    // Have we got valid options ?
    if(pOptions && pOptions->dwSize == sizeof(CMP_CompressOptions))
    {
        // Set weightings ?
        if(pOptions->bUseChannelWeighting && (pOptions->fWeightingRed > 0.0 || pOptions->fWeightingGreen > 0.0 || pOptions->fWeightingBlue > 0.0))
        {
            pCodec->SetParameter("UseChannelWeighting", (CMP_DWORD) 1);
            pCodec->SetParameter("WeightR",
                pOptions->fWeightingRed > MINIMUM_WEIGHT_VALUE ?
                (CODECFLOAT) pOptions->fWeightingRed : MINIMUM_WEIGHT_VALUE);
            pCodec->SetParameter("WeightG",
                pOptions->fWeightingGreen > MINIMUM_WEIGHT_VALUE ?
                (CODECFLOAT) pOptions->fWeightingGreen : MINIMUM_WEIGHT_VALUE);
            pCodec->SetParameter("WeightB",
                pOptions->fWeightingBlue > MINIMUM_WEIGHT_VALUE ?
                (CODECFLOAT) pOptions->fWeightingBlue : MINIMUM_WEIGHT_VALUE);
        }
        pCodec->SetParameter("UseAdaptiveWeighting", (CMP_DWORD) pOptions->bUseAdaptiveWeighting);
        pCodec->SetParameter("DXT1UseAlpha", (CMP_DWORD) pOptions->bDXT1UseAlpha);
        pCodec->SetParameter("AlphaThreshold", (CMP_DWORD) pOptions->nAlphaThreshold);
        // New override to that set quality if compresion for DXTn & ATInN codecs
        if (pOptions->fquality != AMD_CODEC_QUALITY_DEFAULT)
        {
#ifndef _WIN64
            if (pOptions->fquality < 0.3)
                pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_SuperFast);
            else
                if (pOptions->fquality < 0.6)
                    pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_Fast);
                else
#endif
                    pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_Normal);
        }
        else
            pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)pOptions->nCompressionSpeed);


        switch(destType)
        {
        case CT_BC7:
                pCodec->SetParameter("MultiThreading", (CMP_DWORD) !pOptions->bDisableMultiThreading);
                
                if (!pOptions->bDisableMultiThreading)
                    pCodec->SetParameter("NumThreads", (CMP_DWORD) pOptions->dwnumThreads);
                else
                    pCodec->SetParameter("NumThreads", (CMP_DWORD) 1);

                pCodec->SetParameter("ModeMask", (CMP_DWORD) pOptions->dwmodeMask);
                pCodec->SetParameter("ColourRestrict", (CMP_DWORD) pOptions->brestrictColour);
                pCodec->SetParameter("AlphaRestrict", (CMP_DWORD) pOptions->brestrictAlpha);
                pCodec->SetParameter("Quality", (CODECFLOAT) pOptions->fquality);
                break;
#ifdef USE_BASIS
        case CT_BASIS:
#endif
        case CT_ASTC:
                pCodec->SetParameter("Quality", (CODECFLOAT)pOptions->fquality);
                if (!pOptions->bDisableMultiThreading)
                    pCodec->SetParameter("NumThreads", (CMP_DWORD)pOptions->dwnumThreads);
                else
                    pCodec->SetParameter("NumThreads", (CMP_DWORD)1);
                break;
        case CT_GTC:
        case CT_BC6H:
        case CT_BC6H_SF:
                pCodec->SetParameter("Quality", (CODECFLOAT)pOptions->fquality);
                if (!pOptions->bDisableMultiThreading)
                    pCodec->SetParameter("NumThreads", (CMP_DWORD)pOptions->dwnumThreads);
                else
                    pCodec->SetParameter("NumThreads", (CMP_DWORD)1);
#ifdef _DEBUG
                // napatel : remove this after
                // pCodec->SetParameter("NumThreads", (CMP_DWORD)1);
#endif
                break;
        }

        // This will eventually replace the above code for setting codec options
        if (pOptions->NumCmds > 0)
        {
            int maxCmds=pOptions->NumCmds;
            if (pOptions->NumCmds > AMD_MAX_CMDS) maxCmds = AMD_MAX_CMDS;
            for (int i=0; i<maxCmds; i++)
                pCodec->SetParameter(pOptions->CmdSet[i].strCommand, (CMP_CHAR*)pOptions->CmdSet[i].strParameter);
        }

        // GPUOpen issue # 59 fix
        CodecBufferType srcBufferType = GetCodecBufferType(pSourceTexture->format);
        if (NeedSwizzle(pDestTexture->format))
        {
            switch (srcBufferType)
            {
            case CBT_BGRA8888:
            case CBT_BGR888:
                swizzleSrcBuffer = false;
                break;
            default:
                swizzleSrcBuffer = true;
                break;
            }
        }
    }

    CodecBufferType srcBufferType = GetCodecBufferType(pSourceTexture->format);

    CCodecBuffer* pSrcBuffer  = CreateCodecBuffer(srcBufferType, 
                                                  pSourceTexture->nBlockWidth, pSourceTexture->nBlockHeight, pSourceTexture->nBlockDepth,
                                                  pSourceTexture->dwWidth, pSourceTexture->dwHeight, pSourceTexture->dwPitch, pSourceTexture->pData,
                                                  pSourceTexture->dwDataSize);
    CCodecBuffer* pDestBuffer = pCodec->CreateBuffer(
                                                  pDestTexture->nBlockWidth, pDestTexture->nBlockHeight, pDestTexture->nBlockDepth,
                                                  pDestTexture->dwWidth, pDestTexture->dwHeight, pDestTexture->dwPitch, pDestTexture->pData,
                                                  pDestTexture->dwDataSize);

    assert(pSrcBuffer);
    assert(pDestBuffer);
    if(pSrcBuffer == NULL || pDestBuffer == NULL)
    {
        SAFE_DELETE(pCodec);
        SAFE_DELETE(pSrcBuffer);
        SAFE_DELETE(pDestBuffer);
        return CMP_ERR_GENERIC;
    }

    // GPUOpen issue # 59 and #67 fix
    pSrcBuffer->m_bSwizzle = swizzleSrcBuffer;
    DISABLE_FP_EXCEPTIONS;
    CodecError err = pCodec->Compress(*pSrcBuffer, *pDestBuffer, pFeedbackProc);
    RESTORE_FP_EXCEPTIONS;

    SAFE_DELETE(pCodec);
    SAFE_DELETE(pSrcBuffer);
    SAFE_DELETE(pDestBuffer);

    return GetError(err);
}

#ifdef THREADED_COMPRESS

class CATICompressThreadData
{
public:
    CATICompressThreadData();
    ~CATICompressThreadData();

    CCodec* m_pCodec;
    CCodecBuffer* m_pSrcBuffer;
    CCodecBuffer* m_pDestBuffer;
    CMP_Feedback_Proc m_pFeedbackProc;
    CodecError m_errorCode;
};

CATICompressThreadData::CATICompressThreadData() : m_pCodec(NULL), m_pSrcBuffer(NULL), m_pDestBuffer(NULL), 
                                                   m_pFeedbackProc(NULL),
                                                   m_errorCode( CE_OK )
{
}

CATICompressThreadData::~CATICompressThreadData()
{
    SAFE_DELETE(m_pCodec);
    SAFE_DELETE(m_pSrcBuffer);
    SAFE_DELETE(m_pDestBuffer);
}

void ThreadedCompressProc(void *lpParameter)
{
    CATICompressThreadData *pThreadData = (CATICompressThreadData*) lpParameter;
    DISABLE_FP_EXCEPTIONS;
    CodecError err = pThreadData->m_pCodec->Compress(*pThreadData->m_pSrcBuffer, *pThreadData->m_pDestBuffer, pThreadData->m_pFeedbackProc);
    RESTORE_FP_EXCEPTIONS;
    pThreadData->m_errorCode = err;
}

CMP_ERROR ThreadedCompressTexture(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture, const CMP_CompressOptions* pOptions, CMP_Feedback_Proc pFeedbackProc, CodecType destType)
{
    // Note function should not be called for the following Codecs....
    if (destType == CT_BC7)     return CMP_ABORTED; 
    if (destType == CT_GTC)     return CMP_ABORTED;
#ifdef USE_BASIS
    if (destType == CT_BASIS)   return CMP_ABORTED;
#endif
    if (destType == CT_ASTC)  return CMP_ABORTED;

    CMP_DWORD dwMaxThreadCount = min(f_dwProcessorCount, MAX_THREADS);
    CMP_DWORD dwLinesRemaining = pDestTexture->dwHeight;
    CMP_BYTE* pSourceData = pSourceTexture->pData;
    CMP_BYTE* pDestData = pDestTexture->pData;
    CMP_BOOL swizzleSrcBuffer = false;

#ifdef _DEBUG
    if (
        (pDestTexture->format == CMP_FORMAT_ETC2_RGBA) ||
        (pDestTexture->format == CMP_FORMAT_ETC2_RGBA1)
        )
         dwMaxThreadCount = 1;
#endif

    CATICompressThreadData aThreadData[MAX_THREADS];
    std::thread ahThread[MAX_THREADS];

    CMP_DWORD dwThreadCount = 0;
    for(CMP_DWORD dwThread = 0; dwThread < dwMaxThreadCount; dwThread++)
    {
        CATICompressThreadData& threadData = aThreadData[dwThread];

        // Compressing
        threadData.m_pCodec = CreateCodec(destType);
        assert(threadData.m_pCodec);
        if(threadData.m_pCodec == NULL)
            return CMP_ERR_UNABLE_TO_INIT_CODEC;

        // Have we got valid options ?
        if(pOptions && pOptions->dwSize == sizeof(CMP_CompressOptions))
        {
            // Set weightings ?
            if(pOptions->bUseChannelWeighting && (pOptions->fWeightingRed > 0.0 || pOptions->fWeightingGreen > 0.0 || pOptions->fWeightingBlue > 0.0))
            {
                threadData.m_pCodec->SetParameter("UseChannelWeighting", (CMP_DWORD) 1);
                threadData.m_pCodec->SetParameter("WeightR",
                    pOptions->fWeightingRed > MINIMUM_WEIGHT_VALUE ?
                    (CODECFLOAT) pOptions->fWeightingRed : MINIMUM_WEIGHT_VALUE);
                threadData.m_pCodec->SetParameter("WeightG",
                    pOptions->fWeightingGreen > MINIMUM_WEIGHT_VALUE ?
                    (CODECFLOAT) pOptions->fWeightingGreen : MINIMUM_WEIGHT_VALUE);
                threadData.m_pCodec->SetParameter("WeightB",
                    pOptions->fWeightingBlue > MINIMUM_WEIGHT_VALUE ?
                    (CODECFLOAT) pOptions->fWeightingBlue : MINIMUM_WEIGHT_VALUE);
            }
            threadData.m_pCodec->SetParameter("UseAdaptiveWeighting", (CMP_DWORD) pOptions->bUseAdaptiveWeighting);
            threadData.m_pCodec->SetParameter("DXT1UseAlpha", (CMP_DWORD) pOptions->bDXT1UseAlpha);
            threadData.m_pCodec->SetParameter("AlphaThreshold", (CMP_DWORD) pOptions->nAlphaThreshold);

            // New override to that set quality if compresion for DXTn & ATInN codecs
            if (pOptions->fquality != AMD_CODEC_QUALITY_DEFAULT)
            {
                if (pOptions->fquality < 0.3)
                    threadData.m_pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_SuperFast);
                else
                    if (pOptions->fquality < 0.6)
                        threadData.m_pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_Fast);
                    else
                        threadData.m_pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)CMP_Speed_Normal);
            }
            else
                threadData.m_pCodec->SetParameter("CompressionSpeed", (CMP_DWORD)pOptions->nCompressionSpeed);



            switch(destType)
            {
            case CT_BC6H: 
                    // Reserved 
                    break;
            }

            // This will eventually replace the above code for setting codec options
            // It is currently implemented with BC6H and can be expanded to other codec
            if (pOptions->NumCmds > 0)
            {
                int maxCmds = pOptions->NumCmds;
                if (pOptions->NumCmds > AMD_MAX_CMDS) maxCmds = AMD_MAX_CMDS;
                for (int i = 0; i<maxCmds; i++)
                    threadData.m_pCodec->SetParameter(pOptions->CmdSet[i].strCommand, (CMP_CHAR*)pOptions->CmdSet[i].strParameter);
            }


        }

        CodecBufferType srcBufferType = GetCodecBufferType(pSourceTexture->format);

        // GPUOpen issue # 59 fix
        if (NeedSwizzle(pDestTexture->format))
        {
            switch (srcBufferType)
            {
            case CBT_BGRA8888:
            case CBT_BGR888:
                swizzleSrcBuffer = false;
                break;
            default:
                swizzleSrcBuffer = true;
                break;
            }
        }

        CMP_DWORD dwThreadsRemaining = dwMaxThreadCount - dwThread;
        CMP_DWORD dwHeight = 0;
        if(dwThreadsRemaining > 1)
        {
            CMP_DWORD dwBlockHeight = threadData.m_pCodec->GetBlockHeight();
            dwHeight = dwLinesRemaining / dwThreadsRemaining;
            dwHeight = min(((dwHeight + dwBlockHeight - 1) / dwBlockHeight) * dwBlockHeight, dwLinesRemaining); // Round by block height
            dwLinesRemaining -= dwHeight;
        }
        else
            dwHeight = dwLinesRemaining;

        if(dwHeight > 0)
        {
            threadData.m_pSrcBuffer = CreateCodecBuffer(srcBufferType, 
                                                        pSourceTexture->nBlockWidth, pSourceTexture->nBlockHeight, pSourceTexture->nBlockDepth,
                                                        pSourceTexture->dwWidth, dwHeight, pSourceTexture->dwPitch, pSourceData,
                                                        pSourceTexture->dwDataSize);
            threadData.m_pDestBuffer = threadData.m_pCodec->CreateBuffer(
                                                        pDestTexture->nBlockWidth, pDestTexture->nBlockHeight, pDestTexture->nBlockDepth,
                                                        pDestTexture->dwWidth, dwHeight, pDestTexture->dwPitch, pDestData,
                                                        pDestTexture->dwDataSize);

            pSourceData += CalcBufferSize(pSourceTexture->format, pSourceTexture->dwWidth, dwHeight, pSourceTexture->dwPitch, pSourceTexture->nBlockWidth, pSourceTexture->nBlockHeight);
            pDestData += CalcBufferSize(destType, pDestTexture->dwWidth, dwHeight, pDestTexture->nBlockWidth, pDestTexture->nBlockHeight);

            assert(threadData.m_pSrcBuffer);
            assert(threadData.m_pDestBuffer);
            if(threadData.m_pSrcBuffer == NULL || threadData.m_pDestBuffer == NULL)
                return CMP_ERR_GENERIC;

            threadData.m_pSrcBuffer->m_bSwizzle = swizzleSrcBuffer;
            threadData.m_pFeedbackProc = pFeedbackProc;

            ahThread[dwThreadCount++] = std::thread(ThreadedCompressProc, &threadData);
        }
    }

    for ( CMP_DWORD dwThread = 0; dwThread < dwThreadCount; dwThread++ )
    {
        std::thread& curThread = ahThread[dwThread];

        curThread.join();
    }

    CodecError err = CE_OK;
    for(CMP_DWORD dwThread = 0; dwThread < dwThreadCount; dwThread++)
    {
        CATICompressThreadData& threadData = aThreadData[dwThread];

        if(err == CE_OK)
            err = threadData.m_errorCode;

        ahThread[dwThread] = std::thread();
    }

    return GetError(err);
}
#endif // THREADED_COMPRESS

