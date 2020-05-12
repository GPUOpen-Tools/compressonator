//===============================================================================
// Copyright (c) 2020  Advanced Micro Devices, Inc. All rights reserved.
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
//  File Name:   Codec_BASIS.cpp
//  Description: implementation of the CCodec_BASIS class
//
//////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4100)    // Ignore warnings of unreferenced formal parameters

#include "Compressonator.h"

#ifdef USE_BASIS


#ifdef _WIN32
#include "Common.h"
#include "Codec_BASIS.h"
#include <process.h>
#include "debug.h"

//======================================================================================
// #define USE_PRINTF
// #define USE_NOMULTITHREADING 

#ifdef USE_FILEIO
    #include <stdio.h>
    FILE * basis_File = NULL;
    int basis_blockcount = 0;
    int basis_total_MSE = 0;
#endif

// New SDK interfaces used for using external runtime common encoder codec API's
int  (*BASIS_CompressTexture)(void *in, void *out,void *processOptions) = NULL;
int  (*BASIS_DecompressTexture)(void *in, void *out,void *processOptions) = NULL;

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_BASIS::CCodec_BASIS() : CCodec_DXTC(CT_BASIS)
{
    m_LibraryInitialized   = false;
    m_quality = 0.05f;

}

bool CCodec_BASIS::SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue)
{
    if (sValue == NULL) return false;

    if (strcmp(pszParamName, "Quality") == 0)
    {
        m_quality = std::stof(sValue);
        if ((m_quality < 0) || (m_quality > 1.0))
        {
            return false;
        }
        return true;
    }
    else
    return CCodec_DXTC::SetParameter(pszParamName, sValue);
}

bool CCodec_BASIS::SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue)
{
    return CCodec_DXTC::SetParameter(pszParamName, dwValue);
}

bool CCodec_BASIS::SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue)
{
    if (strcmp(pszParamName, "Quality") == 0)
    {
        m_quality = fValue;
        return true;
    }
    else
        return CCodec_DXTC::SetParameter(pszParamName, fValue);
}

CCodec_BASIS::~CCodec_BASIS()
{
    if (m_LibraryInitialized)
    {
        m_LibraryInitialized = false;
    }
}

CodecError CCodec_BASIS::InitializeBASISLibrary()
{
    if (!m_LibraryInitialized)
    {
        m_LibraryInitialized = true;
    }
    return CE_OK;
}

CodecError CCodec_BASIS::EncodeBASISBlock(CMP_BYTE *in,  CMP_BYTE *out)
{
    return CE_OK;
}

CodecError CCodec_BASIS::FinishBASISEncoding(void)
{
    if(!m_LibraryInitialized)
    {
        return CE_Unknown;
    }
    
    return CE_OK;
}

CodecError CCodec_BASIS::Compress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    CodecError result = InitializeBASISLibrary();
    if (result != CE_OK) return result;


    if (BASIS_CompressTexture)
    {
        BASIS_CompressTexture(&bufferIn, &bufferOut, nullptr);
    }

    CodecError EncodeResult = FinishBASISEncoding();

    if (result != CE_Aborted)
        result = EncodeResult;

    return result;
}

CodecError CCodec_BASIS::Decompress(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    CodecError err = InitializeBASISLibrary();
    if (err != CE_OK) return err;

    if (BASIS_DecompressTexture)
    {
        BASIS_DecompressTexture(&bufferIn, &bufferOut, nullptr);
    }
    return CE_OK;
}

// Not implemented
CodecError CCodec_BASIS::Compress_Fast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    return CE_OK;
}

// Not implemented
CodecError CCodec_BASIS::Compress_SuperFast(CCodecBuffer& bufferIn, CCodecBuffer& bufferOut, Codec_Feedback_Proc pFeedbackProc, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
   return CE_OK;
}

#endif // USE_BASIS

#endif
