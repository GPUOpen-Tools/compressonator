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


#include "cmp_gputimestampsdx12.h"

#include "cmp_error.h"


void GPUTimestampsDX12::OnCreate(ID3D12Device* pDevice, DWORD numberOfBackBuffers, UINT node, UINT nodemask) {
    m_NumberOfBackBuffers = numberOfBackBuffers;

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

void GPUTimestampsDX12::OnDestroy() {
    m_pBuffer->Release();

    m_pQueryHeap->Release();
}

void GPUTimestampsDX12::GetTimeStamp(ID3D12GraphicsCommandList *pCommandList, char *label) {
    DWORD measurements = (DWORD)m_labels[m_frame].size();
    pCommandList->EndQuery(m_pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, m_frame*MaxValuesPerFrame + measurements);
    m_labels[m_frame].push_back(label);
}

void GPUTimestampsDX12::CollectTimings(ID3D12GraphicsCommandList *pCommandList) {
    DWORD measurements = (DWORD)m_labels[m_frame].size();

    pCommandList->ResolveQueryData(m_pQueryHeap, D3D12_QUERY_TYPE_TIMESTAMP, m_frame*MaxValuesPerFrame, measurements, m_pBuffer, m_frame * MaxValuesPerFrame);
}

void GPUTimestampsDX12::OnBeginFrame(UINT64 gpuFrequency, std::vector<TimeStamp> *pTimestamp) {
    pTimestamp->clear();

    double millisecondsPerTick = 1000.0 / (double)gpuFrequency;

    CD3DX12_RANGE readRange(m_frame * MaxValuesPerFrame, m_frame * MaxValuesPerFrame + m_labels[m_frame].size());
    UINT64 *pTimingsInTicks = NULL;
    m_pBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pTimingsInTicks));

    for (unsigned int i = 0; i < readRange.End - readRange.Begin; i++) {
        TimeStamp ts = { m_labels[m_frame][i], float(millisecondsPerTick * (double)(pTimingsInTicks[i] - pTimingsInTicks[0])) };
        pTimestamp->push_back(ts);
    }

    m_pBuffer->Unmap(0, NULL);

    m_labels[m_frame].clear();
}

void GPUTimestampsDX12::OnEndFrame() {
    m_frame = (m_frame + 1) % m_NumberOfBackBuffers;
}
