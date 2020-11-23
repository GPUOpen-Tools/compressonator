//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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

#ifndef _PLUGIN_IMAGE_KTX2_H
#define _PLUGIN_IMAGE_KTX2_H

#include "plugininterface.h"
#include "stdint.h"
#include "ktx.h"
#include "ktxint.h"

#ifdef _WIN32
// {D755E017-0485-466E-83AF-58BF5DDA8F05}
static const GUID g_GUID = {0xd755e017, 0x485, 0x466e, {0x83, 0xaf, 0x58, 0xbf, 0x5d, 0xda, 0x8f, 0x5}};

#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR 1
#define TC_PLUGIN_VERSION_MINOR 0

class Plugin_KTX2 : public PluginInterface_Image
{
public:
    Plugin_KTX2();
    virtual ~Plugin_KTX2();

    int TC_PluginSetSharedIO(void* Shared);
    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet);
    int TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet);
    int TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture* srcTexture);
    int TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture* srcTexture);
};


struct CMP_DFD
{
    uint32_t byteLength;
    uint32_t byteOffset;
    uint32_t cmp_format;
};

typedef struct _DFDSampleType
{
    uint32_t bitOffset : 16;
    uint32_t bitLength : 8;
    uint32_t channelType : 8;  // Includes qualifiers
    uint32_t samplePosition0 : 8;
    uint32_t samplePosition1 : 8;
    uint32_t samplePosition2 : 8;
    uint32_t samplePosition3 : 8;
    uint32_t lower;
    uint32_t upper;
} DFDSampleType;


#define IDS_ERROR_FILE_OPEN 1
#define IDS_ERROR_NOT_KTX 2
#define IDS_ERROR_UNSUPPORTED_TYPE 3
#define IDS_ERROR_ALLOCATEMIPSET 4
#define IDS_ERROR_ALLOCATEMIPSLEVELDATA 5

extern void* make_Plugin_KTX2();

// uint8_t FileIdentifier[12] = {0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};

#define R_ATI1N_UNorm 0x8DBB          // GL_COMPRESSED_RED_RGTC1
#define R_ATI1N_SNorm 0x8DBC          // GL_COMPRESSED_SIGNED_RED_RGTC1
#define RG_ATI2N_UNorm 0x8DBD         // GL_COMPRESSED_RG_RGTC2
#define RG_ATI2N_SNorm 0x8DBE         // GL_COMPRESSED_SIGNED_RG_RGTC2
#define RGB_BP_UNSIGNED_FLOAT 0x8E8F  // GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB  //bc6
#define RGB_BP_SIGNED_FLOAT 0x8E8E    // GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
#define RGB_BP_UNorm 0x8E8C           // GL_COMPRESSED_RGBA_BPTC_UNORM_ARB  //bc7
#define ATC_RGB_AMD 0x8C92
#define ATC_RGBA_EXPLICIT_ALPHA_AMD 0x8C93
#define ATC_RGBA_INTERPOLATED_ALPHA_AMD 0x87EE
#define COMPRESSED_RED_RGTC1 0x8DBB  //bc4
#define COMPRESSED_RG_RGTC2 0x8DBD   //bc5 ATI2_XY
#define ETC1_RGB8_OES 0x8D64

// Not supported by GL_COMPRESSED_  Using Custom Temp extension ranges
#define COMPRESSED_FORMAT_DXT5_xGBR_TMP 0x6000
#define COMPRESSED_FORMAT_DXT5_RxBG_TMP 0x6001
#define COMPRESSED_FORMAT_DXT5_RBxG_TMP 0x6002
#define COMPRESSED_FORMAT_DXT5_xRBG_TMP 0x6003
#define COMPRESSED_FORMAT_DXT5_RGxB_TMP 0x6004
#define COMPRESSED_FORMAT_DXT5_xGxR_TMP 0x6005
#define COMPRESSED_FORMAT_ATI1N_UNorm_TMP    0x6007
#define COMPRESSED_FORMAT_ATI1N_SNorm_TMP    0x6008
#define COMPRESSED_FORMAT_ATI2N_UNorm_TMP    0x6009
#define COMPRESSED_FORMAT_ATI2N_SNorm_TMP    0x6010
#define COMPRESSED_FORMAT_ATI2N_XY_UNorm_TMP 0x6011


//----------------------------------------------------------------
// Definitions from etcpack v2.74
//----------------------------------------------------------------

// #define GL_SRGB                                          0x8C40
// #define GL_SRGB8                                         0x8C41
// #define GL_SRGB8_ALPHA8                                  0x8C43
// #define GL_COMPRESSED_R11_EAC                            0x9270
// #define GL_COMPRESSED_SIGNED_R11_EAC                     0x9271
// #define GL_COMPRESSED_RG11_EAC                           0x9272
// #define GL_COMPRESSED_SIGNED_RG11_EAC                    0x9273

#define GL_COMPRESSED_RGB8_ETC2 0x9274
#define GL_COMPRESSED_SRGB8_ETC2 0x9275
#define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC 0x9279
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9276
#define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 0x9277

//// enums copied from GL/GL.h
//#define GL_RED                                            0x1903
//#define GL_RG                                             0x8227
//#define GL_RGB                                            0x1907
//#define GL_RGBA                                           0x1908
//#define GL_BGR                                            0x80E0
//#define GL_BGRA                                           0x80E1
//#define GL_LUMINANCE                                      0x1909
//#define GL_LUMINANCE_ALPHA                                0x190A
//#define GL_UNSIGNED_BYTE                                  0x1401
//#define GL_UNSIGNED_SHORT                                 0x1403
//#define GL_HALF_FLOAT                                     0x140B
//#define GL_FLOAT                                          0x1406

#endif
