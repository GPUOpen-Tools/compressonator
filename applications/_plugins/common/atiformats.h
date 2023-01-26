//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2021 (c), Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================

#ifndef _AMDFORMATS_H_
#define _AMDFORMATS_H_

#include "compressonator.h"
#include "common.h"


typedef struct {
    CMP_FORMAT nFormat;
    CMP_CHAR* pszFormatDesc;
} CMP_FormatDesc;

typedef struct {
    CMP_TextureType nTextureType;
    CMP_CHAR*       pszTextureTypeDesc;
} CMP_TextureTypeDesc;


#ifdef __cplusplus
extern "C" {
#endif
CMP_FORMAT CMP_API CMP_ParseFormat(char* pFormat);
void       CMP_API CMP_Format2FourCC(CMP_FORMAT format, MipSet *pMipSet);
CMP_FORMAT CMP_API CMP_FourCC2Format(CMP_DWORD fourCC);
CMP_BOOL   CMP_API CMP_IsCompressedFormat(CMP_FORMAT format);
CMP_BOOL   CMP_API CMP_IsFloatFormat(CMP_FORMAT InFormat);
CMP_BOOL   CMP_API CMP_IsHDR(CMP_FORMAT InFormat);
CMP_BOOL   CMP_API CMP_IsLossless(CMP_FORMAT InFormat);
CMP_BOOL CMP_API   CMP_IsValidFormat(CMP_FORMAT InFormat);
#ifdef __cplusplus
};
#endif

CMP_TextureType ParseTextureType(char* typeString);

CMP_CHAR*  GetFormatDesc(CMP_FORMAT nFormat);
CMP_CHAR*  GetTextureTypeDesc(CMP_TextureType nTextureType);

CMP_ChannelFormat GetChannelFormat(CMP_FORMAT format);
CMP_BYTE GetChannelFormatBitSize(CMP_FORMAT format);
// TODO: move GetChannelCount() from texture.cpp here

CMP_FORMAT GetFormat(MipSet* pMipSet);

#endif