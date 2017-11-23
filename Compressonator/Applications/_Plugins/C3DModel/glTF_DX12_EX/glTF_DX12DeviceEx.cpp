// AMD SampleDX12 sample code
// 
// Copyright(c) 2017 Advanced Micro Devices, Inc.All rights reserved.
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

#include "stdafx.h"
#include "GltfFeatures.h"

#include "glTF_DX12DeviceEx.h"
#include <DXGIDebug.h>
#include "KeyboardMouse.h"
#include "ImguiDX12.h"
#include "GltfCommon.h"
#include "Misc.h"


#ifdef ENABLE_RENDER_CODE
#include "glTF_DX12RendererEx.h"
#endif


glTF_DX12DeviceEx::glTF_DX12DeviceEx(GLTFCommon gltfLoader[MAX_NUM_OF_NODES], DWORD width, DWORD height, void *pluginManager, void *msghandler) : FrameworkWindows(width, height)
{
    m_pluginManager = (PluginManager*)pluginManager;
    m_msghandler = msghandler;
    m_gltfLoader[0] = &gltfLoader[0];
    m_gltfLoader[1] = &gltfLoader[1];

    if (gltfLoader[1].m_filename.length() > 0)
    {
        m_max_Nodes_loaded = 2;
    }

    m_lastFrameTime = MillisecondsNow();

#ifdef USE_ANIMATION
    m_time          = 0;
    m_bPlay         = true;
#endif
}

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void glTF_DX12DeviceEx::OnCreate(HWND hWnd)
{
#ifdef ENABLE_RENDER_CODE
    // Create device
    //
    ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice)));

    // Get the number of GPU's in the system
    m_nodeCount = m_pDevice->GetNodeCount();

    // Create Queues, in MGPU mode we need one per node/GPU 
    m_pDirectQueue = new ID3D12CommandQueue*[m_nodeCount];
    std::vector<ID3D12CommandQueue *> DirectQueues;
    for (UINT i = 0; i < m_nodeCount; i++)
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.NodeMask = 1 << i;
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
            UINT node = i + 1;
            UINT nodeMask = 1 << i;

            m_Node[curr_Node][i].OnCreate(m_pDevice, m_pDirectQueue[i], node, nodeMask, m_pluginManager, m_msghandler);
            m_Node[curr_Node][i].OnCreateWindowSizeDependentResources(m_pDevice, m_Width, m_Height, node, nodeMask);
        }

        // Load scene data from system memory into all the GPUs (done once per GPU)
        for (UINT i = 0; i < m_nodeCount; i++)
        {
            m_Node[curr_Node][i].LoadScene(m_gltfLoader[curr_Node], m_pluginManager, m_msghandler);
        }
    }

#endif

#ifdef USE_IMGUI
    // init GUI (non gfx stuff)
    if (m_Node[m_curr_Node]->m_UseImGUI)
                    ImGUIDX12_Init((void *)hWnd);
#endif

    // Init Camera, looking at origin
    m_roll = 0.0f;
    m_pitch = 0.0f;

#ifdef ENABLE_RENDER_CODE
    m_state.depthBias           = 70.0f / 100000.0f;
    m_state.exposure = 0.857f;
    m_state.iblFactor = 0.237f;
    m_state.spotLightIntensity  = 3.690f;
    m_state.toneMapper = 0;
    m_state.glow = 0.1f;
    m_state.bDrawBoundingBoxes = false;
    m_state.light.SetFov(XM_PI / 2, 1024, 1024);
    m_state.bGammaTestPattern = false;
    m_state.bDrawSkyDome = false;

    m_state.light.UpdateCamera(3.67f + 3.14159f, 0.58f, 3.0f);
#endif
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void glTF_DX12DeviceEx::OnDestroy()
{
#ifdef USE_IMGUI
    if (m_Node[m_curr_Node]->m_UseImGUI)
                        ImGUIDX12_Shutdown();
#endif

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
bool glTF_DX12DeviceEx::OnEvent(MSG msg)
{
    if (ImGUIDX12_WndProcHandler(msg))
            return true;

    return kbmOnEvent(msg);
}

//--------------------------------------------------------------------------------------
//
// SetFullScreen
//
//--------------------------------------------------------------------------------------
void glTF_DX12DeviceEx::SetFullScreen(bool fullscreen)
{
#ifdef ENABLE_RENDER_CODE
    for (UINT i = 0; i<m_nodeCount; i++)
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
    m_Width = width;
    m_Height = height;

#ifdef ENABLE_RENDER_CODE
    for (UINT i = 0; i < m_nodeCount; i++)
    {
        GPUFlush(m_pDevice, m_pDirectQueue[i]);
    }

    for (UINT i = 0; i < m_nodeCount; i++)
        m_Node[m_curr_Node][i].OnDestroyWindowSizeDependentResources();

    if (m_Width > 0 && m_Height > 0)  // if not minimized
    {
        m_SwapChain.OnResize(width, height);

        for (UINT i = 0; i < m_nodeCount; i++)
        {
            UINT node = i + 1;
            UINT nodeMask = 1 << i;

            m_Node[m_curr_Node][i].OnCreateWindowSizeDependentResources(m_pDevice, m_Width, m_Height, node, nodeMask);
        }
    }

    m_state.camera.SetFov((XM_PI / 4)*1.0f, m_Width, m_Height);
#endif
}

// *1* 
void glTF_DX12DeviceEx::ShowOptions(bool onshow)
{
#ifdef ENABLE_RENDER_CODE
    m_Node[m_curr_Node]->ShowOptions(onshow);
#endif
}

//--------------------------------------------------------------------------------------
//
// OnRender
//
//--------------------------------------------------------------------------------------
void glTF_DX12DeviceEx::OnRender()
{
    static int BackBuffer = 1;

    // Get timings
    //
    double timeNow = MillisecondsNow();
    m_deltaTime = timeNow - m_lastFrameTime;
    m_lastFrameTime = timeNow;

    // Get mouse/keyboard input and update camera

    POINT mouseDelta;
    int mouseButton;
    int mouseWheelDelta;
    kbmGetMouseDelta(&mouseDelta, &mouseWheelDelta, &mouseButton);

    // *1*
    if ((mouseButton & MOUSE_RIGHT_BUTTON) != 0)
    {
        m_roll = 0.0f;
        m_pitch = 0.0f;
    }
    else
        if ((mouseButton & MOUSE_LEFT_BUTTON) != 0)
        {
            m_roll = mouseDelta.x / 100.f;
            m_pitch = mouseDelta.y / 100.f;
        }

#ifdef USE_IMGUI
    // If mouse was not used by the GUI then it's for the camera
    //
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse == false)
    {
#endif


#ifdef ENABLE_RENDER_CODE
        static float distance = m_gltfLoader[m_curr_Node]->m_distance;
        distance -= (float)(mouseWheelDelta / 200.0f) * distance;
        // m_gltfLoader[m_curr_Node]->m_distance -= (float)(mouseWheelDelta / 200.0f) * m_gltfLoader[m_curr_Node]->m_distance;
        m_state.camera.UpdateCamera(m_roll, m_pitch, distance);
#endif

#ifdef USE_IMGUI
    }
#endif

#ifdef USE_IMGUI
    // Build UI and set the scene state. Note that the rendering of the UI happens later.
    //
    if (m_Node[m_curr_Node]->m_UseImGUI)
        ImGUIDX12_UpdateIO();

    if (m_Node[m_curr_Node]->m_UseImGUI)
    {
        ImGui::NewFrame();

        bool opened = true;
        ImGui::Begin("Stats", &opened, ImGuiWindowFlags_ShowBorders);

        if (ImGui::CollapsingHeader("Info"), ImGuiTreeNodeFlags_DefaultOpen)
        {
            ImGui::Text("Resolution       : %ix%i", m_Width, m_Height);
            ImGui::Text("GPU node         : %i", m_SwapChain.GetCurrentNode());
        }

        if (ImGui::CollapsingHeader("Profiler"), ImGuiTreeNodeFlags_DefaultOpen)
        {
#ifdef ENABLE_RENDER_CODE
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
                values[127] = (float)(timeStamps.back().m_milliseconds - timeStamps.front().m_milliseconds);
                float average = values[0];
                for (int i = 0; i < 128 - 1; i++) { values[i] = values[i + 1]; average += values[i]; }
                average /= 128;

                ImGui::Text("%-17s: %7.1f us", "TotalGPUTime", average * 1000.0);
                ImGui::PlotLines("", values, 128, 0, "", 0.0f, 30000.0f, ImVec2(0, 80));
            }
#endif
        }

#ifdef USE_ANIMATION
        if (ImGui::CollapsingHeader("Animation"), ImGuiTreeNodeFlags_DefaultOpen)
        {
            ImGui::Checkbox("Play", &m_bPlay);
            ImGui::SliderFloat("Time", &m_time, 0, 30);
        }      
#endif

#ifdef ENABLE_RENDER_CODE
        if (ImGui::CollapsingHeader("Model Selection"), ImGuiTreeNodeFlags_DefaultOpen)
        {
        // const char * models[] = {"busterDrone", "BoomBox", "SciFiHelmet", "DamagedHelmet","sk_BoonBox","sk_BusterDrone","sk_microphone"};
        // static int selected = 0;
        // if (ImGui::Combo("model", &selected, models, 7))
        // {                
        //     m_gltfLoader->Unload();
        //
        //     switch (selected)
        //     {
        //         case 0: m_gltfLoader->Load("..\\media\\buster_drone\\", "busterDrone.gltf"); break;
        //         case 2: m_gltfLoader->Load("..\\media\\SciFiHelmet\\glTF\\", "SciFiHelmet.gltf"); break;
        //         case 3: m_gltfLoader->Load("..\\media\\DamagedHelmet\\glTF\\", "DamagedHelmet.gltf"); break;
        //         case 4: m_gltfLoader->Load("..\\media\\Sketchfab_BoomBox\\glTF\\", "BoomBox.gltf"); break;
        //         case 5: m_gltfLoader->Load("..\\media\\Sketchfab_busterDrone\\", "busterDrone.gltf"); break;
        //         case 6: m_gltfLoader->Load("..\\media\\Sketchfab_microphone\\", "microphone.gltf"); break;
        //     }
        //
        //     //free resources, unload the current scene, and load new scene...
        //     for (UINT i = 0; i < m_nodeCount; i++)
        //     {
        //         GPUFlush(m_pDevice, m_pDirectQueue[i]);
        //         
        //         m_Node[i].UnloadScene();
        //         m_Node[i].OnDestroyWindowSizeDependentResources();
        //         m_Node[i].OnDestroy();
        //
        //         UINT node = i + 1;
        //         UINT nodeMask = 1 << i;
        //
        //         m_Node[i].OnCreate(m_pDevice, m_pDirectQueue[i], node, nodeMask);
        //         m_Node[i].OnCreateWindowSizeDependentResources(m_pDevice, m_Width, m_Height, node, nodeMask);
        //         m_Node[i].LoadScene(m_gltfLoader);
        //     }
        //
        //     return;
        // }

            const char * tonemappers[] = { "Timothy", "DX11DSK", "Reinhard", "Uncharted2Tonemap", "No tonemapper" };
            ImGui::Combo("tone mapper", &m_state.toneMapper, tonemappers, 5);
            ImGui::SliderFloat("exposure", &m_state.exposure, -1.0f, 2.0f);

            ImGui::SliderFloat("iblFactor", &m_state.iblFactor, 0.0f, 2.0f);
            ImGui::SliderFloat("spotLightIntensity", &m_state.spotLightIntensity, 0.0f, 10.0f);
            ImGui::SliderFloat("glow", &m_state.glow, 0.0f, 1.0f);
        }


        ImGui::Checkbox("Show Bounding Boxes", &m_state.bDrawBoundingBoxes);
        ImGui::Checkbox("Show Sky dome", &m_state.bDrawSkyDome);
        ImGui::Checkbox("Show Gamma test pattern", &m_state.bGammaTestPattern);
#endif

        ImGui::End();
    }
#endif

#ifdef USE_ANIMATION
    // Set animation
    //
    if (m_bPlay)
    {
        m_time += (float)m_deltaTime / 1000.0f;
    }    

    m_gltfLoader[m_curr_Node]->SetAnimationTime(0, m_time);
#endif


#ifdef ENABLE_RENDER_CODE
    // Do Render frame using AFR 
    //
    m_Node[m_curr_Node][m_SwapChain.GetCurrentNode()].OnRender(&m_state, 
                                                                m_SwapChain.GetCurrentBackBufferResource(), 
                                                                m_SwapChain.GetCurrentBackBufferRTV());

    m_SwapChain.Present();
#endif

    BackBuffer++;
    // Flip to new image if loaded after 2 buffer render calls
    if ((DWORD) BackBuffer > m_dwNumberOfBackBuffers)
    {
        BackBuffer = 1;
        m_curr_Node++;
        if (m_curr_Node >= m_max_Nodes_loaded) m_curr_Node = 0;
    }
}

