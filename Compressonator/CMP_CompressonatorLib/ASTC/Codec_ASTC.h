//===============================================================================
// Copyright (c) 2014-2016  Advanced Micro Devices, Inc. All rights reserved.
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
//////////////////////////////////////////////////////////////////////////////////

#if !defined(_CODEC_ASTC_H_INCLUDED_)
#define _CODEC_ASTC_H_INCLUDED_

#include "Compressonator.h"
#include "Codec_DXTC.h"
#include "ASTC_Encode.h"
#include "ASTC_Decode.h"
#include "ASTC_Library.h"
#include "ASTC_Definitions.h"

#include <thread>

class CCodec_ASTC : public CCodec_DXTC
{
public:
    CCodec_ASTC();
    ~CCodec_ASTC();

    virtual bool SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue);
    virtual bool SetParameter(const CMP_CHAR* /*pszParamName*/, CMP_DWORD /*dwValue*/);
    virtual bool SetParameter(const CMP_CHAR* /*pszParamName*/, CODECFLOAT /*fValue*/);

    // Required interfaces
    virtual CodecError Compress             (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, CMP_DWORD_PTR pUser1 = NULL, CMP_DWORD_PTR pUser2 = NULL);
    virtual CodecError Decompress           (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, CMP_DWORD_PTR pUser1 = NULL, CMP_DWORD_PTR pUser2 = NULL);

private:

    // ASTC User configurable variables
    CMP_WORD    m_NumThreads;    
    char        m_BlockRate[64];

    // ASTC Internal status 
    CMP_BOOL    m_LibraryInitialized;
    CMP_INT     m_NumEncodingThreads;
    bool        m_AbortRequested;

    int m_xdim, m_ydim, m_zdim;        // Is now implamented and set by user ( defined in g_ASTCEncode )
    float m_target_bitrate;            // defined in g_ASTCEncode 

                                       // ASTC Encoders and decoders: for encoding use the interfaces below
    ASTCBlockDecoder*    m_decoder;
    ASTCBlockEncoder*    m_encoder[MAX_ASTC_THREADS];

    // Encoder interfaces
    std::thread         *m_EncodingThreadHandle;

    CodecError      EncodeASTCBlock(
        astc_codec_image *input_image,
        uint8_t *bp,
        int xdim,
        int ydim,
        int zdim,
        int x,
        int y,
        int z);

    CodecError      FinishASTCEncoding();
    CodecError      InitializeASTCLibrary();

    // Encoder interfaces
    void find_closest_blockdim_2d(float target_bitrate, int *x, int *y, int consider_illegal);
    void find_closest_blockdim_3d(float target_bitrate, int *x, int *y, int *z, int consider_illegal);
    void find_closest_blockxy_2d(int *x, int *y, int consider_illegal);

    // Internal status 
    CMP_BOOL    m_Use_MultiThreading;
    CMP_WORD    m_LiveThreads;
    CMP_WORD    m_LastThread;

    // Speed and Quality
    double  m_Quality;

};

#endif // !defined(_CODEC_ASTC_H_INCLUDED_)
