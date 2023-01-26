//=====================================================================
// Copyright (c) 2018-2022    Advanced Micro Devices, Inc. All rights reserved.
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
#ifndef _BRLG_Encode_H
#define _BRLG_Encode_H

#if defined(ISPC)||defined(ASPM)
//#include "..\..\common\common_def.h"
#include "common_def.h"
#else
#include "common_def.h"
#endif

typedef struct
{
    // Original image size
    CGU_UINT32     m_srcSize;

    // Source block dimensions to compress
    CGU_UINT32     m_xdim;
    CGU_UINT32     m_ydim;

    // Compression quality to apply during compression
    float   m_quality;
} CMP_BRLGEncode;


CMP_STATIC void SetDefaultBRLGOptions(CMP_BRLGEncode* BRLGEncode)
{
    if (BRLGEncode)
    {
        BRLGEncode->m_srcSize  = 16;
        BRLGEncode->m_xdim      = 4;
        BRLGEncode->m_ydim      = 4;
        BRLGEncode->m_quality   = 1.0f;
    }
}

#endif
