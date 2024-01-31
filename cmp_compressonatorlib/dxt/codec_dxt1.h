//===============================================================================
// Copyright (c) 2007-2024  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   Codec_DXT1.h
//  Description: interface for the CCodec_DXT1 class
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _CODEC_DXT1_H_INCLUDED_
#define _CODEC_DXT1_H_INCLUDED_

#include "codec_common.h"
#include "codec_dxtc.h"

class CCodec_DXT1 : public CCodec_DXTC
{
public:
    CCodec_DXT1();
    virtual ~CCodec_DXT1();

    virtual bool SetParameter(const CMP_CHAR* /*pszParamName*/, CMP_CHAR* /*dwValue*/);
    virtual bool SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue);
    virtual bool GetParameter(const CMP_CHAR* pszParamName, CMP_DWORD& dwValue);

    virtual CodecError Compress(CCodecBuffer&       bufferIn,
                                CCodecBuffer&       bufferOut,
                                Codec_Feedback_Proc pFeedbackProc = NULL,
                                CMP_DWORD_PTR       pUser1        = NULL,
                                CMP_DWORD_PTR       pUser2        = NULL);
    virtual CodecError Compress_Fast(CCodecBuffer&       bufferIn,
                                     CCodecBuffer&       bufferOut,
                                     Codec_Feedback_Proc pFeedbackProc = NULL,
                                     CMP_DWORD_PTR       pUser1        = NULL,
                                     CMP_DWORD_PTR       pUser2        = NULL);
    virtual CodecError Compress_SuperFast(CCodecBuffer&       bufferIn,
                                          CCodecBuffer&       bufferOut,
                                          Codec_Feedback_Proc pFeedbackProc = NULL,
                                          CMP_DWORD_PTR       pUser1        = NULL,
                                          CMP_DWORD_PTR       pUser2        = NULL);
    virtual CodecError Decompress(CCodecBuffer&       bufferIn,
                                  CCodecBuffer&       bufferOut,
                                  Codec_Feedback_Proc pFeedbackProc = NULL,
                                  CMP_DWORD_PTR       pUser1        = NULL,
                                  CMP_DWORD_PTR       pUser2        = NULL);

    virtual CCodecBuffer* CreateBuffer(CMP_BYTE  nBlockWidth,
                                       CMP_BYTE  nBlockHeight,
                                       CMP_BYTE  nBlockDepth,
                                       CMP_DWORD dwWidth,
                                       CMP_DWORD dwHeight,
                                       CMP_DWORD dwPitch    = 0,
                                       CMP_BYTE* pData      = 0,
                                       CMP_DWORD dwDataSize = 0) const;

protected:
    bool     m_bDXT1UseAlpha;
    CMP_BYTE m_nAlphaThreshold;
};

#endif  // !defined(_CODEC_DXT1_H_INCLUDED_)
