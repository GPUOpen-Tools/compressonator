//===============================================================================
// Copyright (c) 2007-2024  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   Codec.h
//  Description: interface for the CCodec class
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _CODEC_H_INCLUDED_
#define _CODEC_H_INCLUDED_

#include "codec_common.h"
#include "codecbuffer.h"

#define SAFE_DELETE(p) \
    if (p)             \
    {                  \
        delete p;      \
        p = NULL;      \
    }

typedef float CODECFLOAT;

namespace AMD_Compress
{

struct CodecParameters
{
    // TODO: add all the other parameters here
    static const CMP_CHAR* NumThreads;           // the number of CPU threads to use during compression/decompression
    static const CMP_CHAR* UseGPUDecompression;  // boolean parameter to indicate whether to use GPU-accelerated decompression
    static const CMP_CHAR* PageSize;             // the page size to use in compression using the Brotli-G codec
    static const CMP_CHAR* TextureWidth;
    static const CMP_CHAR* TextureHeight;
    static const CMP_CHAR* TextureFormat;
    static const CMP_CHAR* MipmapLevels;
    static const CMP_CHAR* Precondition;
    static const CMP_CHAR* Swizzle;
    static const CMP_CHAR* DeltaEncode;
};

class CCodec
{
public:
    CCodec(CodecType codecType);
    virtual ~CCodec();

    virtual bool SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue);
    virtual bool GetParameter(const CMP_CHAR* pszParamName, CMP_DWORD& dwValue);

    virtual bool SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue);
    virtual bool GetParameter(const CMP_CHAR* pszParamName, CODECFLOAT& fValue);

    virtual bool SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* dwValue);

    virtual CodecType GetType() const
    {
        return m_CodecType;
    };

    virtual CMP_DWORD GetBlockHeight()
    {
        return 1;
    };

    virtual CCodecBuffer* CreateBuffer(CMP_BYTE  nBlockWidth,
                                       CMP_BYTE  nBlockHeight,
                                       CMP_BYTE  nBlockDepth,
                                       CMP_DWORD dwWidth,
                                       CMP_DWORD dwHeight,
                                       CMP_DWORD dwPitch    = 0,
                                       CMP_BYTE* pData      = 0,
                                       CMP_DWORD dwDataSize = 0) const = 0;

    virtual CodecError Compress(CCodecBuffer&       bufferIn,
                                CCodecBuffer&       bufferOut,
                                Codec_Feedback_Proc pFeedbackProc = NULL,
                                CMP_DWORD_PTR       pUser1        = NULL,
                                CMP_DWORD_PTR       pUser2        = NULL)   = 0;
    virtual CodecError Decompress(CCodecBuffer&       bufferIn,
                                  CCodecBuffer&       bufferOut,
                                  Codec_Feedback_Proc pFeedbackProc = NULL,
                                  CMP_DWORD_PTR       pUser1        = NULL,
                                  CMP_DWORD_PTR       pUser2        = NULL) = 0;

protected:
    CodecType m_CodecType;
};

}  // namespace AMD_Compress

using namespace AMD_Compress;

bool SupportsSSE();
bool SupportsSSE2();

CCodec* CreateCodec(CodecType nCodecType);

#endif  // !defined(_CODEC_H_INCLUDED_)
