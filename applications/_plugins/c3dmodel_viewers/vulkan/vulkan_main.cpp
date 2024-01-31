// AMD AMDUtils code
//
// Copyright(c) 2018-2024 Advanced Micro Devices, Inc.All rights reserved.
//
// Vulkan Samples
//
// Copyright (C) 2015-2016 Valve Corporation
// Copyright (C) 2015-2016 LunarG, Inc.
// Copyright (C) 2015-2016 Google, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include "vulkan_main.h"

#include "cmp_gltfcommon.h"

#include "tc_pluginapi.h"
#include "tc_plugininternal.h"

#include "compressonator.h"

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_3DModelViewer_Vulkan)
SET_PLUGIN_TYPE("3DMODEL_VIEWER")
SET_PLUGIN_NAME("VULKAN")
#else
void* make_Plugin_3DModelViewer_Vulkan()
{
    return new Plugin_3DModelViewer_Vulkan;
}
#endif

#include "misc.h"
#include "vulkan_device.h"

#ifdef BUILD_AS_PLUGIN_DLL
bool   g_bAbortCompression = false;
CMIPS* g_CMIPS             = nullptr;
#else
extern bool   g_bAbortCompression;
extern CMIPS* g_CMIPS;
#endif

//using namespace ML_vulkan;

Plugin_3DModelViewer_Vulkan::Plugin_3DModelViewer_Vulkan()
{
    m_gltfLoader   = NULL;
    m_VulkanDevice = NULL;
    m_hwnd         = 0L;
    m_ShowViewOk   = false;
}

Plugin_3DModelViewer_Vulkan::~Plugin_3DModelViewer_Vulkan()
{
    if (m_gltfLoader)
    {
        if (m_gltfLoader[0].m_filename.length() > 0)
            m_gltfLoader[0].Unload();
        if (m_gltfLoader[1].m_filename.length() > 0)
            m_gltfLoader[1].Unload();
    }
}

int Plugin_3DModelViewer_Vulkan::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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

int Plugin_3DModelViewer_Vulkan::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        VK_CMips              = static_cast<CMIPS*>(Shared);
        VK_CMips->m_infolevel = 0x01;  // Turn on print Info
        return 0;
    }
    return 1;
}

void Plugin_3DModelViewer_Vulkan::processMSG(void* message)
{
    if (m_VulkanDevice && m_ShowViewOk)
    {
        m_VulkanDevice->OnEvent(message);
    }
}

bool Plugin_3DModelViewer_Vulkan::OnRenderView()
{
    if (m_VulkanDevice && m_ShowViewOk)
    {
        m_VulkanDevice->OnRender();
        return true;
    }
    return false;
}

void Plugin_3DModelViewer_Vulkan::OnReSizeView(CMP_LONG w, CMP_LONG h)
{
    if (m_VulkanDevice && m_ShowViewOk)
    {
        m_VulkanDevice->OnResize(w, h);
    }
}

void Plugin_3DModelViewer_Vulkan::CloseView()
{
    if (m_VulkanDevice)
    {
        m_VulkanDevice->SetFullScreen(false);
        if (m_ShowViewOk)
            m_VulkanDevice->OnDestroy();
        delete m_VulkanDevice;
        m_VulkanDevice = NULL;
        m_ShowViewOk   = false;
    }
}

void* Plugin_3DModelViewer_Vulkan::ShowView(void* data)
{
    m_hwnd = reinterpret_cast<HWND>(data);

    if (m_VulkanDevice)
    {
        int res = m_VulkanDevice->OnCreate(m_hwnd);
        if (res != VK_SUCCESS)
        {
            delete m_VulkanDevice;
            m_VulkanDevice = NULL;
            return NULL;
        }
    }

    m_ShowViewOk = true;

#ifdef USE_NATIVEWINDOW
    ShowWindow(m_hwnd, 10);
#endif

    //  if (m_VulkanDevice)
    //      m_VulkanDevice->OnResize(Width, Height);

    return m_VulkanDevice;
}

//#define USE_NATIVEWINDOW

#ifdef USE_NATIVEWINDOW
// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, message, wParam, lParam);
}
#endif

void* Plugin_3DModelViewer_Vulkan::CreateView(void*             ModelData,
                                              CMP_LONG          Width,
                                              CMP_LONG          Height,
                                              void*             userHWND,
                                              void*             pluginManager,
                                              void*             msghandler,
                                              CMP_Feedback_Proc pFeedbackProc = NULL)
{
    m_ShowViewOk = false;

    // device already created!
    if (m_VulkanDevice)
        return 0;
    if (!ModelData)
        return 0;

    m_gltfLoader = (CMP_GLTFCommon*)ModelData;

    // Check User Canceled status for long glTF file loads!!
    if (VK_CMips)
    {
        if (VK_CMips->m_canceled)
        {
            VK_CMips->m_canceled = false;
            return nullptr;
        }
    }

#ifdef USE_NATIVEWINDOW
    WNDCLASSEX windowClass;

    // init window class
    ZeroMemory(&windowClass, sizeof(WNDCLASSEX));
    windowClass.cbSize        = sizeof(WNDCLASSEX);
    windowClass.style         = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc   = WindowProc;
    windowClass.hInstance     = GetModuleHandle(NULL);
    windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"WindowClass1";
    RegisterClassEx(&windowClass);

    RECT windowRect = {0, 0, Width, Height};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);  // adjust the size

    // create the window and store a handle to it
    m_hwnd = CreateWindowEx(NULL,
                            L"WindowClass1",  // name of the window class
                            L"Native Window",
                            WS_OVERLAPPEDWINDOW,
                            100,
                            100,
                            windowRect.right - windowRect.left,
                            windowRect.bottom - windowRect.top,
                            NULL,                   // we have no parent window, NULL
                            NULL,                   // we aren't using menus, NULL
                            windowClass.hInstance,  // application handle
                            NULL);                  // used with multiple windows, NULL
#else
    //m_parent = (QWidget *)userHWND;
    //m_hwnd = reinterpret_cast<HWND> (m_parent->winId());
    m_hwnd = reinterpret_cast<HWND>(userHWND);
#endif
    //    if (m_hwnd)
    //    {
    m_VulkanDevice = new Vulkan_Device(m_gltfLoader, Width, Height, pluginManager, msghandler);
    //        if (m_VulkanDevice)
    //        {
    //            int res = m_VulkanDevice->OnCreate(m_hwnd);
    //            if (res != VK_SUCCESS)
    //            {
    //                delete m_VulkanDevice;
    //                m_VulkanDevice = NULL;
    //                return nullptr;
    //            }
    //        }
    //    }

    // Check User Canceled status for Aborting Create Device!!
    if (VK_CMips)
    {
        if (VK_CMips->m_canceled)
        {
            VK_CMips->m_canceled = false;
            return nullptr;
        }
    }

    // #ifdef USE_NATIVEWINDOW
    //     ShowWindow(m_hwnd, 10);
    // #endif
    //
    //     if (m_VulkanDevice)
    //         m_VulkanDevice->OnResize(Width, Height);

    return m_VulkanDevice;
}
