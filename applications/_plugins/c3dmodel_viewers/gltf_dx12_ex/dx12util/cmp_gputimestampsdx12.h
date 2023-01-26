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

// In DX12 timestamps are written by the GPU into a system memory resource.
// Its similar to a 'dynamic' buffer but this time it is the GPU who is writing
// and CPU is who is reading. Hence we need a sort of ring buffer to make sure
// we are reading from a chunk of the buffer that is not written to by the GPU
//
#include <vector>
#include <string>

// This class helps insert queries in the command buffer and readback the results.
// The tricky part in fact is reading back the results without stalling the GPU.
// For that it splits the readback heap in <numberOfBackBuffers> pieces and it reads
// from the last used chuck.

struct TimeStamp {
    std::string m_label;
    float       m_milliseconds;
};

class GPUTimestampsDX12 {
  public:
    void OnCreate(ID3D12Device* pDevice, DWORD numberOfBackBuffers, UINT node = 0, UINT nodemask = 0);
    void OnDestroy();

    void GetTimeStamp(ID3D12GraphicsCommandList *pCommandList, char *label);
    void CollectTimings(ID3D12GraphicsCommandList *pCommandList);

    void OnBeginFrame(UINT64 gpuFrequency, std::vector<TimeStamp> *pTimestamp);
    void OnEndFrame();

  private:
    const DWORD MaxValuesPerFrame = 128;
    ID3D12Resource    *m_pBuffer = NULL;
    ID3D12QueryHeap   *m_pQueryHeap = NULL;
    UINT m_index = 0;
    UINT m_frame = 0;
    UINT m_NumberOfBackBuffers = 0;

    std::vector<std::string> m_labels[5];
};

