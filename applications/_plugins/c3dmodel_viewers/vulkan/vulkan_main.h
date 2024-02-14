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

#include "cmp_plugininterface.h"
#include "vulkan_device.h"

#include <QtWidgets/qwidget.h>

#ifdef _WIN32
// {3244041C-1A53-4E79-B417-842D5CEF33C0}
static const GUID g_GUID = {0x3244041c, 0x1a53, 0x4e79, {0xb4, 0x17, 0x84, 0x2d, 0x5c, 0xef, 0x33, 0xc0}};

#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR 1
#define TC_PLUGIN_VERSION_MINOR 0

CMIPS* VK_CMips = NULL;

class Plugin_3DModelViewer_Vulkan : public PluginInterface_3DModel
{
public:
    Plugin_3DModelViewer_Vulkan();
    virtual ~Plugin_3DModelViewer_Vulkan();
    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginSetSharedIO(void* Shared);

    void* CreateView(void* ModelData, CMP_LONG Width, CMP_LONG Height, void* userHWND, void* pluginManager, void* msghandler, CMP_Feedback_Proc pFeedbackProc);
    bool  OnRenderView();
    void  CloseView();
    void* ShowView(void* data);
    void  processMSG(void* message);
    void  OnReSizeView(CMP_LONG w, CMP_LONG h);

private:
    bool            m_ShowViewOk;
    Vulkan_Device*  m_VulkanDevice;
    CMP_GLTFCommon* m_gltfLoader;
    HWND            m_hwnd;
    QWidget*        m_parent;
};

extern void* make_Plugin_3DModelViewer_Vulkan();

#endif