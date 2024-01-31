//===============================================================================
// Copyright (c) 2022-2024  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   Codec_brlg.cpp
//  Description: implementation of the CCodec_BRLG class
//
//////////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4100)  // Ignore warnings of unreferenced formal parameters

#ifdef _WIN32

#include "codec_brlg.h"

#include "brlg_sdk_wrapper.h"
#include "common.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_BRLG::CCodec_BRLG()
    : CCodec_DXTC(CT_BRLG)
{
    m_numThreads          = 0;
    m_useGPUDecompression = false;
    m_pageSize            = 65536;  // Fixed max size for v1.0
    m_textureWidth        = 0;
    m_textureHeight       = 0;
    m_textureFormat       = CMP_FORMAT_Unknown;
    m_numMipmapLevels     = 1;
    m_doPrecondition      = true;
    m_doSwizzle           = false;
    m_doDeltaEncode       = false;
}

bool CCodec_BRLG::SetParameter(const CMP_CHAR* paramName, CMP_CHAR* value)
{
    if (value == NULL)
        return false;

    if (strcmp(paramName, CodecParameters::NumThreads) == 0)
    {
        m_numThreads = (CMP_BYTE)std::stoi(value) & 0xFF;
    }
    else if (strcmp(paramName, CodecParameters::PageSize) == 0)
    {
        m_pageSize = std::stoi(value);
    }
    else
        return CCodec_DXTC::SetParameter(paramName, value);

    return true;
}

bool CCodec_BRLG::SetParameter(const CMP_CHAR* paramName, CMP_DWORD value)
{
    if (strcmp(paramName, CodecParameters::NumThreads) == 0)
    {
        m_numThreads = (CMP_BYTE)value;
    }
    else if (strcmp(paramName, CodecParameters::UseGPUDecompression) == 0)
    {
        m_useGPUDecompression = value != 0;
    }
    else if (strcmp(paramName, CodecParameters::PageSize) == 0)
    {
        m_pageSize = value;
    }
    else if (strcmp(paramName, CodecParameters::TextureWidth) == 0)
    {
        m_textureWidth = value;
    }
    else if (strcmp(paramName, CodecParameters::TextureHeight) == 0)
    {
        m_textureHeight = value;
    }
    else if (strcmp(paramName, CodecParameters::TextureFormat) == 0)
    {
        m_textureFormat = (CMP_FORMAT)value;
    }
    else if (strcmp(paramName, CodecParameters::MipmapLevels) == 0)
    {
        m_numMipmapLevels = value;
    }
    else if (strcmp(paramName, CodecParameters::Precondition) == 0)
    {
        m_doPrecondition = value != 0;
    }
    else if (strcmp(paramName, CodecParameters::Swizzle) == 0)
    {
        m_doSwizzle = value != 0;
    }
    else if (strcmp(paramName, CodecParameters::DeltaEncode) == 0)
    {
        m_doDeltaEncode = value != 0;
    }
    else
        return CCodec_DXTC::SetParameter(paramName, value);

    return true;
}

bool CCodec_BRLG::SetParameter(const CMP_CHAR* paramName, CODECFLOAT value)
{
    return CCodec_DXTC::SetParameter(paramName, value);
}

CCodec_BRLG::~CCodec_BRLG()
{
}

CodecError CCodec_BRLG::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    uint32_t outputSize = 0;

    BRLG::EncodeParameters params = {};

    params.precondition    = m_doPrecondition;
    params.format          = m_textureFormat;
    params.textureWidth    = m_textureWidth;
    params.textureHeight   = m_textureHeight;
    params.numMipmapLevels = m_numMipmapLevels;
    params.doSwizzle       = m_doSwizzle;
    params.doDeltaEncode   = m_doDeltaEncode;

    if (m_textureFormat == CMP_FORMAT_Unknown || m_textureFormat == CMP_FORMAT_BINARY)
    {
        params.precondition = false;
    }

    bool result = BRLG::EncodeDataStream(bufferIn.GetData(), bufferIn.GetDataSize(), bufferOut.GetData(), &outputSize, m_pageSize, params);

    if (!result)
        return CE_Unknown;

    bufferOut.SetDataSize(outputSize);
    return CE_OK;
}

CodecError CCodec_BRLG::Decompress(CCodecBuffer&       bufferIn,
                                   CCodecBuffer&       bufferOut,
                                   Codec_Feedback_Proc pFeedbackProc,
                                   CMP_DWORD_PTR       pUser1,
                                   CMP_DWORD_PTR       pUser2)
{
    bool result = true;

    uint32_t outputSize = bufferOut.GetDataSize();

    if (m_useGPUDecompression)
        result = BRLG::DecodeDataStreamGPU(bufferIn.GetData(), bufferIn.GetDataSize(), bufferOut.GetData(), &outputSize);
    else
        result = BRLG::DecodeDataStreamCPU(bufferIn.GetData(), bufferIn.GetDataSize(), bufferOut.GetData(), &outputSize);

    if (!result)
        return CE_Unknown;

    bufferOut.SetDataSize(outputSize);
    return CE_OK;
}

// Not implemented
CodecError CCodec_BRLG::Compress_Fast(CCodecBuffer&       bufferIn,
                                      CCodecBuffer&       bufferOut,
                                      Codec_Feedback_Proc pFeedbackProc,
                                      CMP_DWORD_PTR       pUser1,
                                      CMP_DWORD_PTR       pUser2)
{
    return CE_OK;
}

// Not implemented
CodecError CCodec_BRLG::Compress_SuperFast(CCodecBuffer&       bufferIn,
                                           CCodecBuffer&       bufferOut,
                                           Codec_Feedback_Proc pFeedbackProc,
                                           CMP_DWORD_PTR       pUser1,
                                           CMP_DWORD_PTR       pUser2)
{
    return CE_OK;
}
#endif
