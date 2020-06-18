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

#include <stdio.h>
#include <stdlib.h>

#include "Common.h"
#include "Compressonator.h"
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"

#ifdef _WIN32
#include "ddraw.h"
#include "d3d9types.h"
#endif

#include "DDS.h"
#include "DDS_File.h"
#include "DDS_DX10.h"
#include "DDS_Helpers.h"

#define _CRT_SECURE_NO_WARNINGS

CMIPS *DDS_CMips = NULL;
const char* g_pszFilename;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_DDS)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("DDS")
#else
void *make_Plugin_DDS() { return new Plugin_DDS; }
#endif

Plugin_DDS::Plugin_DDS()
{
#ifdef _WIN32
    HRESULT hr;
    // Initialize COM (needed for WIC)
    if( FAILED( hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED) ) )
    {
        if (DDS_CMips)
            DDS_CMips->PrintError("Failed to initialize COM (%08X)\n", hr);
    }
#endif
}

Plugin_DDS::~Plugin_DDS()
{
}

int Plugin_DDS::TC_PluginSetSharedIO(void *Shared)
{
    if (Shared)
    {
        DDS_CMips = static_cast<CMIPS *>(Shared);
        return 0;
    }
    return 1;
}

int Plugin_DDS::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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

// No longer supported
int Plugin_DDS::TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture)
{
    (pszFilename);
    (srcTexture);
    return 0;
}

// No longer supported
int Plugin_DDS::TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture)
{
    (pszFilename);
    (srcTexture);
#ifdef _WIN32
    HRESULT hr = S_OK;
    return hr==S_OK?0:-1; // np: need to fix this : make all pligins return long type!
#else
    return 0;
#endif
}

int Plugin_DDS::TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet)
{
   g_pszFilename = pszFilename;
   FILE* pFile = NULL;
   pFile = fopen(pszFilename, ("rb"));
   if(pFile == NULL)
    {
        DDS_CMips->PrintError("Error [%x]: DDS Plugin Failed to load texture file %s\n",IDS_ERROR_FILE_OPEN,pszFilename);
        return PE_Unknown;
    }

    CMP_DWORD dwFileHeader;
    fread(&dwFileHeader ,sizeof(CMP_DWORD), 1, pFile);
    if(dwFileHeader != DDS_HEADER)
    {
        fclose(pFile);
        DDS_CMips->PrintError("Error [%x]: DDS Plugin Failed to load texture file %s\n",IDS_ERROR_NOT_DDS,pszFilename);
        return PE_Unknown;
    }

    DDSD2 ddsd;
    if(fread(&ddsd, sizeof(DDSD2), 1, pFile) != 1)
   {
      fclose(pFile);
        DDS_CMips->PrintError("Error [%x]: DDS Plugin Failed to load texture file %s\n",IDS_ERROR_NOT_DDS,pszFilename);
      return PE_Unknown;
   }

    if(!(ddsd.dwFlags & DDSD_MIPMAPCOUNT))
        ddsd.dwMipMapCount = 1;
    else if(ddsd.dwMipMapCount == 0)
    {
        fclose(pFile);
        DDS_CMips->PrintError("Error [%x]: DDS Plugin Failed to load texture file %s\n",IDS_ERROR_NOT_DDS,pszFilename);
        return PE_Unknown;
    }

    if(ddsd.ddpfPixelFormat.dwFourCC == CMP_MAKEFOURCC('D', 'X', '1', '0'))
        return LoadDDS_DX10(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_A32B32G32R32F)
        return LoadDDS_ABGR32F(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_A16B16G16R16F)
        return LoadDDS_ABGR16F(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_G32R32F)
        return LoadDDS_GR32F(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_R32F)
        return LoadDDS_R32F(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_R16F)
        return LoadDDS_R16F(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_G16R16F)
        return LoadDDS_G16R16F(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_A16B16G16R16)
        return LoadDDS_ABGR16(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_Q16W16V16U16)
        return LoadDDS_ABGR16(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_G16R16)
        return LoadDDS_G16R16(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_L16)
        return LoadDDS_R16(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwFourCC)
        return LoadDDS_FourCC(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwLuminanceBitCount==8 && (ddsd.ddpfPixelFormat.dwFlags & DDPF_LUMINANCE))
        return LoadDDS_G8(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwLuminanceBitCount==16 && (ddsd.ddpfPixelFormat.dwFlags & DDPF_LUMINANCE) && (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS))
        return LoadDDS_AG8(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwLuminanceBitCount==16 && (ddsd.ddpfPixelFormat.dwFlags & DDPF_LUMINANCE)&& (ddsd.ddpfPixelFormat.dwGBitMask==0xffff))
        return LoadDDS_G16(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwAlphaBitDepth==8 && (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHA))
        return LoadDDS_A8(pFile, &ddsd, pMipSet);
    else if((ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB) && !(ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) && (ddsd.ddpfPixelFormat.dwRGBBitCount==16))
        return LoadDDS_RGB565(pFile, &ddsd, pMipSet);
    else if((ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB) && !(ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) && (ddsd.ddpfPixelFormat.dwRGBBitCount==24))
        return LoadDDS_RGB888(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwRGBBitCount==32 && (ddsd.ddpfPixelFormat.dwRBitMask==0x3ff || ddsd.ddpfPixelFormat.dwRBitMask==0x3ff00000))
        return LoadDDS_ARGB2101010(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwRGBBitCount==32 && ddsd.ddpfPixelFormat.dwRBitMask==0xffff && ddsd.ddpfPixelFormat.dwGBitMask==0xffff0000)
        return LoadDDS_G16R16(pFile, &ddsd, pMipSet);
    else if(ddsd.ddpfPixelFormat.dwLuminanceBitCount==16 && (ddsd.ddpfPixelFormat.dwFlags & DDPF_LUMINANCE)&&(ddsd.ddpfPixelFormat.dwRBitMask==0xffff)) // 16bpp Gray
    {
        pMipSet->m_format = CMP_FORMAT_ABGR_16;
        return LoadDDS_ABGR16(pFile, &ddsd, pMipSet);
    }
    else if(ddsd.ddpfPixelFormat.dwRGBBitCount==16 && ddsd.ddpfPixelFormat.dwRBitMask==0xffff)
    {
        pMipSet->m_format = CMP_FORMAT_R_16;
        return LoadDDS_R16(pFile, &ddsd, pMipSet);
    }
    else if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
    {
        return LoadDDS_RGB8888(pFile, &ddsd, pMipSet, (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) ? true : false);
    }

    fclose(pFile);

    DDS_CMips->PrintError("Error [%x]: DDS Plugin Failed to load texture file %s\n",IDS_ERROR_UNSUPPORTED_TYPE,pszFilename);
    return PE_Unknown;

}

int Plugin_DDS::TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet)
{
   assert(pszFilename);
   assert(pMipSet);

   FILE* pFile = NULL;
   pFile = fopen( pszFilename, ("wb"));
   if(pFile == NULL)
    {
        return PE_Unknown;
    }

    fwrite(&DDS_HEADER ,sizeof(CMP_DWORD), 1, pFile);

    if(pMipSet->m_dwFourCC == CMP_FOURCC_G8)
        return SaveDDS_G8(pFile, pMipSet);
    else if(pMipSet->m_dwFourCC == CMP_FOURCC_A8)
        return SaveDDS_A8(pFile, pMipSet);
    else if(IsD3D10Format(pMipSet))
        return SaveDDS_DX10(pFile, pMipSet);
    else if(pMipSet->m_dwFourCC)
        return SaveDDS_FourCC(pFile, pMipSet);
    else if(pMipSet->m_ChannelFormat == CF_Float16)
    {
        if(pMipSet->m_TextureDataType == TDT_R)
            return SaveDDS_R16F(pFile, pMipSet);
        else if(pMipSet->m_TextureDataType == TDT_RG)
            return SaveDDS_RG16F(pFile, pMipSet);
        else
            return SaveDDS_ABGR16F(pFile, pMipSet);
    }
    else if(pMipSet->m_ChannelFormat == CF_Float32)
    {
        if(pMipSet->m_TextureDataType == TDT_R)
            return SaveDDS_R32F(pFile, pMipSet);
        else if(pMipSet->m_TextureDataType == TDT_RG)
            return SaveDDS_RG32F(pFile, pMipSet);
        else
            return SaveDDS_ABGR32F(pFile, pMipSet);
    }
    else if(pMipSet->m_ChannelFormat == CF_2101010)
        return SaveDDS_ARGB2101010(pFile, pMipSet);
    else if(pMipSet->m_ChannelFormat == CF_16bit)
    {
        if(pMipSet->m_TextureDataType == TDT_R)
            return SaveDDS_R16(pFile, pMipSet);
        else if(pMipSet->m_TextureDataType == TDT_RG)
            return SaveDDS_RG16(pFile, pMipSet);
        else
            return SaveDDS_ABGR16(pFile, pMipSet);
    }
    else if(pMipSet->m_TextureDataType == TDT_ARGB)
        return SaveDDS_ARGB8888(pFile, pMipSet);
    else
        return SaveDDS_RGB888(pFile, pMipSet);
}

