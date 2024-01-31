// AMD SampleDX12 sample code
//
// Copyright(c) 2017-2024 Advanced Micro Devices, Inc.All rights reserved.
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

#pragma once

#define ENABLE_RENDER_CODE

// #pragma comment(lib,"d3dcompiler.lib")
// #pragma comment(lib, "D3D12.lib")

#include "cmp_frameworkwindowsvk.h"
#include "cmp_cameravk.h"
#include "cmp_swapchainvk.h"
#include "cmp_devicevk.h"

#ifdef ENABLE_RENDER_CODE
#include "vulkan_renderer.h"
#endif

#include "userinterface.h"
#include "pluginmanager.h"
#define MAX_NUM_OF_NODES 2

// -- Qt Support
#ifdef USE_QT10
#include <QtGUI/qvulkanwindow>
#endif

class CMP_GLTFCommon;
extern CMIPS* VK_CMips;

class Vulkan_Device :
#ifdef USE_QT10
    public QVulkanWindow,
    public FrameworkWindows
#else
    public FrameworkWindows
#endif
{
public:
    int m_FrameCount = 0;

    int    m_frameCount;
    float  m_frameRate;
    float  m_frameRateMin = FLT_MAX;
    double m_elapsedTimer = 0;

    VkInstance   m_inst;
    CMP_DeviceVK m_device;

#ifdef USE_QT10
    QVulkanWindowRenderer* createRenderer() override;
#endif

    // *1* change to MAX_NUM_OF_NODES
    CMP_GLTFCommon* m_gltfLoader[MAX_NUM_OF_NODES];

#ifdef ENABLE_RENDER_CODE
    Vulkan_Renderer*       m_Node[MAX_NUM_OF_NODES];
    Vulkan_Renderer::State m_state;
#endif

    SwapChainVK m_swapChain;

    Vulkan_Device(CMP_GLTFCommon m_gltfLoader[MAX_NUM_OF_NODES], std::uint32_t width, std::uint32_t height, void* pluginManager, void* msghandler);

    int  OnCreate(void* hWnd);
    void OnDestroy();
    void OnRender();
    bool OnEvent(void* msg);
    void OnResize(uint32_t Width, uint32_t Height);
    void SetFullScreen(bool fullscreen);

    // *1* marks changes in code from AMDUtils
    PluginManager* m_pluginManager;
    int            m_curr_Node;
    int            m_max_Nodes_loaded;
    void*          m_msghandler;
    std::uint32_t  m_dwNumberOfBackBuffers;

private:
    // User IO interface settings
    UserInterface UI;

    ImVec2 m_imgui_win_size;
    ImVec2 m_imgui_win_pos;

    int m_mouse_press_xpos, m_mouse_press_ypos;

    float m_roll;
    float m_pitch;

    float  m_time;       // The elapsed time since the previous frame.
    double m_deltaTime;  // The elapsed time since the previous frame.
    double m_lastFrameTime;

#ifdef USE_ANIMATION
    bool m_bPlay;
#endif
};