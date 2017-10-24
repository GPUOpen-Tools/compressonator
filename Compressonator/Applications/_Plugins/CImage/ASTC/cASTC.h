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

#ifndef _PLUGIN_IMAGE_ASTC_H
#define _PLUGIN_IMAGE_ASTC_H

#include "PluginInterface.h"
#include "stdint.h"

// {3C9D75E9-D2CB-43F2-B371-D8D38FC8F306}
#ifdef _WIN32
static const GUID g_GUID = 
{ 0x3c9d75e9, 0xd2cb, 0x43f2, { 0xb3, 0x71, 0xd8, 0xd3, 0x8f, 0xc8, 0xf3, 0x6 } };
#else
static const GUID g_GUID = 
{ 0};
#endif

CMIPS *ASTC_CMips = NULL;

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

class Plugin_ASTC : public PluginInterface_Image
{
    public: 
        Plugin_ASTC();
        virtual ~Plugin_ASTC();

        int TC_PluginSetSharedIO(void* Shared);
        int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
        int TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet);
        int TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet);
        int TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture);
        int TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture);
        int m_xdim;
        int m_ydim;
        int m_zdim;

};


#define IDS_ERROR_FILE_OPEN             1
#define IDS_ERROR_NOT_ASTC              2
#define IDS_ERROR_UNSUPPORTED_TYPE      3
#define IDS_ERROR_OUTOFMEMORY            4

// ---------------- ASTC File Definitions ------------------------
#define MAGIC_FILE_CONSTANT 0x5CA1AB13

struct astc_header
{
    uint8_t magic[4];
    uint8_t blockdim_x;
    uint8_t blockdim_y;
    uint8_t blockdim_z;
    uint8_t xsize[3];            // x-size = xsize[0] + xsize[1] + xsize[2]
    uint8_t ysize[3];            // x-size, y-size and z-size are given in texels;
    uint8_t zsize[3];            // block count is inferred
};

extern void *make_Plugin_ASTC();

#endif
