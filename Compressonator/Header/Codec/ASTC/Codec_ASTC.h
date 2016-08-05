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
#include "ASTC_library.h"
#include "ASTC_Definitions.h"

#ifdef USE_OPENCL
#include <CL\cl.h>

#ifndef CL_VERSION_1_2
#error OpenCL 1.2 or higher is needed
#endif

#endif


class CCodec_ASTC : public CCodec_DXTC
{
public:
    CCodec_ASTC();
    ~CCodec_ASTC();

    virtual bool SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue);
    virtual bool SetParameter(const CMP_CHAR* /*pszParamName*/, CMP_DWORD /*dwValue*/);
    virtual bool SetParameter(const CMP_CHAR* /*pszParamName*/, CODECFLOAT /*fValue*/);

    // Required interfaces
    virtual CodecError Compress             (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, DWORD_PTR pUser1 = NULL, DWORD_PTR pUser2 = NULL);
    virtual CodecError Decompress           (CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc = NULL, DWORD_PTR pUser1 = NULL, DWORD_PTR pUser2 = NULL);

private:

#ifdef USE_OPENCL
    // OpenCL objects
    cl_platform_id  m_opencl_platform;
    cl_device_id    m_opencl_device;
    cl_int          m_user_selected_plat_index; // if < 0 OpenCL platform will be choosen automaticaly
    cl_int          m_user_selected_dev_index;  // if < 0 OpenCL device will be choosen automaticaly
    cl_program      m_opencl_program;
    cl_context      m_opencl_context;
    BOOL            m_OCLInitialized;
#endif

    // ASTC User configurable variables
    WORD    m_NumThreads;    
    char    m_BlockRate[64];

    // ASTC Internal status 
    BOOL     m_LibraryInitialized;
    WORD     m_NumEncodingThreads;
    bool     m_AbortRequested;

    int m_xdim, m_ydim, m_zdim;        // Is now implamented and set by user
    float m_target_bitrate;

    astc_decode_mode m_decode_mode;
    error_weighting_params m_ewp;
    int batch_size;

    // ASTC Encoders and decoders: for encoding use the interfaces below
    ASTCBlockDecoder*    m_decoder;
    ASTCBlockEncoder*    m_encoder[MAX_ASTC_THREADS];

    // Encoder interfaces
    HANDLE*         m_EncodingThreadHandle;

    CodecError      EncodeASTCBlock(
        astc_codec_image *input_image,
        uint8_t *bp,
        int xdim,
        int ydim,
        int zdim,
        int x,
        int y,
        int z,
        astc_decode_mode decode_mode,
        const error_weighting_params * ewp);

    CodecError      FinishASTCEncoding();
    CodecError      InitializeASTCLibrary();
    CodecError      InitializeOCL();
    void            ReleaseOCL();
    void            InitializeASTCSettingsForSetBlockSize();

    // Encoder interfaces
    void find_closest_blockdim_2d(float target_bitrate, int *x, int *y, int consider_illegal);
    void find_closest_blockdim_3d(float target_bitrate, int *x, int *y, int *z, int consider_illegal);

    // Internal status 
    BOOL     m_Use_MultiThreading;
    WORD     m_LiveThreads;
    WORD     m_LastThread;

    // Speed and Quality
    double  m_Quality;

};

#endif // !defined(_CODEC_ASTC_H_INCLUDED_)
