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

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "compressonator.h"

#include "tc_pluginapi.h"
#include "tc_plugininternal.h"

#include "castc.h"

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_ASTC)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("ASTC")
#else
void *make_Plugin_ASTC() {
    return new Plugin_ASTC;
}
#endif


Plugin_ASTC::Plugin_ASTC() {
    //MessageBox(0,"Plugin_TGA","Plugin_KTX",MB_OK);
    m_xdim = 4;
    m_ydim = 4;
    m_zdim = 4;
}

Plugin_ASTC::~Plugin_ASTC() {
    //MessageBox(0,"Plugin_TGA","~Plugin_KTX",MB_OK);
}

int Plugin_ASTC::TC_PluginSetSharedIO(void* Shared) {
    if (Shared) {
        ASTC_CMips = static_cast<CMIPS *>(Shared);
        return 0;
    }
    return 1;
}


int Plugin_ASTC::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) {
    //MessageBox(0,"TC_PluginGetVersion","Plugin_KTX",MB_OK);
#ifdef _WIN32
    pPluginVersion->guid                    = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor        = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor        = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_ASTC::TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture) {
    //MessageBox(0,"TC_PluginFileLoadTexture srcTexture","Plugin_KTX",MB_OK);
    return 0;
}

int Plugin_ASTC::TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture) {
    //MessageBox(0,"TC_PluginFileSaveTexture srcTexture","Plugin_KTX",MB_OK);
    return 0;
}

int Plugin_ASTC::TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet) {
    FILE* pFile = NULL;
    pFile = fopen( pszFilename, "rb");
    if(pFile == NULL) {
        if (ASTC_CMips)
            ASTC_CMips->PrintError("Error(%d): ASTC Plugin ID(%d) opening file = %s ", EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    // Read the header
    astc_header header;
    memset(&header, 0, sizeof(header));

    if(fread(&header, sizeof(astc_header), 1, pFile) != 1) {
        if (ASTC_CMips)
            ASTC_CMips->PrintError("Error(%d): ASTC Plugin ID(%d) invalid ASTC header. Filename = %s ", EL_Error, IDS_ERROR_NOT_ASTC, pszFilename);
        fclose(pFile);
        return -1;
    }

    uint32_t magicval = header.magic[0] + 256 * (uint32_t) (header.magic[1]) + 65536 * (uint32_t) (header.magic[2]) + 16777216 * (uint32_t) (header.magic[3]);

    if (magicval != MAGIC_FILE_CONSTANT) {
        if (ASTC_CMips)
            ASTC_CMips->PrintError("Error(%d): ASTC Plugin ID(%d) invalid ASTC header constant. Filename = %s ", EL_Error, IDS_ERROR_NOT_ASTC, pszFilename);
        fclose(pFile);
        return -1;
    }


    m_xdim = header.blockdim_x;
    m_ydim = header.blockdim_y;
    m_zdim = header.blockdim_z;

    pMipSet->m_nBlockWidth  = m_xdim;
    pMipSet->m_nBlockHeight = m_ydim;
    pMipSet->m_nBlockDepth  = m_zdim;

    if (m_xdim < 3 || m_xdim > 12 || m_ydim < 3 || m_ydim > 12 || (m_zdim < 3 && m_zdim != 1) || m_zdim > 12) {
        if (ASTC_CMips)
            ASTC_CMips->PrintError("Error(%d): ASTC Plugin ID(%d) Block size %d %d %d is not supported. Filename = %s ", EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, pszFilename,m_xdim, m_ydim, m_zdim);
        fclose(pFile);
        return -1;
    }

    int xsize = header.xsize[0] + 256 * header.xsize[1] + 65536 * header.xsize[2];
    int ysize = header.ysize[0] + 256 * header.ysize[1] + 65536 * header.ysize[2];
    int zsize = header.zsize[0] + 256 * header.zsize[1] + 65536 * header.zsize[2];


    int xblocks = (xsize + m_xdim - 1) / m_xdim;
    int yblocks = (ysize + m_ydim - 1) / m_ydim;
    int zblocks = (zsize + m_zdim - 1) / m_zdim;

    pMipSet->m_nWidth  = xsize;
    pMipSet->m_nHeight = ysize;
    pMipSet->m_nDepth  = zsize;
    pMipSet->m_compressed = true;
    pMipSet->m_format     = CMP_FORMAT_ASTC;

    // Allocate compression
    pMipSet->m_ChannelFormat  = CF_Compressed;
    pMipSet->m_nMaxMipLevels  = 1;
    pMipSet->m_nMipLevels     = 1;    // this is overwriiten depending on input.

    if(!ASTC_CMips->AllocateMipSet(pMipSet, CF_8bit, TDT_ARGB, TT_2D, xsize, ysize, zsize)) {
        fclose(pFile);
        return PE_Unknown;
    }


    MipLevel *mipLevel = ASTC_CMips->GetMipLevel(pMipSet, 0);
    int dwTotalSize = xblocks * yblocks * zblocks * 16;

    // Allocate a data buffer
    // The actual size should be xblocks , yblocks, 1
    // Since Decompressed Output Texture is also calculated from these params
    // We will have to oversize the input buffer for now, This should be corrected
    // in future releases.
    if(!ASTC_CMips->AllocateCompressedMipLevelData(mipLevel, xsize, ysize, dwTotalSize)) {
        fclose(pFile);
        if (ASTC_CMips)
            ASTC_CMips->PrintError("Error(%d): ASTC allocate compress memmory Plugin ID(%d)\n", EL_Error, IDS_ERROR_OUTOFMEMORY);
        return PE_Unknown;
    }

    CMP_BYTE* pData = (CMP_BYTE*) (mipLevel->m_pbData);
    pMipSet->m_nMipLevels     = 1;
    int DataSize = mipLevel->m_dwLinearSize;

    fread(pData, DataSize, 1, pFile);

    fclose(pFile);

    return 0;
}


int Plugin_ASTC::TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet) {
    assert(pszFilename);
    assert(pMipSet);


    FILE* pFile = NULL;
    pFile = fopen(pszFilename, "wb");
    if(pFile == NULL) {
        if (ASTC_CMips)
            ASTC_CMips->PrintError("Error(%d): ASTC Plugin ID(%d) saving file = %s ", EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    m_xdim = pMipSet->m_nBlockWidth;
    m_ydim = pMipSet->m_nBlockHeight;
    m_zdim = pMipSet->m_nBlockDepth;

    int xsize = pMipSet->m_nWidth;
    int ysize = pMipSet->m_nHeight;
    int zsize = pMipSet->m_nDepth;

    int xblocks = (xsize + m_xdim - 1) / m_xdim;
    int yblocks = (ysize + m_ydim - 1) / m_ydim;
    int zblocks = (zsize + m_zdim - 1) / m_zdim;

    astc_header hdr;
    memset(&hdr, 0, sizeof(hdr));

    hdr.magic[0] = MAGIC_FILE_CONSTANT & 0xFF;
    hdr.magic[1] = (MAGIC_FILE_CONSTANT >> 8) & 0xFF;
    hdr.magic[2] = (MAGIC_FILE_CONSTANT >> 16) & 0xFF;
    hdr.magic[3] = (MAGIC_FILE_CONSTANT >> 24) & 0xFF;
    hdr.blockdim_x = m_xdim;
    hdr.blockdim_y = m_ydim;
    hdr.blockdim_z = m_zdim;
    hdr.xsize[0] = xsize & 0xFF;
    hdr.xsize[1] = (xsize >> 8) & 0xFF;
    hdr.xsize[2] = (xsize >> 16) & 0xFF;
    hdr.ysize[0] = ysize & 0xFF;
    hdr.ysize[1] = (ysize >> 8) & 0xFF;
    hdr.ysize[2] = (ysize >> 16) & 0xFF;
    hdr.zsize[0] = zsize & 0xFF;
    hdr.zsize[1] = (zsize >> 8) & 0xFF;
    hdr.zsize[2] = (zsize >> 16) & 0xFF;;

    fwrite(&hdr, sizeof(hdr), 1, pFile);


    MipLevel *mipLevel = ASTC_CMips->GetMipLevel(pMipSet,0);
    uint8_t* pData = mipLevel->m_pbData;
    int DataSize = mipLevel->m_dwLinearSize;

    fwrite(pData, DataSize, 1, pFile);

    fclose(pFile);

    return 0;
}

