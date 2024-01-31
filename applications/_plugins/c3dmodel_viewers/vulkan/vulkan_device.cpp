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

#define CMP_PI 3.1428

#include "vulkan_device.h"
#include "cmp_gltfcommon.h"
#include "cmp_imguivk.h"
#include "cmp_keyboardmousevk.h"
#include "cmp_misc.h"
#include "cmp_swapchainvk.h"

#include <glm/glm.hpp>

#ifdef USE_QT10
QVulkanWindowRenderer* Vulkan_Device::createRenderer()
{
    return new Vulkan_Renderer(this);
}
#endif

Vulkan_Device::Vulkan_Device(CMP_GLTFCommon gltfLoader[MAX_NUM_OF_NODES], std::uint32_t width, std::uint32_t height, void* pluginManager, void* msghandler)
    : FrameworkWindows(width, height)
{
    m_pluginManager = (PluginManager*)pluginManager;
    m_msghandler    = msghandler;

    if (gltfLoader == NULL)
        return;

    m_gltfLoader[0] = &gltfLoader[0];
    m_gltfLoader[1] = &gltfLoader[1];

    m_max_Nodes_loaded = 1;
    if (gltfLoader[1].m_filename.length() > 0)
    {
        m_max_Nodes_loaded = 2;
    }

    UI.xTrans = 0.0f;
    UI.yTrans = 0.0f;
    UI.zTrans = 0.0f;

    UI.xRotation   = 0.0f;
    UI.yRotation   = 0.0f;
    UI.zRotation   = 0.0f;
    UI.fill        = true;
    UI.m_showimgui = false;

    // imGui default window size and pos
    m_imgui_win_size.x = 200;
    m_imgui_win_size.y = 500;
    m_imgui_win_pos.x  = 10;
    m_imgui_win_pos.y  = 10;

    m_lastFrameTime         = MillisecondsNow();
    m_curr_Node             = 0;
    m_dwNumberOfBackBuffers = 1;

#ifdef USE_ANIMATION
    m_time  = 0;
    m_bPlay = true;
#endif
}

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
int Vulkan_Device::OnCreate(void* hWnd)
{
    VkResult res;

    std::vector<const char*> instance_layer_names;
    std::vector<const char*> instance_extension_names;

    instance_extension_names.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
    instance_extension_names.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    instance_extension_names.push_back("VK_EXT_debug_report");

    instance_layer_names.push_back("VK_LAYER_LUNARG_standard_validation");

    VkApplicationInfo app_info  = {};
    app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext              = NULL;
    app_info.pApplicationName   = "Vulkan_Viewer";
    app_info.applicationVersion = 1;
    app_info.pEngineName        = "Vulkan_Viewer";
    app_info.engineVersion      = 1;
    app_info.apiVersion         = VK_API_VERSION_1_0;

    VkInstanceCreateInfo inst_info    = {};
    inst_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext                   = NULL;
    inst_info.flags                   = 0;
    inst_info.pApplicationInfo        = &app_info;
    inst_info.enabledLayerCount       = (uint32_t)instance_layer_names.size();
    inst_info.ppEnabledLayerNames     = (uint32_t)instance_layer_names.size() ? instance_layer_names.data() : NULL;
    inst_info.enabledExtensionCount   = (uint32_t)instance_extension_names.size();
    inst_info.ppEnabledExtensionNames = instance_extension_names.data();

    res = vkCreateInstance(&inst_info, NULL, &m_inst);
    if (res != VK_SUCCESS)
    {
        return res;
    }

    m_device.OnCreate(m_inst, hWnd);

    //
    //
    //uint32_t queueFamilyIndices[2] = { (uint32_t)graphics_queue_family_index, (uint32_t)present_queue_family_index };

    // Create Swapchain
    std::uint32_t dwNumberOfBackBuffers = 2;
    m_swapChain.OnCreate(&m_device, dwNumberOfBackBuffers, m_Width, m_Height, hWnd);

    for (int curr_Node = 0; curr_Node < m_max_Nodes_loaded; curr_Node++)
    {
        // Create a instance of the renderer and initialize it, we need to do that for each GPU
        m_Node[curr_Node] = new Vulkan_Renderer(this);
        m_Node[curr_Node]->OnCreate(&m_device);
        m_Node[curr_Node]->OnCreateWindowSizeDependentResources(&m_swapChain, m_Width - 100, m_Height - 100);

        // Load scene data from system memory into all the GPUs (done once per GPU)
        m_Node[curr_Node]->LoadScene(m_gltfLoader[curr_Node], m_pluginManager, m_msghandler);
    }

    //    if (UI.m_showimgui)
    //        ImGUIVK_Init((void *)hWnd);

    // Init Camera, looking at origin
    m_roll  = 0.0f;
    m_pitch = 0.0f;

#ifdef ENABLE_RENDER_CODE
    m_state.depthBias          = 70.0f / 100000.0f;
    m_state.exposure           = 0.857f;
    m_state.iblFactor          = 0.237f;
    m_state.spotLightIntensity = 3.690f;
    m_state.toneMapper         = 0;
    m_state.glow               = 0.1f;
    m_state.bDrawBoundingBoxes = false;
    m_state.light.SetFov(CMP_PI / 2, 1024, 1024);
    //m_state.light.UpdateCamera(0.0f, 1.7f, 3.0f);
    m_state.light.UpdateCamera(3.67f + 3.14159f, 0.58f, 3.0f);
#endif

    m_frameCount   = 0;
    m_frameRate    = 0.0f;
    m_frameRateMin = FLT_MAX;

    return VK_SUCCESS;
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void Vulkan_Device::OnDestroy()
{
    //    if (UI.m_showimgui)
    //        ImGUIVK_Shutdown();

#ifdef ENABLE_RENDER_CODE
    for (int curr_Node = 0; curr_Node < m_max_Nodes_loaded; curr_Node++)
    {
        m_Node[curr_Node]->UnloadScene();
        m_Node[curr_Node]->OnDestroyWindowSizeDependentResources();
        m_Node[curr_Node]->OnDestroy();
    }

    //   for (int curr_Node = 0; curr_Node < m_max_Nodes_loaded; curr_Node++)
    //   {
    //       delete[] m_Node[curr_Node];
    //   }

    m_swapChain.OnDestroy();
#endif
}

//--------------------------------------------------------------------------------------
//
// OnEvent
//
//--------------------------------------------------------------------------------------

static int Roll  = 0;
static int Pitch = 0;
static int m_lastMouseWheelDelta, m_mouseWheelDelta = 0;

bool Vulkan_Device::OnEvent(void* msg)
{
#ifdef _WIN32
    const MSG&  message   = *static_cast<MSG*>(msg);
    static bool Mouse_RBD = false;
    // Always update mouse pos when mouse clicked.
    if (message.message == WM_LBUTTONDOWN)
    {
        m_mouse_press_xpos = (signed short)(message.lParam);
        m_mouse_press_ypos = (signed short)(message.lParam >> 16);
    }

    // Check if mouse is inside imGUI window
    //    if (UI.m_showimgui)
    //    {
    //
    //        if ((m_mouse_press_xpos > m_imgui_win_pos.x) && (m_mouse_press_xpos < (m_imgui_win_pos.x + m_imgui_win_size.x + 20)))
    //        {
    //            if ((m_mouse_press_ypos > m_imgui_win_pos.y) && (m_mouse_press_ypos < (m_imgui_win_pos.y + m_imgui_win_size.y + 20)))
    //            {
    //                return true;
    //            }
    //        }
    //    }

    switch (message.message)
    {
    case WM_COMMAND: {
        int show       = (int)(message.lParam);
        UI.m_showimgui = show ? true : false;
        return true;
    }
    case WM_MOUSEWHEEL:
        m_mouseWheelDelta += (short)HIWORD(message.wParam);
        return true;

    case WM_MOUSEMOVE:
        Roll  = (signed short)(message.lParam);
        Pitch = (signed short)(message.lParam >> 16);

        m_roll  = Roll / 100.0f;   // Roll  about the x axis (y changes)
        m_pitch = Pitch / 100.0f;  // Pitch about the y axis (x changes)
        return true;
    }
#endif
    return true;
}

//--------------------------------------------------------------------------------------
//
// SetFullScreen
//
//--------------------------------------------------------------------------------------
void Vulkan_Device::SetFullScreen(bool fullscreen)
{
    /*
    #ifdef ENABLE_RENDER_CODE
    for (UINT i = 0; i<m_nodeCount; i++)
    {
        GPUFlush(m_pDevice, m_pDirectQueue[i]);
    }

    m_SwapChain.SetFullScreen(fullscreen);
    #endif
    */
}

//--------------------------------------------------------------------------------------
//
// OnResize
//
//--------------------------------------------------------------------------------------
void Vulkan_Device::OnResize(uint32_t width, uint32_t height)
{
    m_Width  = width;
    m_Height = height;
#ifdef ENABLE_RENDER_CODE
    m_state.camera.SetFov(CMP_PI / 4, m_Width, m_Height);
#endif
}

//--------------------------------------------------------------------------------------
//
// OnRender
//
//--------------------------------------------------------------------------------------
void Vulkan_Device::OnRender()
{
    static int BackBuffer = 1;

    //==================================
    // Calculate Frame Rates
    //==================================
    m_frameCount++;

    // Get the number of ms since last timer reset
    m_elapsedTimer = MillisecondsNow() - m_elapsedTimer;
    if (m_elapsedTimer > 1000)
    {
        m_frameRate = 1000.0f / m_frameCount;

        if (m_frameRateMin > m_frameRate)
            m_frameRateMin = m_frameRate;

        m_elapsedTimer = MillisecondsNow();
        m_frameCount   = 0;
    }

    // Get timings
    //
    double timeNow  = MillisecondsNow();
    m_deltaTime     = timeNow - m_lastFrameTime;
    m_lastFrameTime = timeNow;

    // Build UI and set the scene state. Note that the rendering of the UI happens later.
    //
    //    if (UI.m_showimgui)
    //    {
    //        ImGUIVK_UpdateIO();
    //
    //        ImGui::NewFrame();
    //        ImGui::Begin("Stats", NULL, ImGuiWindowFlags_ShowBorders);
    //
    //        if (ImGui::CollapsingHeader("Info"), ImGuiTreeNodeFlags_DefaultOpen)
    //        {
    //            ImGui::Text("Resolution       : %ix%i", m_Width, m_Height);
    //        }
    //
    //
    //        if (ImGui::CollapsingHeader("Animation"), ImGuiTreeNodeFlags_DefaultOpen)
    //        {
    //            //ImGui::Checkbox("Play", &m_bPlay);
    //            ImGui::SliderFloat("Time", &m_time, 0, 30);
    //        }
    //
    //        if (ImGui::CollapsingHeader("Model Selection"), ImGuiTreeNodeFlags_DefaultOpen)
    //        {
    //            const char * tonemappers[] = { "Timothy", "DX11DSK", "Reinhard", "Uncharted2Tonemap", "No tonemapper" };
    //            ImGui::Combo("tone mapper", &m_state.toneMapper, tonemappers, 5);
    //            ImGui::SliderFloat("exposure", &m_state.exposure, -1.0f, 2.0f);
    //
    //            ImGui::SliderFloat("iblFactor", &m_state.iblFactor, 0.0f, 2.0f);
    //            ImGui::SliderFloat("spotLightIntensity", &m_state.spotLightIntensity, 0.0f, 10.0f);
    //            ImGui::SliderFloat("glow", &m_state.glow, 0.0f, 1.0f);
    //        }
    //
    //        ImGui::Checkbox("Show Bounding Boxes", &m_state.bDrawBoundingBoxes);
    //        ImGui::Checkbox("Show Sky dome", &m_state.bDrawSkyDome);
    //        ImGui::Checkbox("Show Gamma test pattern", &m_state.bGammaTestPattern);
    //
    //        ImGui::End();
    //    }

    // If mouse was not used by the GUI then it's for the camera
    //
    //        ImGuiIO& io = ImGui::GetIO();
    //        if (io.WantCaptureMouse == false)
    //        {
    //            if (io.MouseDown[0])
    //            {
    //                m_roll  += io.MouseDelta.x / 100.f;
    //                m_pitch += io.MouseDelta.y / 100.f;
    //            }
    //
    //            static float distance = 4.0f;
    //            distance -= (float)io.MouseWheel / 3.0f;
    //            if (distance <= 1.0f)
    //                distance = 1.0f;
    //
    //            m_state.camera.UpdateCamera(m_roll, m_pitch, distance);
    //        }

    static float distance        = m_gltfLoader[m_curr_Node]->m_distance;
    int          mouseWheelDelta = m_mouseWheelDelta - m_lastMouseWheelDelta;
    distance -= (float)(mouseWheelDelta / 200.0f) * distance;
    m_lastMouseWheelDelta = m_mouseWheelDelta;
    m_state.camera.UpdateCamera(m_roll, m_pitch, distance);

#ifdef USE_ANIMATION
    // Set animation
    //
    if (m_bPlay)
    {
        m_time += (float)m_deltaTime / 1000.0f;
    }

    m_pGltfLoader->SetAnimationTime(0, m_time);
#endif

#ifdef ENABLE_RENDER_CODE
    // Do Render frame using AFR
    //
    m_Node[m_curr_Node]->OnRender(&m_state, &m_swapChain);

    m_swapChain.Present();
#endif

    // BackBuffer++;
    // // Flip to new image if loaded after 2 buffer render calls
    // if ((std::uint32_t)BackBuffer > m_dwNumberOfBackBuffers)
    // {
    //     BackBuffer = 1;
    //     m_curr_Node++;
    //     if (m_curr_Node >= m_max_Nodes_loaded) m_curr_Node = 0;
    // }
}
