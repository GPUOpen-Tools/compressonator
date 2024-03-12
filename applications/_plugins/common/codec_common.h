//=====================================================================
// Copyright (c) 2023-2024, Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================

#ifndef CODEC_COMMON_H_
#define CODEC_COMMON_H_

#include "compressonator.h"

typedef enum _CodecType
{
    CT_Unknown = 0,
    CT_None,
    CT_DXT1,
    CT_DXT3,
    CT_DXT5,
    CT_DXT5_xGBR,
    CT_DXT5_RxBG,
    CT_DXT5_RBxG,
    CT_DXT5_xRBG,
    CT_DXT5_RGxB,
    CT_DXT5_xGxR,
    CT_ATI1N,
    CT_ATI1N_S,
    CT_ATI2N,
    CT_ATI2N_S,
    CT_ATI2N_XY,
    CT_ATI2N_XY_S,
    CT_ATI2N_DXT5,
    CT_ATC_RGB,
    CT_ATC_RGBA_Explicit,
    CT_ATC_RGBA_Interpolated,
    CT_ETC_RGB,
#ifdef SUPPORT_ETC_ALPHA
    CT_ETC_RGBA_Explicit,
    CT_ETC_RGBA_Interpolated,
#endif  // SUPPORT_ETC_ALPHA
    CT_ETC2_RGB,
    CT_ETC2_SRGB,
    CT_ETC2_RGBA,
    CT_ETC2_RGBA1,
    CT_ETC2_SRGBA,
    CT_ETC2_SRGBA1,
    CT_BC6H,
    CT_BC6H_SF,
    CT_BC7,
#if (OPTION_BUILD_ASTC == 1)
    CT_ASTC,
#endif
    CT_APC,
    CT_GTC,
#ifdef USE_BASIS
    CT_BASIS,
#endif
    CT_BRLG,
    CODECS_AMD_INTERNAL
} CodecType;

typedef enum _CODECError
{
    CE_OK = 0,
    CE_Unknown,
    CE_Aborted,
} CodecError;

CodecType GetCodecType(CMP_FORMAT format);
CMP_DWORD CalcBufferSize(CodecType nCodecType, CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight);

#endif