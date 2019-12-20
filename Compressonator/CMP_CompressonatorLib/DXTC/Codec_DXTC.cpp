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
//  File Name:   Codec_DXTC.cpp
//  Description: implementation of the CCodec_DXTC class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Codec_DXTC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCodec_DXTC::CCodec_DXTC(CodecType codecType)
: CCodec_Block_4x4(codecType)
{
    m_bUseChannelWeighting = false;
    m_bUseAdaptiveWeighting = false;
    m_fChannelWeights[0] = m_fBaseChannelWeights[0] = 0.3086f;
    m_fChannelWeights[1] = m_fBaseChannelWeights[1] = 0.6094f;
    m_fChannelWeights[2] = m_fBaseChannelWeights[2] = 0.0820f;
    m_bUseFloat = false;
    m_b3DRefinement = false;
    m_nRefinementSteps = 1;
    m_nCompressionSpeed = CMP_Speed_SuperFast;
    m_bSwizzleChannels = false;

}

CCodec_DXTC::~CCodec_DXTC()
{
}

bool CCodec_DXTC::SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue)
{
    if (sValue == NULL) return false;
    if(strcmp(pszParamName, "UseChannelWeighting") == 0)
        m_bUseChannelWeighting = std::stoi(sValue) > 0 ? true : false;
    else if(strcmp(pszParamName, "UseAdaptiveWeighting") == 0)
        m_bUseAdaptiveWeighting = std::stoi(sValue) > 0 ? true : false;
    else if(strcmp(pszParamName, "3DRefinement") == 0)
        m_b3DRefinement = std::stoi(sValue) > 0 ? true : false;
    else if(strcmp(pszParamName, "RefinementSteps") == 0)
        m_nRefinementSteps = (CMP_BYTE) std::stoi(sValue);
    else if(strcmp(pszParamName, "ForceFloatPath") == 0)
        m_bUseFloat = std::stoi(sValue) > 0 ? true: false;
    else if(strcmp(pszParamName, "CompressionSpeed") == 0)
        m_nCompressionSpeed = (CMP_Speed) std::stoi(sValue);
    else
    if(strcmp(pszParamName, "WeightR") == 0)
    {
        CODECFLOAT fValue = std::stof(sValue);
        m_fChannelWeights[0] = m_fBaseChannelWeights[0] = fValue * fValue;
    }
    else if(strcmp(pszParamName, "WeightG") == 0)
    {
        CODECFLOAT fValue = std::stof(sValue);
        m_fChannelWeights[1] = m_fBaseChannelWeights[1] = fValue * fValue;
    }
    else if(strcmp(pszParamName, "WeightB") == 0)
    {
        CODECFLOAT fValue = std::stof(sValue);
        m_fChannelWeights[2] = m_fBaseChannelWeights[2] = fValue * fValue;
    }
    else
        return CCodec_Block_4x4::SetParameter(pszParamName, sValue);
    return true;
}


bool CCodec_DXTC::SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue)
{
    if(strcmp(pszParamName, "UseChannelWeighting") == 0)
        m_bUseChannelWeighting = dwValue ? true : false;
    else if(strcmp(pszParamName, "UseAdaptiveWeighting") == 0)
        m_bUseAdaptiveWeighting = dwValue ? true : false;
    else if(strcmp(pszParamName, "3DRefinement") == 0)
        m_b3DRefinement = dwValue ? true : false;
    else if(strcmp(pszParamName, "RefinementSteps") == 0)
        m_nRefinementSteps = (CMP_BYTE) dwValue;
    else if(strcmp(pszParamName, "ForceFloatPath") == 0)
        m_bUseFloat = dwValue ? true: false;
    else if(strcmp(pszParamName, "CompressionSpeed") == 0)
        m_nCompressionSpeed = (CMP_Speed) dwValue;
    else if (strcmp(pszParamName, "SwizzleChannels") == 0)
        m_bSwizzleChannels = dwValue > 0 ? true : false;
    else
        return CCodec_Block_4x4::SetParameter(pszParamName, dwValue);
    return true;
}

bool CCodec_DXTC::GetParameter(const CMP_CHAR* pszParamName, CMP_DWORD& dwValue)
{
    if(strcmp(pszParamName, "UseChannelWeighting") == 0)
        dwValue = m_bUseChannelWeighting;
    else if(strcmp(pszParamName, "UseAdaptiveWeighting") == 0)
        dwValue = m_bUseAdaptiveWeighting;
    else if(strcmp(pszParamName, "3DRefinement") == 0)
        dwValue = m_b3DRefinement;
    else if(strcmp(pszParamName, "RefinementSteps") == 0)
        dwValue = m_nRefinementSteps;
    else if(strcmp(pszParamName, "ForceFloatPath") == 0)
        dwValue = m_bUseFloat;
    else if(strcmp(pszParamName, "CompressionSpeed") == 0)
        dwValue = m_nCompressionSpeed;
    else
        return CCodec_Block_4x4::SetParameter(pszParamName, dwValue);
    return true;
}

bool CCodec_DXTC::SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue)
{
    if(strcmp(pszParamName, "WeightR") == 0)
        m_fChannelWeights[0] = m_fBaseChannelWeights[0] = fValue * fValue;
    else if(strcmp(pszParamName, "WeightG") == 0)
        m_fChannelWeights[1] = m_fBaseChannelWeights[1] = fValue * fValue;
    else if(strcmp(pszParamName, "WeightB") == 0)
        m_fChannelWeights[2] = m_fBaseChannelWeights[2] = fValue * fValue;
    else
        return CCodec_Block_4x4::SetParameter(pszParamName, fValue);
    return true;
}

bool CCodec_DXTC::GetParameter(const CMP_CHAR* pszParamName, CODECFLOAT& fValue)
{
    if(strcmp(pszParamName, "WeightR") == 0)
        fValue = sqrt(m_fBaseChannelWeights[0]);
    else if(strcmp(pszParamName, "WeightG") == 0)
        fValue = sqrt(m_fBaseChannelWeights[1]);
    else if(strcmp(pszParamName, "WeightB") == 0)
        fValue = sqrt(m_fBaseChannelWeights[2]);
    else
        return CCodec_Block_4x4::GetParameter(pszParamName, fValue);
    return true;
}
