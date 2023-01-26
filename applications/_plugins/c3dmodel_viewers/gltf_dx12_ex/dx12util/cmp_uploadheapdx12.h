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

#include <d3dx12.h>

//
// This class shows the most efficient way to upload resources to the GPU memory.
// The idea is to create just one upload heap and suballocate memory from it.
// For convenience this class comes with its own command list & submit (FlushAndFinish)
//

class UploadHeapDX12 {
  public:
    void OnCreate(ID3D12Device* pDevice, SIZE_T uSize, ID3D12CommandQueue* pCommandQueue, UINT node = 0, UINT nodemask = 0);
    void OnDestroy();

    UINT8* Suballocate(SIZE_T uSize, UINT64 uAlign);

    UINT8* BasePtr() {
        return m_pDataBegin;
    }
    ID3D12Resource* GetResource() {
        return m_pUploadHeap;
    }
    ID3D12GraphicsCommandList* GetCommandList() {
        return m_pCommandList;
    }

    UINT GetNode() {
        return m_node;
    }
    UINT GetNodeMask() {
        return m_nodemask;
    }

    void FlushAndFinish();

  private:

    ID3D12Resource                *m_pUploadHeap = nullptr;

    ID3D12GraphicsCommandList     *m_pCommandList = nullptr;
    ID3D12CommandQueue            *m_pCommandQueue = nullptr;
    ID3D12CommandAllocator        *m_pCommandAllocator = nullptr;

    UINT8* m_pDataCur = nullptr;      // current position of upload heap
    UINT8* m_pDataEnd = nullptr;      // ending position of upload heap
    UINT8* m_pDataBegin = nullptr;    // starting position of upload heap

    ID3D12Fence                   *m_pFence = nullptr;
    UINT64                         m_fenceValue = 0;
    HANDLE                         m_hEvent;

    UINT m_node = 0;
    UINT m_nodemask = 0;
};

