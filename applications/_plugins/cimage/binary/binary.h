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

#ifndef _PLUGIN_BINARY_H_C75917C1_330B_4AEA_B9A0_B0347DF3968F
#define _PLUGIN_BINARY_H_C75917C1_330B_4AEA_B9A0_B0347DF3968F

#include "plugininterface.h"
#include "cmp_plugininterface.h"

#ifdef _WIN32
// {C75917C1-330B-4AEA-B9A0-B0347DF3968F}
static const GUID g_GUID = { 0xc75917c1, 0x330b, 0x4aea, { 0xb9, 0xa0, 0xb0, 0x34, 0x7d, 0xf3, 0x96, 0x8f } };

#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

#define IDS_ERROR_FILE_OPEN             1
#define IDS_ERROR_REGISTER_FILETYPE     2
#define IDS_ERROR_NOT_BINARY            3
#define IDS_ERROR_UNSUPPORTED_TYPE      4
#define IDS_ERROR_OUTOFMEMORY           5

class Image_Plugin_BINARY : public PluginInterface_Image {
  public:
    Image_Plugin_BINARY();
      virtual ~Image_Plugin_BINARY();

    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginSetSharedIO(void *Shared);
    int TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet);
    int TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet);
    int TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture);
    int TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture);

};

extern CMIPS* BINARY_CMips;
extern void*  make_Codec_Plugin_BINARY();

#endif
