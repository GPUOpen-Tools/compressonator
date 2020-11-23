//=====================================================================
// Copyright 2020 (c), Advanced Micro Devices, Inc. All rights reserved.
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

// Windows Header Files:
#ifdef _WIN32
#include <windows.h>
#endif

#include "ktx2.h"
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "common.h"
#include "softfloat.h"

#include "textureio.h"

#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#include "gl_format.h"
#pragma comment(lib, "opengl32.lib")  // Open GL
#pragma comment(lib, "Glu32.lib")     // Glu
#pragma comment(lib, "glew32.lib")    // glew

using namespace std;

CMIPS* KTX2_CMips;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_KTX2)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("KTX2")
#else
void* make_Plugin_KTX2()
{
    return new Plugin_KTX2;
}
#endif

static void writeId2(std::ostream& dst)
{
    dst << "glTF Compressonator v2.0";
}

Plugin_KTX2::Plugin_KTX2()
{
}

Plugin_KTX2::~Plugin_KTX2()
{
}

int Plugin_KTX2::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        KTX2_CMips = static_cast<CMIPS*>(Shared);
        return 0;
    }
    return 1;
}

int Plugin_KTX2::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{
#ifdef _WIN32
    pPluginVersion->guid = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor    = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor    = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_KTX2::TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture* srcTexture)
{
    return -1;
}

int Plugin_KTX2::TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture* srcTexture)
{
    return -1;
}

int Plugin_KTX2::TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet)
{
    ktxTexture2* texture2 = nullptr;
    ktxTexture*  texture  = nullptr;

    KTX_error_code loadStatus;
    bool         isCompressed = false;
    ktx_uint32_t glInternalformat;
    ktx_uint32_t glType;
    ktx_uint32_t glFormat;

    loadStatus = ktxTexture2_CreateFromNamedFile(pszFilename, KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture2);
    if (loadStatus != KTX_SUCCESS)
    {
        if (KTX2_CMips)
        {
            KTX2_CMips->PrintError(("Error(%x): KTX2 Plugin ID(%d) opening file = %s \n"), loadStatus, IDS_ERROR_FILE_OPEN, pszFilename);
        }
        return -1;
    }



    // CMP_DFD* extended_format = (CMP_DFD *)texture2->pDfd;
    glInternalformat = glGetInternalFormatFromVkFormat((VkFormat)texture2->vkFormat);
    glType           = glGetTypeFromInternalFormat(glInternalformat);
    glFormat         = glGetFormatFromInternalFormat(glInternalformat);

    texture = ktxTexture(texture2);

    isCompressed = texture->isCompressed;
    
    int channelByteSize = 0;

    try {
        if (isCompressed)
        {
            pMipSet->m_compressed      = true;
            pMipSet->m_nBlockHeight    = 4;
            pMipSet->m_nBlockWidth     = 4;
            pMipSet->m_nBlockDepth     = 1;
            pMipSet->m_ChannelFormat   = CF_Compressed;
            pMipSet->m_TextureDataType = TDT_ARGB;
            pMipSet->m_format          = CMP_FORMAT_Unknown;
            channelByteSize            = 1;
    
            // Search using VL formats first
            switch ((VkFormat)texture2->vkFormat)
            {
            case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
                pMipSet->m_format = CMP_FORMAT_BC1;
                break;
            case VK_FORMAT_BC2_UNORM_BLOCK:
                pMipSet->m_format = CMP_FORMAT_BC2;
                break;
            case VK_FORMAT_BC3_UNORM_BLOCK:
                pMipSet->m_format = CMP_FORMAT_BC3;
                // These are unsupport types used to map into cmp formats
                // this is a trick for the CMP compressed DXT5 swizzle types
                // switch (glInternalformat)
                // {
                // case COMPRESSED_FORMAT_DXT5_xGBR_TMP:
                //     pMipSet->m_format = CMP_FORMAT_DXT5_xGBR;
                //     break;
                // case COMPRESSED_FORMAT_DXT5_RxBG_TMP:
                //     pMipSet->m_format = CMP_FORMAT_DXT5_RxBG;
                //     break;
                // case COMPRESSED_FORMAT_DXT5_RBxG_TMP:
                //     pMipSet->m_format = CMP_FORMAT_DXT5_RBxG;
                //     break;
                // case COMPRESSED_FORMAT_DXT5_xRBG_TMP:
                //     pMipSet->m_format = CMP_FORMAT_DXT5_xRBG;
                //     break;
                // case COMPRESSED_FORMAT_DXT5_RGxB_TMP:
                //     pMipSet->m_format = CMP_FORMAT_DXT5_RGxB;
                //     break;
                // case COMPRESSED_FORMAT_DXT5_xGxR_TMP:
                //     pMipSet->m_format = CMP_FORMAT_DXT5_xGxR;
                //     break;
                // }
                break;
            case VK_FORMAT_BC4_UNORM_BLOCK:
                pMipSet->m_format = CMP_FORMAT_BC4;
                break;
            case VK_FORMAT_BC4_SNORM_BLOCK:
                pMipSet->m_format = CMP_FORMAT_BC4_S;
                // if (glInternalformat == COMPRESSED_FORMAT_ATI1N_UNorm_TMP)
                // {
                //     pMipSet->m_format = CMP_FORMAT_ATI1N;
                // }
                break;
            case VK_FORMAT_BC5_UNORM_BLOCK:
                pMipSet->m_format = CMP_FORMAT_BC5;
                //if (glInternalformat == COMPRESSED_FORMAT_ATI2N_UNorm_TMP)
                //{
                //    pMipSet->m_format = CMP_FORMAT_ATI2N;
                //}
                //else if (glInternalformat == COMPRESSED_FORMAT_ATI2N_XY_UNorm_TMP)
                //{
                //    pMipSet->m_format = CMP_FORMAT_ATI2N_XY;
                //}
                break;
            case VK_FORMAT_BC5_SNORM_BLOCK:
                pMipSet->m_format = CMP_FORMAT_BC5_S;
                break;
            case VK_FORMAT_BC6H_UFLOAT_BLOCK:
                pMipSet->m_format = CMP_FORMAT_BC6H;
                break;
            case VK_FORMAT_BC6H_SFLOAT_BLOCK:
                pMipSet->m_format = CMP_FORMAT_BC6H_SF;
                break;
            case VK_FORMAT_BC7_UNORM_BLOCK:
                pMipSet->m_format = CMP_FORMAT_BC7;
                break;
            case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ETC2_RGB; // Skip ETC as ETC2 is backward comp
                break;
            case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ETC2_SRGB;
                break;
            case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ETC2_RGBA;
                break;
            case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ETC2_RGBA1;
                break;
            case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ETC2_SRGBA;
                break;
            case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 4;
                pMipSet->m_nBlockHeight            = 4;
                break;
            case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 5;
                pMipSet->m_nBlockHeight            = 4;
                break;
            case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 5;
                pMipSet->m_nBlockHeight            = 5;
                break;
            case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 6;
                pMipSet->m_nBlockHeight            = 5;
                break;
            case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 6;
                pMipSet->m_nBlockHeight            = 6;
                break;
            case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 8;
                pMipSet->m_nBlockHeight            = 5;
                break;
            case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 8;
                pMipSet->m_nBlockHeight            = 6;
                break;
            case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 8;
                pMipSet->m_nBlockHeight            = 8;
                break;
            case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 10;
                pMipSet->m_nBlockHeight            = 5;
                break;
            case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 10;
                pMipSet->m_nBlockHeight            = 6;
                break;
            case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 10;
                pMipSet->m_nBlockHeight            = 8;
                break;
            case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 10;
                pMipSet->m_nBlockHeight            = 10;
                break;
            case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 12;
                pMipSet->m_nBlockHeight            = 10;
                break;
            case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
                pMipSet->m_format                  = CMP_FORMAT_ASTC;
                pMipSet->m_nBlockWidth             = 12;
                pMipSet->m_nBlockHeight            = 12;
                break;
            }
        }
        else
        {
        pMipSet->m_compressed = false;

        switch (glType)
        {
        case GL_UNSIGNED_BYTE:
            pMipSet->m_ChannelFormat = CF_8bit;
            switch (glFormat)
            {
            case GL_RED:
                pMipSet->m_format          = CMP_FORMAT_R_8;
                pMipSet->m_TextureDataType = TDT_R;
                break;
            case GL_RG:
                pMipSet->m_format          = CMP_FORMAT_RG_8;
                pMipSet->m_TextureDataType = TDT_RG;
                break;
            case GL_RGB:
                pMipSet->m_format          = CMP_FORMAT_RGB_888;
                pMipSet->m_TextureDataType = TDT_XRGB;
                break;
            case GL_RGBA:
            case GL_RGBA8:
                pMipSet->m_format          = CMP_FORMAT_ARGB_8888;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            case GL_BGR:
                pMipSet->m_swizzle         = true;
                pMipSet->m_format          = CMP_FORMAT_RGB_888;
                pMipSet->m_TextureDataType = TDT_XRGB;
                break;
            case GL_BGRA:
                pMipSet->m_swizzle         = true;
                pMipSet->m_format          = CMP_FORMAT_ARGB_8888;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            }
            break;
        case GL_UNSIGNED_SHORT:
            pMipSet->m_ChannelFormat = CF_16bit;
            switch (glFormat)
            {
            case GL_RED:
                pMipSet->m_format          = CMP_FORMAT_R_16;
                pMipSet->m_TextureDataType = TDT_R;
                break;
            case GL_RG:
                pMipSet->m_format          = CMP_FORMAT_RG_16;
                pMipSet->m_TextureDataType = TDT_RG;
                break;
            case GL_RGBA:
                pMipSet->m_format          = CMP_FORMAT_ARGB_16;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            case GL_BGRA:
                pMipSet->m_swizzle         = true;
                pMipSet->m_format          = CMP_FORMAT_ARGB_16;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            }
            break;
        case GL_HALF_FLOAT:
            pMipSet->m_ChannelFormat = CF_Float16;
            switch (glFormat)
            {
            case GL_RED:
                pMipSet->m_format          = CMP_FORMAT_R_16F;
                pMipSet->m_TextureDataType = TDT_R;
                break;
            case GL_RG:
                pMipSet->m_format          = CMP_FORMAT_RG_16F;
                pMipSet->m_TextureDataType = TDT_RG;
                break;
            case GL_RGBA:
                pMipSet->m_format          = CMP_FORMAT_ARGB_16F;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            case GL_BGRA:
                pMipSet->m_swizzle         = true;
                pMipSet->m_format          = CMP_FORMAT_ARGB_16F;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            }
            break;
        case GL_UNSIGNED_INT_2_10_10_10_REV:
            pMipSet->m_format          = CMP_FORMAT_ARGB_2101010;
            pMipSet->m_TextureDataType = TDT_ARGB;
            pMipSet->m_ChannelFormat   = CF_2101010;
            break;
        case GL_FLOAT:
            pMipSet->m_ChannelFormat = CF_Float32;
            switch (glFormat)
            {
            case GL_RED:
                pMipSet->m_format          = CMP_FORMAT_R_32F;
                pMipSet->m_TextureDataType = TDT_R;
                break;
            case GL_RG:
                pMipSet->m_format          = CMP_FORMAT_RG_32F;
                pMipSet->m_TextureDataType = TDT_RG;
                break;
            case GL_RGBA:
                pMipSet->m_format          = CMP_FORMAT_ARGB_32F;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            case GL_BGRA:
                pMipSet->m_swizzle         = true;
                pMipSet->m_format          = CMP_FORMAT_ARGB_32F;
                pMipSet->m_TextureDataType = TDT_ARGB;
                break;
            }
            break;
            break;
        default:
            if (KTX2_CMips)
            {
                KTX2_CMips->PrintError(("Error(%d): KTX2 Plugin ID(%d) unsupported GL format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, glFormat);
            }
            return -1;
        }
    }
    
        if (texture->isCubemap)
    {
        pMipSet->m_TextureType = TT_CubeMap;
    }
        else if (texture->baseDepth > 1 && texture->numFaces == 1)
    {
        pMipSet->m_TextureType = TT_VolumeTexture;
    }
        else if (texture->baseDepth == 1 && texture->numFaces == 1)
        {
            pMipSet->m_TextureType = TT_2D;
        }
        else
    {
        if (KTX2_CMips)
        {
            KTX2_CMips->PrintError(("Error(%d): KTX2 Plugin ID(%d) unsupported texture format\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE);
        }
        return -1;
    }
    
        pMipSet->m_nMipLevels = texture->numLevels;
    
        // Allocate MipSet header
        KTX2_CMips->AllocateMipSet(
            pMipSet, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType, pMipSet->m_TextureType, texture->baseWidth, texture->baseHeight, texture->numFaces);
    
        int w = pMipSet->m_nWidth;
        int h = pMipSet->m_nHeight;
    
        unsigned int totalByteRead = 0;
    
        unsigned int faceSize              = 0;
        unsigned int MipSetdataSize        = 0;
        unsigned int numArrayElement       = texture->numLayers;
        unsigned int TexturedataSize       = texture->dataSize; // This is all data in cubemap levels and mip levels.
        unsigned int TotalMipSetdataSize   = 0;

        for (uint32_t nMipLevel = 0; nMipLevel < texture->numLevels; nMipLevel++)
        {
            if ((w <= 1) || (h <= 1))
            {
                break;
            }
            else
            {
                w = max(1, pMipSet->m_nWidth >> nMipLevel);
                h = max(1, pMipSet->m_nHeight >> nMipLevel);
            }
    
            for (uint32_t face = 0; face < texture->numFaces; ++face)
            {
                // Determine buffer size and set Mip Set Levels
                MipLevel* pMipLevel = KTX2_CMips->GetMipLevel(pMipSet, nMipLevel, face);
                int channelCount = 0;


                if (pMipSet->m_compressed)
                {
                    // calculate the compressed miplevel size to allocate
                    CMP_Texture destGPUMipTexture;
                    destGPUMipTexture.dwSize     = sizeof(CMP_Texture);
                    destGPUMipTexture.dwPitch    = 0;
                    destGPUMipTexture.format     = pMipSet->m_format;
                    destGPUMipTexture.dwWidth    = w;
                    destGPUMipTexture.dwHeight   = h;
                    destGPUMipTexture.nBlockWidth  = pMipSet->m_nBlockWidth;
                    destGPUMipTexture.nBlockHeight = pMipSet->m_nBlockHeight;
                    MipSetdataSize               = CMP_CalculateBufferSize(&destGPUMipTexture);
                    KTX2_CMips->AllocateCompressedMipLevelData(pMipLevel, w, h, MipSetdataSize);
                    TotalMipSetdataSize += pMipLevel->m_dwLinearSize;
                }
                else {
                    channelByteSize = 0;
                    switch (glType)
                    {
                    case GL_UNSIGNED_BYTE:
                        channelByteSize = 1;
                        break;
                    case GL_UNSIGNED_SHORT:
                        channelByteSize = 2;
                        break;
                    case GL_HALF_FLOAT:
                        channelByteSize = 2;
                        break;
                    case GL_FLOAT:
                        channelByteSize = 4;
                        break;
                    default:
                        return -1;
                    }

                    switch (glFormat)
                    {
                    case GL_RED:
                        channelCount = 1;
                        break;
                    case GL_RG:
                        channelCount = 2;
                        break;
                    case GL_RGB:
                        channelCount = 3;
                        break;
                    case GL_BGR:
                        channelCount = 3;
                        break;
                    case GL_RGBA:
                        channelCount = 4;
                        break;
                    case GL_BGRA:
                        channelCount = 4;
                        break;
                    default:
                        return -1;
                    }

                    KTX2_CMips->AllocateMipLevelData(pMipLevel, w, h, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType);
                    MipSetdataSize = pMipLevel->m_dwLinearSize;

                }
    
                CMP_BYTE* pData = (CMP_BYTE*)(pMipLevel->m_pbData);
    
                if (!pData)
                {
                    if (KTX2_CMips)
                    {
                        KTX2_CMips->PrintError(
                            ("Error(%d): KTX2 Plugin ID(%d) Read image data failed, Out of Memory. Format %x\n"), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, glFormat);
                    }
                    return -1;
                }
    
                //
                // Read image data into temporary buffer
                //
    
                ktx_size_t     offset     = 0;
                KTX_error_code dataStatus = ktxTexture_GetImageOffset(ktxTexture(texture), nMipLevel, 0, face, &offset);

                if (dataStatus != KTX_SUCCESS)
                {
                    if (KTX2_CMips)
                    {
                        KTX2_CMips->PrintError("Error(%d): KTX2 Plugin Read image data offset at %d failed\n", dataStatus,offset);
                    }
                    return -1;
                }


                uint8_t*       imageData  = ktxTexture_GetData(ktxTexture(texture)) + offset;
    
                if (imageData == nullptr)
                {
                    if (KTX2_CMips)
                    {
                        KTX2_CMips->PrintError("Error: KTX2 Plugin Read image data at offset %d is null\n",  offset);
                    }
                    return -1;
                }

                if (!pMipSet->m_compressed)
                {
                    size_t               readSize = channelByteSize * channelCount * w * h;
                    std::vector<uint8_t> pixelData(readSize);
    
                    memcpy(&pixelData[0], imageData, readSize);
    
                    int pixelSize       = channelCount * channelByteSize;
                    int targetPixelSize = channelCount * channelByteSize;
                    if (channelCount == 3)
                    {
                        // XRGB conversion.
                        targetPixelSize = 4 * channelByteSize;
                    }
    
                    int py = 0;
                    for (py = 0; py < h; py++)
                    {
                        int px = 0;
                        for (px = 0; px < w; px++)
                        {
                            memcpy(&pData[targetPixelSize * px + py * targetPixelSize * w], &pixelData[pixelSize * px + py * pixelSize * w], pixelSize);
                        }
                    }
                }
                else
                {
                    if (TotalMipSetdataSize <= TexturedataSize)
                        memcpy(pData, imageData, MipSetdataSize);
                    else
                    {
                        if (KTX2_CMips)
                        {
                            KTX2_CMips->PrintError("Error: KTX2 Plugin MipSetdataSize error (%d, %d)\n", TexturedataSize, MipSetdataSize);
                        }
                        return -1;
                    }

                }
            }
        }
    }
    catch (...)
    {
        if (KTX2_CMips)
        {
            KTX2_CMips->PrintError("Error KTX2 Plugin Exception: \n");
        }
        return -1;
    }


    return 0;
}

int Plugin_KTX2::TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet)
{
    assert(pszFilename);
    assert(pMipSet);
    assert(pszFilename);
    assert(pMipSet);

    if (pMipSet->m_pMipLevelTable == NULL)
    {
        if (KTX2_CMips)
            KTX2_CMips->PrintError(("Error(%d): KTX2 Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_ALLOCATEMIPSET, pszFilename);
        return -1;
    }

    if (KTX2_CMips->GetMipLevel(pMipSet, 0) == NULL)
    {
        if (KTX2_CMips)
            KTX2_CMips->PrintError(("Error(%d): KTX2 Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_ALLOCATEMIPSET, pszFilename);
        return -1;
    }

    ktxTextureCreateInfo textureCreateInfo;
    /*!< Internal format for the texture, e.g., GL_RGB8. Ignored when creating a ktxTexture2. */
    textureCreateInfo.baseWidth     = pMipSet->m_nWidth;     /*!< Width of the base level of the texture. */
    textureCreateInfo.baseHeight    = pMipSet->m_nHeight;    /*!< Height of the base level of the texture. */
    textureCreateInfo.baseDepth     = 1;                     /*!< Depth of the base level of the texture. */
    textureCreateInfo.numDimensions = 2;                     /*!< Number of dimensions in the texture, 1, 2 or 3. */
    textureCreateInfo.numLevels     = pMipSet->m_nMipLevels; /*!< Number of mip levels in the texture. Should be 1 if @c generateMipmaps is KTX_TRUE; */
    textureCreateInfo.numLayers     = 1;                     /*!< Number of array layers in the texture. */
    textureCreateInfo.numFaces      = (pMipSet->m_TextureType == TT_CubeMap) ? 6 : 1; /*!< Number of faces: 6 for cube maps, 1 otherwise. */
    textureCreateInfo.isArray       = KTX_FALSE; /*!< Set to KTX_TRUE if the texture is to be an array texture. Means OpenGL will use a GL_TEXTURE_*_ARRAY target. */
    textureCreateInfo.generateMipmaps = KTX_FALSE; /*!< Set to KTX_TRUE if mipmaps should be generated for the texture when loading into a 3D API. */
    textureCreateInfo.pDfd            = nullptr;
    textureCreateInfo.vkFormat        = VK_FORMAT_UNDEFINED;

    bool isCompressed = CMP_IsCompressedFormat(pMipSet->m_format);

    switch (pMipSet->m_TextureDataType)
    {
        case TDT_R:
            {  //single component-- can be Luminance and Alpha case, here only cover R
                if (!isCompressed)
                {
                    // GL_R8;
                    textureCreateInfo.vkFormat         = VK_FORMAT_R8_UNORM;
                    if (pMipSet->m_ChannelFormat == CF_Float16)
                    {
                        // GL_R16F;
                        textureCreateInfo.vkFormat         = VK_FORMAT_R16_SFLOAT;
                    }
                    else if (pMipSet->m_ChannelFormat == CF_Float32)
                    {
                        // GL_R32F;
                        textureCreateInfo.vkFormat         = VK_FORMAT_R32_SFLOAT;
                    }
                }
                else
                {
                    // GL_RED;
                    textureCreateInfo.vkFormat = VK_FORMAT_R8_UNORM;
                }
            }
            break;
         case TDT_RG:
            {  //two component
                if (!isCompressed)
                {
                    // GL_RG8;
                    textureCreateInfo.vkFormat         = VK_FORMAT_R8G8_UNORM;
                    if (pMipSet->m_ChannelFormat == CF_Float16)
                    {
                        // GL_RG16F;
                        textureCreateInfo.vkFormat         = VK_FORMAT_R16G16_SFLOAT;
                    }
                    else if (pMipSet->m_ChannelFormat == CF_Float32)
                    {
                        // GL_RG32F;
                        textureCreateInfo.vkFormat         = VK_FORMAT_R32G32_SFLOAT;
                    }
                }
                else
                {
                    // GL_COMPRESSED_RG;
                    // TODO: KTX2/Vulkan
                }
            }
            break;
        case TDT_XRGB:
            {  //normally 3 component
                if (!isCompressed)
                {
                    // GL_RGB8;
                    textureCreateInfo.vkFormat         = VK_FORMAT_R8G8B8_UNORM;
                    if (pMipSet->m_ChannelFormat == CF_Float16)
                    {
                        // GL_RGB16F;
                        textureCreateInfo.vkFormat         = VK_FORMAT_R16G16B16_SFLOAT;
                    }
                    else if (pMipSet->m_ChannelFormat == CF_Float32)
                    {
                        // GL_RGB32F;
                        textureCreateInfo.vkFormat         = VK_FORMAT_R32G32B32_SFLOAT;
                    }
                }
                else
                {
                    if (pMipSet->m_format == CMP_FORMAT_BC1 || pMipSet->m_format == CMP_FORMAT_DXT1)
                    {
                        // GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
                        // TODO: KTX2/Vulkan
                    }
                    else
                    {
                        // GL_RGB8;
                        textureCreateInfo.vkFormat         = VK_FORMAT_R8G8B8_UNORM;
                        if (pMipSet->m_ChannelFormat == CF_Float16)
                        {
                            // GL_RGB16F;
                            textureCreateInfo.vkFormat         = VK_FORMAT_R16G16B16_SFLOAT;
                        }
                        else if (pMipSet->m_ChannelFormat == CF_Float32)
                        {
                            // GL_RGB32F;
                            textureCreateInfo.vkFormat         = VK_FORMAT_R32G32B32_SFLOAT;
                        }
                    }
                }
            }
            break;
            case TDT_RGB:
                    {  //3 component  uncompressed formats
                    // GL_RGB8;
                    textureCreateInfo.vkFormat = VK_FORMAT_R8G8B8_UNORM;
                    if (pMipSet->m_ChannelFormat == CF_Float16)
                    {
                        // GL_RGB16F;
                        textureCreateInfo.vkFormat = VK_FORMAT_R16G16B16_SFLOAT;
                    }
                    else if (pMipSet->m_ChannelFormat == CF_Float32)
                    {
                        // GL_RGB32F;
                        textureCreateInfo.vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
                    }
                }
            break;
        case TDT_ARGB:
            {  //4 component
            if (!isCompressed)
            {
                // GL_RGBA8;
                textureCreateInfo.vkFormat         = VK_FORMAT_R8G8B8A8_UNORM;
                if (pMipSet->m_ChannelFormat == CF_Float16)
                {
                    // GL_RGBA16F;
                    textureCreateInfo.vkFormat         = VK_FORMAT_R16G16B16A16_SFLOAT;
                }
                else if (pMipSet->m_ChannelFormat == CF_Float32)
                {
                    // GL_RGBA32F;
                    textureCreateInfo.vkFormat         = VK_FORMAT_R32G32B32A32_SFLOAT;
                }
            }
            else
            {
                switch (pMipSet->m_format)
                {
                    case CMP_FORMAT_BC1:
                    case CMP_FORMAT_DXT1:
                        // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                        textureCreateInfo.vkFormat         = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
                        break;
                    case CMP_FORMAT_BC2:
                    case CMP_FORMAT_DXT3:
                        // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                        textureCreateInfo.vkFormat         = VK_FORMAT_BC2_UNORM_BLOCK;
                        break;

                    case CMP_FORMAT_BC3:
                    case CMP_FORMAT_DXT5:
                        // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                        textureCreateInfo.vkFormat         = VK_FORMAT_BC3_UNORM_BLOCK;
                        break;

                    case CMP_FORMAT_BC4:
                        // GL_COMPRESSED_RED_RGTC1;
                        textureCreateInfo.vkFormat         = VK_FORMAT_BC4_UNORM_BLOCK;
                        break;
                    case CMP_FORMAT_BC4_S:
                        // GL_COMPRESSED_SIGNED_RED_RGTC1;
                        textureCreateInfo.vkFormat         = VK_FORMAT_BC4_SNORM_BLOCK;
                        break;
                    case CMP_FORMAT_BC5:
                        // GL_COMPRESSED_RG_RGTC2;
                        textureCreateInfo.vkFormat         = VK_FORMAT_BC5_UNORM_BLOCK;
                        break;
                    case CMP_FORMAT_BC5_S:
                        // GL_COMPRESSED_SIGNED_RG_RGTC2;
                        textureCreateInfo.vkFormat         = VK_FORMAT_BC5_SNORM_BLOCK;
                        break;
                    case CMP_FORMAT_BC6H:
                        // GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;
                        textureCreateInfo.vkFormat         = VK_FORMAT_BC6H_UFLOAT_BLOCK;
                        break;
                    case CMP_FORMAT_BC6H_SF:
                        // GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;
                        textureCreateInfo.vkFormat         = VK_FORMAT_BC6H_SFLOAT_BLOCK;
                        break;
                    case CMP_FORMAT_BC7:
                        // RGB_BP_UNorm;
                        textureCreateInfo.vkFormat         = VK_FORMAT_BC7_UNORM_BLOCK;
                        break;
                    //case CMP_FORMAT_ATI1N:
                    //    // COMPRESSED_FORMAT_ATI1N_UNorm_TMP;
                    //    textureCreateInfo.vkFormat         = VK_FORMAT_BC4_UNORM_BLOCK;
                    //    break;
                    //case CMP_FORMAT_ATI2N:
                    //    // COMPRESSED_FORMAT_ATI2N_UNorm_TMP;
                    //    textureCreateInfo.vkFormat         = VK_FORMAT_BC5_UNORM_BLOCK;
                    //    break;
                    //case CMP_FORMAT_ATI2N_XY:
                    //    // COMPRESSED_FORMAT_ATI2N_XY_UNorm_TMP;
                    //    textureCreateInfo.vkFormat         = VK_FORMAT_BC5_UNORM_BLOCK;
                    // //    break;
                    // case CMP_FORMAT_ATC_RGB:
                    //     // ATC_RGB_AMD;
                    //     textureCreateInfo.vkFormat         = VK_FORMAT_UNDEFINED;
                    //     break;
                    // case CMP_FORMAT_ATC_RGBA_Explicit:
                    //     // ATC_RGBA_EXPLICIT_ALPHA_AMD;
                    //     textureCreateInfo.vkFormat         = VK_FORMAT_UNDEFINED;
                    //     break;
                    // case CMP_FORMAT_ATC_RGBA_Interpolated:
                    //     // ATC_RGBA_INTERPOLATED_ALPHA_AMD;
                    //     textureCreateInfo.vkFormat         = VK_FORMAT_UNDEFINED;
                    //     break;
                    case CMP_FORMAT_ETC_RGB:
                        // GL_ETC1_RGB8_OES;
                        textureCreateInfo.vkFormat         = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
                        break;
                    case CMP_FORMAT_ETC2_RGB:
                        // GL_COMPRESSED_RGB8_ETC2;
                        textureCreateInfo.vkFormat         = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
                        break;
                    case CMP_FORMAT_ETC2_SRGB:
                        // GL_COMPRESSED_SRGB8_ETC2;
                        textureCreateInfo.vkFormat         = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
                        break;
                    case CMP_FORMAT_ETC2_RGBA:
                        // GL_COMPRESSED_RGBA8_ETC2_EAC;
                        textureCreateInfo.vkFormat         = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
                        break;
                    case CMP_FORMAT_ETC2_RGBA1:
                        // GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
                        textureCreateInfo.vkFormat = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
                        break;
                    case CMP_FORMAT_ETC2_SRGBA:
                        // GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
                        textureCreateInfo.vkFormat         = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
                        break;
                    case CMP_FORMAT_ETC2_SRGBA1:
                        // GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
                        textureCreateInfo.vkFormat         = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
                        break;

                    // Not supported by GL_COMPRESSION_ formats
                    // case CMP_FORMAT_DXT5_xGBR:
                    //     // COMPRESSED_FORMAT_DXT5_xGBR_TMP;
                    //     textureCreateInfo.vkFormat         = VK_FORMAT_BC3_UNORM_BLOCK;
                    //     break;
                    // case CMP_FORMAT_DXT5_RxBG:
                    //     // COMPRESSED_FORMAT_DXT5_RxBG_TMP;
                    //     textureCreateInfo.vkFormat         = VK_FORMAT_BC3_UNORM_BLOCK;
                    //     break;
                    // case CMP_FORMAT_DXT5_RBxG:
                    //     // COMPRESSED_FORMAT_DXT5_RBxG_TMP;
                    //     textureCreateInfo.vkFormat         = VK_FORMAT_BC3_UNORM_BLOCK;
                    //     break;
                    // case CMP_FORMAT_DXT5_xRBG:
                    //     // COMPRESSED_FORMAT_DXT5_xRBG_TMP;
                    //     textureCreateInfo.vkFormat         = VK_FORMAT_BC3_UNORM_BLOCK;
                    //     break;
                    // case CMP_FORMAT_DXT5_RGxB:
                    //     // COMPRESSED_FORMAT_DXT5_RGxB_TMP;
                    //     textureCreateInfo.vkFormat         = VK_FORMAT_BC3_UNORM_BLOCK;
                    //     break;
                    // case CMP_FORMAT_DXT5_xGxR:
                    //     // COMPRESSED_FORMAT_DXT5_xGxR_TMP;
                    //     textureCreateInfo.vkFormat         = VK_FORMAT_BC3_UNORM_BLOCK;
                    //     break;
                    case CMP_FORMAT_ASTC:
                        if ((pMipSet->m_nBlockWidth == 4) && (pMipSet->m_nBlockHeight == 4))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 5) && (pMipSet->m_nBlockHeight == 4))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 5) && (pMipSet->m_nBlockHeight == 5))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 6) && (pMipSet->m_nBlockHeight == 5))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 6) && (pMipSet->m_nBlockHeight == 6))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 8) && (pMipSet->m_nBlockHeight == 5))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_8x5_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 8) && (pMipSet->m_nBlockHeight == 6))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_8x6_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 8) && (pMipSet->m_nBlockHeight == 8))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 10) && (pMipSet->m_nBlockHeight == 5))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_10x5_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 10) && (pMipSet->m_nBlockHeight == 6))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_10x6_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 10) && (pMipSet->m_nBlockHeight == 8))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_10x8_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 10) && (pMipSet->m_nBlockHeight == 10))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 12) && (pMipSet->m_nBlockHeight == 10))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_12x10_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
                        }
                        else if ((pMipSet->m_nBlockWidth == 12) && (pMipSet->m_nBlockHeight == 12))
                        {
                            // GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
                        }
                        else
                        {
                            // GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
                            textureCreateInfo.vkFormat         = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
                        }
                        break;
                    case CMP_FORMAT_BASIS:
                        // GL_RGBA8;
                        textureCreateInfo.vkFormat         = VK_FORMAT_R8G8B8A8_UNORM;
                        if (pMipSet->m_ChannelFormat == CF_Float16)
                        {
                            // GL_RGBA16F;
                            textureCreateInfo.vkFormat         = VK_FORMAT_R16G16B16A16_SFLOAT;
                        }
                        else if (pMipSet->m_ChannelFormat == CF_Float32)
                        {
                            // GL_RGBA32F;
                            textureCreateInfo.vkFormat         = VK_FORMAT_R32G32B32A32_SFLOAT;
                        }
                        break;
                }
            }
    }
    break;
    }


    if (textureCreateInfo.vkFormat == VK_FORMAT_UNDEFINED)
    {
        if (KTX2_CMips)
            KTX2_CMips->PrintError("Error: KTX2 plugin. Destination format is not supported.\n");
        return -1;
    }


    ktxTexture2* texture2 = nullptr;
    ktxTexture* texture = nullptr;

    KTX_error_code createStatus;
    createStatus = ktxTexture2_Create(&textureCreateInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture2);

    texture      = ktxTexture(texture2);

    if (createStatus != KTX_SUCCESS)
    {
        if (KTX2_CMips)
        {
            switch (createStatus)
            {
            case KTX_UNSUPPORTED_TEXTURE_TYPE:
                KTX2_CMips->PrintError("Error(KTX2 UNSUPPORTED TEXTURE TYPE) saving file = %s \n", pszFilename);
                break;
            default:
                KTX2_CMips->PrintError("Error(%d): Create status KTX2 Plugin on saving file = %s \n", createStatus, pszFilename);
            }
        }
        return -1;
    }

    int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : CMP_MaxFacesOrSlices(pMipSet, 0);
    for (int nSlice = 0; nSlice < nSlices; nSlice++)
    {
        for (int nMipLevel = 0; nMipLevel < pMipSet->m_nMipLevels; nMipLevel++)
        {

            MipLevel*      pMipLevel = KTX2_CMips->GetMipLevel(pMipSet, nMipLevel, nSlice);
            
            if (pMipLevel)
            {
                KTX_error_code setMemory = ktxTexture_SetImageFromMemory(texture,
                                                                     nMipLevel,
                                                                     0,
                                                                     nSlice,
                                                                     pMipLevel->m_pbData,
                                                                     pMipLevel->m_dwLinearSize);
                if (setMemory != KTX_SUCCESS)
                {
                    KTX2_CMips->PrintError("Error(%d):SetImageFromMemory KTX2 Plugin on saving file = %s \n", setMemory, pszFilename);
                    return -1;
                }
            }
            else
            {
                KTX2_CMips->PrintError("Error:GetMipLevel (%d,%d) KTX2 Plugin on saving file = %s \n", nMipLevel, nSlice, pszFilename);
                return -1;
            }
        }
    }

    if (pMipSet->m_format == CMP_FORMAT_BASIS)
    {
        ktx_uint32_t*  basisQuality = reinterpret_cast<ktx_uint32_t*>(pMipSet->pData);  // m_userData;
        KTX_error_code basisStatus  = ktxTexture2_CompressBasis(texture2, *basisQuality);
        if (basisStatus != KTX_SUCCESS)
        {
            KTX2_CMips->PrintError("Error(%d): Basis status KTX2 Plugin on saving file = %s \n", basisStatus, pszFilename);
            return -1;
        }
    }

    std::stringstream writer;
    writeId2(writer);
    ktxHashList_AddKVPair(&texture->kvDataHead, KTX_WRITER_KEY, (ktx_uint32_t)writer.str().length() + 1, writer.str().c_str());

    KTX_error_code save = ktxTexture_WriteToNamedFile(texture, pszFilename);
    if (save != KTX_SUCCESS)
    {
        KTX2_CMips->PrintError("Error(%d): WriteToNamedFile KTX2 Plugin on saving file = %s \n", save, pszFilename);
        return -1;
    }

    return 0;
}
