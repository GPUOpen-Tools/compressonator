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

#include "cmp_frameworkwindows.h"
#include "cmp_camera.h"
#include "cmp_swapchaindx12.h"

#include "gltf_dx12rendererex.h"

#include <imgui_dx12.h>
#include "imgui.h"

#include "userinterface.h"
#include "pluginmanager.h"

#define MAX_NUM_OF_NODES 2

#define RENDER_FLIP_2FRAMES 0
#define RENDER_FLIP_MANUAL 1

class glTF_DX12RenderEx;
class CMP_GLTFCommon;

extern CMIPS* DX12_CMips;
//
// This is the main class, it manages the state of the sample and does all the high level work without touching the GPU directly.
// This class uses the GPU via the the SampleRenderer class. We would have a SampleRenderer instance for each GPU.
//
// This class takes care of:
//
//    - loading a scene (just the CPU data)
//    - updating the camera
//    - keeping track of time
//    - handling the keyboard
//    - updating the animation
//    - building the UI (but do not renders it)
//    - uses the SampleRenderer to update all the state to the GPU and do the rendering
//

// *1* Minimized FrameworkWindows
class glTF_DX12DeviceEx : public FrameworkWindows, public QWidget
{
public:
    int  OnCreate(HWND hWnd);
    void OnDestroy();
    void OnRender();
    bool OnEvent(MSG msg);
    void OnResize(DWORD Width, DWORD Height);
    void SetFullScreen(bool fullscreen);

    // *1* marks changes in code from AMDUtils
    HWND           m_hWnd;
    PluginManager* m_pluginManager;
    int            m_curr_Node        = 0;
    int            m_max_Nodes_loaded = 1;
    void*          m_msghandler;
    DWORD          m_dwNumberOfBackBuffers;

    glTF_DX12DeviceEx(CMP_GLTFCommon m_gltfLoader[MAX_NUM_OF_NODES], DWORD width, DWORD height, void* pluginManager, void* msghandler, QWidget* parent);

private:
    void processDiffRenderFlip();

    QWidget* m_parent;

    // imGUI
    ImVec2                     m_imgui_win_size;
    ImVec2                     m_imgui_win_pos;
    ImGuiRenderer_DX12         m_ImGuiRenderer;
    QImGUI_WindowWrapper_DX12* m_window;

    // User IO interface settings
    UserInterface UI;

    // pipeline objects
    ID3D12Device* m_pDevice;
    SwapChainDX12 m_SwapChain;

    UINT                 m_nodeCount;
    ID3D12CommandQueue** m_pDirectQueue;

    // *1* change to MAX_NUM_OF_NODES
    CMP_GLTFCommon* m_gltfLoader[MAX_NUM_OF_NODES];

#ifdef ENABLE_RENDER_CODE
    glTF_DX12RendererEx* m_Node[MAX_NUM_OF_NODES];
    ;
    glTF_DX12RendererEx::State m_state;
#endif

    int mouseWheelDelta;
    int m_mouse_press_xpos, m_mouse_press_ypos;

    float m_roll;
    float m_pitch;

    float  m_time;       // The elapsed time since the previous frame.
    double m_deltaTime;  // The elapsed time since the previous frame.
    double m_lastFrameTime;

    float m_TimeSinceLastFlip;
    bool  m_AllowImageFrameFlip;
    int   m_flipState;  // 0: after 2 frames, 1: manual, 2: After a timeout
};
