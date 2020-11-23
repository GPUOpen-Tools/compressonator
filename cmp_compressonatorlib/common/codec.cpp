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
//  File Name:   Codec.cpp
//  Description: Implementation of the CCodec class
//
//////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "codec.h"
#include "codec_ati1n.h"
#include "codec_ati2n.h"
#include "codec_ati2n_dxt5.h"
#include "codec_dxt1.h"
#include "codec_dxt3.h"
#include "codec_dxt5.h"
#include "codec_dxt5_xgbr.h"
#include "codec_dxt5_rxbg.h"
#include "codec_dxt5_rbxg.h"
#include "codec_dxt5_xrbg.h"
#include "codec_dxt5_rgxb.h"
#include "codec_dxt5_xgxr.h"
#include "codec_atc_rgb.h"
#include "codec_atc_rgba_explicit.h"
#include "codec_atc_rgba_interpolated.h"
#include "codec_etc_rgb.h"
#include "codec_etc2_rgb.h"
#include "codec_etc2_rgba.h"
#include "codec_etc2_rgba1.h"
#include "codec_bc6h.h"
#include "codec_bc7.h"
#include "astc/codec_astc.h"

#ifdef _WIN32  //GT only enabled for win build now
#ifdef USE_APC
#include "codec_apc.h"
#endif
#ifdef USE_GTC
#include "codec_gt.h"
#endif
#ifdef USE_BASIS
#include "codec_basis.h"
#endif
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCodec::CCodec(CodecType codecType) {
    m_CodecType = codecType;
}

CCodec::~CCodec() {
}

bool CCodec::SetParameter(const CMP_CHAR* /*pszParamName*/, CMP_CHAR* /*dwValue*/) {
    return false;
}

bool CCodec::SetParameter(const CMP_CHAR* /*pszParamName*/, CMP_DWORD /*dwValue*/) {
    return false;
}

bool CCodec::GetParameter(const CMP_CHAR* /*pszParamName*/, CMP_DWORD& /*dwValue*/) {
    return false;
}

bool CCodec::SetParameter(const CMP_CHAR* /*pszParamName*/, CODECFLOAT /*fValue*/) {
    return false;
}

bool CCodec::GetParameter(const CMP_CHAR* /*pszParamName*/, CODECFLOAT& /*fValue*/) {
    return false;
}

#ifdef _WIN32

#include <intrin.h>

//  Windows
#define cpuid(info, x) __cpuidex(info, x, 0)

#else

//  GCC Intrinsics
#include <cpuid.h>
void cpuid(int info[4], int InfoType) {
    __cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]);
}

#endif

bool SupportsSSE() {
#if defined(USE_SSE)
    int info[4];
    cpuid(info, 0);

    int nIds = info[0];

    if (nIds >= 1) {
        return ((info[3] & ((int)1 << 25)) != 0);
    }
#endif
    return false;
}

bool SupportsSSE2() {
#if defined(USE_SSE2) && defined(_WIN32)
    int info[4];
    cpuid(info, 0);

    int nIds = info[0];

    if (nIds >= 1) {
        return ((info[3] & ((int)1 << 26)) != 0);
    }
#endif
    return false;
}

CCodec* CreateCodec(CodecType nCodecType) {
#ifdef USE_DBGTRACE
    DbgTrace(("nCodecType %d", nCodecType));
#endif
    switch (nCodecType) {
    case CT_DXT1:
        return new CCodec_DXT1;
    case CT_DXT3:
        return new CCodec_DXT3;
    case CT_DXT5:
        return new CCodec_DXT5;
    case CT_DXT5_xGBR:
        return new CCodec_DXT5_xGBR;
    case CT_DXT5_RxBG:
        return new CCodec_DXT5_RxBG;
    case CT_DXT5_RBxG:
        return new CCodec_DXT5_RBxG;
    case CT_DXT5_xRBG:
        return new CCodec_DXT5_xRBG;
    case CT_DXT5_RGxB:
        return new CCodec_DXT5_RGxB;
    case CT_DXT5_xGxR:
        return new CCodec_DXT5_xGxR;
    case CT_ATI1N:
        return new CCodec_ATI1N;
    case CT_ATI1N_S:
        return new CCodec_ATI1N_S;
    case CT_ATI2N:
        return new CCodec_ATI2N;
    case CT_ATI2N_S:
        return new CCodec_ATI2N_S;
    case CT_ATI2N_XY:
        return new CCodec_ATI2N(CT_ATI2N_XY);
    case CT_ATI2N_XY_S:
        return new CCodec_ATI2N_S(CT_ATI2N_XY_S);
    case CT_ATI2N_DXT5:
        return new CCodec_ATI2N_DXT5;
    case CT_ATC_RGB:
        return new CCodec_ATC_RGB;
    case CT_ATC_RGBA_Explicit:
        return new CCodec_ATC_RGBA_Explicit;
    case CT_ATC_RGBA_Interpolated:
        return new CCodec_ATC_RGBA_Interpolated;
    case CT_ETC_RGB:
        return new CCodec_ETC_RGB;
    case CT_ETC2_RGB:
    case CT_ETC2_SRGB:
        return new CCodec_ETC2_RGB(nCodecType);
    case CT_ETC2_RGBA:
    case CT_ETC2_SRGBA:
        return new CCodec_ETC2_RGBA(nCodecType);
    case CT_ETC2_RGBA1:
    case CT_ETC2_SRGBA1:
        return new CCodec_ETC2_RGBA1(nCodecType);
    case CT_BC6H:
    case CT_BC6H_SF:
        return new CCodec_BC6H(nCodecType);
    case CT_BC7:
        return new CCodec_BC7;
    case CT_ASTC:
        return new CCodec_ASTC;
#ifdef _WIN32
#ifdef USE_APC
    case CT_APC:
        return new CCodec_APC;
#endif
#ifdef USE_GTC
    case CT_GTC:
        return new CCodec_GTC;
#endif
#ifdef USE_BASIS
    case CT_BASIS:
        return new CCodec_BASIS;
#endif
#endif
    case CT_Unknown:
    default:
        assert(0);
        return NULL;
    }
}

CMP_DWORD CalcBufferSize(CodecType nCodecType, CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight) {
#ifdef USE_DBGTRACE
    DbgTrace(("IN: nCodecType %d, dwWidth %d, dwHeight %d", nCodecType, dwWidth, dwHeight));
#endif
    CMP_DWORD dwChannels;
    CMP_DWORD dwBitsPerChannel;
    CMP_DWORD buffsize = 0;

    switch (nCodecType) {
    // Block size is 4x4 and 64 bits per block
    case CT_DXT1:
    case CT_ATI1N:
    case CT_ATI1N_S:
    case CT_ATC_RGB:
    case CT_ETC_RGB:
    case CT_ETC2_RGB:
    case CT_ETC2_SRGB:
    case CT_ETC2_RGBA1:
    case CT_ETC2_SRGBA1:
        dwChannels       = 1;
        dwBitsPerChannel = 4;
        dwWidth          = ((dwWidth + 3) / 4) * 4;
        dwHeight         = ((dwHeight + 3) / 4) * 4;
        buffsize         = (dwWidth * dwHeight * dwChannels * dwBitsPerChannel) / 8;
        break;

    // Block size is 4x4 and 128 bits per block
    case CT_ETC2_RGBA:
    case CT_ETC2_SRGBA:
        // dwChannels = 2;
        // dwBitsPerChannel = 4;
        // dwWidth = ((dwWidth + 3) / 4) * 4;
        // dwHeight = ((dwHeight + 3) / 4) * 4;
        // buffsize = (dwWidth * dwHeight * dwChannels * dwBitsPerChannel) / 8;
        dwWidth = ((dwWidth + 3) / 4) * 4;
        dwHeight = ((dwHeight + 3) / 4) * 4;
        buffsize = dwWidth * dwHeight;
        if (buffsize < BC_BLOCK_PIXELS)
            buffsize = BC_BLOCK_PIXELS;
        break;

    // Block size is 4x4 and 128 bits per block
    case CT_DXT3:
    case CT_DXT5:
    case CT_DXT5_xGBR:
    case CT_DXT5_RxBG:
    case CT_DXT5_RBxG:
    case CT_DXT5_xRBG:
    case CT_DXT5_RGxB:
    case CT_DXT5_xGxR:
    case CT_ATI2N:
    case CT_ATI2N_S:
    case CT_ATI2N_XY:
    case CT_ATI2N_XY_S:
    case CT_ATI2N_DXT5:
    case CT_ATC_RGBA_Explicit:
    case CT_ATC_RGBA_Interpolated:
        dwChannels       = 2;
        dwBitsPerChannel = 4;
        dwWidth          = ((dwWidth + 3) / 4) * 4;
        dwHeight         = ((dwHeight + 3) / 4) * 4;
        buffsize         = (dwWidth * dwHeight * dwChannels * dwBitsPerChannel) / 8;
        break;

    // Block size is 4x4 and 128 bits per block
    case CT_BC6H:
    case CT_BC6H_SF:
        dwWidth  = ((dwWidth + 3) / 4) * 4;
        dwHeight = ((dwHeight + 3) / 4) * 4;
        buffsize = dwWidth * dwHeight;
        if (buffsize < BC6H_BLOCK_PIXELS)
            buffsize = BC6H_BLOCK_PIXELS;
        break;

    // Block size is 4x4 and 128 bits per block
    case CT_BC7:
        dwWidth  = ((dwWidth + 3) / 4) * 4;
        dwHeight = ((dwHeight + 3) / 4) * 4;
        buffsize = dwWidth * dwHeight;
        if (buffsize < BC7_BLOCK_PIXELS)
            buffsize = BC7_BLOCK_PIXELS;
        break;

    // Block size ranges from 4x4 to 12x12 and 128 bits per block
    case CT_ASTC:
        dwWidth  = ((dwWidth + nBlockWidth - 1) / nBlockWidth) * 4;
        dwHeight = ((dwHeight + nBlockHeight - 1) / nBlockHeight) * 4;
        buffsize = dwWidth * dwHeight;
        break;
#ifdef _WIN32
#ifdef USE_APC
    case CT_APC:
        if (nBlockWidth <= 0)
            nBlockWidth = 4;
        if (nBlockHeight <= 0)
            nBlockHeight = 4;
        dwWidth  = ((dwWidth + nBlockWidth - 1) / nBlockWidth) * 4;
        dwHeight = ((dwHeight + nBlockHeight - 1) / nBlockHeight) * 4;
        buffsize = dwWidth * dwHeight;
        break;
#endif
        // Block size is 4x4 and 128 bits per block. in future releases its will vary in Block Sizes and bits per block may change to 256
#ifdef USE_GTC
    case CT_GTC:
        dwWidth  = ((dwWidth + 3) / 4) * 4;
        dwHeight = ((dwHeight + 3) / 4) * 4;
        buffsize = dwWidth * dwHeight;
        if (buffsize < (4 * 4))
            buffsize = 4 * 4;
        break;
#endif
#ifdef USE_BASIS
    // Block size is 4x4 and 128 bits per block, needs conformation!!
    case CT_BASIS:
        dwWidth  = ((dwWidth + 3) / 4) * 4;
        dwHeight = ((dwHeight + 3) / 4) * 4;
        buffsize = dwWidth * dwHeight;
        if (buffsize < (4 * 4))
            buffsize = 4 * 4;
        break;
#endif
#endif
    default:
        return 0;
    }

#ifdef USE_DBGTRACE
    DbgTrace(("OUT: %d", buffsize));
#endif

    return buffsize;
}
