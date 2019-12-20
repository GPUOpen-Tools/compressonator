// AMD AMDUtils code
// 
// Copyright(c) 2017 Advanced Micro Devices, Inc.All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248926
// http://go.microsoft.com/fwlink/?LinkId=248929 

#include "stdafx.h"
#include "GltfFeatures.h"
#include "TextureVK.h"
#include "dxgi.h"
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include "Misc.h"
#include "UtilFuncs.h"

VkFormat Translate(DXGI_FORMAT format);

//--------------------------------------------------------------------------------------
// Constructor of the Texture class
// initializes all members
//--------------------------------------------------------------------------------------
Texture::Texture()
{
#ifdef USE_CMP_LOAD
    pMipSet = nullptr;
#endif
}

//--------------------------------------------------------------------------------------
// Destructor of the Texture class
//--------------------------------------------------------------------------------------
Texture::~Texture()
{
    OnDestroy();
}

void Texture::OnDestroy()
{

}

UINT32 Texture::GetDxGiFormat(Texture::DDS_PIXELFORMAT pixelFmt) const
{
    if(pixelFmt.flags & 0x00000004)   //DDPF_FOURCC
    {
            if (MAKEFOURCC( 'D', 'X', 'T', '1' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC1_UNORM;
            }
            if (MAKEFOURCC( 'D', 'X', 'T', '3' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC2_UNORM;
            }
            if (MAKEFOURCC( 'D', 'X', 'T', '5' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC3_UNORM;
            }

            // While pre-multiplied alpha isn't directly supported by the DXGI formats,
            // they are basically the same as these BC formats so they can be mapped
            if (MAKEFOURCC( 'D', 'X', 'T', '2' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC2_UNORM;
            }
            if (MAKEFOURCC( 'D', 'X', 'T', '4' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC3_UNORM;
            }

            if (MAKEFOURCC( 'A', 'T', 'I', '1' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC4_UNORM;
            }
            if (MAKEFOURCC( 'B', 'C', '4', 'U' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC4_UNORM;
            }
            if (MAKEFOURCC( 'B', 'C', '4', 'S' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC4_SNORM;
            }

            if (MAKEFOURCC( 'A', 'T', 'I', '2' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC5_UNORM;
            }
            if (MAKEFOURCC( 'B', 'C', '5', 'U' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC5_UNORM;
            }
            if (MAKEFOURCC( 'B', 'C', '5', 'S' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_BC5_SNORM;
            }

            // BC6H and BC7 are written using the "DX10" extended header

            if (MAKEFOURCC( 'R', 'G', 'B', 'G' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_R8G8_B8G8_UNORM;
            }
            if (MAKEFOURCC( 'G', 'R', 'G', 'B' ) == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_G8R8_G8B8_UNORM;
            }

            if (MAKEFOURCC('Y','U','Y','2') == pixelFmt.fourCC)
            {
                return DXGI_FORMAT_YUY2;
            }

        // Check for D3DFORMAT enums being set here
        switch(pixelFmt.fourCC)
        {
            case 36: // D3DFMT_A16B16G16R16
                return DXGI_FORMAT_R16G16B16A16_UNORM;

            case 110: // D3DFMT_Q16W16V16U16
                return DXGI_FORMAT_R16G16B16A16_SNORM;

            case 111: // D3DFMT_R16F
                return DXGI_FORMAT_R16_FLOAT;

            case 112: // D3DFMT_G16R16F
                return DXGI_FORMAT_R16G16_FLOAT;

            case 113: // D3DFMT_A16B16G16R16F
                return DXGI_FORMAT_R16G16B16A16_FLOAT;

            case 114: // D3DFMT_R32F
                return DXGI_FORMAT_R32_FLOAT;

            case 115: // D3DFMT_G32R32F
                return DXGI_FORMAT_R32G32_FLOAT;

            case 116: // D3DFMT_A32B32G32R32F
                return DXGI_FORMAT_R32G32B32A32_FLOAT;

            default:
                return 0;
            }
    }
    else
    {
        {
            switch(pixelFmt.bitMaskR)
            {
            case 0xff:
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            case 0x00ff0000:
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            case 0xffff:
                return DXGI_FORMAT_R16G16_UNORM;
            case 0x3ff:
                return DXGI_FORMAT_R10G10B10A2_UNORM;
            case 0x7c00:
                return DXGI_FORMAT_B5G5R5A1_UNORM;
            case 0xf800:
                return DXGI_FORMAT_B5G6R5_UNORM;
            case 0:
                return DXGI_FORMAT_A8_UNORM;
            default:
                return 0;
            };

            if (pixelFmt.bitCount == 32)  {

                if (ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0xff000000))
                {
                    return DXGI_FORMAT_R8G8B8A8_UNORM;
                }

                if (ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0xff000000))
                {
                    return DXGI_FORMAT_B8G8R8A8_UNORM;
                }

                if (ISBITMASK(0x00ff0000,0x0000ff00,0x000000ff,0x00000000))
                {
                    return DXGI_FORMAT_B8G8R8X8_UNORM;
                }

                // No DXGI format maps to ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) aka D3DFMT_X8B8G8R8

                // Note that many common DDS reader/writers (including D3DX) swap the
                // the RED/BLUE masks for 10:10:10:2 formats. We assume
                // below that the 'backwards' header mask is being used since it is most
                // likely written by D3DX. The more robust solution is to use the 'DX10'
                // header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

                // For 'correct' writers, this should be 0x000003ff,0x000ffc00,0x3ff00000 for RGB data
                if (ISBITMASK(0x3ff00000,0x000ffc00,0x000003ff,0xc0000000))
                {
                    return DXGI_FORMAT_R10G10B10A2_UNORM;
                }

                // No DXGI format maps to ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) aka D3DFMT_A2R10G10B10

                if (ISBITMASK(0x0000ffff,0xffff0000,0x00000000,0x00000000))
                {
                    return DXGI_FORMAT_R16G16_UNORM;
                }

                if (ISBITMASK(0xffffffff,0x00000000,0x00000000,0x00000000))
                {
                    // Only 32-bit color channel format in D3D9 was R32F
                    return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
                }
            }

            if (pixelFmt.bitMaskR == 0xffff)
                return DXGI_FORMAT_R16G16_UNORM;

            if (pixelFmt.bitMaskR == 0x3ff)
                return DXGI_FORMAT_R10G10B10A2_UNORM;

            if (pixelFmt.bitMaskR == 0x7c00)
                return DXGI_FORMAT_B5G5R5A1_UNORM;

            if (pixelFmt.bitMaskR == 0xf800)
                return DXGI_FORMAT_B5G6R5_UNORM;

            if (pixelFmt.bitMaskR == 0)
                return DXGI_FORMAT_A8_UNORM;
        }
    }
    return 0;
}

bool Texture::isDXT(DXGI_FORMAT format) const
{
    return (format >= DXGI_FORMAT_BC1_TYPELESS) && (format <= DXGI_FORMAT_BC5_SNORM);
}

//--------------------------------------------------------------------------------------
// return the byte size of a pixel (or block if block compressed)
//--------------------------------------------------------------------------------------
UINT32 Texture::GetPixelSize(DXGI_FORMAT fmt) const
{
    switch(fmt)
    {
    case(DXGI_FORMAT_BC1_TYPELESS) :
    case(DXGI_FORMAT_BC1_UNORM) :
    case(DXGI_FORMAT_BC1_UNORM_SRGB) :
    case(DXGI_FORMAT_BC4_TYPELESS) :
    case(DXGI_FORMAT_BC4_UNORM) :
    case(DXGI_FORMAT_BC4_SNORM) :
        return 8;

    case(DXGI_FORMAT_BC2_TYPELESS) :
    case(DXGI_FORMAT_BC2_UNORM) :
    case(DXGI_FORMAT_BC2_UNORM_SRGB) :
    case(DXGI_FORMAT_BC3_TYPELESS) :
    case(DXGI_FORMAT_BC3_UNORM) :
    case(DXGI_FORMAT_BC3_UNORM_SRGB) :
    case(DXGI_FORMAT_BC5_TYPELESS) :
    case(DXGI_FORMAT_BC5_UNORM) :
    case(DXGI_FORMAT_BC5_SNORM) :
    case(DXGI_FORMAT_BC6H_TYPELESS) :
    case(DXGI_FORMAT_BC6H_UF16) :
    case(DXGI_FORMAT_BC6H_SF16) :
    case(DXGI_FORMAT_BC7_TYPELESS) :
    case(DXGI_FORMAT_BC7_UNORM) :
    case(DXGI_FORMAT_BC7_UNORM_SRGB) :
        return 16;

    default:
        break;
    }
    return 0;
}

void Texture::PatchFmt24To32Bit(unsigned char *pDst, unsigned char *pSrc, UINT32 pixelCount)
{
    // copy pixel data, interleave with A
    for(unsigned int i = 0; i < pixelCount; ++i)
    {
        pDst[0] = pSrc[0];
        pDst[1] = pSrc[1];
        pDst[2] = pSrc[2];
        pDst[3] = 0xFF;
        pDst += 4;
        pSrc += 3;
    }
}

bool Texture::isCubemap() const
{
#ifdef USE_CMP_LOAD
    return pMipSet->m_nDepth == 6;
#else
    return m_header.arraySize == 6;
#endif
}

void Texture::CreateSRV(DWORD index, VkImageView *pRV)
{
    VkImageViewCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    info.image = m_pTexture2D;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    info.format = m_format;
    info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    info.subresourceRange.baseMipLevel = 0;
#ifdef USE_CMP_LOAD
    if(pMipSet)
        info.subresourceRange.levelCount = pMipSet->m_nMipLevels;
    else
        info.subresourceRange.levelCount = 1;
#else
    info.subresourceRange.levelCount = m_header.mipMapCount;
#endif
    info.subresourceRange.baseArrayLayer = 0;
    info.subresourceRange.layerCount = 1;
    info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    VkResult res = vkCreateImageView(m_device, &info, NULL, pRV);
    assert(res == VK_SUCCESS);
}

void Texture::CreateDSV(DWORD index, VkImageView *pView)
{
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = NULL;
    view_info.image = m_pTexture2D;
    view_info.format = m_format;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.flags = 0;

    if (m_format == VK_FORMAT_D16_UNORM_S8_UINT || m_format == VK_FORMAT_D24_UNORM_S8_UINT || m_format == VK_FORMAT_D32_SFLOAT_S8_UINT)
    {
        view_info.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VkResult res = vkCreateImageView(m_device, &view_info, NULL, pView);
    assert(res == VK_SUCCESS);

}

INT32 Texture::InitDepthStencil(DeviceVK *pDevice, UINT width, UINT height)
{
    m_device = pDevice->GetDevice();

    m_format = VK_FORMAT_D32_SFLOAT;

    VkResult res;

    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = NULL;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = m_format;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = NULL;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.flags = 0;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;   // TODO: check if VK_IMAGE_TILING_LINEAR is better

                                                   /* Create image */
    res = vkCreateImage(m_device, &image_info, NULL, &m_pTexture2D);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(m_device, m_pTexture2D, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = NULL;
    alloc_info.allocationSize = 0;
    alloc_info.allocationSize = mem_reqs.size;
    alloc_info.memoryTypeIndex = 0;

    bool pass = memory_type_from_properties(pDevice->GetPhysicalDeviceMemoryProperties(), mem_reqs.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        &alloc_info.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    /* Allocate memory */
    res = vkAllocateMemory(m_device, &alloc_info, NULL, &m_deviceMemory);
    assert(res == VK_SUCCESS);

    /* bind memory */
    res = vkBindImageMemory(m_device, m_pTexture2D, m_deviceMemory, 0);
    assert(res == VK_SUCCESS);

    return 0;
}

#ifdef USE_CMP_LOAD
VkFormat MIP2VK_Format(MipSet* pMipsTexture)
{
    VkFormat m_VKnum;
    switch (pMipsTexture->m_format)
    {
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:
        m_VKnum = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
        break;
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_DXT3:
        m_VKnum = VK_FORMAT_BC2_UNORM_BLOCK;
        break;
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_DXT5:
        m_VKnum = VK_FORMAT_BC3_UNORM_BLOCK;
        break;
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_ATI1N:
        m_VKnum = VK_FORMAT_BC4_UNORM_BLOCK;
        break;
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_ATI2N:
    case CMP_FORMAT_ATI2N_XY:
    case CMP_FORMAT_ATI2N_DXT5:
        m_VKnum = VK_FORMAT_BC5_UNORM_BLOCK;
        break;
    case CMP_FORMAT_BC6H:
        m_VKnum = VK_FORMAT_BC6H_UFLOAT_BLOCK;
        break;
    case CMP_FORMAT_BC6H_SF:
        m_VKnum = VK_FORMAT_BC6H_SFLOAT_BLOCK;
        break;
    case CMP_FORMAT_BC7:
        m_VKnum = VK_FORMAT_BC7_UNORM_BLOCK;
        break;
    case CMP_FORMAT_ETC_RGB:
    case CMP_FORMAT_ETC2_RGB:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        break;
    case CMP_FORMAT_ETC2_SRGB:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
        break;
    case CMP_FORMAT_ETC2_RGBA:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
        break;
    case CMP_FORMAT_ETC2_RGBA1:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
        break;
    case CMP_FORMAT_ETC2_SRGBA:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
        break;
    case CMP_FORMAT_ETC2_SRGBA1:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
        break;
	case CMP_FORMAT_ARGB_8888:
		m_VKnum = VK_FORMAT_R8G8B8A8_UNORM;
		break;
	case CMP_FORMAT_ABGR_8888:
		m_VKnum = VK_FORMAT_B8G8R8A8_UNORM;
		break;
    case CMP_FORMAT_ASTC:
        if ((pMipsTexture->m_nBlockWidth == 4) && (pMipsTexture->m_nBlockHeight == 4))
            m_VKnum = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 5) && (pMipsTexture->m_nBlockHeight == 4))
            m_VKnum = VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 5) && (pMipsTexture->m_nBlockHeight == 5))
            m_VKnum = VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 6) && (pMipsTexture->m_nBlockHeight == 5))
            m_VKnum = VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 6) && (pMipsTexture->m_nBlockHeight == 6))
            m_VKnum = VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 8) && (pMipsTexture->m_nBlockHeight == 5))
            m_VKnum = VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 8) && (pMipsTexture->m_nBlockHeight == 6))
            m_VKnum = VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 8) && (pMipsTexture->m_nBlockHeight == 8))
            m_VKnum = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 10) && (pMipsTexture->m_nBlockHeight == 5))
            m_VKnum = VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 10) && (pMipsTexture->m_nBlockHeight == 6))
            m_VKnum = VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth== 10) && (pMipsTexture->m_nBlockHeight == 8))
            m_VKnum = VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 10) && (pMipsTexture->m_nBlockHeight == 10))
            m_VKnum = VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 12) && (pMipsTexture->m_nBlockHeight == 10))
            m_VKnum = VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
        else if ((pMipsTexture->m_nBlockWidth == 12) && (pMipsTexture->m_nBlockHeight == 12))
            m_VKnum = VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        else
            m_VKnum = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        break;
    default:
        m_VKnum = VK_FORMAT_UNDEFINED;
        break;
    }
    return m_VKnum;
}
#endif

//--------------------------------------------------------------------------------------
// entry function to initialize an image from a .DDS texture
//--------------------------------------------------------------------------------------
INT32 Texture::InitFromFile(DeviceVK *pDevice, UploadHeapVK* pUploadHeap, const char *pFilename, void *pluginManager, void *msghandler)
{
    OnDestroy();

    if (VK_CMips)
        setCMIPS(VK_CMips);

    m_device = pDevice->GetDevice();
#ifdef USE_CMP_LOAD

    // load the MipSet and Process it to the Model
    WCHAR wcstrPath[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, pFilename, -1, wcstrPath, MAX_PATH);
    if (LoadImageMipSetFromFile(wcstrPath, pluginManager) == 0)
    {
        if (!pMipSet)
            return -1;

        m_format = MIP2VK_Format(pMipSet);

		if (m_format == VK_FORMAT_UNDEFINED)
		{
			PrintInfo("Error: VKFormat undefined.\n");
			CleanMipSet();
			return -1;
		}

        // Create the Image:
        {
            VkImageCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            info.imageType = VK_IMAGE_TYPE_2D;
            info.format = m_format;
            info.extent.width = pMipSet->m_nWidth;
            info.extent.height = pMipSet->m_nHeight;
            info.extent.depth = 1;
            info.mipLevels = pMipSet->m_nMipLevels;
            info.arrayLayers = 1;
            info.samples = VK_SAMPLE_COUNT_1_BIT;
            info.tiling = VK_IMAGE_TILING_OPTIMAL;
            info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkResult res = vkCreateImage(pDevice->GetDevice(), &info, NULL, &m_pTexture2D);
            assert(res == VK_SUCCESS);

            VkMemoryRequirements mem_reqs;
            vkGetImageMemoryRequirements(pDevice->GetDevice(), m_pTexture2D, &mem_reqs);

            VkMemoryAllocateInfo alloc_info = {};
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = mem_reqs.size;
            alloc_info.memoryTypeIndex = 0;

            bool pass = memory_type_from_properties(pDevice->GetPhysicalDeviceMemoryProperties(), mem_reqs.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                &alloc_info.memoryTypeIndex);
            assert(pass && "No mappable, coherent memory");

            res = vkAllocateMemory(pDevice->GetDevice(), &alloc_info, NULL, &m_deviceMemory);
            assert(res == VK_SUCCESS);

            res = vkBindImageMemory(pDevice->GetDevice(), m_pTexture2D, m_deviceMemory, 0);
            assert(res == VK_SUCCESS);
        }

        struct
        {
            UINT8* pixels;
            DWORD width, height, offset;
        } footprints[12];

        for (CMP_INT mip = 0; mip < pMipSet->m_nMipLevels; mip++)
        {
            DWORD dwWidth;
            dwWidth = pMipSet->m_nWidth >> mip;
            if (dwWidth < 1) dwWidth = 1;

            DWORD dwHeight;
            dwHeight = pMipSet->m_nHeight >> mip;
            if (dwHeight < 1) dwHeight = 1;

            UINT8* pixels = pUploadHeap->Suballocate(dwWidth*dwHeight * 4, 512);

            footprints[mip].pixels = pixels;
            footprints[mip].offset = DWORD(pixels - pUploadHeap->BasePtr());
            footprints[mip].width = dwWidth;
            footprints[mip].height = dwHeight;
        }

        int offset = 0;

        for (CMP_INT a = 0; a < pMipSet->m_nDepth; a++)
        {
            UINT32 mipMapCount = pMipSet->m_nMipLevels;

            for (UINT mip = 0; mip < mipMapCount; ++mip)
            {
                // skip mipmaps that are smaller than blocksize
                if (mip > 0)
                {
                    UINT w0 = footprints[mip - 1].width;
                    UINT w1 = footprints[mip].width;
                    UINT h0 = footprints[mip - 1].height;
                    UINT h1 = footprints[mip].height;
                    if ((w0 == w1) && (h0 == h1))
                    {
                        --mipMapCount;
                        --mip;
                        continue;
                    }
                }

                MipLevel* pInMipLevel = m_CMIPS->GetMipLevel(pMipSet, mip, a);
                if (!pInMipLevel)
                {
                    PrintInfo("Error: Failed to read Miplevel texture data.\n");
                    CleanMipSet();
                    return -1;
                }

                // Valid Mip Level ?
                if (!(pInMipLevel->m_pbData))
                {
                    PrintInfo("Error: Failed to read Miplevel texture data.\n");
                    CleanMipSet();
                    return -1;
                }

                UINT w = pInMipLevel->m_nWidth;
                UINT h = pInMipLevel->m_nHeight;
                //UINT bytePP = header->ddspf.bitCount != 0 ? header->ddspf.bitCount / 8 : GetPixelSize((DXGI_FORMAT)m_header.format);

                //footprints[mip].width = static_cast<UINT>(Align(footprints[mip].width, 512));

                // read DDS
                unsigned char *pData = footprints[mip].pixels;
                for (UINT y = 0; y < h; ++y)
                {
                    memcpy(&pData[footprints[mip].width * 4 * y], pInMipLevel->m_pbData, (DWORD)footprints[mip].width * 4);
                    pInMipLevel->m_pbData += (DWORD)footprints[mip].width * 4;
                }
            }
        }


        // Copy to Image:
        {
            VkImageMemoryBarrier copy_barrier[1] = {};
            copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier[0].image = m_pTexture2D;
            copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_barrier[0].subresourceRange.baseMipLevel = 0;
            copy_barrier[0].subresourceRange.levelCount = pMipSet->m_nMipLevels;
            copy_barrier[0].subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(pUploadHeap->GetCommandList(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);

            for (CMP_INT mip = 0; mip < pMipSet->m_nMipLevels; mip++)
            {
                VkBufferImageCopy region = {};
                region.bufferOffset = footprints[mip].offset;
                region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.layerCount = 1;
                region.imageSubresource.mipLevel = mip;
                region.imageExtent.width = footprints[mip].width;
                region.imageExtent.height = footprints[mip].height;
                region.imageExtent.depth = 1;
                vkCmdCopyBufferToImage(pUploadHeap->GetCommandList(), pUploadHeap->GetResource(), m_pTexture2D, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
            }

            VkImageMemoryBarrier use_barrier[1] = {};
            use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            use_barrier[0].image = m_pTexture2D;
            use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            use_barrier[0].subresourceRange.levelCount = 1;
            use_barrier[0].subresourceRange.levelCount = pMipSet->m_nMipLevels;
            use_barrier[0].subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(pUploadHeap->GetCommandList(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, use_barrier);
        }
    }

    // Done cleanup temp used mipset
    //CleanMipSet();
#else
    typedef enum RESOURCE_DIMENSION
    {
        RESOURCE_DIMENSION_UNKNOWN = 0,
        RESOURCE_DIMENSION_BUFFER = 1,
        RESOURCE_DIMENSION_TEXTURE1D = 2,
        RESOURCE_DIMENSION_TEXTURE2D = 3,
        RESOURCE_DIMENSION_TEXTURE3D = 4
    } RESOURCE_DIMENSION;

    typedef struct
    {
        UINT32           dxgiFormat;
        RESOURCE_DIMENSION  resourceDimension;
        UINT32           miscFlag;
        UINT32           arraySize;
        UINT32           reserved;
    } DDS_HEADER_DXT10;

    // get the last 4 char (assuming this is the file extension)
    size_t len = strlen(pFilename);
    char ext[5] = { 0 };
    for (int i = 0; i < 4; ++i)
    {
        ext[i] = tolower(pFilename[len - 4 + i]);
    }

    // check if the extension is known
    UINT32 ext4CC = *reinterpret_cast<const UINT32 *>(ext);
    if (ext4CC != 'sdd.')
        return -1;

    if (GetFileAttributesA(pFilename) == 0xFFFFFFFF)
        return -1;

    HANDLE hFile = CreateFileA(pFilename,             // file to open
        GENERIC_READ,          // open for reading
        FILE_SHARE_READ,       // share for reading
        NULL,                  // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);                 // no attr. template
    if (hFile == INVALID_HANDLE_VALUE)
        return -1;

    LARGE_INTEGER largeFileSize;
    GetFileSizeEx(hFile, &largeFileSize);
    assert(0 == largeFileSize.HighPart);
    UINT32 fileSize = largeFileSize.LowPart;
    UINT32 rawTextureSize = fileSize;

    // read the header
    char headerData[4 + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10)];
    DWORD dwBytesRead = 0;
    if (ReadFile(hFile, headerData, 4 + sizeof(DDS_HEADER) + sizeof(DDS_HEADER_DXT10), &dwBytesRead,
        NULL))
    {
        char *pByteData = headerData;
        UINT32 dwMagic = *reinterpret_cast<UINT32 *>(pByteData);
        pByteData += 4;
        rawTextureSize -= 4;

        DDS_HEADER *header = reinterpret_cast<DDS_HEADER *>(pByteData);
        pByteData += sizeof(DDS_HEADER);
        rawTextureSize -= sizeof(DDS_HEADER);

        DDS_HEADER_DXT10 *header10 = NULL;
        if (dwMagic == '01XD')   // "DX10"
        {
            header10 = reinterpret_cast<DDS_HEADER_DXT10 *>(&pByteData[4]);
            pByteData += sizeof(DDS_HEADER_DXT10);
            rawTextureSize -= sizeof(DDS_HEADER_DXT10);

            DDS_HEADER_INFO dx10header =
            {
                header->dwWidth,
                header->dwHeight,
                header->dwDepth,
                header10->arraySize,
                header->dwMipMapCount,
                header10->dxgiFormat
            };
            m_header = dx10header;
        }
        else if (dwMagic == ' SDD')   // "DDS "
        {
            // DXGI
            UINT32 arraySize = (header->dwCubemapFlags == 0xfe00) ? 6 : 1;
            UINT32 dxgiFormat = GetDxGiFormat(header->ddspf);
            UINT32 mipMapCount = header->dwMipMapCount ? header->dwMipMapCount : 1;

            DDS_HEADER_INFO dx10header =
            {
                header->dwWidth,
                header->dwHeight,
                header->dwDepth ? header->dwDepth : 1,
                arraySize,
                mipMapCount,
                dxgiFormat
            };
            m_header = dx10header;
        }
        else
        {
            return -1;
        }

        m_format = Translate((DXGI_FORMAT)m_header.format);

        // Create the Image:
        {
            VkImageCreateInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            info.imageType = VK_IMAGE_TYPE_2D;
            info.format = m_format;
            info.extent.width = header->dwWidth;
            info.extent.height = header->dwHeight;
            info.extent.depth = 1;
            info.mipLevels = header->dwMipMapCount;
            info.arrayLayers = 1;
            info.samples = VK_SAMPLE_COUNT_1_BIT;
            info.tiling = VK_IMAGE_TILING_OPTIMAL;
            info.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            VkResult res = vkCreateImage(pDevice->GetDevice(), &info, NULL, &m_pTexture2D);
            assert(res == VK_SUCCESS);

            VkMemoryRequirements mem_reqs;
            vkGetImageMemoryRequirements(pDevice->GetDevice(), m_pTexture2D, &mem_reqs);

            VkMemoryAllocateInfo alloc_info = {};
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = mem_reqs.size;
            alloc_info.memoryTypeIndex = 0;

            bool pass = memory_type_from_properties(pDevice->GetPhysicalDeviceMemoryProperties(), mem_reqs.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                &alloc_info.memoryTypeIndex);
            assert(pass && "No mappable, coherent memory");

            res = vkAllocateMemory(pDevice->GetDevice(), &alloc_info, NULL, &m_deviceMemory);
            assert(res == VK_SUCCESS);

            res = vkBindImageMemory(pDevice->GetDevice(), m_pTexture2D, m_deviceMemory, 0);
            assert(res == VK_SUCCESS);
        }

        struct
        {
            UINT8* pixels;
            DWORD width, height, offset;
        } footprints[12];

        for (UINT mip = 0; mip < m_header.mipMapCount; mip++)
        {
            DWORD dwWidth;
            dwWidth = header->dwWidth >> mip;
            if (dwWidth < 1) dwWidth = 1;

            DWORD dwHeight;
            dwHeight = header->dwHeight >> mip;
            if (dwHeight < 1) dwHeight =1;

            UINT8* pixels  = pUploadHeap->Suballocate(dwWidth*dwHeight * 4, 512);

            footprints[mip].pixels = pixels;
            footprints[mip].offset = DWORD(pixels - pUploadHeap->BasePtr());
            footprints[mip].width = dwWidth;
            footprints[mip].height = dwHeight;
        }

        if (header->ddspf.bitCount == 24)
        {
            // alloc CPU memory & read DDS
            unsigned char *fileMemory = new unsigned char[rawTextureSize];
            unsigned char *pFile = fileMemory;
            SetFilePointer(hFile, fileSize - rawTextureSize, 0, FILE_BEGIN);
            ReadFile(hFile, fileMemory, rawTextureSize, &dwBytesRead, NULL);

            for (DWORD a = 0; a < m_header.arraySize; a++)
            {
                for (UINT mip = 0; mip < m_header.mipMapCount; mip++)
                {
                    // convert DDS to 32 bit
                    unsigned char *pData = footprints[mip].pixels;
                    for (UINT y = 0; y < footprints[mip].height; ++y)
                    {
                        PatchFmt24To32Bit(&pData[footprints[mip].width*y], pFile, footprints[mip].width);
                        pFile += footprints[mip].width * 3;
                    }
                }
            }
            delete[] fileMemory;
        }
        else
        {
            SetFilePointer(hFile, fileSize - rawTextureSize, 0, FILE_BEGIN);

            int offset = 0;

            for (DWORD a = 0; a < m_header.arraySize; a++)
            {
                UINT32 mipMapCount = m_header.mipMapCount;

                for (UINT mip = 0; mip < mipMapCount; ++mip)
                {
                    // skip mipmaps that are smaller than blocksize
                    if (mip > 0)
                    {
                        UINT w0 = footprints[mip - 1].width;
                        UINT w1 = footprints[mip].width;
                        UINT h0 = footprints[mip - 1].height;
                        UINT h1 = footprints[mip].height;
                        if ((w0 == w1) && (h0 == h1))
                        {
                            --mipMapCount;
                            --mip;
                            continue;
                        }
                    }
                    UINT w = isDXT((DXGI_FORMAT)m_header.format) ? footprints[mip].width / 4 : footprints[mip].width;
                    UINT h = isDXT((DXGI_FORMAT)m_header.format) ? footprints[mip].height / 4 : footprints[mip].height;
                    UINT bytePP = header->ddspf.bitCount != 0 ? header->ddspf.bitCount / 8 : GetPixelSize((DXGI_FORMAT)m_header.format);

                    //footprints[mip].width = static_cast<UINT>(Align(footprints[mip].width, 512));

                    // read DDS
                    unsigned char *pData = footprints[mip].pixels;
                    for (UINT y = 0; y < h; ++y)
                    {
                        ReadFile(hFile, &pData[footprints[mip].width * 4 * y], (DWORD)footprints[mip].width * 4, &dwBytesRead, NULL);
                    }
                }
            }
        }

        CloseHandle(hFile);

        // Copy to Image:
        {
            VkImageMemoryBarrier copy_barrier[1] = {};
            copy_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            copy_barrier[0].dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            copy_barrier[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            copy_barrier[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            copy_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            copy_barrier[0].image = m_pTexture2D;
            copy_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copy_barrier[0].subresourceRange.baseMipLevel = 0;
            copy_barrier[0].subresourceRange.levelCount = m_header.mipMapCount;
            copy_barrier[0].subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(pUploadHeap->GetCommandList(), VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, copy_barrier);

            for (UINT mip = 0; mip < m_header.mipMapCount; mip++)
            {
                VkBufferImageCopy region = {};
                region.bufferOffset = footprints[mip].offset;
                region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.layerCount = 1;
                region.imageSubresource.mipLevel = mip;
                region.imageExtent.width = footprints[mip].width;
                region.imageExtent.height = footprints[mip].height;
                region.imageExtent.depth = 1;
                vkCmdCopyBufferToImage(pUploadHeap->GetCommandList(), pUploadHeap->GetResource(), m_pTexture2D, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
            }

            VkImageMemoryBarrier use_barrier[1] = {};
            use_barrier[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            use_barrier[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            use_barrier[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            use_barrier[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            use_barrier[0].newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            use_barrier[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            use_barrier[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            use_barrier[0].image = m_pTexture2D;
            use_barrier[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            use_barrier[0].subresourceRange.levelCount = 1;
            use_barrier[0].subresourceRange.levelCount = m_header.mipMapCount;
            use_barrier[0].subresourceRange.layerCount = 1;
            vkCmdPipelineBarrier(pUploadHeap->GetCommandList(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, use_barrier);
        }
    }

#endif
    return 0;
}

VkFormat Translate(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_B8G8R8A8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
    case DXGI_FORMAT_R8G8B8A8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
    default: assert(true);  return VK_FORMAT_UNDEFINED;
    }
}
