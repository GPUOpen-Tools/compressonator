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
//  File Name:   Codec_Block_8x8.h
//  Description: interface for the CCodec_Block_8x8 class
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _CODEC_BLOCK_8x8_H_INCLUDED_
#define _CODEC_BLOCK_8x8_H_INCLUDED_

#include "Codec_Block.h"

class CCodec_Block_8x8 : public CCodec_Block  
{
public:
    CCodec_Block_8x8(CodecType codecType);
    virtual ~CCodec_Block_8x8();

    virtual CCodecBuffer* CreateBuffer(
        CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth, 
        CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch = 0, CMP_BYTE* pData = 0,CMP_DWORD dwDataSize = 0) const;

    virtual CMP_DWORD GetBlockHeight() {return 4;};
};

#endif // !defined(_CODEC_BLOCK_8x8_H_INCLUDED_)
