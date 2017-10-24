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

#pragma once

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")

#include "AmdUtil\FrameworkWindows.h"
#include "AmdUtil\Camera.h"
#include "AmdUtil\SwapChainDX12.h"

#include "glTF_DX12Renderer.h"

#define MAX_NUM_OF_NODES    2

class glTF_DX12Device : public FrameworkWindows
{

public:
    PluginManager *m_pluginManager;
    void *m_msghandler;
    int m_FrameCount[MAX_NUM_OF_NODES] = { 0 };

    Camera camera;

    // pipeline objects
    int                      m_curr_Node        = 0;
    int                      m_max_Nodes_loaded = 1;
    ID3D12Device            *m_pDevice;

    UINT                     m_nodeCount;
    DWORD                    m_dwNumberOfBackBuffers;

    ID3D12CommandQueue     **m_pDirectQueue;
    glTF_DX12Renderer       *m_Node[MAX_NUM_OF_NODES];
    SwapChainDX12            m_SwapChain;
    GLTFLoader              *m_gltfLoader[MAX_NUM_OF_NODES];

    HWND                     m_hWnd;

    glTF_DX12Device(GLTFLoader m_gltfLoader[MAX_NUM_OF_NODES], DWORD width, DWORD height, void *pluginManager, void *msghandler);
    void OnCreate(HWND hWnd);

    void OnDestroy();
    void OnRender();
    bool OnEvent(MSG msg);
    void OnResize(DWORD Width, DWORD Height);

    void SetFullScreen(bool fullscreen);
    void ShowOptions(bool onshow);
    
private:

    float               m_roll;
    float               m_pitch;

    double              m_startingTime;       ///< The starting time of the previous frame.
    double              m_elapsedTime;        ///< The elapsed time since the previous frame.
};


