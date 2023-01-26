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

#include <vulkan/vulkan.h>

#include <cstdint>


// Forward Declaration
class CMP_DeviceVK;

// This class, on creation allocates a number of command lists. Using a ring buffer
// these commandLists are recycled when they are no longer used by the GPU. See the
// 'ring.h' for more details on allocation and recycling
//
class CommandListRingVK {
    std::uint32_t m_memTotalSize;
    RingWithTabs m_mem;

    CMP_DeviceVK* m_pDevice;

    VkCommandPool        m_commandPool;
    VkCommandBuffer      *m_pCommandBuffer;

  public:
    void             OnCreate(CMP_DeviceVK* pDevice, std::uint32_t numberOfBackBuffers, std::uint32_t memTotalSize);
    void OnDestroy();
    VkCommandBuffer *GetNewCommandList();
    void OnBeginFrame();
};

