//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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
/// \file ATIFormats.h
/// \version 2.21
//
//=====================================================================

#ifndef _AMDFORMATS_H_
#define _AMDFORMATS_H_

#include "Compressonator.h"
#include "Common.h"


typedef struct
{
    CMP_FORMAT nFormat;
    const CMP_CHAR* pszFormatDesc;
} CMP_FormatDesc; 

typedef struct
{
    CMP_TextureType nTextureType;
    CMP_CHAR*       pszTextureTypeDesc;
} CMP_TextureTypeDesc; 


#ifdef __cplusplus
extern "C" {
#endif
    CMP_FORMAT CMP_API CMP_ParseFormat(char* pFormat);
    void       CMP_API CMP_Format2FourCC(CMP_FORMAT format, MipSet *pMipSet);
#ifdef __cplusplus
};
#endif

const CMP_CHAR*  GetFormatDesc(CMP_FORMAT nFormat);
CMP_CHAR*  GetTextureTypeDesc(CMP_TextureType nTextureType);
#endif