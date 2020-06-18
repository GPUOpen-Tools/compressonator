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
//
//  File Name:   Codec_Block_4x4.cpp
//  Description: implementation of the CCodec_Block_4x4 class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Codec_Block_4x4.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_Block_4x4::CCodec_Block_4x4(CodecType codecType)
: CCodec_Block(codecType)
{
    m_bUseSSE = SupportsSSE();
    m_bUseSSE2 = SupportsSSE2();
}

CCodec_Block_4x4::~CCodec_Block_4x4()
{

}

CCodecBuffer* CCodec_Block_4x4::CreateBuffer(
                                            CMP_BYTE nBlockWidth, CMP_BYTE nBlockHeight, CMP_BYTE nBlockDepth,
                                            CMP_DWORD dwWidth, CMP_DWORD dwHeight, CMP_DWORD dwPitch, CMP_BYTE* pData,
                                            CMP_DWORD dwDataSize) const
{
    return CreateCodecBuffer(CBT_4x4Block_8BPP, nBlockWidth, nBlockHeight, nBlockDepth,dwWidth, dwHeight, dwPitch, pData,dwDataSize);
}

bool CCodec_Block_4x4::SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue)
{
    if(strcmp(pszParamName, "UseSSE2") == 0)
        m_bUseSSE2 = (std::stoi(sValue) > 0)  ? SupportsSSE2() : false;
    else
      if(strcmp(pszParamName, "UseSSE") == 0)
        m_bUseSSE2 = (std::stoi(sValue) > 0)  ? SupportsSSE() : false;    
    else
      return CCodec_Block::SetParameter(pszParamName, sValue);
    return true;
}

bool CCodec_Block_4x4::SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue)
{
    if(strcmp(pszParamName, "UseSSE2") == 0)
        m_bUseSSE2 = dwValue ? SupportsSSE2() : false;
    else if(strcmp(pszParamName, "UseSSE") == 0)
        m_bUseSSE2 = dwValue ? SupportsSSE() : false;
    else
        return CCodec_Block::SetParameter(pszParamName, dwValue);
    return true;
}

bool CCodec_Block_4x4::GetParameter(const CMP_CHAR* pszParamName, CMP_DWORD& dwValue)
{
    if(strcmp(pszParamName, "UseSSE2") == 0)
        dwValue = m_bUseSSE2;
    if(strcmp(pszParamName, "UseSSE") == 0)
        dwValue = m_bUseSSE;
    else
        return CCodec_Block::SetParameter(pszParamName, dwValue);
    return true;
}

bool CCodec_Block_4x4::SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue)
{
    return CCodec_Block::SetParameter(pszParamName, fValue);
}

bool CCodec_Block_4x4::GetParameter(const CMP_CHAR* pszParamName, CODECFLOAT& fValue)
{
    return CCodec_Block::GetParameter(pszParamName, fValue);
}

