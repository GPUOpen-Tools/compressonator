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
#include "cmp_ring.h"
#include "cmp_resourceviewheapsdx12.h"

// This class mimics the behaviour or the DX11 dynamic buffers.
// It does so by suballocating memory from a huge buffer. The buffer is used in a ring fashion.
// Allocated memory is taken from the tail, freed memory makes the head advance;
// See 'ring.h' to get more details on the ring buffer.
//
// The class knows when to free memory by just knowing:
//    1) the amount of memory used per frame
//    2) the number of backbuffers
//    3) When a new frame just started ( indicated by OnBeginFrame() )
//         - This will free the data of the oldest frame so it can be reused for the new frame

class DynamicBufferRingDX12 {
  public:
    void OnCreate(ID3D12Device* pDevice, DWORD numberOfBackBuffers, DWORD memTotalSize, ResourceViewHeapsDX12 *pHeaps, DWORD srvTotalSize, UINT node = 0, UINT nodemask = 0);
    void OnDestroy();

    bool AllocIndexBuffer(DWORD numbeOfIndices, UINT strideInBytes, void **pData, D3D12_INDEX_BUFFER_VIEW *pView);
    bool AllocVertexBuffer(DWORD numbeOfVertices, UINT strideInBytes, void **pData, D3D12_VERTEX_BUFFER_VIEW *pView);
    bool AllocConstantBuffer(DWORD size, void **pData, D3D12_GPU_DESCRIPTOR_HANDLE *pGpuDescriptor);

    void OnBeginFrame();

  private:

    RingWithTabs m_mem;
    RingWithTabs m_srv;

    char *m_pData;
    CBV_SRV_UAV m_CBV;

    ID3D12Device* m_pDevice;
    ID3D12Resource *m_pBuffer;
};
