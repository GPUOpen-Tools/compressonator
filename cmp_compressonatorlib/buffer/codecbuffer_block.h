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
//  File Name:   CodecBuffer_Block.h
//  Description: interface for the CCodecBuffer_Block class
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _CODECBUFFER_BLOCK_H_INCLUDED_
#define _CODECBUFFER_BLOCK_H_INCLUDED_

#include "codecbuffer.h"

class CCodecBuffer_Block : public CCodecBuffer {
  public:
    CCodecBuffer_Block( CodecBufferType nCodecBufferType,
                        CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
                        CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch = 0, CMP_BYTE* pData = 0,CMP_DWORD dwDataSize = 0);

    virtual ~CCodecBuffer_Block();

    virtual CodecBufferType GetBufferType() const {
        return m_nCodecBufferType;
    };
    virtual CMP_DWORD GetChannelDepth() const {
        return 0;
    };
    virtual CMP_DWORD GetChannelCount() const {
        return 0;
    };
    virtual bool IsFloat() const {
        return false;
    };

    virtual bool ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_DWORD* pBlock, CMP_DWORD dwBlockSize);
    virtual bool WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_DWORD* pBlock, CMP_DWORD dwBlockSize);

  protected:
    CodecBufferType m_nCodecBufferType;
    CMP_DWORD m_dwBlockSize;
    CMP_DWORD m_dwBlockWidth;
    CMP_DWORD m_dwBlockHeight;
    CMP_DWORD m_dwBlockBPP;
};

#endif // !defined(_CODECBUFFER_BLOCK_H_INCLUDED_)
