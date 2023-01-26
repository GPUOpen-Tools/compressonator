//=====================================================================
// Copyright 2022 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#include "common.h"
#include "compressonator.h"
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "textureio.h"
#include "atiformats.h"

#ifdef _WIN32
#pragma warning(disable : 4996)  //'fopen': This function or variable may be unsafe.
#endif

#include "brlg.h"

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

CMIPS*      BRLG_CMips = NULL;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Image_Plugin_BRLG)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("BRLG")
#else
void* make_Image_Plugin_BRLG()
{
    return new Image_Plugin_BRLG;
}
#endif

Image_Plugin_BRLG::Image_Plugin_BRLG()
{
#ifdef _WIN32
    HRESULT hr;
    // Initialize COM (needed for WIC)
    if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("Failed to initialize COM (%08X)\n", hr);
    }
#endif
}

Image_Plugin_BRLG::~Image_Plugin_BRLG()
{
}

int Image_Plugin_BRLG::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        BRLG_CMips = static_cast<CMIPS*>(Shared);
        return 0;
    }
    return 1;
}

int Image_Plugin_BRLG::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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

// No longer supported
int Image_Plugin_BRLG::TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture* srcTexture)
{
    (pszFilename);
    (srcTexture);
    return 0;
}

// No longer supported
int Image_Plugin_BRLG::TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture* srcTexture)
{
    (pszFilename);
    (srcTexture);
#ifdef _WIN32
    HRESULT hr = S_OK;
    return hr == S_OK ? 0 : -1;  // np: need to fix this : make all plugins return long type!
#else
    return 0;
#endif
}

int Image_Plugin_BRLG::TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet)
{
    FILE* inFile = NULL;
    inFile = fopen(pszFilename, "rb");
    if (inFile == NULL)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("Error(%d): BRLG Plugin ID(%d) opening file = %s ", EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return PE_InitErr;
    }

    // Read the header
    BRLG_Header header = {};

    if (fread(&header, sizeof(BRLG_Header), 1, inFile) != 1)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("Error(%d): BRLG Plugin invalid header. Filename = %s ", EL_Error, pszFilename);
        fclose(inFile);
        return PE_InitErr;
    }

    // check file identifier
    for (int i = 0; i < sizeof(header.fileType)/sizeof(header.fileType[0]); ++i)
    {
        if (header.fileType[i] != BRLG_FILE_IDENTIFIER[i])
        {
            if (BRLG_CMips)
                BRLG_CMips->PrintError("Error(%d): BRLG Plugin invalid file identifier. Filename = %s ", EL_Error, pszFilename);
            fclose(inFile);
            return PE_InitErr;
        }
    }

    if (header.majorVersion != TC_PLUGIN_VERSION_MAJOR)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("Error(%d): BRLG Plugin invalid file version. Filename = %s ", EL_Error, pszFilename);
        fclose(inFile);
        return PE_InitErr;
    }

    CMP_INT textureWidth  = header.originalWidth;
    CMP_INT textureHeight = header.originalHeight;

    CMP_FORMAT originalFormat = header.originalFormat;
    CMP_TextureDataType textureDataType = header.originalTextureDataType;
    CMP_TextureType     textureType     = header.originalTextureType;
    CMP_ChannelFormat   channelFormat   = GetChannelFormat(originalFormat);

    // Allocate compression
    pMipSet->m_nMaxMipLevels = 1;
    pMipSet->m_nMipLevels = 1;  // this is overwriiten depending on input.

    if (!BRLG_CMips->AllocateMipSet(pMipSet, channelFormat, textureDataType, textureType, textureWidth, textureHeight, 1))
    {
        fclose(inFile);
        return PE_Unknown;
    }

    pMipSet->m_compressed   = true;
    pMipSet->m_format       = CMP_FORMAT_BROTLIG;
    pMipSet->m_nBlockWidth  = 1;
    pMipSet->m_nBlockHeight = 1;
    pMipSet->m_transcodeFormat = originalFormat;

    MipLevel* mipLevel = BRLG_CMips->GetMipLevel(pMipSet, 0);

    if (!BRLG_CMips->AllocateCompressedMipLevelData(mipLevel, textureWidth, textureHeight, header.compressedDataSize))
    {
        fclose(inFile);
        if (BRLG_CMips)
            BRLG_CMips->PrintError("Error(%d): BRLG allocate compress memory Plugin ID(%d)\n", EL_Error, IDS_ERROR_OUTOFMEMORY);
        return PE_Unknown;
    }

    CMP_BYTE* pData = (CMP_BYTE*)(mipLevel->m_pbData);
    pMipSet->m_nMipLevels = 1;

    // read original file name from input file
    if (header.extraDataSize > 0)
    {
        BRLG_ExtraInfo* extraInfo = (BRLG_ExtraInfo*)calloc(1, sizeof(BRLG_ExtraInfo));

        fread(extraInfo, header.extraDataSize, 1, inFile);

        pMipSet->m_pReservedData = extraInfo;
    }

    fread(pData, mipLevel->m_dwLinearSize, 1, inFile);

    fclose(inFile);

    return PE_OK;
}

int Image_Plugin_BRLG::TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet)
{
    assert(pszFilename);
    assert(pMipSet);

    FILE* outFile = NULL;
    outFile = fopen(pszFilename, "wb");
    if (outFile == NULL)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("Error(%d): BRLG Plugin ID(%d) saving file = %s ", EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return PE_InitErr;
    }

    BRLG_Header header = {};

    *((uint32_t*)header.fileType) = *((uint32_t*)BRLG_FILE_IDENTIFIER);
    header.majorVersion = TC_PLUGIN_VERSION_MAJOR;
    header.headerSize   = sizeof(BRLG_Header);
    header.compressedDataSize = pMipSet->dwDataSize;

    header.originalWidth = pMipSet->m_nWidth;
    header.originalHeight = pMipSet->m_nHeight;

    header.originalFormat = pMipSet->m_transcodeFormat;
    header.originalTextureType = pMipSet->m_TextureType; 
    header.originalTextureDataType = pMipSet->m_TextureDataType;

    header.compressedDataOffset = sizeof(BRLG_Header);

    BRLG_ExtraInfo* extraInfo = 0;

    if (pMipSet->m_pReservedData)
    {
        extraInfo = (BRLG_ExtraInfo*)pMipSet->m_pReservedData;

        header.extraDataSize = sizeof(BRLG_ExtraInfo);
        header.compressedDataOffset += header.extraDataSize;
    }

    fwrite(&header, sizeof(header), 1, outFile);

    if (extraInfo)
    {
        fwrite(extraInfo, sizeof(BRLG_ExtraInfo), 1, outFile);
    }

    MipLevel* mipLevel = BRLG_CMips->GetMipLevel(pMipSet, 0);
    
    fwrite(mipLevel->m_pbData, pMipSet->dwDataSize, 1, outFile);

    fclose(outFile);
    
    return PE_OK;
    
}
