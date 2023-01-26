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

#include "cmp_staticconstantbufferpooldx12.h"

#include "cmp_error.h"


#include <d3dx12.h>

#define ALIGN(a) ((a + 255) & ~255)

void StaticConstantBufferPoolDX12::OnCreate(ID3D12Device* pDevice, DWORD totalMemSize, ResourceViewHeapsDX12 *pHeaps, DWORD cbvEntriesSize, bool bUseVidMem, UINT node, UINT nodemask) {
    m_pDevice = pDevice;

    m_totalMemSize = totalMemSize;
    m_memOffset = 0;
    m_pData = NULL;
    m_bUseVidMem = bUseVidMem;

    m_cbvEntriesSize = cbvEntriesSize;
    m_cbvOffset = 0;
    m_pCBVDesc = new D3D12_CONSTANT_BUFFER_VIEW_DESC[cbvEntriesSize];

    if (bUseVidMem) {
        ThrowIfFailed(
            pDevice->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT, node, nodemask),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(totalMemSize),
                D3D12_RESOURCE_STATE_COMMON,
                nullptr,
                IID_PPV_ARGS(&m_pVidMemBuffer))
        );
    }

    ThrowIfFailed(
        pDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD, node, nodemask),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(totalMemSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_pSysMemBuffer))
    );
    m_pSysMemBuffer->SetName(L"StaticBufferPoolDX12::OnCreate");

    CD3DX12_RANGE readRange(0, 0);
    m_pSysMemBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pData));
}

void StaticConstantBufferPoolDX12::OnDestroy() {
    if (m_bUseVidMem) {
        //releasing vid mem only, the upload heap should have been called when UploadData had been called
        m_pVidMemBuffer->Release();
    } else {
        CD3DX12_RANGE readRange(0, 0);
        m_pSysMemBuffer->Unmap(0, &readRange);
        m_pSysMemBuffer->Release();
    }

    delete(m_pCBVDesc);
}

bool StaticConstantBufferPoolDX12::AllocConstantBuffer(DWORD size, void **pData, DWORD *pIndex) {
    size = ALIGN(size);
    if (m_memOffset + size > m_totalMemSize) {
        char *msg = (char *) "Out of memory for Buffer Pool!";
        MessageBoxA(0, msg, "", 0);
        return false;
    }

    *pData = (void *)(m_pData + m_memOffset);

    m_pCBVDesc[m_cbvOffset].SizeInBytes = size;
    m_pCBVDesc[m_cbvOffset].BufferLocation = m_memOffset + ((m_bUseVidMem) ? m_pVidMemBuffer->GetGPUVirtualAddress() : m_pSysMemBuffer->GetGPUVirtualAddress());

    //returning an index allows to create more CBV for a constant buffer, this is useful when packing CBV into tables
    *pIndex = m_cbvOffset;

    m_memOffset += size;
    m_cbvOffset += 1;

    if (m_memOffset > m_totalMemSize) {
        char *msg = (char *) "Error: Buffer Pool memOffset > total memory";
        MessageBoxA(0, msg, "", 0);
    }
    if (m_cbvOffset > m_cbvEntriesSize) {
        char *msg = (char *) "Error: Buffer Pool cbvOffset > cbvEntriesSize";
        MessageBoxA(0, msg, "", 0);
    }

    return true;
}

bool StaticConstantBufferPoolDX12::CreateCBV(DWORD index, int srvOffset, CBV_SRV_UAV *pCBV) {
    m_pDevice->CreateConstantBufferView(&m_pCBVDesc[index], pCBV->GetCPU(srvOffset));
    return true;
}

void StaticConstantBufferPoolDX12::UploadData(ID3D12GraphicsCommandList *pCmdList) {
    if (m_bUseVidMem) {
        CD3DX12_RANGE readRange(0, 0);
        m_pSysMemBuffer->Unmap(0, &readRange);

        pCmdList->CopyResource(m_pVidMemBuffer, m_pSysMemBuffer);

        // With 'dynamic resources' we can use a same resource to hold Constant, Index and Vertex buffers.
        // That is because we dont need to use a transition.
        //
        // With static buffers though we need to transition them and we only have 2 options
        //      1) D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
        //      2) D3D12_RESOURCE_STATE_INDEX_BUFFER
        // Because we need to transition the whole buffer we cant have now Index buffers to share the
        // same resource with the Vertex or Constant buffers. Hence is why we need separate classes.
        // For Index and Vertex buffers we *could* use the same resource, but index buffers need their own resource.
        // Please note that in the interest of clarity vertex buffers and constant buffers have been split into two different classes though
        pCmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pVidMemBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
    }
}

void StaticConstantBufferPoolDX12::FreeUploadHeap() {
    m_pSysMemBuffer->Release();
}

