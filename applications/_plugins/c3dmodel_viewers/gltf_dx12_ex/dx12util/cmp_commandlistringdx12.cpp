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

#include "cmp_commandlistringdx12.h"

#include "cmp_error.h"


//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void CommandListRingDX12::OnCreate(ID3D12Device* pDevice, ID3D12CommandQueue *pQueue, DWORD numberOfBackBuffers, DWORD memTotalSize, UINT nodeMask) {
    m_memTotalSize = memTotalSize;

    m_mem.OnCreate(numberOfBackBuffers, memTotalSize);

    m_ppCommandList = new ID3D12GraphicsCommandList*[memTotalSize];
    m_ppCommandAllocator = new ID3D12CommandAllocator*[memTotalSize];

    D3D12_COMMAND_QUEUE_DESC queueDesc = pQueue->GetDesc();

    // Closing all the command buffers so we can call reset on them the first time we use them, otherwise the runtime would show a warning.
    for (UINT i = 0; i < memTotalSize; i++) {
        ThrowIfFailed(pDevice->CreateCommandAllocator(queueDesc.Type, IID_PPV_ARGS(&m_ppCommandAllocator[i])));
        ThrowIfFailed(pDevice->CreateCommandList(nodeMask, queueDesc.Type, m_ppCommandAllocator[i], nullptr, IID_PPV_ARGS(&m_ppCommandList[i])));

        m_ppCommandList[i]->Close();
    }

    pQueue->ExecuteCommandLists(memTotalSize, (ID3D12CommandList *const *)m_ppCommandList);

}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void CommandListRingDX12::OnDestroy() {
    for (UINT i = 0; i < m_memTotalSize; i++) {
        m_ppCommandList[i]->Release();
        m_ppCommandAllocator[i]->Release();
    }

    delete [] m_ppCommandList;
    delete [] m_ppCommandAllocator;
}

//--------------------------------------------------------------------------------------
//
// GetNewCommandList
//
//--------------------------------------------------------------------------------------
ID3D12GraphicsCommandList *CommandListRingDX12::GetNewCommandList() {
    DWORD index;
    if (m_mem.Alloc(1, &index) == false)
        return NULL;

    ThrowIfFailed(m_ppCommandAllocator[index]->Reset());
    ThrowIfFailed(m_ppCommandList[index]->Reset(m_ppCommandAllocator[index], nullptr));

    return m_ppCommandList[index];
}

//--------------------------------------------------------------------------------------
//
// OnBeginFrame
//
//--------------------------------------------------------------------------------------
void CommandListRingDX12::OnBeginFrame() {
    m_mem.OnBeginFrame();
}