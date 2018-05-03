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

#ifdef USE_CMP_LOAD
PluginManager          g_pluginManager;
bool                   g_bAbortCompression = false;
CMIPS*                 g_CMIPS = NULL;
#endif

VkFormat Translate(DXGI_FORMAT format);

//--------------------------------------------------------------------------------------
// Constructor of the Texture class
// initializes all members
//--------------------------------------------------------------------------------------
Texture::Texture()
{
#ifdef USE_CMP_LOAD
    pMipSet = NULL;
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

#ifdef USE_CMP_LOAD
    if (pMipSet)
    {
        free(pMipSet);
        pMipSet = NULL;
    }
    if (g_CMIPS)
    {
        free(g_CMIPS);
        g_CMIPS = NULL;
    }
#endif
}

//--------------------------------------------------------------------------------------
// retrieve the GetDxGiFormat from a DDS_PIXELFORMAT
// based on http://msdn.microsoft.com/en-us/library/windows/desktop/bb943991(v=vs.85).aspx
//--------------------------------------------------------------------------------------
UINT32 Texture::GetDxGiFormat(Texture::DDS_PIXELFORMAT pixelFmt) const
{
    if(pixelFmt.flags & 0x00000004)   //DDPF_FOURCC
    {
        // Check for D3DFORMAT enums being set here
        switch(pixelFmt.fourCC)
        {
        case '1TXD':
            return DXGI_FORMAT_BC1_UNORM;
        case '3TXD':
            return DXGI_FORMAT_BC2_UNORM;
        case '5TXD':
            return DXGI_FORMAT_BC3_UNORM;
        case 'U4CB':
            return DXGI_FORMAT_BC4_UNORM;
        case 'A4CB':
            return DXGI_FORMAT_BC4_SNORM;
        case '2ITA':
            return DXGI_FORMAT_BC5_UNORM;
        case 'S5CB':
            return DXGI_FORMAT_BC5_SNORM;
        case 'GBGR':
            return DXGI_FORMAT_R8G8_B8G8_UNORM;
        case 'BGRG':
            return DXGI_FORMAT_G8R8_G8B8_UNORM;
        case 36:
            return DXGI_FORMAT_R16G16B16A16_UNORM;
        case 110:
            return DXGI_FORMAT_R16G16B16A16_SNORM;
        case 111:
            return DXGI_FORMAT_R16_FLOAT;
        case 112:
            return DXGI_FORMAT_R16G16_FLOAT;
        case 113:
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case 114:
            return DXGI_FORMAT_R32_FLOAT;
        case 115:
            return DXGI_FORMAT_R32G32_FLOAT;
        case 116:
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
        }
    }
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
    return m_header.arraySize == 6;
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
    info.subresourceRange.levelCount = m_header.mipMapCount;
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
DXGI_FORMAT CMP2DXGIFormat(CMP_FORMAT cmp_format)
{
    DXGI_FORMAT dxgi_format;

    switch (cmp_format)
    {
        // Compression formats ----------
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:
        dxgi_format = DXGI_FORMAT_BC1_UNORM;
        break;
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_DXT3:
        dxgi_format = DXGI_FORMAT_BC2_UNORM;
        break;
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_DXT5:
        dxgi_format = DXGI_FORMAT_BC3_UNORM;
        break;
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_ATI1N:
        dxgi_format = DXGI_FORMAT_BC4_UNORM;
        break;
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_ATI2N:
    case CMP_FORMAT_ATI2N_XY:
    case CMP_FORMAT_ATI2N_DXT5:
        dxgi_format = DXGI_FORMAT_BC5_UNORM;
        break;
    case CMP_FORMAT_BC6H:
        dxgi_format = DXGI_FORMAT_BC6H_UF16;
        break;
    case CMP_FORMAT_BC6H_SF:
        dxgi_format = DXGI_FORMAT_BC6H_SF16;
        break;
    case CMP_FORMAT_BC7:
        dxgi_format = DXGI_FORMAT_BC7_UNORM;
        break;
        //uncompressed format
    case CMP_FORMAT_ARGB_8888:
    case CMP_FORMAT_RGBA_8888:
        dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
    case CMP_FORMAT_ABGR_8888:
    case CMP_FORMAT_BGRA_8888:
        dxgi_format = DXGI_FORMAT_B8G8R8A8_UNORM;
        break;
    case CMP_FORMAT_RGB_888:
        dxgi_format = DXGI_FORMAT_B8G8R8A8_UNORM;
        break;
    case CMP_FORMAT_BGR_888:
        dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        break;
    case CMP_FORMAT_RG_8:
        dxgi_format = DXGI_FORMAT_R8_UNORM;
        break;
    case CMP_FORMAT_R_8:
        dxgi_format = DXGI_FORMAT_R8_UNORM;
        break;
    case CMP_FORMAT_ARGB_2101010:
        dxgi_format = DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;
        break;
    case CMP_FORMAT_R_16:
        dxgi_format = DXGI_FORMAT_R16_UNORM;
        break;
    case CMP_FORMAT_RGBE_32F:
        dxgi_format = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;
        break;
    case CMP_FORMAT_ARGB_16F:
    case CMP_FORMAT_ABGR_16F:
    case CMP_FORMAT_RGBA_16F:
    case CMP_FORMAT_BGRA_16F:
        dxgi_format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        break;
    case CMP_FORMAT_R_16F:
        dxgi_format = DXGI_FORMAT_R16_FLOAT;
        break;
    case CMP_FORMAT_ARGB_32F:
    case CMP_FORMAT_ABGR_32F:
    case CMP_FORMAT_RGBA_32F:
    case CMP_FORMAT_BGRA_32F:
        dxgi_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
    case CMP_FORMAT_RGB_32F:
    case CMP_FORMAT_BGR_32F:
        dxgi_format = DXGI_FORMAT_R32G32B32_FLOAT;
        break;
    case CMP_FORMAT_RG_32F:
        dxgi_format = DXGI_FORMAT_R32G32_FLOAT;
        break;
    case CMP_FORMAT_R_32F:
        dxgi_format = DXGI_FORMAT_R32_FLOAT;
        break;
        // Unknown compression mapping to Direct X
    case CMP_FORMAT_ASTC:
    case CMP_FORMAT_ATC_RGB:
    case CMP_FORMAT_ATC_RGBA_Explicit:
    case CMP_FORMAT_ATC_RGBA_Interpolated:
    case CMP_FORMAT_DXT5_xGBR:
    case CMP_FORMAT_DXT5_RxBG:
    case CMP_FORMAT_DXT5_RBxG:
    case CMP_FORMAT_DXT5_xRBG:
    case CMP_FORMAT_DXT5_RGxB:
    case CMP_FORMAT_DXT5_xGxR:
    case CMP_FORMAT_ETC_RGB:
    case CMP_FORMAT_ETC2_RGB:
    case CMP_FORMAT_GT:
        // -----------------------------------
    case CMP_FORMAT_Unknown:
    default:
        dxgi_format = DXGI_FORMAT_UNKNOWN;
        break;
    }

    return dxgi_format;
}
#endif

//--------------------------------------------------------------------------------------
// entry function to initialize an image from a .DDS texture
//--------------------------------------------------------------------------------------
INT32 Texture::InitFromFile(DeviceVK *pDevice, UploadHeapVK* pUploadHeap, const WCHAR *pFilename, void *pluginManager, void *msghandler)
{
    OnDestroy();

    m_device = pDevice->GetDevice();

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

    if (PrintStatusLine == NULL) {
        PrintStatusLine = (void(*)(char*))(msghandler);
    }

#ifdef USE_CMP_LOAD

    try
    {
        // get the ext and load image with amd compressonator image plugin
        char *fileExt;
        wstring ws(pFilename);
        string sFilename(ws.begin(), ws.end());
        size_t dot = sFilename.find_last_of('.');
        std::string temp;

        if (dot != std::string::npos) {
            temp = (sFilename.substr(dot + 1, sFilename.size() - dot));
            std::transform(temp.begin(), temp.end(), temp.begin(), toupper);
            fileExt = (char*)temp.data();
        }

        CMIPS localcMIPS;
        pMipSet = new MipSet();

        if (pMipSet == NULL)
        {
            OutputDebugString(TEXT(__FUNCTION__) TEXT(" failed.\n"));
            if (VK_CMips)
            {
                VK_CMips->Print("Error loading file: Out of memory for MipSet data");
            }
            return -1;
        }
        memset(pMipSet, 0, sizeof(MipSet));

        if (VK_CMips)
        {
            char fname[_MAX_FNAME];
            getFileNameExt(sFilename.c_str(), fname, _MAX_FNAME);
            VK_CMips->SetProgress(0);
            if (VK_CMips->m_canceled)
                return -1;
            VK_CMips->Print("Loading: %s", fname);
        }

        if (AMDLoadMIPSTextureImage(sFilename.c_str(), pMipSet, false, pluginManager) != 0)
        {
            if (pMipSet->m_pMipLevelTable) {
                localcMIPS.FreeMipSet(pMipSet);
                pMipSet->m_pMipLevelTable = NULL;
            }

            if (pMipSet) {
                delete pMipSet;
                pMipSet = NULL;
            }
            if (VK_CMips)
            {
                VK_CMips->Print("Error: reading image, data type not supported");
            }
            return -1;
        }

        if (pMipSet)
        {
            if (pMipSet->m_format == CMP_FORMAT_Unknown)
            {
                pMipSet->m_format = GetFormat(pMipSet);
            }

            pMipSet->m_swizzle = KeepSwizzle(pMipSet->m_format);

            if (pMipSet->m_compressed || (pMipSet->m_ChannelFormat == CF_Compressed))
            {
                pMipSet->m_compressed = true;
                Config configsetting;
                configsetting.swizzle = pMipSet->m_swizzle;
                pMipSet = DecompressMIPSet(pMipSet, GPUDecode_INVALID, &configsetting, NULL);
                if (pMipSet == NULL)
                {
                    if (VK_CMips)
                    {
                        VK_CMips->Print("Error: reading compressed image");
                    }
                    return -1;
                }
            }
            if (pMipSet->m_swizzle)
                SwizzleMipMap(pMipSet);
        }
        else
        {
            if (VK_CMips)
            {
                VK_CMips->Print("Error: reading image, data type not supported");
            }
            return -1;
        }

        //=============== Add Vulkan Specific Code Here ===================
        //=================================================================

        if (pMipSet->m_pMipLevelTable) {
            localcMIPS.FreeMipSet(pMipSet);
            pMipSet->m_pMipLevelTable = NULL;
        }

        if (pMipSet) {
            delete pMipSet;
            pMipSet = NULL;
        }

    }
    catch (std::bad_alloc)
    {
        if (VK_CMips)
        {
            VK_CMips->m_canceled = true;
            VK_CMips->Print("Error: Out of Memory while loading textures!");
        }
        return -1;
    }

    //======================================================================================
#else
// get the last 4 char (assuming this is the file extension)
size_t len = wcslen(pFilename);
char ext[5] = { 0 };
size_t numConverted = 0;
wcstombs_s(&numConverted, ext, 5, &pFilename[len - 4], 4);
for (int i = 0; i < 4; ++i)
{
    ext[i] = tolower(ext[i]);
}

// check if the extension is known
UINT32 ext4CC = *reinterpret_cast<const UINT32 *>(ext);
if (ext4CC != 'sdd.')
return -1;

if (GetFileAttributes(pFilename) == 0xFFFFFFFF)
return -1;

HANDLE hFile = CreateFile(pFilename,             // file to open
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
        DWORD dwWidth  = 1;
        DWORD dwHeight = 1;
        UINT8* pixels   = pUploadHeap->Suballocate(dwWidth*dwHeight * 4, 512);

        if ((header->dwWidth >> mip) > 1)
            dwWidth = header->dwWidth >> mip;

        if ((header->dwHeight >> mip) > 1)
            dwHeight = header->dwHeight >> mip;

        
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

//--------------------------------------------------------------------------------------
// entry function to initialize an image from a .DDS texture
//--------------------------------------------------------------------------------------
INT32 Texture::InitFromFile(DeviceVK *pDevice, UploadHeapVK* pUploadHeap, const char *pFilename)
{
    OnDestroy();

    m_device = pDevice->GetDevice();

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
