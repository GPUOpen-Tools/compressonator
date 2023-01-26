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

#ifndef _PLUGIN_BGR_H_004DF021_0993_4F67_A5E9_2313A7C30ADE
#define _PLUGIN_BGR_H_004DF021_0993_4F67_A5E9_2313A7C30ADE

#include "plugininterface.h"
#include "cmp_plugininterface.h"

#ifdef _WIN32
// {004DF021-0993-4F67-A5E9-2313A7C30ADE}
static const GUID g_GUID = {0x4df021, 0x993, 0x4f67, {0xa5, 0xe9, 0x23, 0x13, 0xa7, 0xc3, 0xa, 0xde}};

#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

#define IDS_ERROR_FILE_OPEN             1
#define IDS_ERROR_REGISTER_FILETYPE     2
#define IDS_ERROR_NOT_BRLG              3
#define IDS_ERROR_UNSUPPORTED_TYPE      4
#define IDS_ERROR_OUTOFMEMORY           5

class Image_Plugin_BRLG : public PluginInterface_Image {
  public:
    Image_Plugin_BRLG();
      virtual ~Image_Plugin_BRLG();

    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginSetSharedIO(void *Shared);
    int TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet);
    int TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet);
    int TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture);
    int TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture);

};

extern CMIPS* BRLG_CMips;
extern void*  make_Codec_Plugin_BRLG();


#endif
