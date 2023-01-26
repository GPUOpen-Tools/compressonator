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
#include "cmp_fileio.h"

#ifdef _WIN32
#pragma warning(disable : 4996)  //'fopen': This function or variable may be unsafe.
#endif

#include "binary.h"

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

CMIPS* BINARY_CMips = NULL;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Image_Plugin_BINARY)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("BINARY")
#else
void* make_Image_Plugin_BINARY()
{
    return new Image_Plugin_BINARY;
}
#endif

Image_Plugin_BINARY::Image_Plugin_BINARY()
{
#ifdef _WIN32
    HRESULT hr;
    // Initialize COM (needed for WIC)
    if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
    {
        if (BINARY_CMips)
            BINARY_CMips->PrintError("Failed to initialize COM (%08X)\n", hr);
    }
#endif
}

Image_Plugin_BINARY::~Image_Plugin_BINARY()
{
}

int Image_Plugin_BINARY::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        BINARY_CMips = static_cast<CMIPS*>(Shared);
        return 0;
    }
    return 1;
}

int Image_Plugin_BINARY::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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
int Image_Plugin_BINARY::TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture* srcTexture)
{
    (pszFilename);
    (srcTexture);
    return 0;
}

// No longer supported
int Image_Plugin_BINARY::TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture* srcTexture)
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

int Image_Plugin_BINARY::TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet)
{
    FILE* inFile = NULL;

    inFile = fopen(pszFilename, "rb");
    if (inFile == NULL)
    {
        if (BINARY_CMips)
            BINARY_CMips->PrintError("Error(%d): BINARY Plugin ID(%d) opening file = %s\n", EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    uintmax_t fileSize = CMP_GetFileSize(std::string(pszFilename));

    bool success = g_CMIPS->AllocateMipSet(pMipSet, CF_8bit, TDT_8, TT_1D, fileSize, 1, 1);

    if (!success)
    {
        if (BINARY_CMips)
            BINARY_CMips->PrintError("Error: Failed to allocate MipSet during file loading.\n");
        return -1;
    }

    CMP_MipLevel* baseMipLevel = g_CMIPS->GetMipLevel(pMipSet, 0);
    success = g_CMIPS->AllocateCompressedMipLevelData(baseMipLevel, fileSize, 1, fileSize);

    if (!success)
    {
        if (BINARY_CMips)
            BINARY_CMips->PrintError("Error: Failed to allocate MipLevel data during file loading.\n");
        return -1;
    }

    fread(baseMipLevel->m_pbData, baseMipLevel->m_dwLinearSize, 1, inFile);

    pMipSet->m_format = CMP_FORMAT_BINARY;
    pMipSet->m_nMipLevels = 1;
    pMipSet->m_transcodeFormat = CMP_FORMAT_BINARY;

    // Saving the original file data
    BRLG_ExtraInfo* fileInfo = (BRLG_ExtraInfo*)calloc(1, sizeof(BRLG_ExtraInfo));

    std::string fileName = CMP_GetFileName(pszFilename);
    memcpy(fileInfo->fileName, fileName.c_str(), fileName.size());

    pMipSet->m_pReservedData = fileInfo;

    fclose(inFile);
    return 0;
}

int Image_Plugin_BINARY::TC_PluginFileSaveTexture(const char* fileName, MipSet* mipSet)
{
    assert(fileName);
    assert(mipSet);

    if (!fileName)
    {
        if (BINARY_CMips)
            BINARY_CMips->PrintError("Error: BINARY Plugin couldn't find file name to write\n", IDS_ERROR_FILE_OPEN);
        return -1;
    }

    FILE* outFile = NULL;

    outFile = fopen(fileName, "wb");
    if (outFile == NULL)
    {
        if (BINARY_CMips)
            BINARY_CMips->PrintError("Error: BINARY Plugin ID(%d) saving file = %s ", IDS_ERROR_FILE_OPEN, fileName);
        return -1;
    }

    CMP_MipLevel* baseMipLevel = g_CMIPS->GetMipLevel(mipSet, 0);
    fwrite(baseMipLevel->m_pbData, baseMipLevel->m_dwLinearSize, 1, outFile);

    fclose(outFile);

    if (BINARY_CMips)
        BINARY_CMips->Print("Saved output to: %s\n", fileName);

    return 0;
}
