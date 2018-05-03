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

#include "stdafx.h"
#include "GPUTimerDX12.h"



void GPUTimerDX12::OnCreate(ID3D12Device* pDevice, DWORD numberOfBackBuffers, UINT node, UINT nodemask)
{
    m_NumberOfBackBuffers = numberOfBackBuffers;

    for (DWORD i = 0; i<numberOfBackBuffers; i++)
        m_measurements[i] = 0;

    D3D12_QUERY_HEAP_DESC queryHeapDesc = {};
    queryHeapDesc.Count = MaxValuesPerFrame * numberOfBackBuffers;
    queryHeapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
    queryHeapDesc.NodeMask = nodemask;
    ThrowIfFailed(pDevice->CreateQueryHeap(&queryHeapDesc, IID_PPV_ARGS(&m_pQueryHeap)));

    ThrowIfFailed(
        pDevice->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK, node, nodemask),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(sizeof(uint64_t) * numberOfBackBuffers * MaxValuesPerFrame),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_pBuffer))
    );

    //pDevice->SetStablePowerState(true);

}

void GPUTimerDX12::OnDestroy()
{
    m_pBuffer->Release();

    m_pQueryHeap->Release();
}

void GPUTimerDX12::GetTimeStamp(ID3D12GraphicsCommandList *pCommandList)
{
    pCommandList->EndQuery(m_pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, m_frame*MaxValuesPerFrame + m_measurements[m_frame]);
    m_measurements[m_frame]++;
}

void GPUTimerDX12::CollectTimings(ID3D12GraphicsCommandList *pCommandList)
{
    //pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pBuffer, D3D12_RESOURCE_STATE_PREDICATION, D3D12_RESOURCE_STATE_COPY_DEST));
    pCommandList->ResolveQueryData(m_pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, m_frame*MaxValuesPerFrame, m_measurements[m_frame], m_pBuffer, m_frame * MaxValuesPerFrame);
    //pCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PREDICATION));
}

void GPUTimerDX12::OnBeginFrame(UINT64 *pData, DWORD *pCount)
{
    *pCount = m_measurements[m_frame];
    
    CD3DX12_RANGE readRange(0, 0);
    m_pBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pData));

    memcpy(pData, &m_pData[m_frame * MaxValuesPerFrame], *pCount * sizeof(uint64_t));

    m_pBuffer->Unmap(0, &readRange);
    
    m_measurements[m_frame] = 0;
}

void GPUTimerDX12::OnEndFrame()
{
    m_frame = (m_frame + 1) % m_NumberOfBackBuffers;
}
