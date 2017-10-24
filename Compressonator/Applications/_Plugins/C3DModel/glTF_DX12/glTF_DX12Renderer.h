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
#include <D3D12.h>
#include "AmdUtil\UploadHeapDX12.h"
#include "AmdUtil\Texture.h"
#include "AmdUtil\DynamicBufferRingDX12.h"
#include "AmdUtil\StaticBufferPoolDX12.h"
#include "AmdUtil\StaticConstantBufferPoolDX12.h"
#include "AmdUtil\CommandListRingDX12.h"
#include "AmdUtil\FenceDX12.h"
#include "AmdUtil\ResourceViewHeapsDX12.h"
#include "AmdUtil\Camera.h"
#include "AmdUtil\GPUTimerDX12.h"
#include "AmdUtil\ImguiDX12.h"
#include "AmdUtil\gltf.h"

static const int cNumSwapBufs = 2;

#define USE_VID_MEM true

class glTF_DX12Renderer
{
public:

    ID3D12Device                    *m_pDevice;
    ID3D12CommandQueue              *m_pDirectQueue;

    Texture                         m_pDepthBuffer;
    DSV                             m_DepthBufferDSV;

    // Initialize helper classes
    ResourceViewHeapsDX12           m_Heaps;
    UploadHeapDX12                  m_UploadHeap;
    FenceDX12                       m_FrameFence;
    DynamicBufferRingDX12           m_ConstantBufferRing;
    StaticBufferPoolDX12            m_StaticBufferPool;
    StaticConstantBufferPoolDX12    m_StaticConstantBufferPool;
    CommandListRingDX12             m_CommandListRing;
    GPUTimerDX12                    m_GPUTimer;

    bool                            m_UseImGUI = false;
    ImGUIDX12                       m_ImGUI;

    GlTF                            *m_gltf;

    DWORD m_Width, m_Height;
    D3D12_VIEWPORT mViewPort;
    D3D12_RECT mRectScissor;

    void ShowOptions(bool doshow);

    void OnCreate(ID3D12Device* pDevice, ID3D12CommandQueue* pDirectQueue, UINT node, UINT nodemask);
    void OnDestroy();
    
    void OnCreateWindowSizeDependentResources(ID3D12Device* pDevice, DWORD Width, DWORD Height, UINT node, UINT nodemask);
    void OnDestroyWindowSizeDependentResources();

    void LoadScene(GLTFLoader *gltfData, void *pluginManager, void *msghandler);
    void UnloadScene();

    void OnRender(int mIndexLastSwapBuf, double m_elapsedTime, ID3D12Resource* pRenderTarget, D3D12_CPU_DESCRIPTOR_HANDLE *pRenderTargetSRV, Camera *pCam);
};

