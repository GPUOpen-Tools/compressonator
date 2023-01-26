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

#ifndef _DDS_H
#define _DDS_H

#include "cmp_plugininterface.h"
#include "plugininterface.h"

#ifdef _WIN32
#include "ddraw.h"
#include "d3d9types.h"
#endif

// {F3D02C4D-BE5F-4074-9098-BB13D59EF875}
#ifdef _WIN32
static const GUID g_GUID = { 0xf3d02c4d, 0xbe5f, 0x4074, { 0x90, 0x98, 0xbb, 0x13, 0xd5, 0x9e, 0xf8, 0x75 } };
#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

#define IDS_ERROR_FILE_OPEN             1
#define IDS_ERROR_REGISTER_FILETYPE     2
#define IDS_ERROR_NOT_DDS               3
#define IDS_ERROR_UNSUPPORTED_TYPE      4

// #define _USEDIRECTX

class Plugin_DDS : public PluginInterface_Image {
  public:
    Plugin_DDS();
    virtual ~Plugin_DDS();

    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginSetSharedIO(void *Shared);
    int TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet);
    int TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet);
    int TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture);
    int TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture);

};

extern CMIPS *DDS_CMips;
extern void *make_Plugin_DDS();


#endif
