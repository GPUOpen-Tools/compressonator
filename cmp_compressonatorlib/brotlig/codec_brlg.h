//===============================================================================
// Copyright (c) 2014-2018  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   Codec_brlg.h
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _CODEC_BRLG_H_INCLUDED_
#define _CODEC_BRLG_H_INCLUDED_

#include "compressonator.h"
#include "codec_dxtc.h"
#include "brlg_encode.h"
#include "brlg_decode.h"

#include <thread>

extern BRLG_Encode g_BRLGEncode;

struct BRLGEncodeThreadParam
{
    BRLGBlockEncoder* encoder;
    // Max storage buffer for blocks size (256) change this for proper window sizes used in Brotli-G
    CMP_BYTE             in[256];
    CMP_BYTE             *out;
    volatile CMP_BOOL    run;
    volatile CMP_BOOL    exit;
};

class CCodec_BRLG : public CCodec_DXTC
{
  public:
     CCodec_BRLG();
    ~CCodec_BRLG();

    virtual bool SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue);
    virtual bool SetParameter(const CMP_CHAR* /*pszParamName*/, CMP_DWORD /*dwValue*/);
    virtual bool SetParameter(const CMP_CHAR* /*pszParamName*/, CODECFLOAT /*fValue*/);

    // Required interfaces
    virtual CodecError Compress             (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, CMP_DWORD_PTR pUser1 = NULL, CMP_DWORD_PTR pUser2 = NULL);
    virtual CodecError Compress_Fast        (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, CMP_DWORD_PTR pUser1 = NULL, CMP_DWORD_PTR pUser2 = NULL);
    virtual CodecError Compress_SuperFast   (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, CMP_DWORD_PTR pUser1 = NULL, CMP_DWORD_PTR pUser2 = NULL);
    virtual CodecError Decompress           (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, CMP_DWORD_PTR pUser1 = NULL, CMP_DWORD_PTR pUser2 = NULL);


  private:

    BRLGEncodeThreadParam* m_EncodeParameterStorage;

    // source block dimentions (typically 4x4x1)
    int m_xdim;
    int m_ydim;

    // User configurable variables
    CMP_WORD    m_NumThreads;
    bool m_UseGPUDecompression;
    CMP_DWORD m_PageSize;

    //Internal status
    CMP_BOOL     m_LibraryInitialized;
    CMP_BOOL     m_Use_MultiThreading;
    CMP_INT      m_NumEncodingThreads;
    CMP_WORD     m_LiveThreads;
    CMP_WORD     m_LastThread;

    // Encoders and decoders: for encding use the interfaces below
    std::thread*         m_EncodingThreadHandle;
    BRLGBlockEncoder*    m_encoder[128]; // Max 128 threads? make this a definition or variable size
    BRLGBlockDecoder*    m_decoder;

    // Encoder interfaces
    CodecError    InitializeBRLGLibrary();
    CodecError    EncodeBRLGBlock(
        CMP_BYTE   *in, //[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],
        CMP_BYTE   *out
    );
    CodecError    FinishBRLGEncoding(void);
};

#endif // !defined(_CODEC_DXT5_H_INCLUDED_)
