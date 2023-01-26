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
#include "cmp_dynamicbufferringdx12.h"
#include "cmp_staticbufferpooldx12.h"
#include "cmp_uploadheapdx12.h"
#include "cmp_camera.h"

#include <wrl.h>

class DrawCube {
    struct per_object {
        DirectX::XMMATRIX mMatrix;
        DirectX::XMVECTOR vCenter;
        DirectX::XMVECTOR vRadius;
        DirectX::XMVECTOR vColor;
    };


    // all bounding boxes of all the meshes use the same geometry, shaders and pipelines.
    UINT m_NumIndices;
    D3D12_INDEX_BUFFER_VIEW m_IBV;
    D3D12_VERTEX_BUFFER_VIEW m_VBV;
    Microsoft::WRL::ComPtr<ID3D12RootSignature>    m_RootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>    m_PipelineRender;

    DynamicBufferRingDX12 *m_pDynamicBufferRing;
    ResourceViewHeapsDX12 *m_pResourceViewHeaps;

  public:
    DrawCube();

    ~DrawCube();

    void OnCreate(ID3D12Device* pDevice, UINT node, UploadHeapDX12* pUploadHeap, ResourceViewHeapsDX12 *pHeaps, DynamicBufferRingDX12 *pDynamicBufferRing, StaticBufferPoolDX12 *pStaticBufferPool);
    void Draw(ID3D12GraphicsCommandList* pCommandList, DWORD dwCount, per_object *pMatrixList);
};

