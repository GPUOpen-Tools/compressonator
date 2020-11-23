//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
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
//--------------------------------------------------------------------------------------
// This header defines constants and structures that are useful when parsing
// DDS files.  DDS files were originally designed to use several structures
// and constants that are native to DirectDraw and are defined in ddraw.h,
// such as DDSURFACEDESC2 and DDSCAPS2.  This file defines similar
// (compatible) constants and structures so that one can use DDS files
// without needing to include ddraw.h.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------

#ifndef _DDS_FILE_H
#define _DDS_FILE_H

#include "plugininterface.h"

#ifdef _WIN32
#include "ddraw.h"
#include "d3d9types.h"
#endif

#define PLUGIN_NAME ("DDS Plugin")

#ifndef _WIN32

typedef struct _DDCOLORKEY
{
    CMP_DWORD dwColorSpaceLowValue;  // low boundary of color space that is to
    // be treated as Color Key, inclusive
    CMP_DWORD dwColorSpaceHighValue;  // high boundary of color space that is
    // to be treated as Color Key, inclusive
} DDCOLORKEY;

typedef struct _DDSCAPS2
{
    CMP_DWORD dwCaps;  // capabilities of surface wanted
    CMP_DWORD dwCaps2;
    CMP_DWORD dwCaps3;
    union
    {
        CMP_DWORD dwCaps4;
        CMP_DWORD dwVolumeDepth;
    };
} DDSCAPS2;

typedef struct _DDPIXELFORMAT
{
    CMP_DWORD dwSize;    // size of structure
    CMP_DWORD dwFlags;   // pixel format flags
    CMP_DWORD dwFourCC;  // (FOURCC code)
    union
    {
        CMP_DWORD dwRGBBitCount;            // how many bits per pixel
        CMP_DWORD dwYUVBitCount;            // how many bits per pixel
        CMP_DWORD dwZBufferBitDepth;        // how many total bits/pixel in z buffer (including any stencil bits)
        CMP_DWORD dwAlphaBitDepth;          // how many bits for alpha channels
        CMP_DWORD dwLuminanceBitCount;      // how many bits per pixel
        CMP_DWORD dwBumpBitCount;           // how many bits per "buxel", total
        CMP_DWORD dwPrivateFormatBitCount;  // Bits per pixel of private driver formats. Only valid in texture
        // format list and if DDPF_D3DFORMAT is set
    };
    union
    {
        CMP_DWORD dwRBitMask;          // mask for red bit
        CMP_DWORD dwYBitMask;          // mask for Y bits
        CMP_DWORD dwStencilBitDepth;   // how many stencil bits (note: dwZBufferBitDepth-dwStencilBitDepth is total Z-only bits)
        CMP_DWORD dwLuminanceBitMask;  // mask for luminance bits
        CMP_DWORD dwBumpDuBitMask;     // mask for bump map U delta bits
        CMP_DWORD dwOperations;        // DDPF_D3DFORMAT Operations
    };
    union
    {
        CMP_DWORD dwGBitMask;       // mask for green bits
        CMP_DWORD dwUBitMask;       // mask for U bits
        CMP_DWORD dwZBitMask;       // mask for Z bits
        CMP_DWORD dwBumpDvBitMask;  // mask for bump map V delta bits
        struct
        {
            CMP_WORD wFlipMSTypes;  // Multisample methods supported via flip for this D3DFORMAT
            CMP_WORD wBltMSTypes;   // Multisample methods supported via blt for this D3DFORMAT
        } MultiSampleCaps;
    };
    union
    {
        CMP_DWORD dwBBitMask;              // mask for blue bits
        CMP_DWORD dwVBitMask;              // mask for V bits
        CMP_DWORD dwStencilBitMask;        // mask for stencil bits
        CMP_DWORD dwBumpLuminanceBitMask;  // mask for luminance in bump map
    };
    union
    {
        CMP_DWORD dwRGBAlphaBitMask;        // mask for alpha channel
        CMP_DWORD dwYUVAlphaBitMask;        // mask for alpha channel
        CMP_DWORD dwLuminanceAlphaBitMask;  // mask for alpha channel
        CMP_DWORD dwRGBZBitMask;            // mask for Z channel
        CMP_DWORD dwYUVZBitMask;            // mask for Z channel
    };
} DDPIXELFORMAT;

#define DDSD_CAPS 0x00000001l  // default
#define DDSD_HEIGHT 0x00000002l
#define DDSD_WIDTH 0x00000004l
#define DDSD_PITCH 0x00000008l
#define DDSD_BACKBUFFERCOUNT 0x00000020l
#define DDSD_ZBUFFERBITDEPTH 0x00000040l
#define DDSD_ALPHABITDEPTH 0x00000080l
#define DDSD_LPSURFACE 0x00000800l
#define DDSD_PIXELFORMAT 0x00001000l
#define DDSD_CKDESTOVERLAY 0x00002000l
#define DDSD_CKDESTBLT 0x00004000l
#define DDSD_CKSRCOVERLAY 0x00008000l
#define DDSD_CKSRCBLT 0x00010000l
#define DDSD_MIPMAPCOUNT 0x00020000l
#define DDSD_REFRESHRATE 0x00040000l
#define DDSD_LINEARSIZE 0x00080000l
#define DDSD_TEXTURESTAGE 0x00100000l
#define DDSD_FVF 0x00200000l
#define DDSD_SRCVBHANDLE 0x00400000l
#define DDSD_DEPTH 0x00800000l
#define DDSD_ALL 0x00fff9eel

#define DDSCAPS_RESERVED1 0x00000001l
#define DDSCAPS_ALPHA 0x00000002l
#define DDSCAPS_BACKBUFFER 0x00000004l
#define DDSCAPS_COMPLEX 0x00000008l
#define DDSCAPS_FLIP 0x00000010l
#define DDSCAPS_FRONTBUFFER 0x00000020l
#define DDSCAPS_OFFSCREENPLAIN 0x00000040l
#define DDSCAPS_OVERLAY 0x00000080l
#define DDSCAPS_PALETTE 0x00000100l
#define DDSCAPS_PRIMARYSURFACE 0x00000200l
#define DDSCAPS_RESERVED3 0x00000400l
#define DDSCAPS_PRIMARYSURFACELEFT 0x00000000l
#define DDSCAPS_SYSTEMMEMORY 0x00000800l
#define DDSCAPS_TEXTURE 0x00001000l
#define DDSCAPS_3DDEVICE 0x00002000l
#define DDSCAPS_VIDEOMEMORY 0x00004000l
#define DDSCAPS_VISIBLE 0x00008000l
#define DDSCAPS_WRITEONLY 0x00010000l
#define DDSCAPS_ZBUFFER 0x00020000l
#define DDSCAPS_OWNDC 0x00040000l
#define DDSCAPS_LIVEVIDEO 0x00080000l
#define DDSCAPS_HWCODEC 0x00100000l
#define DDSCAPS_MODEX 0x00200000l
#define DDSCAPS_MIPMAP 0x00400000l
#define DDSCAPS_RESERVED2 0x00800000l
#define DDSCAPS_ALLOCONLOAD 0x04000000l
#define DDSCAPS_VIDEOPORT 0x08000000l
#define DDSCAPS_LOCALVIDMEM 0x10000000l
#define DDSCAPS_NONLOCALVIDMEM 0x20000000l
#define DDSCAPS_STANDARDVGAMODE 0x40000000l
#define DDSCAPS_OPTIMIZED 0x80000000l

#define DDSCAPS2_RESERVED4 0x00000002L
#define DDSCAPS2_HARDWAREDEINTERLACE 0x00000000L
#define DDSCAPS2_HINTDYNAMIC 0x00000004L
#define DDSCAPS2_HINTSTATIC 0x00000008L
#define DDSCAPS2_TEXTUREMANAGE 0x00000010L
#define DDSCAPS2_RESERVED1 0x00000020L
#define DDSCAPS2_RESERVED2 0x00000040L
#define DDSCAPS2_OPAQUE 0x00000080L
#define DDSCAPS2_HINTANTIALIASING 0x00000100L
#define DDSCAPS2_CUBEMAP 0x00000200L
#define DDSCAPS2_CUBEMAP_POSITIVEX 0x00000400L
#define DDSCAPS2_CUBEMAP_NEGATIVEX 0x00000800L
#define DDSCAPS2_CUBEMAP_POSITIVEY 0x00001000L
#define DDSCAPS2_CUBEMAP_NEGATIVEY 0x00002000L
#define DDSCAPS2_CUBEMAP_POSITIVEZ 0x00004000L
#define DDSCAPS2_CUBEMAP_NEGATIVEZ 0x00008000L
#define DDSCAPS2_CUBEMAP_ALLFACES                                                                                                                     \
    (DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP_POSITIVEZ | \
     DDSCAPS2_CUBEMAP_NEGATIVEZ)

#define DDSCAPS2_MIPMAPSUBLEVEL 0x00010000L
#define DDSCAPS2_D3DTEXTUREMANAGE 0x00020000L
#define DDSCAPS2_DONOTPERSIST 0x00040000L
#define DDSCAPS2_STEREOSURFACELEFT 0x00080000L
#define DDSCAPS2_VOLUME 0x00200000L
#define DDSCAPS2_NOTUSERLOCKABLE 0x00400000L
#define DDSCAPS2_POINTS 0x00800000L
#define DDSCAPS2_RTPATCHES 0x01000000L
#define DDSCAPS2_NPATCHES 0x02000000L
#define DDSCAPS2_RESERVED3 0x04000000L
#define DDSCAPS2_DISCARDBACKBUFFER 0x10000000L
#define DDSCAPS2_ENABLEALPHACHANNEL 0x20000000L
#define DDSCAPS2_EXTENDEDFORMATPRIMARY 0x40000000L
#define DDSCAPS2_ADDITIONALPRIMARY 0x80000000L
#define DDSCAPS3_MULTISAMPLE_MASK 0x0000001FL
#define DDSCAPS3_MULTISAMPLE_QUALITY_MASK 0x000000E0L
#define DDSCAPS3_MULTISAMPLE_QUALITY_SHIFT 5
#define DDSCAPS3_RESERVED1 0x00000100L
#define DDSCAPS3_RESERVED2 0x00000200L
#define DDSCAPS3_LIGHTWEIGHTMIPMAP 0x00000400L
#define DDSCAPS3_AUTOGENMIPMAP 0x00000800L
#define DDSCAPS3_DMAP 0x00001000L

#define DDPF_ALPHAPIXELS 0x00000001l
#define DDPF_ALPHA 0x00000002l
#define DDPF_FOURCC 0x00000004l
#define DDPF_PALETTEINDEXED4 0x00000008l
#define DDPF_PALETTEINDEXEDTO8 0x00000010l
#define DDPF_PALETTEINDEXED8 0x00000020l
#define DDPF_RGB 0x00000040l
#define DDPF_COMPRESSED 0x00000080l
#define DDPF_RGBTOYUV 0x00000100l
#define DDPF_YUV 0x00000200l
#define DDPF_ZBUFFER 0x00000400l
#define DDPF_PALETTEINDEXED1 0x00000800l
#define DDPF_PALETTEINDEXED2 0x00001000l
#define DDPF_ZPIXELS 0x00002000l
#define DDPF_STENCILBUFFER 0x00004000l
#define DDPF_ALPHAPREMULT 0x00008000l
#define DDPF_LUMINANCE 0x00020000l
#define DDPF_BUMPLUMINANCE 0x00040000l
#define DDPF_BUMPDUDV 0x00080000l

typedef enum DXGI_FORMAT
{
    DXGI_FORMAT_UNKNOWN                    = 0,
    DXGI_FORMAT_R32G32B32A32_TYPELESS      = 1,
    DXGI_FORMAT_R32G32B32A32_FLOAT         = 2,
    DXGI_FORMAT_R32G32B32A32_UINT          = 3,
    DXGI_FORMAT_R32G32B32A32_SINT          = 4,
    DXGI_FORMAT_R32G32B32_TYPELESS         = 5,
    DXGI_FORMAT_R32G32B32_FLOAT            = 6,
    DXGI_FORMAT_R32G32B32_UINT             = 7,
    DXGI_FORMAT_R32G32B32_SINT             = 8,
    DXGI_FORMAT_R16G16B16A16_TYPELESS      = 9,
    DXGI_FORMAT_R16G16B16A16_FLOAT         = 10,
    DXGI_FORMAT_R16G16B16A16_UNORM         = 11,
    DXGI_FORMAT_R16G16B16A16_UINT          = 12,
    DXGI_FORMAT_R16G16B16A16_SNORM         = 13,
    DXGI_FORMAT_R16G16B16A16_SINT          = 14,
    DXGI_FORMAT_R32G32_TYPELESS            = 15,
    DXGI_FORMAT_R32G32_FLOAT               = 16,
    DXGI_FORMAT_R32G32_UINT                = 17,
    DXGI_FORMAT_R32G32_SINT                = 18,
    DXGI_FORMAT_R32G8X24_TYPELESS          = 19,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT       = 20,
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS   = 21,
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT    = 22,
    DXGI_FORMAT_R10G10B10A2_TYPELESS       = 23,
    DXGI_FORMAT_R10G10B10A2_UNORM          = 24,
    DXGI_FORMAT_R10G10B10A2_UINT           = 25,
    DXGI_FORMAT_R11G11B10_FLOAT            = 26,
    DXGI_FORMAT_R8G8B8A8_TYPELESS          = 27,
    DXGI_FORMAT_R8G8B8A8_UNORM             = 28,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB        = 29,
    DXGI_FORMAT_R8G8B8A8_UINT              = 30,
    DXGI_FORMAT_R8G8B8A8_SNORM             = 31,
    DXGI_FORMAT_R8G8B8A8_SINT              = 32,
    DXGI_FORMAT_R16G16_TYPELESS            = 33,
    DXGI_FORMAT_R16G16_FLOAT               = 34,
    DXGI_FORMAT_R16G16_UNORM               = 35,
    DXGI_FORMAT_R16G16_UINT                = 36,
    DXGI_FORMAT_R16G16_SNORM               = 37,
    DXGI_FORMAT_R16G16_SINT                = 38,
    DXGI_FORMAT_R32_TYPELESS               = 39,
    DXGI_FORMAT_D32_FLOAT                  = 40,
    DXGI_FORMAT_R32_FLOAT                  = 41,
    DXGI_FORMAT_R32_UINT                   = 42,
    DXGI_FORMAT_R32_SINT                   = 43,
    DXGI_FORMAT_R24G8_TYPELESS             = 44,
    DXGI_FORMAT_D24_UNORM_S8_UINT          = 45,
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS      = 46,
    DXGI_FORMAT_X24_TYPELESS_G8_UINT       = 47,
    DXGI_FORMAT_R8G8_TYPELESS              = 48,
    DXGI_FORMAT_R8G8_UNORM                 = 49,
    DXGI_FORMAT_R8G8_UINT                  = 50,
    DXGI_FORMAT_R8G8_SNORM                 = 51,
    DXGI_FORMAT_R8G8_SINT                  = 52,
    DXGI_FORMAT_R16_TYPELESS               = 53,
    DXGI_FORMAT_R16_FLOAT                  = 54,
    DXGI_FORMAT_D16_UNORM                  = 55,
    DXGI_FORMAT_R16_UNORM                  = 56,
    DXGI_FORMAT_R16_UINT                   = 57,
    DXGI_FORMAT_R16_SNORM                  = 58,
    DXGI_FORMAT_R16_SINT                   = 59,
    DXGI_FORMAT_R8_TYPELESS                = 60,
    DXGI_FORMAT_R8_UNORM                   = 61,
    DXGI_FORMAT_R8_UINT                    = 62,
    DXGI_FORMAT_R8_SNORM                   = 63,
    DXGI_FORMAT_R8_SINT                    = 64,
    DXGI_FORMAT_A8_UNORM                   = 65,
    DXGI_FORMAT_R1_UNORM                   = 66,
    DXGI_FORMAT_R9G9B9E5_SHAREDEXP         = 67,
    DXGI_FORMAT_R8G8_B8G8_UNORM            = 68,
    DXGI_FORMAT_G8R8_G8B8_UNORM            = 69,
    DXGI_FORMAT_BC1_TYPELESS               = 70,
    DXGI_FORMAT_BC1_UNORM                  = 71,
    DXGI_FORMAT_BC1_UNORM_SRGB             = 72,
    DXGI_FORMAT_BC2_TYPELESS               = 73,
    DXGI_FORMAT_BC2_UNORM                  = 74,
    DXGI_FORMAT_BC2_UNORM_SRGB             = 75,
    DXGI_FORMAT_BC3_TYPELESS               = 76,
    DXGI_FORMAT_BC3_UNORM                  = 77,
    DXGI_FORMAT_BC3_UNORM_SRGB             = 78,
    DXGI_FORMAT_BC4_TYPELESS               = 79,
    DXGI_FORMAT_BC4_UNORM                  = 80,
    DXGI_FORMAT_BC4_SNORM                  = 81,
    DXGI_FORMAT_BC5_TYPELESS               = 82,
    DXGI_FORMAT_BC5_UNORM                  = 83,
    DXGI_FORMAT_BC5_SNORM                  = 84,
    DXGI_FORMAT_B5G6R5_UNORM               = 85,
    DXGI_FORMAT_B5G5R5A1_UNORM             = 86,
    DXGI_FORMAT_B8G8R8A8_UNORM             = 87,
    DXGI_FORMAT_B8G8R8X8_UNORM             = 88,
    DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
    DXGI_FORMAT_B8G8R8A8_TYPELESS          = 90,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB        = 91,
    DXGI_FORMAT_B8G8R8X8_TYPELESS          = 92,
    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB        = 93,
    DXGI_FORMAT_BC6H_TYPELESS              = 94,
    DXGI_FORMAT_BC6H_UF16                  = 95,
    DXGI_FORMAT_BC6H_SF16                  = 96,
    DXGI_FORMAT_BC7_TYPELESS               = 97,
    DXGI_FORMAT_BC7_UNORM                  = 98,
    DXGI_FORMAT_BC7_UNORM_SRGB             = 99,
    DXGI_FORMAT_AYUV                       = 100,
    DXGI_FORMAT_Y410                       = 101,
    DXGI_FORMAT_Y416                       = 102,
    DXGI_FORMAT_NV12                       = 103,
    DXGI_FORMAT_P010                       = 104,
    DXGI_FORMAT_P016                       = 105,
    DXGI_FORMAT_420_OPAQUE                 = 106,
    DXGI_FORMAT_YUY2                       = 107,
    DXGI_FORMAT_Y210                       = 108,
    DXGI_FORMAT_Y216                       = 109,
    DXGI_FORMAT_NV11                       = 110,
    DXGI_FORMAT_AI44                       = 111,
    DXGI_FORMAT_IA44                       = 112,
    DXGI_FORMAT_P8                         = 113,
    DXGI_FORMAT_A8P8                       = 114,
    DXGI_FORMAT_B4G4R4A4_UNORM             = 115,
    DXGI_FORMAT_FORCE_UINT                 = 0xffffffff
} DXGI_FORMAT;

typedef enum _D3DFORMAT
{
    D3DFMT_UNKNOWN = 0,

    D3DFMT_R8G8B8       = 20,
    D3DFMT_A8R8G8B8     = 21,
    D3DFMT_X8R8G8B8     = 22,
    D3DFMT_R5G6B5       = 23,
    D3DFMT_X1R5G5B5     = 24,
    D3DFMT_A1R5G5B5     = 25,
    D3DFMT_A4R4G4B4     = 26,
    D3DFMT_R3G3B2       = 27,
    D3DFMT_A8           = 28,
    D3DFMT_A8R3G3B2     = 29,
    D3DFMT_X4R4G4B4     = 30,
    D3DFMT_A2B10G10R10  = 31,
    D3DFMT_A8B8G8R8     = 32,
    D3DFMT_X8B8G8R8     = 33,
    D3DFMT_G16R16       = 34,
    D3DFMT_A2R10G10B10  = 35,
    D3DFMT_A16B16G16R16 = 36,

    D3DFMT_A8P8 = 40,
    D3DFMT_P8   = 41,

    D3DFMT_L8   = 50,
    D3DFMT_A8L8 = 51,
    D3DFMT_A4L4 = 52,

    D3DFMT_V8U8        = 60,
    D3DFMT_L6V5U5      = 61,
    D3DFMT_X8L8V8U8    = 62,
    D3DFMT_Q8W8V8U8    = 63,
    D3DFMT_V16U16      = 64,
    D3DFMT_A2W10V10U10 = 67,

    D3DFMT_UYVY      = CMP_MAKEFOURCC('U', 'Y', 'V', 'Y'),
    D3DFMT_R8G8_B8G8 = CMP_MAKEFOURCC('R', 'G', 'B', 'G'),
    D3DFMT_YUY2      = CMP_MAKEFOURCC('Y', 'U', 'Y', '2'),
    D3DFMT_G8R8_G8B8 = CMP_MAKEFOURCC('G', 'R', 'G', 'B'),
    D3DFMT_DXT1      = CMP_MAKEFOURCC('D', 'X', 'T', '1'),
    D3DFMT_DXT2      = CMP_MAKEFOURCC('D', 'X', 'T', '2'),
    D3DFMT_DXT3      = CMP_MAKEFOURCC('D', 'X', 'T', '3'),
    D3DFMT_DXT4      = CMP_MAKEFOURCC('D', 'X', 'T', '4'),
    D3DFMT_DXT5      = CMP_MAKEFOURCC('D', 'X', 'T', '5'),

    D3DFMT_D16_LOCKABLE = 70,
    D3DFMT_D32          = 71,
    D3DFMT_D15S1        = 73,
    D3DFMT_D24S8        = 75,
    D3DFMT_D24X8        = 77,
    D3DFMT_D24X4S4      = 79,
    D3DFMT_D16          = 80,

    D3DFMT_D32F_LOCKABLE = 82,
    D3DFMT_D24FS8        = 83,

    D3DFMT_L16 = 81,

    D3DFMT_VERTEXDATA = 100,
    D3DFMT_INDEX16    = 101,
    D3DFMT_INDEX32    = 102,

    D3DFMT_Q16W16V16U16 = 110,

    D3DFMT_MULTI2_ARGB8 = CMP_MAKEFOURCC('M', 'E', 'T', '1'),

    // Floating point surface formats

    // s10e5 formats (16-bits per channel)
    D3DFMT_R16F          = 111,
    D3DFMT_G16R16F       = 112,
    D3DFMT_A16B16G16R16F = 113,

    // IEEE s23e8 formats (32-bits per channel)
    D3DFMT_R32F          = 114,
    D3DFMT_G32R32F       = 115,
    D3DFMT_A32B32G32R32F = 116,

    D3DFMT_CxV8U8 = 117,
} D3DFORMAT;

enum D3D10_RESOURCE_DIMENSION
{
    D3D10_RESOURCE_DIMENSION_UNKNOWN   = 0,
    D3D10_RESOURCE_DIMENSION_BUFFER    = 1,
    D3D10_RESOURCE_DIMENSION_TEXTURE1D = 2,
    D3D10_RESOURCE_DIMENSION_TEXTURE2D = 3,
    D3D10_RESOURCE_DIMENSION_TEXTURE3D = 4
};

typedef enum D3D10_RESOURCE_MISC_FLAG
{
    D3D10_RESOURCE_MISC_GENERATE_MIPS     = 0x1L,
    D3D10_RESOURCE_MISC_SHARED            = 0x2L,
    D3D10_RESOURCE_MISC_TEXTURECUBE       = 0x4L,
    D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX = 0x10L,
    D3D10_RESOURCE_MISC_GDI_COMPATIBLE    = 0x20L
} D3D10_RESOURCE_MISC_FLAG;

#else
#include "dxgiformat.h"
#endif

extern const char* g_pszFilename;

// DDSD2
// Required for 64bit compatability
typedef struct _DDSD2
{                        // DDS_HEADER
    CMP_DWORD dwSize;    // size:                size of the DDSURFACEDESC structure
    CMP_DWORD dwFlags;   // flags:               determines what fields are valid
    CMP_DWORD dwHeight;  // height               height of surface to be created
    CMP_DWORD dwWidth;   // width                width of input surface
    union
    {
        CMP_DWORD lPitch;        // pitchOrLinearSize    distance to start of next line (return value only)
        CMP_DWORD dwLinearSize;  //                      Formless late-allocated optimized surface size
    };
    union
    {
        CMP_DWORD dwDepth;            // depth                the depth if this is a volume texture, only if DDS_HEADER_FLAGS_VOLUME is set
        CMP_DWORD dwBackBufferCount;  //                      number of back buffers requested
    };
    union
    {
        CMP_DWORD dwMipMapCount;  // mipMapCount          number of mip-map levels requestde
        CMP_DWORD dwRefreshRate;  //                      refresh rate (used when display mode is described)
        CMP_DWORD dwSrcVBHandle;  //                      The source used in VB::Optimize
    };
    CMP_DWORD dwAlphaBitDepth;  // reserved1[0]          depth of alpha buffer requested
    CMP_DWORD dwReserved;       // reserved1[1]
    CMP_DWORD lpSurface;        // reserved1[2]          pointer to the associated surface memory
    union
    {
        DDCOLORKEY ddckCKDestOverlay;  // reserved1[3,4]        color key for destination overlay use
        CMP_DWORD  dwEmptyFaceColor;   //                      Physical color for empty cubemap faces
    };
    DDCOLORKEY ddckCKDestBlt;     // reserved1[5,6]        color key for destination blt use
    DDCOLORKEY ddckCKSrcOverlay;  // reserved1[7,8]        color key for source overlay use
    DDCOLORKEY ddckCKSrcBlt;      // reserved1[9,10]       color key for source blt use
    union
    {
        DDPIXELFORMAT ddpfPixelFormat;  // DDS_PIXELFORMAT      pixel format description of the surface (FourCC's)
        CMP_DWORD     dwFVF;            //                      vertex format description of vertex buffers
    };
    DDSCAPS2  ddsCaps;         // caps,caps2,cap3,caps4    direct draw surface capabilities
    CMP_DWORD dwTextureStage;  // reserved2                stage in multitexture cascade
                               //                          for extended DDSHeader10 info see DDS_DX10
} DDSD2;

// Simplified version
struct DDS_PIXELFORMAT
{
    uint32_t size;
    uint32_t flags;
    uint32_t fourCC;
    uint32_t RGBBitCount;
    uint32_t RBitMask;
    uint32_t GBitMask;
    uint32_t BBitMask;
    uint32_t ABitMask;
};

struct DDS_FILE_HEADER
{
    uint32_t        size;
    uint32_t        flags;
    uint32_t        height;
    uint32_t        width;
    uint32_t        pitchOrLinearSize;
    uint32_t        depth;  // only if DDS_HEADER_FLAGS_VOLUME is set in flags
    uint32_t        mipMapCount;
    uint32_t        reserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t        caps;
    uint32_t        caps2;
    uint32_t        caps3;
    uint32_t        caps4;
    uint32_t        reserved2;
};

struct DDS_FILE_HEADER_DXT10
{
    DXGI_FORMAT dxgiFormat;
    uint32_t    resourceDimension;
    uint32_t    miscFlag;  // see D3D11_RESOURCE_MISC_FLAG
    uint32_t    arraySize;
    uint32_t    miscFlags2;  // see DDS_MISC_FLAGS2
};

#define MAX_FORMAT_LENGTH 160
#define MAX_ERROR_LENGTH 240

static const CMP_DWORD DDS_HEADER = CMP_MAKEFOURCC('D', 'D', 'S', ' ');
#define DDS_FOURCC 0x00000004                   // DDPF_FOURCC
#define DDS_RGB 0x00000040                      // DDPF_RGB
#define DDS_RGBA 0x00000041                     // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE 0x00020000                // DDPF_LUMINANCE
#define DDS_LUMINANCEA 0x00020001               // DDPF_LUMINANCE | DDPF_ALPHAPIXELS
#define DDS_ALPHAPIXELS 0x00000001              // DDPF_ALPHAPIXELS
#define DDS_ALPHA 0x00000002                    // DDPF_ALPHA
#define DDS_PAL8 0x00000020                     // DDPF_PALETTEINDEXED8
#define DDS_PAL8A 0x00000021                    // DDPF_PALETTEINDEXED8 | DDPF_ALPHAPIXELS
#define DDS_BUMPDUDV 0x00080000                 // DDPF_BUMPDUDV

#define DDS_HEADER_FLAGS_TEXTURE 0x00001007     // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
#define DDS_HEADER_FLAGS_MIPMAP 0x00020000      // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_VOLUME 0x00800000      // DDSD_DEPTH
#define DDS_HEADER_FLAGS_PITCH 0x00000008       // DDSD_PITCH
#define DDS_HEADER_FLAGS_LINEARSIZE 0x00080000  // DDSD_LINEARSIZE
#define DDS_CUBEMAP 0x00000200                  // DDSCAPS2_CUBEMAP
#define DDS_FLAGS_VOLUME 0x00200000             // DDSCAPS2_VOLUME

TC_PluginError LoadDDS_ABGR32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_ABGR16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_GR32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_R32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_R16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G16R16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_FourCC(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_RGB565(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_RGB888(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_RGB8888(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet, bool bAlpha);
TC_PluginError LoadDDS_RGB8888_S(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet, bool bAlpha);
TC_PluginError LoadDDS_ARGB2101010(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_ABGR16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G16R16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_R16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_AG8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_A8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);

TC_PluginError SaveDDS_ABGR32F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RG32F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_R32F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ABGR16F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RG16F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_R16F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ARGB8888(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RGBA8888_S(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ARGB2101010(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ABGR16(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_R16(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RG16(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RGB888(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_FourCC(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_G8(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_A8(FILE* pFile, const MipSet* pMipSet);

#endif
