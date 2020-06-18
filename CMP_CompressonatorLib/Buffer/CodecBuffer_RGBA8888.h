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
//  File Name:   CodecBuffer_RGBA8888.h
//  Description: interface for the CCodecBuffer_RGBA8888 class
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _CODECBUFFER_RGBA8888_H_INCLUDED_
#define _CODECBUFFER_RGBA8888_H_INCLUDED_

#include "CodecBuffer.h"

class CCodecBuffer_RGBA8888 : public CCodecBuffer  
{
public:
    CCodecBuffer_RGBA8888(
        CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
        CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch = 0, CMP_BYTE* pData = 0,CMP_DWORD dwDataSize = 0);
    virtual ~CCodecBuffer_RGBA8888();

    virtual void Copy(CCodecBuffer& srcBuffer);

    virtual CodecBufferType GetBufferType() const {return CBT_RGBA8888;};
    virtual CMP_DWORD GetChannelDepth() const {return 8;};
    virtual CMP_DWORD GetChannelCount() const {return 4;};
    virtual bool IsFloat() const {return false;};

    virtual bool ReadBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]);
    virtual bool ReadBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]);
    virtual bool ReadBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]);
    virtual bool ReadBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]);

    virtual bool WriteBlockR(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]);
    virtual bool WriteBlockG(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]);
    virtual bool WriteBlockB(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]);
    virtual bool WriteBlockA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]);

    virtual bool ReadBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]);
    virtual bool WriteBlockRGBA(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[]);

protected:
    virtual bool ReadBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[], CMP_DWORD dwChannelOffset);
    virtual bool WriteBlock(CMP_DWORD x, CMP_DWORD y, CMP_BYTE w, CMP_BYTE h, CMP_BYTE block[], CMP_DWORD dwChannelOffset);
};

#endif // !defined(_CODECBUFFER_RGBA8888_H_INCLUDED_)
