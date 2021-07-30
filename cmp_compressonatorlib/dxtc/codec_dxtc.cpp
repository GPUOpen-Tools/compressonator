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

#include "common.h"
#include "codec_dxtc.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCodec_DXTC::CCodec_DXTC(CodecType codecType)
    : CCodec_Block_4x4(codecType) {
    m_bUseChannelWeighting = false;
    m_bUseAdaptiveWeighting = false;
    m_fChannelWeights[0] = m_fBaseChannelWeights[0] = 0.3086f;
    m_fChannelWeights[1] = m_fBaseChannelWeights[1] = 0.6094f;
    m_fChannelWeights[2] = m_fBaseChannelWeights[2] = 0.0820f;
    m_bUseFloat = false;
    m_b3DRefinement = false;
    m_nRefinementSteps = 0;
    m_nCompressionSpeed = CMP_Speed_SuperFast;
    m_bSwizzleChannels = false;
    m_fQuality = 1.0f;

    memset(&m_BC15Options,0,sizeof(CMP_BC15Options));
    m_BC15Options.m_fquality = 1.0f;
    m_BC15Options.m_fChannelWeights[0] = 1.0f;
    m_BC15Options.m_fChannelWeights[1] = 1.0f;
    m_BC15Options.m_fChannelWeights[2] = 1.0f;
}

CCodec_DXTC::~CCodec_DXTC() {
}

bool CCodec_DXTC::SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue) {
    if (sValue == NULL) return false;
    if(strcmp(pszParamName, "UseChannelWeighting") == 0) {
        m_bUseChannelWeighting = std::stoi(sValue) > 0 ? true : false;
        m_BC15Options.m_bUseChannelWeighting = m_bUseChannelWeighting ;
    }
    else if(strcmp(pszParamName, "UseAdaptiveWeighting") == 0) {
        m_bUseAdaptiveWeighting = std::stoi(sValue) > 0 ? true : false;
        m_BC15Options.m_bUseAdaptiveWeighting = m_bUseAdaptiveWeighting;
    }
    else if(strcmp(pszParamName, "ForceFloatPath") == 0)        // Not used in latest codec
        m_bUseFloat = std::stoi(sValue) > 0 ? true: false;
    else if(strcmp(pszParamName, "CompressionSpeed") == 0)      // Not used in latest codec
        m_nCompressionSpeed = (CMP_Speed) std::stoi(sValue);
    else if(strcmp(pszParamName, "WeightR") == 0) {
        CODECFLOAT fValue = std::stof(sValue);
        m_fChannelWeights[0] = m_fBaseChannelWeights[0] = fValue * fValue;
        m_BC15Options.m_fChannelWeights[0] = m_fChannelWeights[0];
    } else if(strcmp(pszParamName, "WeightG") == 0) {
        CODECFLOAT fValue = std::stof(sValue);
        m_fChannelWeights[1] = m_fBaseChannelWeights[1] = fValue * fValue;
        m_BC15Options.m_fChannelWeights[1] = m_fChannelWeights[1];
    } else if(strcmp(pszParamName, "WeightB") == 0) {
        CODECFLOAT fValue = std::stof(sValue);
        m_fChannelWeights[2] = m_fBaseChannelWeights[2] = fValue * fValue;
        m_BC15Options.m_fChannelWeights[2] = m_fChannelWeights[2];
    } else if (strcmp(pszParamName, "Quality") == 0) {
        m_fQuality = std::stof(sValue);
        if ((m_fQuality < 0) || (m_fQuality > 1.0)) return false;
        m_BC15Options.m_fquality = m_fQuality;
    } else if ((strcmp(pszParamName, "RefineSteps") == 0)     ||    // new option
               (strcmp(pszParamName, "RefinementSteps") == 0) ||    // old options
               (strcmp(pszParamName, "3DRefinement") == 0)          // old options
              ){
        m_nRefinementSteps = (CMP_BYTE)(std::stoi(sValue));
        if ((m_nRefinementSteps < 0) || (m_nRefinementSteps > 2)) return false;
        m_BC15Options.m_nRefinementSteps = m_nRefinementSteps;
    }
    else
        return CCodec_Block_4x4::SetParameter(pszParamName, sValue);
    return true;
}


bool CCodec_DXTC::SetParameter(const CMP_CHAR* pszParamName, CMP_DWORD dwValue) {
    if(strcmp(pszParamName, "UseChannelWeighting") == 0)
        m_bUseChannelWeighting = dwValue ? true : false;
    else if(strcmp(pszParamName, "UseAdaptiveWeighting") == 0)
        m_bUseAdaptiveWeighting = dwValue ? true : false;
    else if(strcmp(pszParamName, "3DRefinement") == 0)       // Old
        m_b3DRefinement = dwValue ? true : false;
    else if(strcmp(pszParamName, "RefinementSteps") == 0)    // Old
        m_nRefinementSteps = (CMP_BYTE) dwValue;
    else if(strcmp(pszParamName, "RefineSteps") == 0) {      // Newer setting
        m_b3DRefinement = dwValue ? true : false;
        m_nRefinementSteps = (CMP_BYTE) dwValue;
        m_BC15Options.m_nRefinementSteps = m_nRefinementSteps;
    }
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

bool CCodec_DXTC::GetParameter(const CMP_CHAR* pszParamName, CMP_DWORD& dwValue) {
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

bool CCodec_DXTC::SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue) {
    if(strcmp(pszParamName, "WeightR") == 0) {
        m_fChannelWeights[0] = m_fBaseChannelWeights[0] = fValue * fValue;
        m_BC15Options.m_fChannelWeights[0] = m_fChannelWeights[0] ;
    }
    else if(strcmp(pszParamName, "WeightG") == 0) {
        m_fChannelWeights[1] = m_fBaseChannelWeights[1] = fValue * fValue;
        m_BC15Options.m_fChannelWeights[1] = m_fChannelWeights[1] ;
    }
    else if(strcmp(pszParamName, "WeightB") == 0) {
        m_fChannelWeights[2] = m_fBaseChannelWeights[2] = fValue * fValue;
        m_BC15Options.m_fChannelWeights[2] = m_fChannelWeights[2] ;
    }
    else if (strcmp(pszParamName, "Quality") == 0) {
        m_fQuality = fValue;
        m_BC15Options.m_fquality = m_fQuality;
    }
    else
        return CCodec_Block_4x4::SetParameter(pszParamName, fValue);
    return true;
}

bool CCodec_DXTC::GetParameter(const CMP_CHAR* pszParamName, CODECFLOAT& fValue) {
    if(strcmp(pszParamName, "WeightR") == 0)
        fValue = sqrt(m_BC15Options.m_fChannelWeights[0]);
    else if(strcmp(pszParamName, "WeightG") == 0)
        fValue = sqrt(m_BC15Options.m_fChannelWeights[1]);
    else if(strcmp(pszParamName, "WeightB") == 0)
        fValue = sqrt(m_BC15Options.m_fChannelWeights[2]);
    else if (strcmp(pszParamName, "Quality") == 0) 
        fValue = m_BC15Options.m_fquality;
    else
        return CCodec_Block_4x4::GetParameter(pszParamName, fValue);
    return true;
}
