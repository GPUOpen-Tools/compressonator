//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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
/// \file ATIFormats.h
/// \version 2.21
//
//=====================================================================

#include "atiformats.h"

#include <stdio.h>
#include <cctype>
#ifdef _WIN32
#include <tchar.h>
#endif

#include "texture.h"

CMP_TextureTypeDesc g_TextureTypeDesc[] = {
    {TT_2D,"2D"},
    {TT_CubeMap,"CubeMap"},
    {TT_VolumeTexture,"Volume"},
    {TT_2D_Block,"2D_Block"},
    {TT_1D,"1D"}
};

CMP_FormatDesc g_FormatDesc[] = {

    {CMP_FORMAT_Unknown,                 "Unknown"},

    // 4 channels 
    {CMP_FORMAT_ARGB_8888,               "ARGB_8888"},
    {CMP_FORMAT_RGBA_8888,               "RGBA_8888"},
    {CMP_FORMAT_RGBA_16F,                "RGBA_16F"},
    {CMP_FORMAT_ARGB_16F,                "ARGB_16F"},
    {CMP_FORMAT_ARGB_32F,                "ARGB_32F"},
    {CMP_FORMAT_RGBA_8888_S,             "RGBA_8888_S"},
    {CMP_FORMAT_RGBA_1010102,            "RGBA_1010102"},

#ifdef CMP_ENABLE_TRANSCODECHANNEL_SUPPORT

    // alternate 4 channels 
    {CMP_FORMAT_ARGB_16, "ARGB_16"},
    {CMP_FORMAT_RGBA_8888_S, "RGBA_8888_S"},
    {CMP_FORMAT_ARGB_2101010, "ARGB_2101010"},

    // 3 channels
    {CMP_FORMAT_RGB_888, "RGB_888"},

    // 2 channels 
    {CMP_FORMAT_RG_8,                    "RG_8"},
    {CMP_FORMAT_RG_16,                   "RG_16"},
    {CMP_FORMAT_RG_16F,                  "RG_16F"},
    {CMP_FORMAT_RG_32F,                  "RG_32F"},

    // 1 channel 
    {CMP_FORMAT_R_8,                     "R_8"},
    {CMP_FORMAT_R_16,                    "R_16"},
    {CMP_FORMAT_R_16F,                   "R_16F"},
    {CMP_FORMAT_R_32F,                   "R_32F"},
#endif

    // Compressed
#if (OPTION_BUILD_ASTC == 1)
    {CMP_FORMAT_ASTC,                    "ASTC"},
#endif

    {CMP_FORMAT_ATI1N,                   "ATI1N"},
    {CMP_FORMAT_ATI2N,                   "ATI2N"},
    {CMP_FORMAT_ATI2N_XY,                "ATI2N_XY"},
    {CMP_FORMAT_ATI2N_DXT5,              "ATI2N_DXT5"},
    {CMP_FORMAT_ATC_RGB,                 "ATC_RGB"},
    {CMP_FORMAT_ATC_RGBA_Explicit,       "ATC_RGBA_EXPLICIT"},
    {CMP_FORMAT_ATC_RGBA_Interpolated,   "ATC_RGBA_INTERPOLATED"},

    {CMP_FORMAT_BC1,                     "BC1"},
    {CMP_FORMAT_BC2,                     "BC2"},
    {CMP_FORMAT_BC3,                     "BC3"},
    {CMP_FORMAT_BC4,                     "BC4"},
    {CMP_FORMAT_BC4_S,                   "BC4_S"},
    {CMP_FORMAT_BC5,                     "BC5"},
    {CMP_FORMAT_BC5_S,                   "BC5_S"},
    {CMP_FORMAT_BC6H,                    "BC6H"},
    {CMP_FORMAT_BC6H_SF,                 "BC6H_SF" },
    {CMP_FORMAT_BC7,                     "BC7"},


    {CMP_FORMAT_DXT1,                    "DXT1"},
    {CMP_FORMAT_DXT3,                    "DXT3"},
    {CMP_FORMAT_DXT5,                    "DXT5"},
    {CMP_FORMAT_DXT5_xGBR,               "DXT5_XGBR"},
    {CMP_FORMAT_DXT5_RxBG,               "DXT5_RXBG"},
    {CMP_FORMAT_DXT5_RBxG,               "DXT5_RBXG"},
    {CMP_FORMAT_DXT5_xRBG,               "DXT5_XRBG"},
    {CMP_FORMAT_DXT5_RGxB,               "DXT5_RGXB"},
    {CMP_FORMAT_DXT5_xGxR,               "DXT5_XGXR"},

    {CMP_FORMAT_ETC_RGB,                 "ETC_RGB"},
    {CMP_FORMAT_ETC2_RGB,                "ETC2_RGB" },
    {CMP_FORMAT_ETC2_RGBA,               "ETC2_RGBA" },
    {CMP_FORMAT_ETC2_RGBA1,              "ETC2_RGBA1" },
#ifdef ENABLE_USER_ETC2S_FORMATS
    {CMP_FORMAT_ETC2_SRGB,               "ETC2_SRGB" },
    {CMP_FORMAT_ETC2_SRGBA,              "ETC2_SRGBA" },
    {CMP_FORMAT_ETC2_SRGBA1,             "ETC2_SRGBA1" },
#endif
#ifdef USE_GTC
     {CMP_FORMAT_GTC,                     "GTC" },
#endif
#ifdef USE_APC
     {CMP_FORMAT_APC,                    "APC"},
#endif
#ifdef USE_BASIS
    {CMP_FORMAT_BASIS,                   "BASIS" },
#endif
    {CMP_FORMAT_BINARY,                 "CMPBIN"},
    {CMP_FORMAT_BROTLIG,                "BRLG"},

};


static const CMP_DWORD FORMAT_DESC_COUNT = sizeof(g_FormatDesc) / sizeof(g_FormatDesc[0]);
static const CMP_DWORD TEXTURE_TYPE_DESC_COUNT = sizeof(g_TextureTypeDesc) / sizeof(g_TextureTypeDesc[0]);

CMP_FORMAT CMP_API CMP_ParseFormat(char* pFormat) {
    if(pFormat == NULL)
        return CMP_FORMAT_Unknown;

    char pszFormat[128];

    int i = 0;
    for (const char *iter = pFormat; *iter != '\0'; iter++) {
        if (i < 127) {
            pszFormat[i++] = (char)std::toupper(*iter);
        }
    }

    pszFormat[i] = '\0';

    for (CMP_DWORD j = 0; j < FORMAT_DESC_COUNT; j++) {
        if (strcmp(pszFormat, g_FormatDesc[j].pszFormatDesc) == 0)
            return g_FormatDesc[j].nFormat;
    }

    return CMP_FORMAT_Unknown;
}

CMP_TextureType ParseTextureType(char* typeString)
{
    if (!typeString)
        return TT_Unknown;
    
    char buffer[128] = {};
    for (int i = 0; i < sizeof(buffer)/sizeof(buffer[0]) && typeString[i] != '\0'; ++i)
    {
        buffer[i] = (char)std::toupper(typeString[i]);
    }

    for (int i = 0; i < TEXTURE_TYPE_DESC_COUNT; ++i)
    {
        if (strcmp(buffer, g_TextureTypeDesc[i].pszTextureTypeDesc) == 0 )
            return g_TextureTypeDesc[i].nTextureType;
    }

    return TT_Unknown;
}

CMP_CHAR* GetFormatDesc(CMP_FORMAT nFormat) {
    for (CMP_DWORD i = 0; i < FORMAT_DESC_COUNT; i++)
    {
        if (nFormat == g_FormatDesc[i].nFormat)
            return g_FormatDesc[i].pszFormatDesc;
    }
    return g_FormatDesc[0].pszFormatDesc;
}

CMP_CHAR* GetTextureTypeDesc(CMP_TextureType nTextureType) { // depthsupport
    for(CMP_DWORD i = 0; i < FORMAT_DESC_COUNT; i++)
        if(nTextureType == g_TextureTypeDesc[i].nTextureType)
            return g_TextureTypeDesc[i].pszTextureTypeDesc;

    return g_TextureTypeDesc[0].pszTextureTypeDesc;
}

void  CMP_API CMP_Format2FourCC(CMP_FORMAT format, MipSet *pMipSet) {
    switch(format) {

    case CMP_FORMAT_BC4_S:
        pMipSet->m_dwFourCC = CMP_FOURCC_BC4S;
        break;

    case CMP_FORMAT_BC4:
    case CMP_FORMAT_ATI1N:
        pMipSet->m_dwFourCC = CMP_FOURCC_ATI1N;
        break;

    case CMP_FORMAT_ATI2N:
        pMipSet->m_dwFourCC   =  CMP_FOURCC_ATI2N;
        break;

    case CMP_FORMAT_BC5_S:
        pMipSet->m_dwFourCC  = CMP_FOURCC_BC5S;
        break;

    case CMP_FORMAT_BC5:
    case CMP_FORMAT_ATI2N_XY:
        pMipSet->m_dwFourCC    = CMP_FOURCC_ATI2N;
        pMipSet->m_dwFourCC2   = CMP_FOURCC_ATI2N_XY;   // Swizzled format
        break;

    case CMP_FORMAT_ATI2N_DXT5:
        pMipSet->m_dwFourCC     = CMP_FOURCC_ATI2N_DXT5;
        break;

    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DXT1;
        break;

    case CMP_FORMAT_BC2:
    case CMP_FORMAT_DXT3:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DXT3;
        break;

    case CMP_FORMAT_BC3:
    case CMP_FORMAT_DXT5:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DXT5;
        break;
    case CMP_FORMAT_DXT5_xGBR:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DXT5_xGBR;
        break;
    case CMP_FORMAT_DXT5_RxBG:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DXT5_RxBG;
        break;
    case CMP_FORMAT_DXT5_RBxG:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DXT5_RBxG;
        break;
    case CMP_FORMAT_DXT5_xRBG:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DXT5_xRBG;
        break;
    case CMP_FORMAT_DXT5_RGxB:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DXT5_RGxB;
        break;
    case CMP_FORMAT_DXT5_xGxR:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DXT5_xGxR;
        break;

    case CMP_FORMAT_ATC_RGB:
        pMipSet->m_dwFourCC = CMP_FOURCC_ATC_RGB;
        break;
    case CMP_FORMAT_ATC_RGBA_Explicit:
        pMipSet->m_dwFourCC = CMP_FOURCC_ATC_RGBA_EXPLICIT;
        break;
    case CMP_FORMAT_ATC_RGBA_Interpolated:
        pMipSet->m_dwFourCC = CMP_FOURCC_ATC_RGBA_INTERP;
        break;

    case CMP_FORMAT_ETC_RGB:
        pMipSet->m_dwFourCC = CMP_FOURCC_ETC_RGB;
        break;
    case CMP_FORMAT_ETC2_RGB:
        pMipSet->m_dwFourCC = CMP_FOURCC_ETC2_RGB;
        break;
    case CMP_FORMAT_ETC2_SRGB:
        pMipSet->m_dwFourCC = CMP_FOURCC_ETC2_SRGB;
        break;
    case CMP_FORMAT_ETC2_RGBA:
        pMipSet->m_dwFourCC = CMP_FOURCC_ETC2_RGBA;
        break;
    case CMP_FORMAT_ETC2_RGBA1:
        pMipSet->m_dwFourCC = CMP_FOURCC_ETC2_RGBA1;
        break;
    case CMP_FORMAT_ETC2_SRGBA:
        pMipSet->m_dwFourCC = CMP_FOURCC_ETC2_SRGBA;
        break;
    case CMP_FORMAT_ETC2_SRGBA1:
        pMipSet->m_dwFourCC = CMP_FOURCC_ETC2_SRGBA1;
        break;
#ifdef USE_GTC
    case CMP_FORMAT_GTC:
        pMipSet->m_dwFourCC = CMP_FOURCC_GTC;
        break;
#endif
    case CMP_FORMAT_BROTLIG:
        pMipSet->m_dwFourCC = CMP_FOURCC_BROTLIG;
        break;
#ifdef USE_APC
    case CMP_FORMAT_APC:
        pMipSet->m_dwFourCC = CMP_FOURCC_APC;
        break;
#endif
#ifdef USE_BASIS
    case CMP_FORMAT_BASIS:
        pMipSet->m_dwFourCC =  CMP_FOURCC_BASIS;
        break;
#endif
    case CMP_FORMAT_BC6H:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DX10;
        break;
    case CMP_FORMAT_BC6H_SF:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DX10;
        break;
    case CMP_FORMAT_BC7:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DX10;
        break;
#if (OPTION_BUILD_ASTC == 1)
    case CMP_FORMAT_ASTC:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DX10;
        break;
#endif
    default:
        pMipSet->m_dwFourCC =  CMP_FOURCC_DX10;
    }
}

CMP_FORMAT CMP_API CMP_FourCC2Format(CMP_DWORD fourCC)
{

    switch (fourCC)
    {
    case CMP_FOURCC_BC4S:
        return (CMP_FORMAT_BC4_S);
        break;

    case CMP_FOURCC_ATI1N:
    case CMP_FOURCC_BC4U:
        return (CMP_FORMAT_BC4); // CMP_FORMAT_ATI1N:
        break;

    case CMP_FOURCC_BC5S:
        return (CMP_FORMAT_BC5_S);
        break;

    case CMP_FOURCC_ATI2N:
    case CMP_FOURCC_BC5:
    case CMP_FOURCC_BC5U:
        return (CMP_FORMAT_BC5); // CMP_FOURCC_ATI2N;
        break;

    case CMP_FOURCC_ATI2N_XY:
        return (CMP_FORMAT_ATI2N_XY);
        break;

    case CMP_FOURCC_ATI2N_DXT5:
        return(CMP_FORMAT_ATI2N_DXT5);
        break;

    case CMP_FOURCC_DXT1:
        return (CMP_FORMAT_BC1); // CMP_FORMAT_DXT1:
        break;

    case CMP_FOURCC_DXT3:
        return (CMP_FORMAT_BC2); //CMP_FORMAT_DXT3:
        break;

    case CMP_FOURCC_DXT5:
        return (CMP_FORMAT_BC3); // CMP_FORMAT_DXT5:
        break;
    case CMP_FOURCC_DXT5_xGBR:
        return (CMP_FORMAT_DXT5_xGBR);
        break;
    case CMP_FOURCC_DXT5_RxBG:
        return (CMP_FORMAT_DXT5_RxBG);
        break;
    case CMP_FOURCC_DXT5_RBxG:
        return (CMP_FORMAT_DXT5_RBxG);
        break;
    case CMP_FOURCC_DXT5_xRBG:
        return (CMP_FORMAT_DXT5_xRBG);
        break;
    case CMP_FOURCC_DXT5_RGxB:
        return (CMP_FORMAT_DXT5_RGxB);
        break;
    case CMP_FOURCC_DXT5_xGxR:
        return (CMP_FORMAT_DXT5_xGxR);
        break;
    case CMP_FOURCC_ATC_RGB:
        return (CMP_FORMAT_ATC_RGB);
        break;
    case CMP_FOURCC_ATC_RGBA_EXPLICIT:
        return (CMP_FORMAT_ATC_RGBA_Explicit);
        break;
    case CMP_FOURCC_ATC_RGBA_INTERP:
        return (CMP_FORMAT_ATC_RGBA_Interpolated);
        break;
    case CMP_FOURCC_ETC_RGB:
        return (CMP_FORMAT_ETC_RGB);
        break;
    case CMP_FOURCC_ETC2_RGB:
        return (CMP_FORMAT_ETC2_RGB);
        break;
    case CMP_FOURCC_ETC2_SRGB:
        return (CMP_FORMAT_ETC2_SRGB);
        break;
    case CMP_FOURCC_ETC2_RGBA:
        return (CMP_FORMAT_ETC2_RGBA);
        break;
    case CMP_FOURCC_ETC2_RGBA1:
        return (CMP_FORMAT_ETC2_RGBA1);
        break;
    case CMP_FOURCC_ETC2_SRGBA:
        return (CMP_FORMAT_ETC2_SRGBA);
        break;
    case CMP_FOURCC_ETC2_SRGBA1:
        return (CMP_FORMAT_ETC2_SRGBA1);
        break;
#ifdef USE_GTC
    case CMP_FOURCC_GTC:
        return (CMP_FORMAT_GTC);
        break;
#endif
    case CMP_FOURCC_BROTLIG:
        return (CMP_FORMAT_BROTLIG);
        break;
#ifdef USE_APC
    case CMP_FOURCC_APC:
        return (CMP_FORMAT_APC);
        break;
#endif
#ifdef USE_BASIS
    case CMP_FOURCC_BASIS:
        return (CMP_FORMAT_BASIS);
        break;
#endif
    }
    return (CMP_FORMAT_Unknown);
}

CMP_BOOL CMP_API CMP_IsCompressedFormat(CMP_FORMAT format) {

    switch (format)
    {
#if (OPTION_BUILD_ASTC == 1)
    case CMP_FORMAT_ASTC:
#endif
    case CMP_FORMAT_ATI1N:
    case CMP_FORMAT_ATI2N:
    case CMP_FORMAT_ATI2N_XY:
    case CMP_FORMAT_ATI2N_DXT5:
    case CMP_FORMAT_ATC_RGB:
    case CMP_FORMAT_ATC_RGBA_Explicit:
    case CMP_FORMAT_ATC_RGBA_Interpolated:
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_BC4_S:
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_BC5_S:
    case CMP_FORMAT_BC6H:
    case CMP_FORMAT_BC6H_SF:
    case CMP_FORMAT_BC7:
    case CMP_FORMAT_DXT1:
    case CMP_FORMAT_DXT3:
    case CMP_FORMAT_DXT5:
    case CMP_FORMAT_DXT5_xGBR:
    case CMP_FORMAT_DXT5_RxBG:
    case CMP_FORMAT_DXT5_RBxG:
    case CMP_FORMAT_DXT5_xRBG:
    case CMP_FORMAT_DXT5_RGxB:
    case CMP_FORMAT_DXT5_xGxR:
    case CMP_FORMAT_ETC_RGB:
    case CMP_FORMAT_ETC2_RGB:
    case CMP_FORMAT_ETC2_SRGB:
    case CMP_FORMAT_ETC2_RGBA:
    case CMP_FORMAT_ETC2_RGBA1:
    case CMP_FORMAT_ETC2_SRGBA:
    case CMP_FORMAT_ETC2_SRGBA1:
    case CMP_FORMAT_PVRTC:
#ifdef USE_APC
    case CMP_FORMAT_APC:  //< APC Texture Compressor
#endif
    case CMP_FORMAT_BROTLIG:
    case CMP_FORMAT_GTC:    //< GTC   Fast Gradient Texture Compressor
    case CMP_FORMAT_BASIS:  //< BASIS compression
    {
        return true;
    }
    break;
    default:
        break;
    }

    return false;
}


CMP_BOOL CMP_API CMP_IsFloatFormat(CMP_FORMAT InFormat)
{
    switch (InFormat)
    {
    case CMP_FORMAT_ARGB_16F:
    case CMP_FORMAT_ABGR_16F:
    case CMP_FORMAT_RGBA_16F:
    case CMP_FORMAT_BGRA_16F:
    case CMP_FORMAT_RG_16F:
    case CMP_FORMAT_R_16F:
    case CMP_FORMAT_ARGB_32F:
    case CMP_FORMAT_ABGR_32F:
    case CMP_FORMAT_RGBA_32F:
    case CMP_FORMAT_BGRA_32F:
    case CMP_FORMAT_RGB_32F:
    case CMP_FORMAT_BGR_32F:
    case CMP_FORMAT_RG_32F:
    case CMP_FORMAT_R_32F:
    case CMP_FORMAT_BC6H:
    case CMP_FORMAT_BC6H_SF:
    case CMP_FORMAT_RGBE_32F:
    {
        return true;
    }
    break;
    default:
        break;
    }

    return false;
}

CMP_BOOL CMP_API CMP_IsHDR(CMP_FORMAT InFormat)
{
    return CMP_IsFloatFormat(InFormat);
}

CMP_BOOL CMP_API CMP_IsLossless(CMP_FORMAT InFormat)
{
    switch (InFormat)
    {
    case CMP_FORMAT_BROTLIG:
    case CMP_FORMAT_BINARY: {
        return true;
    }
    break;
    default:
        break;
    }

    return false;
}

CMP_BOOL CMP_API CMP_IsValidFormat(CMP_FORMAT InFormat)
{
    switch (InFormat)
    {
        case CMP_FORMAT_RGBA_8888_S          :
        case CMP_FORMAT_ARGB_8888_S          :
        case CMP_FORMAT_ARGB_8888            :
        case CMP_FORMAT_ABGR_8888            :
        case CMP_FORMAT_RGBA_8888            :
        case CMP_FORMAT_BGRA_8888            :
        case CMP_FORMAT_RGB_888              :
        case CMP_FORMAT_RGB_888_S            :
        case CMP_FORMAT_BGR_888              :
        case CMP_FORMAT_RG_8_S               :
        case CMP_FORMAT_RG_8                 :
        case CMP_FORMAT_R_8_S                :
        case CMP_FORMAT_R_8                  :
        case CMP_FORMAT_ARGB_2101010         :
        case CMP_FORMAT_RGBA_1010102         :
        case CMP_FORMAT_ARGB_16              :
        case CMP_FORMAT_ABGR_16              :
        case CMP_FORMAT_RGBA_16              :
        case CMP_FORMAT_BGRA_16              :
        case CMP_FORMAT_RG_16                :
        case CMP_FORMAT_R_16                 :
        case CMP_FORMAT_RGBE_32F             :
        case CMP_FORMAT_ARGB_16F             :
        case CMP_FORMAT_ABGR_16F             :
        case CMP_FORMAT_RGBA_16F             :
        case CMP_FORMAT_BGRA_16F             :
        case CMP_FORMAT_RG_16F               :
        case CMP_FORMAT_R_16F                :
        case CMP_FORMAT_ARGB_32F             :
        case CMP_FORMAT_ABGR_32F             :
        case CMP_FORMAT_RGBA_32F             :
        case CMP_FORMAT_BGRA_32F             :
        case CMP_FORMAT_RGB_32F              :
        case CMP_FORMAT_BGR_32F              :
        case CMP_FORMAT_RG_32F               :
        case CMP_FORMAT_R_32F                :
        case CMP_FORMAT_BROTLIG              :
        case CMP_FORMAT_BC1                  :
        case CMP_FORMAT_BC2                  :
        case CMP_FORMAT_BC3                  :
        case CMP_FORMAT_BC4                  :
        case CMP_FORMAT_BC4_S                :
        case CMP_FORMAT_BC5                  :
        case CMP_FORMAT_BC5_S                :
        case CMP_FORMAT_BC6H                 :
        case CMP_FORMAT_BC6H_SF              :
        case CMP_FORMAT_BC7                  :
        case CMP_FORMAT_ATI1N                :
        case CMP_FORMAT_ATI2N                :
        case CMP_FORMAT_ATI2N_XY             :
        case CMP_FORMAT_ATI2N_DXT5           :
        case CMP_FORMAT_DXT1                 :
        case CMP_FORMAT_DXT3                 :
        case CMP_FORMAT_DXT5                 :
        case CMP_FORMAT_DXT5_xGBR            :
        case CMP_FORMAT_DXT5_RxBG            :
        case CMP_FORMAT_DXT5_RBxG            :
        case CMP_FORMAT_DXT5_xRBG            :
        case CMP_FORMAT_DXT5_RGxB            :
        case CMP_FORMAT_DXT5_xGxR            :
        case CMP_FORMAT_ATC_RGB              :
        case CMP_FORMAT_ATC_RGBA_Explicit    :
        case CMP_FORMAT_ATC_RGBA_Interpolated:
        case CMP_FORMAT_ASTC                 :    
        case CMP_FORMAT_APC                  :    
        case CMP_FORMAT_PVRTC                :    
        case CMP_FORMAT_ETC_RGB              :    
        case CMP_FORMAT_ETC2_RGB             :    
        case CMP_FORMAT_ETC2_SRGB            :    
        case CMP_FORMAT_ETC2_RGBA            :    
        case CMP_FORMAT_ETC2_RGBA1           :    
        case CMP_FORMAT_ETC2_SRGBA           :    
        case CMP_FORMAT_ETC2_SRGBA1          :    
        case CMP_FORMAT_BINARY               :    
        case CMP_FORMAT_GTC                  :    
        case CMP_FORMAT_BASIS                :    
        {
            return true;
        }
    }
    return false;
}

CMP_ChannelFormat GetChannelFormat(CMP_FORMAT format)
{
    switch (format)
    {
    case CMP_FORMAT_ARGB_32F:
    case CMP_FORMAT_ABGR_32F:
    case CMP_FORMAT_RGBA_32F:
    case CMP_FORMAT_BGRA_32F:
    case CMP_FORMAT_RGB_32F:
    case CMP_FORMAT_BGR_32F:
    case CMP_FORMAT_RG_32F:
    case CMP_FORMAT_R_32F:
        return CF_Float32;
    case CMP_FORMAT_ARGB_16F:
    case CMP_FORMAT_ABGR_16F:
    case CMP_FORMAT_RGBA_16F:
    case CMP_FORMAT_BGRA_16F:
    case CMP_FORMAT_RG_16F:
    case CMP_FORMAT_R_16F:
        return CF_Float16;
        break;
    case CMP_FORMAT_RGBA_1010102:
        return CF_1010102;
    case CMP_FORMAT_ARGB_2101010:
        return CF_2101010;
    case CMP_FORMAT_ARGB_16:
    case CMP_FORMAT_ABGR_16:
    case CMP_FORMAT_RGBA_16:
    case CMP_FORMAT_BGRA_16:
    case CMP_FORMAT_RG_16:
    case CMP_FORMAT_R_16:
        return CF_16bit;
    }

    return CF_8bit;
}

CMP_BYTE GetChannelFormatBitSize(CMP_FORMAT format)
{
    switch (format)
    {
    case CMP_FORMAT_ARGB_16F:
    case CMP_FORMAT_ABGR_16F:
    case CMP_FORMAT_RGBA_16F:
    case CMP_FORMAT_BGRA_16F:
    case CMP_FORMAT_RG_16F:
    case CMP_FORMAT_R_16F:
    case CMP_FORMAT_ARGB_16:
    case CMP_FORMAT_ABGR_16:
    case CMP_FORMAT_RGBA_16:
    case CMP_FORMAT_BGRA_16:
    case CMP_FORMAT_RG_16:
    case CMP_FORMAT_R_16:
    case CMP_FORMAT_BC6H:
    case CMP_FORMAT_BC6H_SF:
    case CMP_FORMAT_ARGB_2101010:
    { 
        return (CMP_BYTE)(16);
    }
    break;

    case CMP_FORMAT_ARGB_32F:
    case CMP_FORMAT_ABGR_32F:
    case CMP_FORMAT_RGBA_32F:
    case CMP_FORMAT_BGRA_32F:
    case CMP_FORMAT_RGB_32F:
    case CMP_FORMAT_BGR_32F:
    case CMP_FORMAT_RG_32F:
    case CMP_FORMAT_R_32F:
    case CMP_FORMAT_RGBE_32F:
    {
        return (CMP_BYTE)(32);
    }
    break;
    }

    return (CMP_BYTE)(8);
}

static CMP_FORMAT GetFormat(CMP_DWORD dwFourCC) {
    switch(dwFourCC) {
    case CMP_FOURCC_ATI1N:              return CMP_FORMAT_ATI1N;
    case CMP_FOURCC_ATI2N:              return CMP_FORMAT_ATI2N;
    case CMP_FOURCC_ATI2N_XY:           return CMP_FORMAT_ATI2N_XY;
    case CMP_FOURCC_ATI2N_DXT5:         return CMP_FORMAT_ATI2N_DXT5;
    case CMP_FOURCC_DXT1:               return CMP_FORMAT_DXT1;
    case CMP_FOURCC_DXT3:               return CMP_FORMAT_DXT3;
    case CMP_FOURCC_DXT5:               return CMP_FORMAT_DXT5;
    case CMP_FOURCC_DXT5_xGBR:          return CMP_FORMAT_DXT5_xGBR;
    case CMP_FOURCC_DXT5_RxBG:          return CMP_FORMAT_DXT5_RxBG;
    case CMP_FOURCC_DXT5_RBxG:          return CMP_FORMAT_DXT5_RBxG;
    case CMP_FOURCC_DXT5_xRBG:          return CMP_FORMAT_DXT5_xRBG;
    case CMP_FOURCC_DXT5_RGxB:          return CMP_FORMAT_DXT5_RGxB;
    case CMP_FOURCC_DXT5_xGxR:          return CMP_FORMAT_DXT5_xGxR;

    // Deprecated but still supported for decompression
    // Some definition are not valid FOURCC values nut are used as Custom formats
    // so that DDS files can be used for storage
    case CMP_FOURCC_DXT5_GXRB:          return CMP_FORMAT_DXT5_xRBG;
    case CMP_FOURCC_DXT5_GRXB:          return CMP_FORMAT_DXT5_RxBG;
    case CMP_FOURCC_DXT5_RXGB:          return CMP_FORMAT_DXT5_xGBR;
    case CMP_FOURCC_DXT5_BRGX:          return CMP_FORMAT_DXT5_RGxB;

    case CMP_FOURCC_ATC_RGB:            return CMP_FORMAT_ATC_RGB;
    case CMP_FOURCC_ATC_RGBA_EXPLICIT:  return CMP_FORMAT_ATC_RGBA_Explicit;
    case CMP_FOURCC_ATC_RGBA_INTERP:    return CMP_FORMAT_ATC_RGBA_Interpolated;
    case CMP_FOURCC_ETC_RGB:            return CMP_FORMAT_ETC_RGB;
    case CMP_FOURCC_ETC2_RGB:           return CMP_FORMAT_ETC2_RGB;
    case CMP_FOURCC_ETC2_SRGB:          return CMP_FORMAT_ETC2_SRGB;
    case CMP_FOURCC_ETC2_RGBA:          return CMP_FORMAT_ETC2_RGBA;
    case CMP_FOURCC_ETC2_RGBA1:         return CMP_FORMAT_ETC2_RGBA1;
    case CMP_FOURCC_ETC2_SRGBA:         return CMP_FORMAT_ETC2_SRGBA;
    case CMP_FOURCC_ETC2_SRGBA1:        return CMP_FORMAT_ETC2_SRGBA1;
    case CMP_FOURCC_BC4S:               return CMP_FORMAT_BC4_S;
    case CMP_FOURCC_BC4:
    case CMP_FOURCC_BC4U:               return CMP_FORMAT_ATI1N;  
    case CMP_FOURCC_BC5:                return CMP_FORMAT_BC5;
    case CMP_FOURCC_BC5S:               return CMP_FORMAT_BC5_S;
    case CMP_FOURCC_BC6H:               return CMP_FORMAT_BC6H;
    case CMP_FOURCC_BC7:                return CMP_FORMAT_BC7;
#if (OPTION_BUILD_ASTC == 1)
    case CMP_FOURCC_ASTC:               return CMP_FORMAT_ASTC;
#endif
#ifdef USE_APC
    case CMP_FOURCC_APC:                return CMP_FORMAT_APC;
#endif
#ifdef USE_GTC
    case CMP_FOURCC_GTC:                return CMP_FORMAT_GTC;
#endif
#ifdef USE_BASIS
    case CMP_FOURCC_BASIS:              return CMP_FORMAT_BASIS;
#endif
    case CMP_FOURCC_BROTLIG:            return CMP_FORMAT_BROTLIG;
    default:
        return CMP_FORMAT_Unknown;
    }
}

CMP_FORMAT GetFormat(MipSet* pMipSet) {
    assert(pMipSet);
    if(pMipSet == NULL)
        return CMP_FORMAT_Unknown;

    switch(pMipSet->m_ChannelFormat) {
    case CF_8bit:
        switch(pMipSet->m_TextureDataType) {
        case TDT_R:
            return CMP_FORMAT_R_8;
        case TDT_RG:
            return CMP_FORMAT_RG_8;
        default:
            return CMP_FORMAT_ARGB_8888;
        }
    case CF_Float16:
        switch(pMipSet->m_TextureDataType) {
        case TDT_R:
            return CMP_FORMAT_R_16F;
        case TDT_RG:
            return CMP_FORMAT_RG_16F;
        default:
            return CMP_FORMAT_RGBA_16F;
        }
    case CF_Float32:
        switch(pMipSet->m_TextureDataType) {
        case TDT_R:
            return CMP_FORMAT_R_32F;
        case TDT_RG:
            return CMP_FORMAT_RG_32F;
        default:
            return CMP_FORMAT_ARGB_32F;
        }
    case CF_Float9995E:
        return CMP_FORMAT_RGBE_32F;

    case CF_Compressed:
        return GetFormat(pMipSet->m_dwFourCC2 ? pMipSet->m_dwFourCC2 : pMipSet->m_dwFourCC);
    case CF_16bit:
        switch(pMipSet->m_TextureDataType) {
        case TDT_R:
            return CMP_FORMAT_R_16;
        case TDT_RG:
            return CMP_FORMAT_RG_16;
        default:
            return CMP_FORMAT_ARGB_16;
        }
    case CF_2101010:
        return CMP_FORMAT_ARGB_2101010;
    case CF_1010102:
        return CMP_FORMAT_RGBA_1010102;

#ifdef ARGB_32_SUPPORT
    case CF_32bit:
        switch(pMipSet->m_TextureDataType) {
        case TDT_R:
            return CMP_FORMAT_R_32;
        case TDT_RG:
            return CMP_FORMAT_RG_32;
        default:
            return CMP_FORMAT_ARGB_32;
        }
#endif // ARGB_32_SUPPORT

    default:
        return CMP_FORMAT_Unknown;
    }
}