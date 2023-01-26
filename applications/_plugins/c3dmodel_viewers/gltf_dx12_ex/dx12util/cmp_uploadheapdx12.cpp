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


#include <assert.h>
#include "cmp_uploadheapdx12.h"
#include "cmp_misc.h"

#include "cmp_error.h"



//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void UploadHeapDX12::OnCreate(ID3D12Device* pDevice, SIZE_T uSize, ID3D12CommandQueue* pCommandQueue, UINT node, UINT nodemask) {
    m_pCommandQueue = pCommandQueue;
    m_node = node;
    m_nodemask = nodemask;

    // Create command list and allocators

    pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator));
    pDevice->CreateCommandList(nodemask, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, nullptr, IID_PPV_ARGS(&m_pCommandList));

    // Create buffer to suballocate

    ThrowIfFailed(
        pDevice->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD, node, nodemask),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(uSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_pUploadHeap)
        )
    );

    D3D12_RANGE range = {};
    range.Begin = 0;
    range.End = uSize;
    ThrowIfFailed(m_pUploadHeap->Map(0, &range, (void**)&m_pDataBegin));

    m_pDataCur = m_pDataBegin;
    m_pDataEnd = m_pDataBegin + m_pUploadHeap->GetDesc().Width;

    m_hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
    m_fenceValue = 0;
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void UploadHeapDX12::OnDestroy() {
    D3D12_RANGE range = {};
    m_pUploadHeap->Unmap(0, &range);

    m_pCommandList->Release();
    m_pCommandAllocator->Release();
    m_pUploadHeap->Release();
    m_pFence->Release();
}

//--------------------------------------------------------------------------------------
//
// SuballocateFromUploadHeap
//
//--------------------------------------------------------------------------------------
UINT8* UploadHeapDX12::Suballocate(SIZE_T uSize, UINT64 uAlign) {
    m_pDataCur = reinterpret_cast<UINT8*>(Align(reinterpret_cast<SIZE_T>(m_pDataCur), uAlign));

    // flush operations if we ran out of space in the heap

    if (m_pDataCur >= m_pDataEnd || m_pDataCur + uSize >= m_pDataEnd) {
        FlushAndFinish();
    }

    UINT8* pRet = m_pDataCur;
    m_pDataCur += uSize;
    return pRet;
}

//--------------------------------------------------------------------------------------
//
// FlushAndFinish
//
//--------------------------------------------------------------------------------------
void UploadHeapDX12::FlushAndFinish() {
    // Close & submit

    ThrowIfFailed(m_pCommandList->Close());
    m_pCommandQueue->ExecuteCommandLists(1, CommandListCast(&m_pCommandList));

    // Make sure it's been processed by the GPU

    m_pCommandQueue->Signal(m_pFence, ++m_fenceValue);
    m_pFence->SetEventOnCompletion(m_fenceValue, m_hEvent);
    WaitForSingleObject(m_hEvent, INFINITE);

    // Reset so it can be reused

    m_pCommandAllocator->Reset();
    m_pCommandList->Reset(m_pCommandAllocator, nullptr);

    m_pDataCur = m_pDataBegin;
}
