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
#include "glTF_DX12Device.h"
#include <DXGIDebug.h>
#pragma comment(lib, "dxgi.lib")
#include "AmdUtil\KeyboardMouse.h"
#include "AmdUtil\ImguiDX12.h"
#include "AmdUtil\gltf.h"
#include "AmdUtil\Misc.h"


glTF_DX12Device::glTF_DX12Device(GLTFLoader gltfLoader[MAX_NUM_OF_NODES], DWORD width, DWORD height, void *pluginManager, void *msghandler) : FrameworkWindows(width, height)
{
    m_pluginManager = (PluginManager*)pluginManager;
    m_msghandler    = msghandler;
    m_gltfLoader[0] = &gltfLoader[0];
    m_gltfLoader[1] = &gltfLoader[1];

    if (gltfLoader[1].m_filename.length() > 0)
    {
        m_max_Nodes_loaded = 2;
    }

    m_startingTime  = MillisecondsNow();
    kbmInit();
}

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void glTF_DX12Device::OnCreate(HWND hWnd)
{
    m_hWnd = hWnd;

#ifdef DEBUG
    // Enable the D3D12 debug layer.
    {
        ID3D12Debug *pDebugController;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController)));
        pDebugController->EnableDebugLayer();
        pDebugController->Release();
    }
#endif

    // Create device

    ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pDevice)));
    //m_pDevice->SetStablePowerState(TRUE);

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
    m_dwNumberOfBackBuffers = 2; // *m_max_Nodes_loaded;
    m_SwapChain.OnCreate(m_pDevice, m_dwNumberOfBackBuffers, &DirectQueues, m_Width, m_Height, hWnd);


    for (int curr_Node = 0; curr_Node < m_max_Nodes_loaded; curr_Node++)
    {

        // Create a instance of the renderer and initialize it, we need to do that for each GPU
        m_Node[curr_Node] = new glTF_DX12Renderer[m_nodeCount]();
        for (UINT i = 0; i < m_nodeCount; i++)
        {
            UINT node = i + 1;
            UINT nodeMask = 1 << i;

            m_Node[curr_Node][i].OnCreate(m_pDevice, m_pDirectQueue[i], node, nodeMask);
            m_Node[curr_Node][i].OnCreateWindowSizeDependentResources(m_pDevice, m_Width, m_Height, node, nodeMask);
        }

        // Load scene data from system memory into all the GPUs (done once per GPU)
        for (UINT i = 0; i < m_nodeCount; i++)
        {
            m_Node[curr_Node][i].LoadScene(m_gltfLoader[curr_Node], m_pluginManager, m_msghandler);
        }
    }

    // init GUI (non gfx stuff)
    ImGUIDX12_Init((void *)hWnd);

    // Init Camera, looking at origin
    m_roll  = 0.0f;
    m_pitch = 0.0f;
    camera.SetPosition(XMVectorSet(0.0f, 0.0f, -4.0f, 0.0f));
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void glTF_DX12Device::OnDestroy()
{
    ImGUIDX12_Shutdown();
    
    for (UINT i = 0; i < m_nodeCount; i++)
    {
        GPUFlush(m_pDevice, m_pDirectQueue[i]);
        m_pDirectQueue[i]->Release();
    }

    for (int curr_Node = 0; curr_Node < m_max_Nodes_loaded; curr_Node++)
    {

        for (UINT i = 0; i < m_nodeCount; i++)
        {
            m_Node[curr_Node][i].UnloadScene();
            m_Node[curr_Node][i].OnDestroyWindowSizeDependentResources();
            m_Node[curr_Node][i].OnDestroy();
        }

    }

    m_SwapChain.OnDestroy();
    m_pDevice->Release();

#ifdef DEBUG
    // // Report live objects
    // {
    //     IDXGIDebug1 *pDxgiDebug;
    //     if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDxgiDebug))))
    //     {
    //         pDxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
    //         pDxgiDebug->Release();
    //     }
    // }
#endif
}

//--------------------------------------------------------------------------------------
//
// OnEvent
//
//--------------------------------------------------------------------------------------
bool glTF_DX12Device::OnEvent(MSG msg)
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
void glTF_DX12Device::SetFullScreen(bool fullscreen)
{
    for (UINT i = 0; i<m_nodeCount; i++)
    {
        GPUFlush(m_pDevice, m_pDirectQueue[i]);
    }

    m_SwapChain.SetFullScreen(fullscreen);
}

//--------------------------------------------------------------------------------------
//
// OnResize
//
//--------------------------------------------------------------------------------------
void glTF_DX12Device::OnResize(DWORD width, DWORD height)
{
    m_Width = width;
    m_Height = height;

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

    camera.SetFov(XM_PI / 4, m_Width, m_Height);
}

void glTF_DX12Device::ShowOptions(bool onshow)
{
    m_Node[m_curr_Node]->ShowOptions(onshow);
}

//--------------------------------------------------------------------------------------
//
// OnRender
//
//--------------------------------------------------------------------------------------
void glTF_DX12Device::OnRender()
{
    static int BackBuffer = 1;

    // change size and positions basd on first node
    // Get timings
    double timeNow = MillisecondsNow();
    m_elapsedTime = timeNow - m_startingTime;
    m_startingTime = timeNow;

    // Get mouse/keyboard input and update camera

    POINT mouseDelta;
    int mouseButton;
    int mouseWheelDelta;
    kbmGetMouseDelta(&mouseDelta, &mouseWheelDelta, &mouseButton);


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

    // wasd camera type
    //camera.UpdateCamera(m_roll, m_pitch, kbmKeyStateArray(), m_elapsedTime);

    static float distance = 4.0f;
    distance -= (float)mouseWheelDelta / 240.0f;
    if (distance <= 1.0f)
        distance = 1.0f;
    camera.UpdateCamera(m_roll, m_pitch, distance);

    // Do Render frame
    m_Node[m_curr_Node][m_SwapChain.GetCurrentNode()].OnRender(m_FrameCount[m_curr_Node]++, m_elapsedTime,
                                                            m_SwapChain.GetCurrentBackBufferResource(),
                                                            m_SwapChain.GetCurrentBackBufferRTV(), &camera);

    m_SwapChain.Present();

    BackBuffer++;
    // Flip to new image if loaded after 2 buffer render calls
    if (BackBuffer > m_dwNumberOfBackBuffers)
    {
        BackBuffer = 1;
        m_curr_Node++;
        if (m_curr_Node >= m_max_Nodes_loaded) m_curr_Node = 0;
    }

}

