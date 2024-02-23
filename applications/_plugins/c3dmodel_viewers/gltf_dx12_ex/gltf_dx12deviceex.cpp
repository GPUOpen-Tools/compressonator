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

#include "gltf_dx12deviceex.h"

#include "cmp_gltfcommon.h"
#include "cmp_misc.h"
#include "cmp_error.h"

#ifdef ENABLE_RENDER_CODE
#include "gltf_dx12rendererex.h"
#endif

#include <dxgidebug.h>
#include <directxmath.h>

using namespace DirectX;

static int BackBuffer = 1;

glTF_DX12DeviceEx::glTF_DX12DeviceEx(CMP_GLTFCommon gltfLoader[MAX_NUM_OF_NODES],
                                     DWORD          width,
                                     DWORD          height,
                                     void*          pluginManager,
                                     void*          msghandler,
                                     QWidget*       parent)
    : FrameworkWindows(width, height)
{
    m_parent = parent;

    m_pluginManager = (PluginManager*)pluginManager;
    m_msghandler    = msghandler;

    if (gltfLoader == NULL)
        return;

    m_gltfLoader[0] = &gltfLoader[0];
    m_gltfLoader[1] = &gltfLoader[1];

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

    m_window = new QImGUI_WidgetWindowWrapper_DX12(m_parent);

    m_lastFrameTime = MillisecondsNow();

    m_time             = 0;
    UI.m_playanimation = false;
}

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
int glTF_DX12DeviceEx::OnCreate(HWND hWnd)
{
#ifdef ENABLE_RENDER_CODE
    // Create device
    //
    ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice)));

    // Get the number of GPU's in the system
    m_nodeCount = m_pDevice->GetNodeCount();

    // Create Queues, in MGPU mode we need one per node/GPU
    m_pDirectQueue = new ID3D12CommandQueue*[m_nodeCount];
    std::vector<ID3D12CommandQueue*> DirectQueues;
    for (UINT i = 0; i < m_nodeCount; i++)
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags                    = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type                     = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.NodeMask                 = 1 << i;
        m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pDirectQueue[i]));
        m_pDirectQueue[i]->SetName(L"DirectQueue");
        DirectQueues.push_back(m_pDirectQueue[i]);
    }

    // Create Swapchain 2 per each model loaded
    m_dwNumberOfBackBuffers = 2;
    m_SwapChain.OnCreate(m_pDevice, m_dwNumberOfBackBuffers, &DirectQueues, m_Width, m_Height, hWnd);

    for (int curr_Node = 0; curr_Node < m_max_Nodes_loaded; curr_Node++)
    {
        // Create a instance of the renderer and initialize it, we need to do that for each GPU
        m_Node[curr_Node] = new glTF_DX12RendererEx[m_nodeCount]();
        for (UINT i = 0; i < m_nodeCount; i++)
        {
            UINT node     = i + 1;
            UINT nodeMask = 1 << i;

            m_Node[curr_Node][i].OnCreate(
                m_pDevice, m_pDirectQueue[i], node, nodeMask, m_pluginManager, m_msghandler, &m_ImGuiRenderer, curr_Node == 0 ? m_window : NULL);
            m_Node[curr_Node][i].OnCreateWindowSizeDependentResources(m_pDevice, m_Width, m_Height, node, nodeMask);
        }

        // Load scene data from system memory into all the GPUs (done once per GPU)
        for (UINT i = 0; i < m_nodeCount; i++)
        {
            m_Node[curr_Node][i].LoadScene(m_gltfLoader[curr_Node], m_pluginManager, m_msghandler);
        }
    }

#endif

    // Init Camera, looking at origin
    m_roll          = 0.0f;
    m_pitch         = 0.0f;
    mouseWheelDelta = 0;

#ifdef ENABLE_RENDER_CODE
    m_state.depthBias          = 70.0f / 100000.0f;
    m_state.exposure           = 0.857f;
    m_state.iblFactor          = 0.237f;
    m_state.spotLightIntensity = 3.690f;
    m_state.toneMapper         = 0;
    m_state.glow               = 0.1f;
    m_state.bDrawBoundingBoxes = false;
    m_state.light.SetFov(XM_PI / 2, 1024, 1024);
    m_state.bGammaTestPattern = false;
    m_state.bDrawSkyDome      = false;

    m_state.light.UpdateCamera(3.67f + 3.14159f, 0.58f, 3.0f);
#endif

    // This is used to allow user to flip between two rendered views in a controlled maner
    // by default if two models are loaded each model will be displayed one after another
    // after 2 renders, setting this to true enables the flip else it will only render
    // the model that was viewed last!
    m_AllowImageFrameFlip = true;
    m_TimeSinceLastFlip   = 0.0f;
    m_flipState           = RENDER_FLIP_2FRAMES;

    return 0;
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void glTF_DX12DeviceEx::OnDestroy()
{
#ifdef ENABLE_RENDER_CODE
    for (int curr_Node = 0; curr_Node < m_max_Nodes_loaded; curr_Node++)
    {
        for (UINT i = 0; i < m_nodeCount; i++)
        {
            if (curr_Node == 0)
            {
                GPUFlush(m_pDevice, m_pDirectQueue[i]);
            }

            m_Node[curr_Node][i].UnloadScene();
            m_Node[curr_Node][i].OnDestroyWindowSizeDependentResources();
            m_Node[curr_Node][i].OnDestroy();
        }
    }

    for (UINT i = 0; i < m_nodeCount; i++)
    {
        m_pDirectQueue[i]->Release();
    }

    delete[] m_pDirectQueue;

    for (int curr_Node = 0; curr_Node < m_max_Nodes_loaded; curr_Node++)
    {
        delete[] m_Node[curr_Node];
    }

    m_SwapChain.OnDestroy();
    m_pDevice->Release();
#endif
}

//--------------------------------------------------------------------------------------
//
// OnEvent
//
//--------------------------------------------------------------------------------------

static float g_Roll  = 0;
static float g_Pitch = 0;
static int   m_lastMouseWheelDelta, m_mouseWheelDelta = 0;

bool glTF_DX12DeviceEx::OnEvent(MSG msg)
{
    static bool Mouse_RBD = false;
    // Always update mouse pos when mouse clicked.
    if (msg.message == WM_LBUTTONDOWN)
    {
        m_mouse_press_xpos = (signed short)(msg.lParam);
        m_mouse_press_ypos = (signed short)(msg.lParam >> 16);
    }

    // Check if mouse is inside imGUI window
    if (UI.m_showimgui)
    {
        if ((m_mouse_press_xpos > m_imgui_win_pos.x) && (m_mouse_press_xpos < (m_imgui_win_pos.x + m_imgui_win_size.x + 20)))
        {
            if ((m_mouse_press_ypos > m_imgui_win_pos.y) && (m_mouse_press_ypos < (m_imgui_win_pos.y + m_imgui_win_size.y + 20)))
            {
                m_mouse_press_xpos = 0;
                m_mouse_press_ypos = 0;
                return true;
            }
        }
    }

    switch (msg.message)
    {
    case WM_KEYDOWN: {
        int key   = (int)(msg.lParam);
        int state = (int)(msg.wParam);

        if (state == 1)
        {
            m_flipState = key;
            // When changing view states default to Original View first
            BackBuffer  = 1;
            m_curr_Node = 0;
        }
        else if (state == 2)
        {  // if maual flip is on this will toggle views
            if (m_flipState == RENDER_FLIP_MANUAL)
            {
                m_AllowImageFrameFlip = true;
                BackBuffer            = 1;
                m_curr_Node           = (key > 0) ? 1 : 0;
            }
        }
        return true;
    }
    case WM_COMMAND: {
        int show       = (int)(msg.lParam);
        UI.m_showimgui = show ? true : false;
        return true;
    }
    case WM_MOUSEWHEEL:
        m_mouseWheelDelta += (short)HIWORD(msg.wParam);
        return true;

    case WM_MOUSEMOVE:
        int Roll  = (signed short)(msg.lParam);
        int Pitch = (signed short)(msg.lParam >> 16);
        g_Roll    = Roll / 100.0f;
        g_Pitch   = Pitch / 100.0f;
        return true;
    }

    return true;
}

//--------------------------------------------------------------------------------------
//
// SetFullScreen
//
//--------------------------------------------------------------------------------------
void glTF_DX12DeviceEx::SetFullScreen(bool fullscreen)
{
#ifdef ENABLE_RENDER_CODE
    for (UINT i = 0; i < m_nodeCount; i++)
    {
        GPUFlush(m_pDevice, m_pDirectQueue[i]);
    }

    m_SwapChain.SetFullScreen(fullscreen);
#endif
}

//--------------------------------------------------------------------------------------
//
// OnResize
//
//--------------------------------------------------------------------------------------
void glTF_DX12DeviceEx::OnResize(DWORD width, DWORD height)
{
    m_Width  = width;
    m_Height = height;

#ifdef ENABLE_RENDER_CODE
    // resize all render views (This can be a single model or more as is the case for ImageDiff views)
    for (INT rep = 0; rep < m_max_Nodes_loaded; rep++)
    {
        for (UINT i = 0; i < m_nodeCount; i++)
        {
            GPUFlush(m_pDevice, m_pDirectQueue[i]);
        }

        for (UINT i = 0; i < m_nodeCount; i++)
            m_Node[rep][i].OnDestroyWindowSizeDependentResources();

        if (m_Width > 0 && m_Height > 0)
        {  // if not minimized
            m_SwapChain.OnResize(width, height);

            for (UINT i = 0; i < m_nodeCount; i++)
            {
                UINT node     = i + 1;
                UINT nodeMask = 1 << i;

                m_Node[rep][i].OnCreateWindowSizeDependentResources(m_pDevice, m_Width, m_Height, node, nodeMask);
            }
        }
    }

    m_state.camera.SetFov((XM_PI / 4) * 1.0f, m_Width, m_Height);
#endif
}

//--------------------------------------------------------------------------------------
//
// OnRender
//
//--------------------------------------------------------------------------------------

void glTF_DX12DeviceEx::processDiffRenderFlip()
{
    if (m_flipState == RENDER_FLIP_2FRAMES)
    {
        BackBuffer++;
        // Flip to new image if loaded after 2 buffer render calls
        if ((DWORD)BackBuffer > m_dwNumberOfBackBuffers)
        {
            BackBuffer = 1;
            m_curr_Node++;
            if (m_curr_Node >= m_max_Nodes_loaded)
                m_curr_Node = 0;
        }
    }
    else if (m_flipState == RENDER_FLIP_MANUAL)
    {
        if (m_AllowImageFrameFlip)
        {
            m_AllowImageFrameFlip = false;
            if (m_dwNumberOfBackBuffers > 1)
            {
                if (m_curr_Node >= m_max_Nodes_loaded)
                    m_curr_Node = 0;
            }
        }
    }
}

void glTF_DX12DeviceEx::OnRender()
{
    // Get timings
    //
    double timeNow  = MillisecondsNow();
    m_deltaTime     = timeNow - m_lastFrameTime;
    m_lastFrameTime = timeNow;

    static float distance        = m_gltfLoader[m_curr_Node]->m_distance;
    int          mouseWheelDelta = m_mouseWheelDelta - m_lastMouseWheelDelta;
    distance -= (float)(mouseWheelDelta / 200.0f) * distance;
    m_lastMouseWheelDelta = m_mouseWheelDelta;
    m_state.camera.UpdateCamera(g_Roll, g_Pitch, distance);

    if (UI.m_showimgui)
    {
        m_ImGuiRenderer.newFrame();
        {
            ImGui::Begin("Stats", NULL);  // ImGuiWindowFlags_ShowBorders);

            m_imgui_win_size = ImGui::GetWindowSize();
            m_imgui_win_pos  = ImGui::GetWindowPos();

            if (ImGui::CollapsingHeader("Info"), ImGuiTreeNodeFlags_DefaultOpen)
            {
                ImGui::Text("Resolution       : %ix%i", m_Width, m_Height);
                ImGui::Text("GPU node         : %i", m_SwapChain.GetCurrentNode());
                ImGui::Text("Total Indices    : %i", UI.m_TotalNumIndices);
                ImGui::Text("Total LoadTime   : %i ms", m_Node[m_curr_Node][m_SwapChain.GetCurrentNode()].m_TimeToLoadScene);
            }
            if (ImGui::CollapsingHeader("Profiler"), ImGuiTreeNodeFlags_DefaultOpen)
            {
                std::vector<TimeStamp> timeStamps = m_Node[m_curr_Node][m_SwapChain.GetCurrentNode()].GetTimingValues();
                if (timeStamps.size() > 0)
                {
                    for (unsigned int i = 1; i < timeStamps.size(); i++)
                    {
                        float DeltaTime = ((float)(timeStamps[i].m_milliseconds - timeStamps[i - 1].m_milliseconds));
                        ImGui::Text("%-17s: %7.1f us", timeStamps[i].m_label.c_str(), DeltaTime * 1000.0);
                    }

                    //scrolling data and average computing
                    static float values[128];
                    values[127]   = (float)(timeStamps.back().m_milliseconds - timeStamps.front().m_milliseconds);
                    float average = values[0];
                    for (int i = 0; i < 128 - 1; i++)
                    {
                        values[i] = values[i + 1];
                        average += values[i];
                    }
                    average /= 128;

                    ImGui::Text("%-17s: %7.1f us", "Total GPU Time", average * 1000.0);
                    // ImGui::PlotLines("", values, 128, 0, "", 0.0f, 30000.0f, ImVec2(0, 80));
                }
            }

            if (ImGui::CollapsingHeader("Animation"), ImGuiTreeNodeFlags_DefaultOpen)
            {
                ImGui::Checkbox("Play", &UI.m_playanimation);
                ImGui::SliderFloat("Time", &m_time, 0, 30);
            }

            if (ImGui::CollapsingHeader("Model Selection"), ImGuiTreeNodeFlags_DefaultOpen)
            {
                const char* tonemappers[] = {"Timothy", "DX11DSK", "Reinhard", "Uncharted2Tonemap", "No tonemapper"};
                ImGui::Combo("Tone Mapper", &m_state.toneMapper, tonemappers, 5);
                ImGui::SliderFloat("Exposure", &m_state.exposure, -1.0f, 2.0f);
                ImGui::SliderFloat("iblFactor", &m_state.iblFactor, 0.0f, 2.0f);
                ImGui::SliderFloat("Spot Light", &m_state.spotLightIntensity, 0.0f, 10.0f);
                ImGui::SliderFloat("Glow", &m_state.glow, 0.0f, 1.0f);
            }
            ImGui::Checkbox("Show Bounding Boxes", &m_state.bDrawBoundingBoxes);
            ImGui::Checkbox("Show Sky dome", &m_state.bDrawSkyDome);
            // ImGui::Checkbox("Show Gamma test pattern", &m_state.bGammaTestPattern);

            ImGui::End();
        }
        ImGui::EndFrame();
    }

    // Set animation
    //
    if (UI.m_playanimation)
    {
        m_time += (float)m_deltaTime / 1000.0f;
    }

    m_gltfLoader[m_curr_Node]->SetAnimationTime(0, m_time);

#ifdef ENABLE_RENDER_CODE
    // Do Render frame using AFR
    //
    m_Node[m_curr_Node][m_SwapChain.GetCurrentNode()].OnRender(
        &m_state, m_SwapChain.GetCurrentBackBufferResource(), m_SwapChain.GetCurrentBackBufferRTV(), &m_ImGuiRenderer, &UI);

    m_SwapChain.Present();
#endif

    processDiffRenderFlip();
}
