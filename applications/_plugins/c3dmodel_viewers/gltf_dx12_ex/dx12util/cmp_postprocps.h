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
#include "cmp_staticbufferpooldx12.h"

#include <windows.h>
#include <d3d12.h>

#include <string>

class PostProcPS {
  public:
    void OnCreate(
        ID3D12Device* pDevice,
        UINT node,
        std::string path,
        std::string string,
        DXGI_FORMAT outFormat,
        ResourceViewHeapsDX12 *pResourceViewHeaps,
        StaticBufferPoolDX12 *pStaticBufferPool,
        DWORD dwSRVTableSize,
        D3D12_BLEND_DESC *pBlendDesc = NULL,
        D3D12_DEPTH_STENCIL_DESC *pDepthStencilDesc = NULL,
        UINT sampleDescCount = 1
    );
    void OnDestroy();
    void Draw(ID3D12GraphicsCommandList* pCommandList, DWORD dwSRVTableSize, CBV_SRV_UAV *pSRVTable, D3D12_GPU_DESCRIPTOR_HANDLE constantBuffer);

  private:
    ResourceViewHeapsDX12       *m_pHeaps;
    ID3D12Device                *m_pDevice;

    D3D12_VERTEX_BUFFER_VIEW     verticesView;
    StaticBufferPoolDX12        *m_pStaticBufferPool;

    ResourceViewHeapsDX12       *m_pResourceViewHeaps;

    D3D12_GPU_DESCRIPTOR_HANDLE *m_pGpuDescriptor;

    ID3D12RootSignature         *m_pRootSignature;
    ID3D12PipelineState            *m_pPipelineRender;
    SAMPLER                     m_sampler;
    bool                        m_ready = false;
};


