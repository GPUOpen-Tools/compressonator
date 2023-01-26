// AMD AMDUtils code
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

#pragma once

#include "cmp_resourceviewheapsdx12.h"
#include "cmp_dynamicbufferringdx12.h"
#include "cmp_commandlistringdx12.h"
#include "cmp_uploadheapdx12.h"

#include "imgui.h"

// This is the rendering backend for the excellent ImGUI library.

class ImGUIDX12 {
  public:
    void OnCreate(ID3D12Device* pDevice, UploadHeapDX12 *pUploadHeap, ResourceViewHeapsDX12 *pHeaps, DynamicBufferRingDX12 *pConstantBufferRing, UINT node, UINT nodemask);
    void OnDestroy();

    void Draw(ID3D12GraphicsCommandList *pCmdLst);

  private:
    ID3D12Device              *m_pDevice;
    ResourceViewHeapsDX12     *m_pResourceViewHeaps;
    DynamicBufferRingDX12     *m_pConstBuf;

    ID3D12Resource            *m_pTexture2D;
    ID3D12PipelineState       *m_pPipelineState;
    ID3D12RootSignature       *m_pRootSignature;

    SAMPLER                    m_sampler;
    CBV_SRV_UAV                   m_pTextureSRV;

    UINT m_node;
    UINT m_nodeMask;
};


bool ImGUIDX12_Init(void* hwnd);
void ImGUIDX12_Shutdown();
void ImGUIDX12_UpdateIO();
LRESULT ImGUIDX12_WndProcHandler(MSG msg);