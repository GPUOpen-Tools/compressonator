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

#ifndef _PLUGIN_IMAGE_KTX_H
#define _PLUGIN_IMAGE_KTX_H

#include "plugininterface.h"
#include "stdint.h"
#include "ktx.h"
#include "ktxint.h"

// {737CE1F6-F448-499E-B8B5-585F9A22893C}
#ifdef _WIN32
static const GUID g_GUID = 
{ 0x737ce1f6, 0xf448, 0x499e, { 0xb8, 0xb5, 0x58, 0x5f, 0x9a, 0x22, 0x89, 0x3c } };
#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

class Plugin_KTX : public PluginInterface_Image
{
    public: 
        Plugin_KTX();
        virtual ~Plugin_KTX();

        int TC_PluginSetSharedIO(void* Shared);
        int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
        int TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet);
        int TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet);
        int TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture);
        int TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture);

};


#define IDS_ERROR_FILE_OPEN             1
#define IDS_ERROR_NOT_KTX               2
#define IDS_ERROR_UNSUPPORTED_TYPE      3
#define IDS_ERROR_ALLOCATEMIPSET        4
#define IDS_ERROR_ALLOCATEMIPSLEVELDATA 5

extern void *make_Plugin_KTX();

// ---------------- KTX File Definitions ------------------------

/*
https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/

for each keyValuePair that fits in bytesOfKeyValueData
    UInt32   keyAndValueByteSize
    Byte     keyAndValue[keyAndValueByteSize]
    Byte     valuePadding[3 - ((keyAndValueByteSize + 3) % 4)]
end
  
for each mipmap_level in numberOfMipmapLevels*
    UInt32 imageSize; 
    for each array_element in numberOfArrayElements*
       for each face in numberOfFaces
           for each z_slice in pixelDepth*
               for each row or row_of_blocks in pixelHeight*
                   for each pixel or block_of_pixels in pixelWidth
                       Byte data[format-specific-number-of-bytes]**
                   end
               end
           end
           Byte cubePadding[0-3]
       end
    end
    Byte mipPadding[3 - ((imageSize + 3) % 4)]
end

Notes:
        *  Replace with 1 if this field is 0
        ** Uncompressed texture data matches a GL_UNPACK_ALIGNMENT of 4

*/

uint8_t FileIdentifier[12] = {
   0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
};

struct ktx_header
{
    uint8_t   identifier[12];
    uint32_t  endianness;                // should be 0x04030201 if 0x01020304 then all data below must be switched
    uint32_t  gl_Type;                    // 0 = compressed data else use OpenGL Spec to determine uncompressed data type (OpenGL 4.4 tables 8.3) 
    uint32_t  gl_TypeSize;                // endness data size for texture data stored in file 0=size of gl_type, 1 = for compressed data
    uint32_t  gl_Format;                // 0 = compressed data else use OpenGL spec to determine format (IoebGL 4.4 table 8.3 )
    uint32_t  gl_InternalFormat;        // for compressed data use OpenGL 4.4 table 8.14 else use tables 8.12 & 8.13
    uint32_t  gl_BaseInternalFormat;    // use OpenGL 4.4 table 8.11
    uint32_t  pixelWidth;                // size of texture image for level 0, No rounding to block size is applied to compressed textures
    uint32_t  pixelHeight;                // 
    uint32_t  pixelDepth;                // pixelDepth = 0 for 2D and cube textures 
    uint32_t  numberOfArrayElements;    // 0 for no array element (not a texture array) else specifies the number of array elements
    uint32_t  numberOfFaces;            // number of cube map faces (for cubemap or cubemap arrays = 6, else 1)
    uint32_t  numberOfMipmapLevels;        // 1 for non-mipmapped textures, 0 for mipped mapmapped: full pyramid should be generated from level 0 at load time
    uint32_t  bytesOfKeyValueData;        // Key value pairs 
};



enum scanline_copy_method
{
    R8_TO_RGBA8,
    RG8_TO_RGBA8,
    RGB8_TO_RGBA8,
    RGBA8_TO_RGBA8,
    BGR8_TO_RGBA8,
    BGRA8_TO_RGBA8,
    L8_TO_RGBA8,
    LA8_TO_RGBA8,

    RGBX8_TO_RGBA8,
    BGRX8_TO_RGBA8,

    R16_TO_RGBA16F,
    RG16_TO_RGBA16F,
    RGB16_TO_RGBA16F,
    RGBA16_TO_RGBA16F,
    BGR16_TO_RGBA16F,
    BGRA16_TO_RGBA16F,
    L16_TO_RGBA16F,
    LA16_TO_RGBA16F,

    R16F_TO_RGBA16F,
    RG16F_TO_RGBA16F,
    RGB16F_TO_RGBA16F,
    RGBA16F_TO_RGBA16F,
    BGR16F_TO_RGBA16F,
    BGRA16F_TO_RGBA16F,
    L16F_TO_RGBA16F,
    LA16F_TO_RGBA16F,

    R32F_TO_RGBA16F,
    RG32F_TO_RGBA16F,
    RGB32F_TO_RGBA16F,
    RGBA32F_TO_RGBA16F,
    BGR32F_TO_RGBA16F,
    BGRA32F_TO_RGBA16F,
    L32F_TO_RGBA16F,
    LA32F_TO_RGBA16F
};


#define R_ATI1N_UNorm                                   0x8DBB    // GL_COMPRESSED_RED_RGTC1
#define R_ATI1N_SNorm                                   0x8DBC    // GL_COMPRESSED_SIGNED_RED_RGTC1
#define RG_ATI2N_UNorm                                  0x8DBD    // GL_COMPRESSED_RG_RGTC2
#define RG_ATI2N_SNorm                                  0x8DBE    // GL_COMPRESSED_SIGNED_RG_RGTC2
#define RGB_BP_UNSIGNED_FLOAT                           0x8E8F    // GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB  //bc6
#define RGB_BP_SIGNED_FLOAT                             0x8E8E    // GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB
#define RGB_BP_UNorm                                    0x8E8C    // GL_COMPRESSED_RGBA_BPTC_UNORM_ARB  //bc7
#define COMPRESSED_RED_RGTC1                            0x8DBB    //bc4
#define COMPRESSED_RG_RGTC2                             0x8DBD    //bc5 ATI2_XY

// Legacy setting should move to reserved temp space!!
#define ATC_RGB_AMD                                     0x8C92
#define ATC_RGBA_EXPLICIT_ALPHA_AMD                     0x8C93
#define ATC_RGBA_INTERPOLATED_ALPHA_AMD                 0x87EE
#define ETC1_RGB8_OES                                   0x8D64
#define COMPRESSED_FORMAT_DXT5_xGBR                     0x83F3
#define COMPRESSED_FORMAT_DXT5_RxBG                     0x83F4
#define COMPRESSED_FORMAT_DXT5_RBxG                     0x83F5
#define COMPRESSED_FORMAT_DXT5_xRBG                     0x83F6
#define COMPRESSED_FORMAT_DXT5_RGxB                     0x83F7
#define COMPRESSED_FORMAT_DXT5_xGxR                     0x83F8

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

#define GL_COMPRESSED_RGB8_ETC2                             0x9274
#define GL_COMPRESSED_SRGB8_ETC2                            0x9275
#define GL_COMPRESSED_RGBA8_ETC2_EAC                        0x9278
#define GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC                 0x9279
#define GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2         0x9276
#define GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2        0x9277

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
