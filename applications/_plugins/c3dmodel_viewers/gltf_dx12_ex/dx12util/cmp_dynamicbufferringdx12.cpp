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

#include "cmp_dynamicbufferringdx12.h"
#include "cmp_misc.h"

#include "cmp_error.h"

#include <d3dx12.h>

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void DynamicBufferRingDX12::OnCreate(ID3D12Device* pDevice, DWORD numberOfBackBuffers, DWORD memTotalSize, ResourceViewHeapsDX12 *pHeaps, DWORD srvTotalSize, UINT node, UINT nodemask) {
    m_pDevice = pDevice;

    memTotalSize = (DWORD)Align(memTotalSize,256);

    m_mem.OnCreate(numberOfBackBuffers, memTotalSize);
    m_srv.OnCreate(numberOfBackBuffers, srvTotalSize);


    pHeaps->AllocCBV_SRV_UAVDescriptor(srvTotalSize, &m_CBV);

    ThrowIfFailed(pDevice->CreateCommittedResource(
                      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD, node, nodemask),
                      D3D12_HEAP_FLAG_NONE,
                      &CD3DX12_RESOURCE_DESC::Buffer(memTotalSize),
                      D3D12_RESOURCE_STATE_GENERIC_READ,
                      nullptr,
                      IID_PPV_ARGS(&m_pBuffer)));
    m_pBuffer->SetName(L"DynamicBufferRingDX12::OnCreate");

    CD3DX12_RANGE readRange(0, 0);
    m_pBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pData));
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void DynamicBufferRingDX12::OnDestroy() {
    CD3DX12_RANGE readRange(0, 0);
    m_pBuffer->Unmap(0, &readRange);
    m_pBuffer->Release();
    m_mem.OnDestroy();
    m_srv.OnDestroy();
}

//--------------------------------------------------------------------------------------
//
// AllocConstantBuffer
//
//--------------------------------------------------------------------------------------
bool DynamicBufferRingDX12::AllocConstantBuffer(DWORD size, void **pData, D3D12_GPU_DESCRIPTOR_HANDLE *pGpuDescriptor) {
    size = (DWORD)Align(size, 256);

    DWORD memOffset;
    if (m_mem.Alloc(size, &memOffset) == false) {
        char *msg = "Ran out of mem for 'dynamic' ring buffers, please increase the allocated size";
        MessageBoxA(0, msg, "", 0);
        return false;
    }

    DWORD srvOffset;
    if (m_srv.Alloc(1, &srvOffset) == false) {
        char *msg = "Ran out of srv in the heap, please increase the heap size";
        MessageBoxA(0, msg, "", 0);
        return false;
    }

    *pData = (void *)(m_pData + memOffset);

    D3D12_CONSTANT_BUFFER_VIEW_DESC constBufViewDesc = {};
    constBufViewDesc.SizeInBytes = size;
    constBufViewDesc.BufferLocation = m_pBuffer->GetGPUVirtualAddress() + memOffset;

    m_pDevice->CreateConstantBufferView(&constBufViewDesc, m_CBV.GetCPU(srvOffset));

    *pGpuDescriptor = m_CBV.GetGPU(srvOffset);

    return true;
}

//--------------------------------------------------------------------------------------
//
// AllocVertexBuffer
//
//--------------------------------------------------------------------------------------
bool DynamicBufferRingDX12::AllocVertexBuffer(DWORD numbeOfVertices, UINT strideInBytes, void **pData, D3D12_VERTEX_BUFFER_VIEW *pView) {
    DWORD size = (DWORD)Align(numbeOfVertices * strideInBytes, 256);

    DWORD memOffset;
    if (m_mem.Alloc(size, &memOffset) == false)
        return false;

    *pData = (void *)(m_pData + memOffset);


    pView->BufferLocation = m_pBuffer->GetGPUVirtualAddress() + memOffset;
    pView->StrideInBytes = strideInBytes;
    pView->SizeInBytes = size;

    return true;
}

bool DynamicBufferRingDX12::AllocIndexBuffer(DWORD numbeOfIndices, UINT strideInBytes, void **pData, D3D12_INDEX_BUFFER_VIEW *pView) {
    DWORD size = (DWORD)Align(numbeOfIndices*strideInBytes, 256);

    DWORD memOffset;
    if (m_mem.Alloc(size, &memOffset) == false)
        return false;

    *pData = (void *)(m_pData + memOffset);

    pView->BufferLocation = m_pBuffer->GetGPUVirtualAddress() + memOffset;
    pView->Format = (strideInBytes == 4) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
    pView->SizeInBytes = size;

    return true;
}

//--------------------------------------------------------------------------------------
//
// OnBeginFrame
//
//--------------------------------------------------------------------------------------
void DynamicBufferRingDX12::OnBeginFrame() {
    m_mem.OnBeginFrame();
    m_srv.OnBeginFrame();
}
