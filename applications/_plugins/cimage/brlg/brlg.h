//=====================================================================
// Copyright 2016-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
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

#include <vector>

#include "cmp_plugininterface.h"

#define BRLG_PLUGIN_VERSION_MAJOR 2
#define BRLG_PLUGIN_VERSION_MINOR 0

#define BRLG_PLUGIN_ERROR_FILE_OPEN 1
#define BRLG_PLUGIN_ERROR_REGISTER_FILETYPE 2
#define BRLG_PLUGIN_ERROR_NOT_BRLG 3
#define BRLG_PLUGIN_ERROR_UNSUPPORTED_TYPE 4
#define BRLG_PLUGIN_ERROR_OUTOFMEMORY 5

class Image_Plugin_BRLG : public PluginInterface_Image
{
public:
    Image_Plugin_BRLG();
    virtual ~Image_Plugin_BRLG();

    int TC_PluginGetVersion(TC_PluginVersion* pluginVersion);
    int TC_PluginSetSharedIO(void* sharedIO);

    int TC_PluginFileLoadTexture(const char* fileName, MipSet* srcTexture);
    int TC_PluginFileLoadTexture(const char* fileName, CMP_Texture* srcTexture);

    int TC_PluginFileSaveTexture(const char* fileName, MipSet* srcTexture);
    int TC_PluginFileSaveTexture(const char* fileName, CMP_Texture* srcTexture);

    // These two functions are the only ones that fully support the new BRLG version 2 paradigm
    // So in a way the older functions are deprecated, though they will still work
    // But the loading especially should only be done through the new LoadPackagedTextures function
    int LoadPackagedTextures(const char* srcFileName, std::vector<CMP_MipSet>& destTextures);
    int SavePackagedTextures(const char* destFileName, const std::vector<CMP_MipSet>& srcTextures);
};

#endif
