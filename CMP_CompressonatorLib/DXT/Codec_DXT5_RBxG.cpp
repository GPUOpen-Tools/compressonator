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
//  File Name:   Codec_DXT5_RBxG.cpp
//  Description: implementation of the CCodec_DXT5_RBxG class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Compressonator.h"
#include "Codec_DXT5_RBxG.h"

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////////////

CCodec_DXT5_RBxG::CCodec_DXT5_RBxG() :
CCodec_DXT5_Swizzled(CT_DXT5_RBxG)
{

}

CCodec_DXT5_RBxG::~CCodec_DXT5_RBxG()
{

}

// Disable erroneous C4715 warning
#pragma warning(push)
#pragma warning(disable:4715) 


bool CCodec_DXT5_RBxG::SetParameter(const CMP_CHAR* pszParamName, CMP_CHAR* sValue)
{
    if (sValue == NULL) return false;
    CODECFLOAT fValue = std::stof(sValue);
    if(strcmp(pszParamName, "WeightR") == 0)
        m_fChannelWeights[1] = m_fBaseChannelWeights[1] = fValue;
    else if(strcmp(pszParamName, "WeightG") == 0)
        m_fChannelWeights[0] = m_fBaseChannelWeights[0] = fValue;
    else if(strcmp(pszParamName, "WeightB") == 0)
        m_fChannelWeights[2] = m_fBaseChannelWeights[2] = fValue;
    else
        return CCodec_DXT5_Swizzled::SetParameter(pszParamName, sValue);
    return true;
}


bool CCodec_DXT5_RBxG::SetParameter(const CMP_CHAR* pszParamName, CODECFLOAT fValue)
{
    if(strcmp(pszParamName, "WeightR") == 0)
        m_fChannelWeights[0] = m_fBaseChannelWeights[0] = fValue;
    else if(strcmp(pszParamName, "WeightG") == 0)
        m_fChannelWeights[2] = m_fBaseChannelWeights[2] = fValue;
    else if(strcmp(pszParamName, "WeightB") == 0)
        m_fChannelWeights[1] = m_fBaseChannelWeights[1] = fValue;
    else
        return CCodec_DXT5_Swizzled::SetParameter(pszParamName, fValue);
    return true;
}

bool CCodec_DXT5_RBxG::GetParameter(const CMP_CHAR* pszParamName, CODECFLOAT& fValue)
{
    if(strcmp(pszParamName, "WeightR") == 0)
        fValue = m_fBaseChannelWeights[0];
    else if(strcmp(pszParamName, "WeightG") == 0)
        fValue = m_fBaseChannelWeights[2];
    else if(strcmp(pszParamName, "WeightB") == 0)
        fValue = m_fBaseChannelWeights[1];
    else
        return CCodec_DXT5_Swizzled::GetParameter(pszParamName, fValue);
    return true;
}

// Re-enable erroneous C4715 warning
#pragma warning(pop)

void CCodec_DXT5_RBxG::ReadBlock(CCodecBuffer& buffer, CMP_DWORD x, CMP_DWORD y, CMP_BYTE block[BLOCK_SIZE_4X4X4])
{
    CMP_BYTE dwTempBlock[BLOCK_SIZE_4X4X4];
    buffer.ReadBlockRGBA(x, y, 4, 4, dwTempBlock);
    for(CMP_DWORD i = 0; i < BLOCK_SIZE_4X4; i++)
        ((CMP_DWORD*)block)[i] = SWIZZLE_RGBA_RBxG(((CMP_DWORD*)dwTempBlock)[i]);
}

void CCodec_DXT5_RBxG::WriteBlock(CCodecBuffer& buffer, CMP_DWORD x, CMP_DWORD y, CMP_BYTE block[BLOCK_SIZE_4X4X4])
{
    CMP_BYTE dwTempBlock[BLOCK_SIZE_4X4X4];
    for(CMP_DWORD i = 0; i < BLOCK_SIZE_4X4; i++)
        ((CMP_DWORD*)dwTempBlock)[i] = SWIZZLE_RBxG_RGBA(((CMP_DWORD*)block)[i]);
    buffer.WriteBlockRGBA(x, y, 4, 4, dwTempBlock);
}

void CCodec_DXT5_RBxG::ReadBlock(CCodecBuffer& buffer, CMP_DWORD x, CMP_DWORD y, CODECFLOAT block[BLOCK_SIZE_4X4X4])
{
    CODECFLOAT fTempBlock[BLOCK_SIZE_4X4X4];
    buffer.ReadBlockRGBA(x, y, 4, 4, fTempBlock);
    for(CMP_DWORD i = 0; i < BLOCK_SIZE_4X4; i++)
    {
        block[(i* 4) + RGBA32F_OFFSET_R] = fTempBlock[(i* 4) + RGBA32F_OFFSET_R];
        block[(i* 4) + RGBA32F_OFFSET_G] = fTempBlock[(i* 4) + RGBA32F_OFFSET_B];
        block[(i* 4) + RGBA32F_OFFSET_B] = 0;
        block[(i* 4) + RGBA32F_OFFSET_A] = fTempBlock[(i* 4) + RGBA32F_OFFSET_G];
    }
}

void CCodec_DXT5_RBxG::WriteBlock(CCodecBuffer& buffer, CMP_DWORD x, CMP_DWORD y, CODECFLOAT block[BLOCK_SIZE_4X4X4])
{
    CODECFLOAT fTempBlock[BLOCK_SIZE_4X4X4];
    for(CMP_DWORD i = 0; i < BLOCK_SIZE_4X4; i++)
    {
        fTempBlock[(i* 4) + RGBA32F_OFFSET_R] = block[(i* 4) + RGBA32F_OFFSET_R];
        fTempBlock[(i* 4) + RGBA32F_OFFSET_G] = block[(i* 4) + RGBA32F_OFFSET_A];
        //fTempBlock[(i* 4) + RGBA32F_OFFSET_B] = block[(i* 4) + RGBA32F_OFFSET_B];
        fTempBlock[(i * 4) + RGBA32F_OFFSET_B] = block[(i * 4) + RGBA32F_OFFSET_G];
        fTempBlock[(i* 4) + RGBA32F_OFFSET_A] = 0.0;
    }
    buffer.WriteBlockRGBA(x, y, 4, 4, fTempBlock);
}