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
#include "cmp_ringvk.h"
#include "cmp_devicevk.h"

// Simulates DX11 style static buffers. For dynamic buffers please see 'DynamicBufferRingDX12.h'
//
// This class allows suballocating small chuncks of memory from a huge buffer that is allocated on creation
// This class is specialized in constant buffers.
//
class StaticConstantBufferPoolVK {
    CMP_DeviceVK* m_pDevice;

    RingWithTabs m_mem;

    VkBuffer                m_buffer;
    VkDeviceMemory          m_deviceMemory;

    std::uint32_t           m_totalMemSize;
    std::uint32_t           m_memOffset;
    char            *m_pData;

    bool            m_bUseVidMem;

  public:
    void OnCreate(CMP_DeviceVK* pDevice, std::uint32_t totalMemSize);
    void OnDestroy();
    bool AllocConstantBuffer(std::uint32_t size, void **pData, VkDescriptorBufferInfo *pOut);
    void UploadData(VkCommandBuffer cmd_buf);
    void FreeUploadHeap();

};


