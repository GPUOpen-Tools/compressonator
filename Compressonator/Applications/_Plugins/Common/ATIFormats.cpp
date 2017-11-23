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

#include <stdio.h>
#include <cctype>
#ifdef _WIN32
#include <tchar.h>
#endif

#include "Compressonator.h"
#include "ATIFormats.h"

CMP_FormatDesc g_FormatDesc[] =
{
   {CMP_FORMAT_Unknown,                "Unknown"},
   {CMP_FORMAT_ARGB_8888,               "ARGB_8888"},
   {CMP_FORMAT_RGB_888,                 "RGB_888"},
   {CMP_FORMAT_RG_8,                    "RG_8"},
   {CMP_FORMAT_R_8,                     "R_8"},
   {CMP_FORMAT_ARGB_2101010,            "ARGB_2101010"},
   {CMP_FORMAT_ARGB_16,                 "ARGB_16"},
   {CMP_FORMAT_RG_16,                   "RG_16"},
   {CMP_FORMAT_R_16,                    "R_16"},
   {CMP_FORMAT_ARGB_16F,                "ARGB_16F"},
   {CMP_FORMAT_RG_16F,                  "RG_16F"},
   {CMP_FORMAT_R_16F,                   "R_16F"},
   {CMP_FORMAT_ARGB_32F,                "ARGB_32F"},
   {CMP_FORMAT_RG_32F,                  "RG_32F"},
   {CMP_FORMAT_R_32F,                   "R_32F"},
   {CMP_FORMAT_DXT1,                    "DXT1"},
   {CMP_FORMAT_DXT3,                    "DXT3"},
   {CMP_FORMAT_DXT5,                    "DXT5"},
   {CMP_FORMAT_DXT5_xGBR,               "DXT5_XGBR"},
   {CMP_FORMAT_DXT5_RxBG,               "DXT5_RXBG"},
   {CMP_FORMAT_DXT5_RBxG,               "DXT5_RBXG"},
   {CMP_FORMAT_DXT5_xRBG,               "DXT5_XRBG"},
   {CMP_FORMAT_DXT5_RGxB,               "DXT5_RGXB"},
   {CMP_FORMAT_DXT5_xGxR,               "DXT5_XGXR"},
   {CMP_FORMAT_ATI1N,                   "ATI1N"},
   {CMP_FORMAT_ATI2N,                   "ATI2N"},
   {CMP_FORMAT_ATI2N_XY,                "ATI2N_XY"},
   {CMP_FORMAT_ATI2N_DXT5,              "ATI2N_DXT5"},
   {CMP_FORMAT_BC1,                     "BC1"},
   {CMP_FORMAT_BC2,                     "BC2"},
   {CMP_FORMAT_BC3,                     "BC3"},
   {CMP_FORMAT_BC4,                     "BC4"},
   {CMP_FORMAT_BC5,                     "BC5"},
   {CMP_FORMAT_ATC_RGB,                 "ATC_RGB"},
   {CMP_FORMAT_ATC_RGBA_Explicit,       "ATC_RGBA_EXPLICIT"},
   {CMP_FORMAT_ATC_RGBA_Interpolated,   "ATC_RGBA_INTERPOLATED"},
   {CMP_FORMAT_ETC_RGB,                 "ETC_RGB"},
   {CMP_FORMAT_ETC2_RGB,                "ETC2_RGB" },
   {CMP_FORMAT_BC6H,                    "BC6H"},
   {CMP_FORMAT_BC6H_SF,                 "BC6H_SF" },
   {CMP_FORMAT_BC7,                     "BC7"},
   {CMP_FORMAT_ASTC,                    "ASTC"},
#ifdef USE_GT
   { CMP_FORMAT_GT,                      "GT" },
#endif
};


CMP_DWORD g_dwFormatDescCount = sizeof(g_FormatDesc) / sizeof(g_FormatDesc[0]);

CMP_FORMAT ParseFormat(char* pFormat)
{
   if(pFormat == NULL)
      return CMP_FORMAT_Unknown;

   char pszFormat[128];

   int i = 0;
   for (char *iter = pFormat; *iter != '\0'; iter++)
   {
       if (i < 127)
           pszFormat[i++] = (char)std::toupper(*iter);
   }

   pszFormat[i] = '\0';
   
   for (CMP_DWORD j = 0; j < g_dwFormatDescCount; j++) {
       if (strcmp(pszFormat, g_FormatDesc[j].pszFormatDesc) == 0)
           return g_FormatDesc[j].nFormat;
   }

   return CMP_FORMAT_Unknown;
}

CMP_CHAR* GetFormatDesc(CMP_FORMAT nFormat)
{
   for(CMP_DWORD i = 0; i < g_dwFormatDescCount; i++)
      if(nFormat == g_FormatDesc[i].nFormat)
         return g_FormatDesc[i].pszFormatDesc;

   return g_FormatDesc[0].pszFormatDesc;
}
