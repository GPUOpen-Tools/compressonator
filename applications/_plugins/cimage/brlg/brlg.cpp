//=====================================================================
// Copyright 2022-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#include "brlg.h"

#include <stdio.h>
#include <stdlib.h>

#include "atiformats.h"
#include "cmp_fileio.h"
#include "common.h"
#include "compressonator.h"
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "textureio.h"

#ifdef _WIN32
#pragma warning(disable : 4996)  //'fopen': This function or variable may be unsafe.
#endif

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _WIN32
// {004DF021-0993-4F67-A5E9-2313A7C30ADE}
static const GUID g_GUID = {0x4df021, 0x993, 0x4f67, {0xa5, 0xe9, 0x23, 0x13, 0xa7, 0xc3, 0xa, 0xde}};
#else
static const GUID g_GUID = {0};
#endif

static CMIPS* BRLG_CMips = NULL;

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

int Image_Plugin_BRLG::TC_PluginSetSharedIO(void* sharedIO)
{
    if (sharedIO)
    {
        BRLG_CMips = static_cast<CMIPS*>(sharedIO);
        return 0;
    }
    return 1;
}

int Image_Plugin_BRLG::TC_PluginGetVersion(TC_PluginVersion* pluginVersion)
{
#ifdef _WIN32
    pluginVersion->guid = g_GUID;
#endif
    pluginVersion->dwAPIVersionMajor    = TC_API_VERSION_MAJOR;
    pluginVersion->dwAPIVersionMinor    = TC_API_VERSION_MINOR;
    pluginVersion->dwPluginVersionMajor = BRLG_PLUGIN_VERSION_MAJOR;
    pluginVersion->dwPluginVersionMinor = BRLG_PLUGIN_VERSION_MINOR;

    return 0;
}

// No longer supported
int Image_Plugin_BRLG::TC_PluginFileLoadTexture(const char* fileName, CMP_Texture* srcTexture)
{
    (fileName);
    (srcTexture);
    return 0;
}

// No longer supported
int Image_Plugin_BRLG::TC_PluginFileSaveTexture(const char* fileName, CMP_Texture* srcTexture)
{
    (fileName);
    (srcTexture);
#ifdef _WIN32
    HRESULT hr = S_OK;
    return hr == S_OK ? 0 : -1;  // np: need to fix this : make all plugins return long type!
#else
    return 0;
#endif
}

int Image_Plugin_BRLG::TC_PluginFileLoadTexture(const char* fileName, MipSet* srcTexture)
{
    std::vector<CMP_MipSet> resultMipSets;

    if (!srcTexture || !fileName)
        return PE_InitErr;

    int error = LoadPackagedTextures(fileName, resultMipSets);

    if (error != PE_OK)
        return error;

    if (resultMipSets.size() > 1)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError(
                "ERROR: Expected to only load one texture from BRLG file, but loaded %d textures. Use \"LoadPackagedTextures\" function when loading "
                "non-legacy BRLG files.\n",
                resultMipSets.size());

        return BRLG_PLUGIN_ERROR_UNSUPPORTED_TYPE;
    }

    if (resultMipSets.size() == 0)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("ERROR: No data could be loaded from the BRLG file \"%s\"\n", fileName);
        return BRLG_PLUGIN_ERROR_UNSUPPORTED_TYPE;
    }

    *srcTexture = resultMipSets[0];

    return PE_OK;
}

static CMP_DWORD ReadBlockData(CMP_MipSet& destTexture, FILE* inFile, CMP_DWORD compressedDataSize = 0)
{
    // Read block header

    BRLG_BlockHeader blockHeader = {};

    if (fread(&blockHeader, sizeof(BRLG_BlockHeader), 1, inFile) != 1)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("ERROR: BRLG Plug-in encountered an invalid block header.\n");

        return 0;
    }

    // Allocate and configure mipset

    CMP_INT textureWidth  = blockHeader.originalWidth;
    CMP_INT textureHeight = blockHeader.originalHeight;

    CMP_TextureDataType textureDataType = blockHeader.originalTextureDataType;
    CMP_TextureType     textureType     = blockHeader.originalTextureType;
    CMP_ChannelFormat   channelFormat   = GetChannelFormat(blockHeader.originalFormat);

    destTexture.m_nMaxMipLevels = 1;
    destTexture.m_nMipLevels    = 1;  // this is overwriiten depending on input.

    if (!BRLG_CMips->AllocateMipSet(&destTexture, channelFormat, textureDataType, textureType, textureWidth, textureHeight, 1))
    {
        return 0;
    }

    destTexture.m_compressed      = true;
    destTexture.m_format          = CMP_FORMAT_BROTLIG;
    destTexture.m_nBlockWidth     = 4;
    destTexture.m_nBlockHeight    = 4;
    destTexture.m_nBlockDepth     = 1;
    destTexture.m_transcodeFormat = blockHeader.originalFormat;
    destTexture.m_nMipLevels      = 1;

    CMP_DWORD compressedBlockSize = blockHeader.compressedBlockSize;
    if (compressedDataSize != 0)
        compressedBlockSize = compressedDataSize;

    MipLevel* mipLevel = BRLG_CMips->GetMipLevel(&destTexture, 0);
    if (!BRLG_CMips->AllocateCompressedMipLevelData(mipLevel, textureWidth, textureHeight, compressedBlockSize))
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("ERROR: BRLG Plug-in failed to allocate destination mipset data.\n");
        return 0;
    }

    // Read extra info

    if (blockHeader.extraDataSize > 0)
    {
        char* originalFileName = (char*)malloc(blockHeader.extraDataSize);
        fread(originalFileName, blockHeader.extraDataSize, 1, inFile);

        BRLG_ExtraInfo* extraInfo = (BRLG_ExtraInfo*)calloc(1, sizeof(BRLG_ExtraInfo));
        extraInfo->numChars       = blockHeader.extraDataSize;
        extraInfo->fileName       = originalFileName;

        destTexture.m_pReservedData = extraInfo;
    }

    // Read compressed data
    fread((CMP_BYTE*)(mipLevel->m_pbData), mipLevel->m_dwLinearSize, 1, inFile);

    CMP_DWORD totalReadSize = sizeof(blockHeader) + blockHeader.extraDataSize + mipLevel->m_dwLinearSize;
    return totalReadSize;
}

int Image_Plugin_BRLG::LoadPackagedTextures(const char* srcFileName, std::vector<CMP_MipSet>& destTextures)
{
    FILE* inFile = fopen(srcFileName, "rb");
    if (inFile == NULL)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("Error(%d): BRLG Plugin ID(%d) opening file = %s ", EL_Error, BRLG_PLUGIN_ERROR_FILE_OPEN, srcFileName);
        return PE_InitErr;
    }

    // Read the header

    BRLG_FileHeader fileHeader = {};

    if (fread(&fileHeader, sizeof(BRLG_FileHeader), 1, inFile) != 1)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("Error(%d): BRLG Plugin invalid file header. Filename = %s ", EL_Error, srcFileName);
        fclose(inFile);
        return BRLG_PLUGIN_ERROR_UNSUPPORTED_TYPE;
    }

    // Validate header

    for (int i = 0; i < sizeof(fileHeader.fileType) / sizeof(fileHeader.fileType[0]); ++i)
    {
        if (fileHeader.fileType[i] != BRLG_FILE_IDENTIFIER[i])
        {
            if (BRLG_CMips)
                BRLG_CMips->PrintError("Error(%d): BRLG Plugin invalid file identifier. Filename = %s ", EL_Error, srcFileName);
            fclose(inFile);
            return BRLG_PLUGIN_ERROR_NOT_BRLG;
        }
    }

    if (fileHeader.majorVersion < 1 || fileHeader.majorVersion > BRLG_PLUGIN_VERSION_MAJOR)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("Error(%d): BRLG Plugin invalid file version. Filename = %s ", EL_Error, srcFileName);
        fclose(inFile);
        return BRLG_PLUGIN_ERROR_NOT_BRLG;
    }

    // If the file version is 1, then it is an easy case and we just load a single block and quit
    if (fileHeader.majorVersion == 1)
    {
        CMP_MipSet destTexture   = {};
        CMP_DWORD  totalReadSize = ReadBlockData(destTexture, inFile, fileHeader.compressedDataSize);

        if (totalReadSize == 0)
        {
            if (BRLG_CMips)
                BRLG_CMips->PrintError("ERROR: Could not read BRLG block data in file \"%s\".\n", srcFileName);

            fclose(inFile);
            return BRLG_PLUGIN_ERROR_UNSUPPORTED_TYPE;
        }

        destTextures.push_back(std::move(destTexture));
        fclose(inFile);
        return PE_OK;
    }

    // File version is > 1, so we need to read all of the data blocks

    CMP_DWORD remainingBytes = fileHeader.compressedDataSize;

    while (remainingBytes > 0)
    {
        CMP_MipSet destTexture = {};

        CMP_DWORD bytesRead = ReadBlockData(destTexture, inFile);

        if (bytesRead == 0)
        {
            if (BRLG_CMips)
                BRLG_CMips->PrintError("ERROR: Could not read BRLG block data in file \"%s\".\n", srcFileName);

            fclose(inFile);
            return BRLG_PLUGIN_ERROR_UNSUPPORTED_TYPE;
        }

        destTextures.push_back(std::move(destTexture));

        remainingBytes -= bytesRead;
    }

    fclose(inFile);

    return PE_OK;
}

static void WriteFileHeader(FILE* destFile, CMP_DWORD totalFileSize)
{
    BRLG_FileHeader fileHeader = {};

    *((uint32_t*)fileHeader.fileType) = *((uint32_t*)BRLG_FILE_IDENTIFIER);
    fileHeader.majorVersion           = BRLG_PLUGIN_VERSION_MAJOR;
    fileHeader.headerSize             = sizeof(BRLG_FileHeader);
    fileHeader.compressedDataSize     = totalFileSize - fileHeader.headerSize;

    fwrite(&fileHeader, sizeof(BRLG_FileHeader), 1, destFile);
}

static void WriteBlockHeader(FILE* destFile, const MipSet* texture)
{
    BRLG_BlockHeader blockHeader = {};

    blockHeader.originalWidth  = texture->m_nWidth;
    blockHeader.originalHeight = texture->m_nHeight;

    blockHeader.originalFormat = texture->m_transcodeFormat;

    blockHeader.originalTextureType     = texture->m_TextureType;
    blockHeader.originalTextureDataType = texture->m_TextureDataType;

    BRLG_ExtraInfo* extraInfo = (BRLG_ExtraInfo*)texture->m_pReservedData;
    if (extraInfo)
    {
        blockHeader.extraDataSize = extraInfo->numChars;
    }

    blockHeader.compressedBlockSize = texture->dwDataSize;

    fwrite(&blockHeader, sizeof(BRLG_BlockHeader), 1, destFile);

    if (extraInfo)
    {
        fwrite(extraInfo->fileName, extraInfo->numChars, 1, destFile);
    }

    MipLevel* mipLevel = BRLG_CMips->GetMipLevel(texture, 0);
    fwrite(mipLevel->m_pbData, texture->dwDataSize, 1, destFile);
}

int Image_Plugin_BRLG::TC_PluginFileSaveTexture(const char* fileName, MipSet* srcTexture)
{
    assert(fileName);
    assert(srcTexture);

    CMP_DWORD totalFileSize = sizeof(BRLG_FileHeader) + sizeof(BRLG_BlockHeader);

    if (srcTexture->m_pReservedData)
    {
        BRLG_ExtraInfo* extraInfo = (BRLG_ExtraInfo*)srcTexture->m_pReservedData;
        totalFileSize += extraInfo->numChars;
    }

    totalFileSize += srcTexture->dwDataSize;

    // Check if the base directory of the file exists, if not we create it
    std::string baseDirectory = CMP_GetBaseDir(std::string(fileName));
    if (!baseDirectory.empty() && !CMP_DirExists(baseDirectory))
    {
        if (!CMP_CreateDir(baseDirectory))
        {
            if (BRLG_CMips)
                BRLG_CMips->PrintError(
                    "ERROR: Destination path \"%s\" is within a directory that doesn't exist and there was an error creating the directory.\n");
            return BRLG_PLUGIN_ERROR_FILE_OPEN;
        }
    }

    FILE* outFile = NULL;
    outFile       = fopen(fileName, "wb");
    if (outFile == NULL)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("ERROR: BRLG Plug-in failed to open file \"%s\"\n", fileName);
        return PE_InitErr;
    }

    WriteFileHeader(outFile, totalFileSize);
    WriteBlockHeader(outFile, srcTexture);

    fclose(outFile);

    return PE_OK;
}

int Image_Plugin_BRLG::SavePackagedTextures(const char* destFileName, const std::vector<CMP_MipSet>& srcTextures)
{
    assert(destFileName);

    CMP_DWORD totalFileSize = sizeof(BRLG_FileHeader);

    for (const CMP_MipSet& texture : srcTextures)
    {
        totalFileSize += sizeof(BRLG_BlockHeader);

        if (texture.m_pReservedData)
        {
            BRLG_ExtraInfo* extraInfo = (BRLG_ExtraInfo*)texture.m_pReservedData;
            totalFileSize += extraInfo->numChars;
        }

        totalFileSize += texture.dwDataSize;
    }

    // Check if the base directory of the file exists, if not we create it
    std::string baseDirectory = CMP_GetBaseDir(std::string(destFileName));
    if (!baseDirectory.empty() && !CMP_DirExists(baseDirectory))
    {
        if (!CMP_CreateDir(baseDirectory))
        {
            if (BRLG_CMips)
                BRLG_CMips->PrintError(
                    "ERROR: Destination path \"%s\" is within a directory that doesn't exist and there was an error creating the directory.\n");
            return BRLG_PLUGIN_ERROR_FILE_OPEN;
        }
    }

    FILE* outFile = fopen(destFileName, "wb");
    if (outFile == NULL)
    {
        if (BRLG_CMips)
            BRLG_CMips->PrintError("ERROR: BRLG Plug-in failed to open file \"%s\"\n", destFileName);
        return PE_InitErr;
    }

    WriteFileHeader(outFile, totalFileSize);

    for (const CMP_MipSet& texture : srcTextures)
    {
        WriteBlockHeader(outFile, &texture);
    }

    fclose(outFile);

    return PE_OK;
}