//=====================================================================
// Copyright 2018-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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
#ifndef _PLUGIN_IMAGE_BMP_H
#define _PLUGIN_IMAGE_BMP_H

#include "plugininterface.h"
#include "gltf_dx12deviceex.h"

#ifdef _WIN32
// {2505D1C0-D0F3-4E57-BCED-8358689D3FCC}
static const GUID g_GUID = {0x2505d1c0, 0xd0f3, 0x4e57, {0xbc, 0xed, 0x83, 0x58, 0x68, 0x9d, 0x3f, 0xcc}};

#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR 1
#define TC_PLUGIN_VERSION_MINOR 0

CMIPS* DX12_CMips = NULL;

class Plugin_glTF_DX12_EX : public PluginInterface_3DModel
{
public:
    Plugin_glTF_DX12_EX();
    virtual ~Plugin_glTF_DX12_EX();
    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginSetSharedIO(void* Shared);

    void* CreateView(void* ModelData, CMP_LONG Width, CMP_LONG Height, void* userHWND, void* pluginManager, void* msghandler, CMP_Feedback_Proc pFeedbackProc);

    void* ShowView(void* data);
    void  CloseView();

    void processMSG(void* message);

    bool OnRenderView();
    void OnReSizeView(CMP_LONG w, CMP_LONG h);

private:
    glTF_DX12DeviceEx* m_glTF_DX12DeviceEx;
    CMP_GLTFCommon*    m_gltfLoader;
    HWND               m_hwnd;
    QWidget*           m_parent;
};

extern void* make_Plugin_glTF_DX12_EX();

#endif