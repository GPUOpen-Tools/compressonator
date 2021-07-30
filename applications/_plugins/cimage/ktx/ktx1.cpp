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

#ifdef _WIN32
#include <windows.h>
#endif

#include "ktx1.h"
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "common.h"
#include "softfloat.h"

#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)                 //&& !defined(NO_LEGACY_BEHAVIOR)
#pragma comment(lib, "glew32.lib")  // glew
#else
#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "libglew32d.lib")  // glew
#else
#pragma comment(lib, "libglew32.lib")  // glew
#endif
#else
#pragma comment(lib, "libglew32.lib")  // glew
#endif
#endif

using namespace std;

using namespace std;

CMIPS *KTX_CMips;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_KTX)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("KTX")
#else
void *make_Plugin_KTX() { return new Plugin_KTX; } 
#endif

uint32_t Endian_Conversion(uint32_t dword)
{
    return (((dword>>24)&0x000000FF) | ((dword>>8)&0x0000FF00) | ((dword<<8)&0x00FF0000) | ((dword<<24)&0xFF000000));
}

Plugin_KTX::Plugin_KTX()
{ 
}

Plugin_KTX::~Plugin_KTX()
{ 
}

int Plugin_KTX::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        KTX_CMips = static_cast<CMIPS *>(Shared);
        return 0;
    }
    return 1;
}

int Plugin_KTX::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{ 
#ifdef _WIN32
    pPluginVersion->guid                    = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor        = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor        = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_KTX::TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture)
{
    return -1;
}

int Plugin_KTX::TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture)
{
    return -1;
}

uint32_t ktx_u32_byterev(uint32_t v)
{
    return (v >> 24) | ((v >> 8) & 0xFF00) | ((v << 8) & 0xFF0000) | (v << 24);
}

// perform endianness switch on raw data
static void switch_endianness2(void *dataptr, int bytes)
{
    int i;
    uint8_t *data = (uint8_t *) dataptr;

    for (i = 0; i < bytes / 2; i++)
    {
        uint8_t d0 = data[0];
        uint8_t d1 = data[1];
        data[0] = d1;
        data[1] = d0;
        data += 2;
    }
}

static void switch_endianness4(void *dataptr, int bytes)
{
    int i;
    uint8_t *data = (uint8_t *) dataptr;

    for (i = 0; i < bytes / 4; i++)
    {
        uint8_t d0 = data[0];
        uint8_t d1 = data[1];
        uint8_t d2 = data[2];
        uint8_t d3 = data[3];
        data[0] = d3;
        data[1] = d2;
        data[2] = d1;
        data[3] = d0;
        data += 4;
    }
}

static void copy_scanline(void *dst, const void *src, int pixels, int method)
{

#define id(x) (x)
#define u16_sf16(x) float_to_sf16( x * (1.0f/65535.0f), SF_NEARESTEVEN )
#define f32_sf16(x) sf32_to_sf16( x, SF_NEARESTEVEN )

#define COPY_R( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[i]); \
        d[4*i+1] = 0; \
        d[4*i+2] = 0; \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_RG( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[2*i]); \
        d[4*i+1] = convfunc(s[2*i+1]); \
        d[4*i+2] = 0; \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_RGB( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[3*i]); \
        d[4*i+1] = convfunc(s[3*i+1]); \
        d[4*i+2] = convfunc(s[3*i+2]); \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_BGR( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[3*i+2]); \
        d[4*i+1] = convfunc(s[3*i+1]); \
        d[4*i+2] = convfunc(s[3*i]); \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_RGBX( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[4*i]); \
        d[4*i+1] = convfunc(s[4*i+1]); \
        d[4*i+2] = convfunc(s[4*i+2]); \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_BGRX( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[4*i+2]); \
        d[4*i+1] = convfunc(s[4*i+1]); \
        d[4*i+2] = convfunc(s[4*i]); \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_RGBA( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[4*i]); \
        d[4*i+1] = convfunc(s[4*i+1]); \
        d[4*i+2] = convfunc(s[4*i+2]); \
        d[4*i+3] = convfunc(s[4*i+3]); \
        } \
    } while(0); \
    break;

#define COPY_BGRA( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[4*i+2]); \
        d[4*i+1] = convfunc(s[4*i+1]); \
        d[4*i+2] = convfunc(s[4*i]); \
        d[4*i+3] = convfunc(s[4*i+3]); \
        } \
    } while(0); \
    break;

#define COPY_L( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[i]); \
        d[4*i+1] = convfunc(s[i]); \
        d[4*i+2] = convfunc(s[i]); \
        d[4*i+3] = oneval; \
        } \
    } while(0); \
    break;

#define COPY_LA( dsttype, srctype, convfunc, oneval ) \
    do { \
    srctype *s = (srctype *)src; \
    dsttype *d = (dsttype *)dst; \
    for(i=0;i<pixels;i++)\
        {\
        d[4*i] = convfunc(s[2*i]); \
        d[4*i+1] = convfunc(s[2*i]); \
        d[4*i+2] = convfunc(s[2*i]); \
        d[4*i+3] = convfunc(s[2*i+1]); \
        } \
    } while(0); \
    break;

    int i;
    switch (method)
    {
    case R8_TO_RGBA8:
        COPY_R(uint8_t, uint8_t, id, 0xFF);
    case RG8_TO_RGBA8:
        COPY_RG(uint8_t, uint8_t, id, 0xFF);
    case RGB8_TO_RGBA8:
        COPY_RGB(uint8_t, uint8_t, id, 0xFF);
    case RGBA8_TO_RGBA8:
        COPY_RGBA(uint8_t, uint8_t, id, 0xFF);
    case BGR8_TO_RGBA8:
        COPY_BGR(uint8_t, uint8_t, id, 0xFF);
    case BGRA8_TO_RGBA8:
        COPY_BGRA(uint8_t, uint8_t, id, 0xFF);
    case RGBX8_TO_RGBA8:
        COPY_RGBX(uint8_t, uint8_t, id, 0xFF);
    case BGRX8_TO_RGBA8:
        COPY_BGRX(uint8_t, uint8_t, id, 0xFF);
    case L8_TO_RGBA8:
        COPY_L(uint8_t, uint8_t, id, 0xFF);
    case LA8_TO_RGBA8:
        COPY_LA(uint8_t, uint8_t, id, 0xFF);

    case R16F_TO_RGBA16F:
        COPY_R(uint16_t, uint16_t, id, 0x3C00);
    case RG16F_TO_RGBA16F:
        COPY_RG(uint16_t, uint16_t, id, 0x3C00);
    case RGB16F_TO_RGBA16F:
        COPY_RGB(uint16_t, uint16_t, id, 0x3C00);
    case RGBA16F_TO_RGBA16F:
        COPY_RGBA(uint16_t, uint16_t, id, 0x3C00);
    case BGR16F_TO_RGBA16F:
        COPY_BGR(uint16_t, uint16_t, id, 0x3C00);
    case BGRA16F_TO_RGBA16F:
        COPY_BGRA(uint16_t, uint16_t, id, 0x3C00);
    case L16F_TO_RGBA16F:
        COPY_L(uint16_t, uint16_t, id, 0x3C00);
    case LA16F_TO_RGBA16F:
        COPY_LA(uint16_t, uint16_t, id, 0x3C00);

    case R16_TO_RGBA16F:
        COPY_R(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case RG16_TO_RGBA16F:
        COPY_RG(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case RGB16_TO_RGBA16F:
        COPY_RGB(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case RGBA16_TO_RGBA16F:
        COPY_RGBA(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case BGR16_TO_RGBA16F:
        COPY_BGR(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case BGRA16_TO_RGBA16F:
        COPY_BGRA(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case L16_TO_RGBA16F:
        COPY_L(uint16_t, uint16_t, u16_sf16, 0x3C00);
    case LA16_TO_RGBA16F:
        COPY_LA(uint16_t, uint16_t, u16_sf16, 0x3C00);

    case R32F_TO_RGBA16F:
        COPY_R(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case RG32F_TO_RGBA16F:
        COPY_RG(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case RGB32F_TO_RGBA16F:
        COPY_RGB(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case RGBA32F_TO_RGBA16F:
        COPY_RGBA(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case BGR32F_TO_RGBA16F:
        COPY_BGR(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case BGRA32F_TO_RGBA16F:
        COPY_BGRA(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case L32F_TO_RGBA16F:
        COPY_L(uint16_t, uint32_t, f32_sf16, 0x3C00);
    case LA32F_TO_RGBA16F:
        COPY_LA(uint16_t, uint32_t, f32_sf16, 0x3C00);
    };
}

int Plugin_KTX::TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet)
{
    FILE* pFile = NULL;
    pFile = fopen(pszFilename, ("rb"));
    if (pFile == NULL)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) opening file = %s \n"), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    //using libktx
    KTX_header fheader;
    KTX_texinfo texinfo;
    if (fread(&fheader, sizeof(KTX_header), 1, pFile) != 1)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) invalid KTX header. Filename = %s \n"), EL_Error, IDS_ERROR_NOT_KTX, pszFilename);
        fclose(pFile);
        return -1;
    }

    if (_ktxCheckHeader(&fheader, &texinfo) != KTX_SUCCESS)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) invalid KTX header. Filename = %s \n"), EL_Error, IDS_ERROR_NOT_KTX, pszFilename);
        fclose(pFile);
        return -1;
    }

    if (texinfo.compressed)
    {
        pMipSet->m_compressed   = true;
        pMipSet->m_nBlockHeight = 4;
        pMipSet->m_nBlockWidth  = 4;
        pMipSet->m_nBlockDepth  = 1;
        pMipSet->m_ChannelFormat = CF_Compressed;

        switch (fheader.glInternalFormat)
        {
        case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            pMipSet->m_format = CMP_FORMAT_BC1;
            pMipSet->m_TextureDataType = TDT_XRGB;
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            pMipSet->m_format = CMP_FORMAT_BC1;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            pMipSet->m_format = CMP_FORMAT_BC2;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            pMipSet->m_format = CMP_FORMAT_BC3;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RED_RGTC1:
            //pMipSet->m_format = CMP_FORMAT_ATI1N;
            pMipSet->m_format            = CMP_FORMAT_BC4;
            pMipSet->m_TextureDataType   = TDT_ARGB;
            break;
        case GL_COMPRESSED_SIGNED_RED_RGTC1:
            //pMipSet->m_format = CMP_FORMAT_ATI2N;
            pMipSet->m_format            = CMP_FORMAT_BC4_S;
            pMipSet->m_TextureDataType   = TDT_ARGB;
            break;
        case GL_COMPRESSED_RG_RGTC2:
            //pMipSet->m_format = CMP_FORMAT_ATI2N_XY;
            pMipSet->m_format            = CMP_FORMAT_BC5;
            pMipSet->m_TextureDataType   = TDT_ARGB;
            break;
        case GL_COMPRESSED_SIGNED_RG_RGTC2:
            //pMipSet->m_format = CMP_FORMAT_ATI2N_DXT5;
            pMipSet->m_format            = CMP_FORMAT_BC5_S;
            pMipSet->m_TextureDataType   = TDT_ARGB;
            break;
        case RGB_BP_UNorm:
            pMipSet->m_format = CMP_FORMAT_BC7;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case RGB_BP_UNSIGNED_FLOAT:
            pMipSet->m_format = CMP_FORMAT_BC6H;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case RGB_BP_SIGNED_FLOAT:
            pMipSet->m_format = CMP_FORMAT_BC6H_SF;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case ATC_RGB_AMD:
            pMipSet->m_format = CMP_FORMAT_ATC_RGB;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case ATC_RGBA_EXPLICIT_ALPHA_AMD:
            pMipSet->m_format = CMP_FORMAT_ATC_RGBA_Explicit;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case ATC_RGBA_INTERPOLATED_ALPHA_AMD: 
            pMipSet->m_format = CMP_FORMAT_ATC_RGBA_Interpolated;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_4x4_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 4;
            pMipSet->m_nBlockHeight = 4;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_5x4_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 5;
            pMipSet->m_nBlockHeight = 4;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_5x5_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 5;
            pMipSet->m_nBlockHeight = 5;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_6x5_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 6;
            pMipSet->m_nBlockHeight = 5;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_6x6_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 6;
            pMipSet->m_nBlockHeight = 6;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_8x5_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 8;
            pMipSet->m_nBlockHeight = 5;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_8x6_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 8;
            pMipSet->m_nBlockHeight = 6;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_10x5_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 10;
            pMipSet->m_nBlockHeight = 5;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_10x6_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 10;
            pMipSet->m_nBlockHeight = 6;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_8x8_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 8;
            pMipSet->m_nBlockHeight = 8;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_10x8_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 10;
            pMipSet->m_nBlockHeight = 8;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_10x10_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 10;
            pMipSet->m_nBlockHeight = 10;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_12x10_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 12;
            pMipSet->m_nBlockHeight = 10;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA_ASTC_12x12_KHR:
            pMipSet->m_format = CMP_FORMAT_ASTC;
            pMipSet->m_nBlockWidth = 12;
            pMipSet->m_nBlockHeight = 12;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case ETC1_RGB8_OES: 
            pMipSet->m_format = CMP_FORMAT_ETC_RGB;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGB8_ETC2:
            pMipSet->m_format = CMP_FORMAT_ETC2_RGB;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_SRGB8_ETC2:
            pMipSet->m_format = CMP_FORMAT_ETC2_SRGB;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGBA8_ETC2_EAC:
            pMipSet->m_format = CMP_FORMAT_ETC2_RGBA;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            pMipSet->m_format = CMP_FORMAT_ETC2_RGBA1;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
            pMipSet->m_format = CMP_FORMAT_ETC2_SRGBA;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            pMipSet->m_format = CMP_FORMAT_ETC2_SRGBA1;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_FORMAT_DXT5_RxBG :
            pMipSet->m_format = CMP_FORMAT_DXT5_RxBG;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_FORMAT_DXT5_RBxG :
            pMipSet->m_format = CMP_FORMAT_DXT5_RBxG;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_FORMAT_DXT5_xRBG :
            pMipSet->m_format = CMP_FORMAT_DXT5_xRBG;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_FORMAT_DXT5_RGxB :
            pMipSet->m_format = CMP_FORMAT_DXT5_RGxB;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
        case COMPRESSED_FORMAT_DXT5_xGxR :
            pMipSet->m_format = CMP_FORMAT_DXT5_xGxR;
            pMipSet->m_TextureDataType = TDT_ARGB;
            break;
           default:
               if (KTX_CMips)
                   KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) unsupported GL format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
              fclose(pFile);
              return -1;
        }
    }
    else
    {
        pMipSet->m_compressed = false;
        switch (fheader.glType) 
        {
        case GL_UNSIGNED_BYTE:
            pMipSet->m_ChannelFormat = CF_8bit;
            switch (fheader.glFormat)
            {
            case GL_RED:
                pMipSet->m_format = CMP_FORMAT_R_8;
                pMipSet->m_TextureDataType = TDT_R;
                break;
            case GL_RG:
                pMipSet->m_format = CMP_FORMAT_RG_8;
                pMipSet->m_TextureDataType = TDT_RG;
                break;
            case GL_RGB:
                pMipSet->m_format = CMP_FORMAT_RGB_888;
                pMipSet->m_TextureDataType = TDT_RGB;
                break;
            case GL_RGBA:
            case GL_RGBA8:
                pMipSet->m_format = CMP_FORMAT_ARGB_8888;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            case GL_BGR: // CMP Needs a new format to handle this correctly (swizzled)
                pMipSet->m_swizzle = true;
                pMipSet->m_format = CMP_FORMAT_RGB_888;
                pMipSet->m_TextureDataType = TDT_RGB;
                break;
            case GL_BGRA:
                pMipSet->m_swizzle = true;
                pMipSet->m_format = CMP_FORMAT_ARGB_8888;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            }
            break;
        case GL_UNSIGNED_SHORT:
            pMipSet->m_ChannelFormat = CF_16bit;
            switch (fheader.glFormat)
            {
            case GL_RED:
                pMipSet->m_format = CMP_FORMAT_R_16;
                pMipSet->m_TextureDataType = TDT_R;
                break;
            case GL_RG:
                pMipSet->m_format = CMP_FORMAT_RG_16;
                pMipSet->m_TextureDataType = TDT_RG;
                break;
            case GL_RGBA:
                pMipSet->m_format = CMP_FORMAT_ARGB_16;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            case GL_BGRA:
                pMipSet->m_swizzle = true;
                pMipSet->m_format = CMP_FORMAT_ARGB_16;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            }
            break;
        case GL_HALF_FLOAT:
            pMipSet->m_ChannelFormat = CF_Float16;
            switch (fheader.glFormat)
            {
            case GL_RED:
                pMipSet->m_format = CMP_FORMAT_R_16F;
                pMipSet->m_TextureDataType = TDT_R;
                break;
            case GL_RG:
                pMipSet->m_format = CMP_FORMAT_RG_16F;
                pMipSet->m_TextureDataType = TDT_RG;
                break;
            case GL_RGBA:
                pMipSet->m_format = CMP_FORMAT_ARGB_16F;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            case GL_BGRA:
                pMipSet->m_swizzle = true;
                pMipSet->m_format = CMP_FORMAT_ARGB_16F;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            }
            break;
        case GL_UNSIGNED_INT_2_10_10_10_REV:
            pMipSet->m_format = CMP_FORMAT_ARGB_2101010;
            pMipSet->m_TextureDataType = TDT_ARGB;
            pMipSet->m_ChannelFormat = CF_2101010;
            break;
        case GL_FLOAT:
            pMipSet->m_ChannelFormat = CF_Float32;
            switch (fheader.glFormat)
            {
            case GL_RED:
                pMipSet->m_format = CMP_FORMAT_R_32F;
                pMipSet->m_TextureDataType = TDT_R;
                break;
            case GL_RG:
                pMipSet->m_format = CMP_FORMAT_RG_32F;
                pMipSet->m_TextureDataType = TDT_RG;
                break;
            case GL_RGBA:
                pMipSet->m_format = CMP_FORMAT_ARGB_32F;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            case GL_BGRA:
                pMipSet->m_swizzle = true;
                pMipSet->m_format = CMP_FORMAT_ARGB_32F;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            }
            break;
            break;
        default:
            if (KTX_CMips)
                KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) unsupported GL format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
            fclose(pFile);
            return -1;
        }
    }

    switch (texinfo.glTarget) 
    {
        case GL_TEXTURE_2D:
            pMipSet->m_TextureType = TT_2D;
            break;
        case GL_TEXTURE_3D:
            pMipSet->m_TextureType = TT_VolumeTexture;
            break;
        case GL_TEXTURE_CUBE_MAP:
            pMipSet->m_TextureType = TT_CubeMap;
            break;
        default:
            if (KTX_CMips)
                KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) unsupported texture format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, texinfo.glTarget);
            fclose(pFile);
            return -1;
    }

    if (fheader.numberOfMipmapLevels == 0) fheader.numberOfMipmapLevels = 1;
    //if (fheader.numberOfArrayElements != 0)
    //{
    //    if (KTX_CMips)
    //        KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) array textures not supported %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.numberOfArrayElements);
    //    fclose(pFile);
    //    return -1;
    //}
   

    pMipSet->m_nMipLevels = fheader.numberOfMipmapLevels;
    if (fheader.numberOfFaces < 1) fheader.numberOfFaces = 1; // depthsupport
    pMipSet->m_nDepth     = fheader.numberOfFaces;
    pMipSet->dwWidth      = fheader.pixelWidth;
    pMipSet->dwHeight     = fheader.pixelHeight;

    // Allocate MipSet header
    KTX_CMips->AllocateMipSet(pMipSet,
        pMipSet->m_ChannelFormat,
        pMipSet->m_TextureDataType,
        pMipSet->m_TextureType,
        fheader.pixelWidth,
        fheader.pixelHeight,
        pMipSet->m_nDepth               // depthsupport
    );

    if (pMipSet->m_nMipLevels > pMipSet->m_nMaxMipLevels)
    {
        pMipSet->m_nMipLevels = pMipSet->m_nMaxMipLevels;
    }

    fclose(pFile);
    pFile = NULL;
    pFile = fopen(pszFilename, "rb");
    if (pFile == NULL)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) loading file = %s "), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    //skip key value data
    int imageSizeOffset = sizeof(KTX_header) + fheader.bytesOfKeyValueData;
    if (fseek(pFile, imageSizeOffset, SEEK_SET))
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) Seek past key/vals in KTX compressed bitmap file failed. Format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
        fclose(pFile);
        return -1;
    }


    int w = pMipSet->m_nWidth;
    int h = pMipSet->m_nHeight;

    unsigned int totalByteRead=0;

    unsigned int faceSize = 0;
    unsigned int faceSizeRounded = 0;
    unsigned int numArrayElement = fheader.numberOfArrayElements;

    for (int nMipLevel = 0; nMipLevel < fheader.numberOfMipmapLevels; nMipLevel++)
    {
        if ((w <= 0) || (h <= 0)) break;

        totalByteRead = fread(&faceSize, 1, sizeof(khronos_uint32_t), pFile);
        if (totalByteRead == 0) {
            if (KTX_CMips)
                KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) Read image data size failed. Format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
            fclose(pFile);
            return -1;
        }
        if (fheader.endianness == KTX_ENDIAN_REF_REV) {
            _ktxSwapEndian32(&faceSize, 1);
        }
        faceSizeRounded = (faceSize + 3) & ~(khronos_uint32_t)3;

        for (int face = 0; face < fheader.numberOfFaces; ++face)
        {
            // Determine buffer size and set Mip Set Levels 
            MipLevel *pMipLevel = KTX_CMips->GetMipLevel(pMipSet, nMipLevel, face);
            if (pMipSet->m_compressed)
                KTX_CMips->AllocateCompressedMipLevelData(pMipLevel, w, h, faceSizeRounded);
            else
                KTX_CMips->AllocateMipLevelData(pMipLevel, w, h, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType);

            // handle padded KTX data
            // store size to be read at this miplevel to check with face size in KTX file

            int sizeTobeRead = 0;
            int numChannel = 0;
            int bytesPerChannel = 0;

            if (pMipSet->m_compressed)
            {
                sizeTobeRead = faceSizeRounded;
            }
            else 
            {
                switch (pMipSet->m_TextureDataType)
                {
                case TDT_XRGB:
                    numChannel = 3;
                    break;
                case TDT_ARGB:
                case TDT_NORMAL_MAP:
                    numChannel = 4;
                    break;

                case TDT_R:
                    numChannel = 1;
                    break;

                case TDT_RG:
                    numChannel = 2;
                    break;

                default:
                    return -1;
                }

                switch (pMipSet->m_ChannelFormat)
                {
                case CF_8bit:
                case CF_2101010:
                case CF_Float9995E:
                    bytesPerChannel = 1;
                    break;

                case CF_16bit:
                case CF_Float16:
                    bytesPerChannel = 2;
                    break;

                case CF_32bit:
                case CF_Float32:
                    bytesPerChannel = 4;
                    break;

                default:
                    return -1;
                }

                sizeTobeRead = w * h * numChannel * bytesPerChannel;
            }
            CMP_BYTE* pData = (CMP_BYTE*)(pMipLevel->m_pbData);

            if (!pData)
            {
                if (KTX_CMips)
                    KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) Read image data failed, Out of Memory. Format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
                fclose(pFile);
                return -1;
            }

            //read image data
            unsigned int bytesRead = 0;
            //size to be read has to be same as face size, else padding is done in the KTX file
            if (sizeTobeRead == faceSizeRounded)
            {
                bytesRead = fread(pData, 1, faceSizeRounded, pFile);
            }
            else if(faceSizeRounded > sizeTobeRead) // padding in KTX file
            {
                std::vector<CMP_BYTE> pTempData;
                pTempData.resize(faceSizeRounded);
                bytesRead = fread(pTempData.data(), 1, faceSizeRounded, pFile);
                int paddedBytes = faceSizeRounded - sizeTobeRead;

                int n = 0;
                for (int i = 0; i < pTempData.size(); i++)
                {
                    for (int w_i = 0; w_i < w; w_i++)
                    {
                        for (int j = 0; j < numChannel*bytesPerChannel; j++)
                        {
                            pData[n++] = pTempData[i++];
                        }
                    }
                    //skip padded bytes
                    i += (paddedBytes / h)-1;
                }
            }

            if (bytesRead != faceSizeRounded)
            {
                if (KTX_CMips)
                    KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) Read image data failed. Unexpectec EOF. Format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, fheader.glFormat);
                fclose(pFile);
                return -1;
            } 
        }
        // next miplevel width and height
        w = max(0, w >> 1);
        h = max(0, h >> 1);
    }

    return 0;
}

int Plugin_KTX::TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet)
{
    assert(pszFilename);
    assert(pMipSet);

    FILE* pFile = NULL;
    pFile = fopen(pszFilename, "wb");
    if (pFile == NULL)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    //using libktx
    KTX_texture_info textureinfo;
    KTX_image_info* inputMip = new KTX_image_info[pMipSet->m_nMipLevels];

    unsigned pDataLen = 0;
    CMP_BYTE* pData = NULL;
    bool isCompressed = false;

    if (pMipSet->m_TextureType == TT_CubeMap)
        textureinfo.numberOfFaces = 6;
    else
        textureinfo.numberOfFaces = 1;

    //todo: handle array textures
    textureinfo.numberOfArrayElements = 0;

    if (pMipSet->m_pMipLevelTable == NULL)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_ALLOCATEMIPSET, pszFilename);
        return -1;
    }

    if (KTX_CMips->GetMipLevel(pMipSet, 0) == NULL)
    {
        if (KTX_CMips)
            KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_ALLOCATEMIPSET, pszFilename);
        return -1;
    }

    int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : CMP_MaxFacesOrSlices(pMipSet, 0);
    
    for (int nSlice = 0; nSlice < nSlices; nSlice++)
    {
        for (int nMipLevel = 0; nMipLevel < pMipSet->m_nMipLevels; nMipLevel++)
        {
            inputMip[nMipLevel].size = KTX_CMips->GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize;
            inputMip[nMipLevel].data = KTX_CMips->GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData;
        }
    }

    switch (pMipSet->m_format)
    {
    //uncompressed format case
    case CMP_FORMAT_ARGB_8888 :
    case CMP_FORMAT_RGB_888 :
    case CMP_FORMAT_RG_8 :
    case CMP_FORMAT_R_8 :
        isCompressed = false;
        textureinfo.glType = GL_UNSIGNED_BYTE;
        textureinfo.glTypeSize = 1;
        break;
    case  CMP_FORMAT_ARGB_2101010 :      
        textureinfo.glType = GL_UNSIGNED_INT_2_10_10_10_REV;
        textureinfo.glTypeSize = 1;
        break;
    case  CMP_FORMAT_ARGB_16 :   
        textureinfo.glType = GL_UNSIGNED_SHORT;
        textureinfo.glTypeSize = 2;
        break;
    case  CMP_FORMAT_RG_16 :
        textureinfo.glType = GL_UNSIGNED_SHORT;
        textureinfo.glTypeSize = 2;
        break;
    case  CMP_FORMAT_R_16 :      
        textureinfo.glType = GL_UNSIGNED_SHORT;
        textureinfo.glTypeSize = 2;
        break;
    case  CMP_FORMAT_ARGB_16F :                 
    case  CMP_FORMAT_RG_16F :                   
    case  CMP_FORMAT_R_16F : 
        textureinfo.glType = GL_HALF_FLOAT;
        textureinfo.glTypeSize = 2;
        break;
    case  CMP_FORMAT_ARGB_32F :                 
    case  CMP_FORMAT_RGB_32F :                  
    case  CMP_FORMAT_RG_32F :                   
    case  CMP_FORMAT_R_32F :
        textureinfo.glType = GL_FLOAT;
        textureinfo.glTypeSize = 4;
        break;
    //compressed format case
    case CMP_FORMAT_ATI1N :                   
    case CMP_FORMAT_ATI2N :                   
    case CMP_FORMAT_ATI2N_XY :                
    case CMP_FORMAT_ATI2N_DXT5 :              
    case CMP_FORMAT_ATC_RGB :                 
    case CMP_FORMAT_ATC_RGBA_Explicit :       
    case CMP_FORMAT_ATC_RGBA_Interpolated :   
    case CMP_FORMAT_BC1 :
    case CMP_FORMAT_BC2 :
    case CMP_FORMAT_BC3 :
    case CMP_FORMAT_BC4 :
    case CMP_FORMAT_BC4_S:
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_BC5_S:
    case CMP_FORMAT_BC6H:
    case CMP_FORMAT_BC6H_SF:
    case CMP_FORMAT_BC7 :
    case CMP_FORMAT_DXT1 :
    case CMP_FORMAT_DXT3 :
    case CMP_FORMAT_DXT5 :
    case CMP_FORMAT_DXT5_xGBR :
    case CMP_FORMAT_DXT5_RxBG :
    case CMP_FORMAT_DXT5_RBxG :
    case CMP_FORMAT_DXT5_xRBG :
    case CMP_FORMAT_DXT5_RGxB :
    case CMP_FORMAT_DXT5_xGxR :
    case CMP_FORMAT_ETC_RGB :
    case CMP_FORMAT_ETC2_RGB:
    case CMP_FORMAT_ETC2_SRGB:
    case CMP_FORMAT_ETC2_RGBA:
    case CMP_FORMAT_ETC2_RGBA1:
    case CMP_FORMAT_ETC2_SRGBA:
    case CMP_FORMAT_ETC2_SRGBA1:
    case CMP_FORMAT_ASTC :
#ifdef USE_GTC
    case  CMP_FORMAT_GTC:
#endif
#ifdef USE_BASIS
    case  CMP_FORMAT_BASIS:
#endif
        isCompressed            = true;
        textureinfo.glType      = 0;
        textureinfo.glTypeSize  = 1;
        textureinfo.glFormat    = 0;
        break;
    //default case
    default:
        isCompressed = false;
        textureinfo.glType = GL_UNSIGNED_BYTE;
        textureinfo.glTypeSize = 1;
        break;
    }

    switch (pMipSet->m_TextureDataType)
    {
    case TDT_R:  //single component-- can be Luminance and Alpha case, here only cover R
    {
        if (!isCompressed) 
        {
            textureinfo.glFormat = textureinfo.glBaseInternalFormat = GL_RED;
            textureinfo.glInternalFormat = GL_RED;
        }
        else
        {
            textureinfo.glBaseInternalFormat = GL_RED;
            textureinfo.glInternalFormat = GL_RED;
        }
    }
    break;
    case TDT_RG:  //two component
    {
        if (!isCompressed)
        {
            textureinfo.glFormat = textureinfo.glBaseInternalFormat = GL_RG;
            textureinfo.glInternalFormat = GL_RG; 
        }
        else
        {
            textureinfo.glBaseInternalFormat = GL_RG;
            textureinfo.glInternalFormat = GL_COMPRESSED_RG;
        }
    }
    break;
    case TDT_XRGB:  //normally 3 component
    {
        if (!isCompressed)
        {
            textureinfo.glFormat = textureinfo.glBaseInternalFormat = GL_RGB;
            textureinfo.glInternalFormat = GL_RGB;
        }
        else
        {
          
            switch (pMipSet->m_format)
            {
                case CMP_FORMAT_BC1 :
                case CMP_FORMAT_DXT1:
                    textureinfo.glBaseInternalFormat = GL_RGB;
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
                    break;
                case CMP_FORMAT_ETC_RGB:
                    textureinfo.glBaseInternalFormat = GL_RGB;
                    textureinfo.glInternalFormat = ETC1_RGB8_OES;
                    break;
                case CMP_FORMAT_ETC2_RGB:
                    textureinfo.glBaseInternalFormat = GL_RGB;
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGB8_ETC2;
                    break;
                case CMP_FORMAT_ETC2_SRGB:
                    textureinfo.glBaseInternalFormat = GL_SRGB;
                    textureinfo.glInternalFormat = GL_COMPRESSED_SRGB8_ETC2;
                    break;
                default:
                    textureinfo.glBaseInternalFormat = GL_RGB;
                    textureinfo.glInternalFormat = GL_RGB; //other compressed format not found in qtangle header
                    break;
            }
        }
    }
    break;
    case TDT_ARGB:  //4 component
    {
        if (!isCompressed)
        {
            textureinfo.glFormat = textureinfo.glBaseInternalFormat = GL_RGBA;
            textureinfo.glInternalFormat = GL_RGBA8;
        }
        else
        {
            textureinfo.glBaseInternalFormat = GL_RGBA;
            switch (pMipSet->m_format)
            {
            case CMP_FORMAT_BC1:
            case CMP_FORMAT_DXT1:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                break;
            case CMP_FORMAT_BC2:
            case CMP_FORMAT_DXT3:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                break;
            case CMP_FORMAT_BC3:
            case CMP_FORMAT_DXT5:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                break;
            case CMP_FORMAT_BC4:
                textureinfo.glInternalFormat = GL_COMPRESSED_RED_RGTC1;
                break;
            case CMP_FORMAT_BC4_S:
                textureinfo.glInternalFormat = GL_COMPRESSED_SIGNED_RED_RGTC1;
                break;
            case CMP_FORMAT_BC5:
                textureinfo.glInternalFormat = GL_COMPRESSED_RG_RGTC2;
                break;
            case CMP_FORMAT_BC5_S:
                textureinfo.glInternalFormat = GL_COMPRESSED_SIGNED_RG_RGTC2;
                break;
            case CMP_FORMAT_BC6H:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
                break;
            case CMP_FORMAT_BC6H_SF:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
                break;
            case CMP_FORMAT_BC7:
                textureinfo.glInternalFormat = RGB_BP_UNorm; 
                break;
            case  CMP_FORMAT_ATI1N:
                textureinfo.glInternalFormat = R_ATI1N_UNorm;
                break;
            case  CMP_FORMAT_ATI2N:
                textureinfo.glInternalFormat = R_ATI1N_SNorm;
                break;
            case  CMP_FORMAT_ATI2N_XY:
                textureinfo.glInternalFormat = RG_ATI2N_UNorm;
                break;
            case  CMP_FORMAT_ATI2N_DXT5:
                textureinfo.glInternalFormat = RG_ATI2N_SNorm;
                break;
            case  CMP_FORMAT_ATC_RGB:
                textureinfo.glInternalFormat = ATC_RGB_AMD;
                break;
            case  CMP_FORMAT_ATC_RGBA_Explicit:
                textureinfo.glInternalFormat = ATC_RGBA_EXPLICIT_ALPHA_AMD;
                break;
            case  CMP_FORMAT_ATC_RGBA_Interpolated:
                textureinfo.glInternalFormat = ATC_RGBA_INTERPOLATED_ALPHA_AMD;
                break;

            case  CMP_FORMAT_ASTC:
                if ((pMipSet->m_nBlockWidth == 4) && (pMipSet->m_nBlockHeight == 4))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
                else if ((pMipSet->m_nBlockWidth == 5) && (pMipSet->m_nBlockHeight == 4))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
                else if ((pMipSet->m_nBlockWidth == 5) && (pMipSet->m_nBlockHeight == 5))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
                else if ((pMipSet->m_nBlockWidth == 6) && (pMipSet->m_nBlockHeight == 5))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
                else if ((pMipSet->m_nBlockWidth == 6) && (pMipSet->m_nBlockHeight == 6))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
                else if ((pMipSet->m_nBlockWidth == 8) && (pMipSet->m_nBlockHeight == 5))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_8x5_KHR;
                else if ((pMipSet->m_nBlockWidth == 8) && (pMipSet->m_nBlockHeight == 6))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_8x6_KHR;
                else if ((pMipSet->m_nBlockWidth == 8) && (pMipSet->m_nBlockHeight == 8))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
                else if ((pMipSet->m_nBlockWidth == 10) && (pMipSet->m_nBlockHeight == 5))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x5_KHR;
                else if ((pMipSet->m_nBlockWidth == 10) && (pMipSet->m_nBlockHeight == 6))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x6_KHR;
                else if ((pMipSet->m_nBlockWidth == 10) && (pMipSet->m_nBlockHeight == 8))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x8_KHR;
                else if ((pMipSet->m_nBlockWidth == 10) && (pMipSet->m_nBlockHeight == 10))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
                else if ((pMipSet->m_nBlockWidth == 12) && (pMipSet->m_nBlockHeight == 10))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_12x10_KHR;
                else if ((pMipSet->m_nBlockWidth == 12) && (pMipSet->m_nBlockHeight == 12))
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
                else 
                    textureinfo.glInternalFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
                break;
            case CMP_FORMAT_ETC_RGB:
                textureinfo.glInternalFormat = ETC1_RGB8_OES;
                break;
            case CMP_FORMAT_ETC2_RGB:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGB8_ETC2;
                break;
            case CMP_FORMAT_ETC2_SRGB:
                textureinfo.glInternalFormat = GL_COMPRESSED_SRGB8_ETC2;
                break;
            case CMP_FORMAT_ETC2_RGBA:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGBA8_ETC2_EAC;
                break;
            case CMP_FORMAT_ETC2_RGBA1:
                textureinfo.glInternalFormat = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
                break;
            case CMP_FORMAT_ETC2_SRGBA:
                textureinfo.glBaseInternalFormat = GL_SRGB8_ALPHA8;
                textureinfo.glInternalFormat = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
                break;
            case CMP_FORMAT_ETC2_SRGBA1:
                textureinfo.glBaseInternalFormat = GL_SRGB8_ALPHA8;
                textureinfo.glInternalFormat = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
                break;
            case CMP_FORMAT_DXT5_xGBR:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_xGBR;
                break;
            case CMP_FORMAT_DXT5_RxBG:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_RxBG;
                break;
            case CMP_FORMAT_DXT5_RBxG:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_RBxG;
                break;
            case CMP_FORMAT_DXT5_xRBG:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_xRBG;
                break;
            case CMP_FORMAT_DXT5_RGxB:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_RGxB;
                break;
            case CMP_FORMAT_DXT5_xGxR:
                textureinfo.glInternalFormat = COMPRESSED_FORMAT_DXT5_xGxR;
                break;
                
            }
        }
    }
    break;
    default:
        break;
    }
    
    textureinfo.pixelWidth = pMipSet->m_nWidth;
    textureinfo.pixelHeight = pMipSet->m_nHeight;
    textureinfo.pixelDepth = 0; //for 1D, 2D and cube texture , depth =0;

    //1D 
    if (pMipSet->m_nHeight == 1 && pData != NULL) {
        delete(pData);
        pData = NULL;
        pDataLen = 0;
    }

    textureinfo.numberOfMipmapLevels = pMipSet->m_nMipLevels;

    KTX_error_code save = ktxWriteKTXF(pFile, &textureinfo, pDataLen, pData, pMipSet->m_nMipLevels, inputMip);
    if (save == KTX_SUCCESS) {
        fclose(pFile);
    }
    else {
        if (KTX_CMips)
            KTX_CMips->PrintError(("Error(%d): KTX Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        fclose(pFile);
        return -1;
    }

    return 0;
}


