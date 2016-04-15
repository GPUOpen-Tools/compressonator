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
//  File Name:   Codec.cpp
//  Description: Implementation of the CCodec class
//
//////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Codec.h"
#include "Codec_ATI1N.h"
#include "Codec_ATI2N.h"
#include "Codec_ATI2N_DXT5.h"
#include "Codec_DXT1.h"
#include "Codec_DXT3.h"
#include "Codec_DXT5.h"
#include "Codec_DXT5_xGBR.h"
#include "Codec_DXT5_RxBG.h"
#include "Codec_DXT5_RBxG.h"
#include "Codec_DXT5_xRBG.h"
#include "Codec_DXT5_RGxB.h"
#include "Codec_DXT5_xGxR.h"
#include "Codec_ATC_RGB.h"
#include "Codec_ATC_RGBA_Explicit.h"
#include "Codec_ATC_RGBA_Interpolated.h"
#include "Codec_ETC_RGB.h"
#include "Codec_BC6H.h"
#include "Codec_BC7.h"
#include "ASTC\Codec_ASTC.h"
#include "Codec_GT.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCodec::CCodec(CodecType codecType)
{
    m_CodecType = codecType;
}

CCodec::~CCodec()
{

}

bool CCodec::SetParameter(const CMP_CHAR* /*pszParamName*/, CMP_CHAR*  /*dwValue*/)
{
    return false;
}

bool CCodec::SetParameter(const CMP_CHAR* /*pszParamName*/, CMP_DWORD /*dwValue*/)
{
    return false;
}

bool CCodec::GetParameter(const CMP_CHAR* /*pszParamName*/, CMP_DWORD& /*dwValue*/)
{
    return false;
}

bool CCodec::SetParameter(const CMP_CHAR* /*pszParamName*/, CODECFLOAT /*fValue*/)
{
    return false;
}

bool CCodec::GetParameter(const CMP_CHAR* /*pszParamName*/, CODECFLOAT& /*fValue*/)
{
    return false;
}

bool SupportsSSE()
{
#if defined(USE_SSE)
    return IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE) ? true : false;
#else
    return false;
#endif
}

bool SupportsSSE2()
{
#if defined(USE_SSE2)
    return IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE) ? true : false;
#else
    return false;
#endif
}

CCodec* CreateCodec(CodecType nCodecType)
{
#ifdef USE_DBGTRACE
    DbgTrace(("nCodecType %d",nCodecType));
#endif
    switch(nCodecType)
    {
        case CT_DXT1:                       return new CCodec_DXT1;
        case CT_DXT3:                       return new CCodec_DXT3;
        case CT_DXT5:                       return new CCodec_DXT5;
        case CT_DXT5_xGBR:                  return new CCodec_DXT5_xGBR;
        case CT_DXT5_RxBG:                  return new CCodec_DXT5_RxBG;
        case CT_DXT5_RBxG:                  return new CCodec_DXT5_RBxG;
        case CT_DXT5_xRBG:                  return new CCodec_DXT5_xRBG;
        case CT_DXT5_RGxB:                  return new CCodec_DXT5_RGxB;
        case CT_DXT5_xGxR:                  return new CCodec_DXT5_xGxR;
        case CT_ATI1N:                      return new CCodec_ATI1N;
        case CT_ATI2N:                      return new CCodec_ATI2N;
        case CT_ATI2N_XY:                   return new CCodec_ATI2N(CT_ATI2N_XY);
        case CT_ATI2N_DXT5:                 return new CCodec_ATI2N_DXT5;
        case CT_ATC_RGB:                    return new CCodec_ATC_RGB;
        case CT_ATC_RGBA_Explicit:          return new CCodec_ATC_RGBA_Explicit;
        case CT_ATC_RGBA_Interpolated:      return new CCodec_ATC_RGBA_Interpolated;
        case CT_ETC_RGB:                    return new CCodec_ETC_RGB;
        case CT_BC6H:                       return new CCodec_BC6H;
        case CT_BC7:                        return new CCodec_BC7;
        case CT_ASTC:                       return new CCodec_ASTC;
        case CT_GT:                         return new CCodec_GT;
        case CT_Unknown:
        default:
            assert(0);
            return NULL;
    }
}

CMP_DWORD CalcBufferSize(CodecType nCodecType, CMP_DWORD dwWidth, CMP_DWORD dwHeight)
{
#ifdef USE_DBGTRACE
    DbgTrace(("IN: nCodecType %d, dwWidth %d, dwHeight %d",nCodecType,dwWidth,dwHeight));
#endif
    CMP_DWORD dwChannels;
    CMP_DWORD dwBitsPerChannel;
    CMP_DWORD buffsize = 0;

    switch(nCodecType)
    {
        case CT_DXT1:
        case CT_ATI1N:
        case CT_ATC_RGB:
        case CT_ETC_RGB:
            dwChannels       = 1;
            dwBitsPerChannel = 4;
            dwWidth = ((dwWidth + 3) / 4) * 4;
            dwHeight = ((dwHeight + 3) / 4) * 4;
            buffsize = (dwWidth * dwHeight * dwChannels * dwBitsPerChannel) / 8;
            break;

        case CT_DXT3:
        case CT_DXT5:
        case CT_DXT5_xGBR:
        case CT_DXT5_RxBG:
        case CT_DXT5_RBxG:
        case CT_DXT5_xRBG:
        case CT_DXT5_RGxB:
        case CT_DXT5_xGxR:
        case CT_ATI2N:
        case CT_ATI2N_XY:
        case CT_ATI2N_DXT5:
        case CT_ATC_RGBA_Explicit:
        case CT_ATC_RGBA_Interpolated:
            dwChannels       = 2;
            dwBitsPerChannel = 4;
            dwWidth = ((dwWidth + 3) / 4) * 4;
            dwHeight = ((dwHeight + 3) / 4) * 4;
            buffsize = (dwWidth * dwHeight * dwChannels * dwBitsPerChannel) / 8;
            break;
        case CT_BC6H:    
            dwWidth  = ((dwWidth + 3) / 4) * 4;
            dwHeight = ((dwHeight + 3) / 4) * 4;
            buffsize = dwWidth * dwHeight; 
            if (buffsize < BC6H_BLOCK_BYTES) buffsize = BC6H_BLOCK_BYTES; 
            break;
        case CT_BC7:    
            // Fix: July 25 2014 was 4x larger than it should be
            // Aug 1: Added increas in buffer size so that its divisable by 4
            dwWidth  = ((dwWidth + 3) / 4) * 4;
            dwHeight = ((dwHeight + 3) / 4) * 4;
            buffsize = dwWidth * dwHeight; 
            if (buffsize < BC7_BLOCK_BYTES) buffsize = BC7_BLOCK_BYTES; 
            break;
        case CT_ASTC:    //notes: to be implemented! this is veriable size so addtional code is needed.
            dwWidth  = ((dwWidth + 3) / 4) * 4;
            dwHeight = ((dwHeight + 3) / 4) * 4;
            buffsize = dwWidth * dwHeight; 
            break;
        case CT_GT:
            // Aug 1: Added increas in buffer size so that its divisable by 4
            dwWidth = ((dwWidth + 3) / 4) * 4;
            dwHeight = ((dwHeight + 3) / 4) * 4;
            buffsize = dwWidth * dwHeight;
            if (buffsize < (4*4)) buffsize = 4*4;
            break;
        default:
            return 0;
    }

#ifdef USE_DBGTRACE
    DbgTrace(("OUT: %d",buffsize));
#endif

    return buffsize;
}

CMP_BYTE DeriveB(CMP_BYTE R, CMP_BYTE G)
{
    CODECFLOAT   r, g, b;
    r = (CODECFLOAT)R;
    g = (CODECFLOAT)G;

    // Get R and G in range -1->1
    r /= 255.f;
    g /= 255.f;
    r *= 2.f;
    g *= 2.f;
    r -= 1.f;
    g -= 1.f;

    b = min(r*r + g*g, 1.0f);
    // Now derive blue component
    b = (CODECFLOAT)sqrt(1 - b);
    b *= 127.f;
    b += 128.f;
    b = min(b, 255.f);
    b = max(b, 0.0f);

    return (CMP_BYTE)b;
}

CMP_BYTE DeriveR(CMP_BYTE B, CMP_BYTE G)
{
    CODECFLOAT   r, g, b;
    b = (CODECFLOAT)B;
    g = (CODECFLOAT)G;

    // Get R and G in range -1->1
    b /= 255.f;
    g /= 255.f;
    b *= 2.f;
    g *= 2.f;
    b -= 1.f;
    g -= 1.f;

    r = min(b*b + g*g, 1.0f);
    // Now derive blue component
    r = (CODECFLOAT)sqrt(1 - r);
    r *= 127.f;
    r += 128.f;
    r = min(b, 255.f);
    r = max(b, 0.0f);

    return (CMP_BYTE)r;
}


CODECFLOAT DeriveB(CODECFLOAT R, CODECFLOAT G)
{
    CODECFLOAT r, g, b;
    r = (CODECFLOAT)R;
    g = (CODECFLOAT)G;

    // Get R and G in range -1->1
    r *= 2.f;
    g *= 2.f;
    r -= 1.f;
    g -= 1.f;

    b = min(r*r + g*g, 1.0f);
    // Now derive blue component
    b = (CODECFLOAT)sqrt(1 - b);

    b += 1.f;
    b /= 2.f;

    return b;
}