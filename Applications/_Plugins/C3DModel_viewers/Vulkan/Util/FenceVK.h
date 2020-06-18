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
#include "Error.h"

//
// Just a simple class that automatically increments the fence counter
// Please check the 'WaitForFence(UINT64 olderFence)' member, it does 
// show how to track how ahead the CPU is from the GPU. 
//
class FenceDX12
{
    UINT64 m_fenceCounter;
    HANDLE    m_hEvent;
    ID3D12Fence *m_pFence;

public:
    FenceDX12()
    {
        m_hEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    }

    ~FenceDX12()
    {
        CloseHandle(m_hEvent);
    }

    void OnCreate(ID3D12Device* pDevice)
    {
        m_fenceCounter = 0;
        ThrowIfFailed(pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)));
    }

    void OnDestroy()
    {
        m_pFence->Release();
    }

    void IssueFence(ID3D12CommandQueue* pCommandQueue)
    {
        m_fenceCounter++;
        ThrowIfFailed(pCommandQueue->Signal(m_pFence, m_fenceCounter));        
    }

    // This member is useful for tracking how ahead the CPU is from the GPU
    //
    // If the fence is used once per frame, calling this function with  
    // WaitForFence(3) will make sure the CPU is no more than 3 frames ahead
    //
    void WaitForFence(UINT64 olderFence)
    {
        if (m_fenceCounter > olderFence)
        {
            UINT64 valueToWaitFor = m_fenceCounter - olderFence;

            if (m_pFence->GetCompletedValue() <= valueToWaitFor)
            {
                ThrowIfFailed(m_pFence->SetEventOnCompletion(valueToWaitFor, m_hEvent));
                WaitForSingleObject(m_hEvent, INFINITE);
            }
        }
    }

    void WaitForFence(ID3D12CommandQueue* pCommandQueue)
    {
        ThrowIfFailed(pCommandQueue->Wait(m_pFence, m_fenceCounter));
    }
};


void GPUFlush(ID3D12Device *pDevice, ID3D12CommandQueue *pQueue);