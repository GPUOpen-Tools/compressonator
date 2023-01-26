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


#include "cmp_commandlistringvk.h"

#include "cmp_devicevk.h"

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void CommandListRingVK::OnCreate(CMP_DeviceVK* pDevice, std::uint32_t numberOfBackBuffers, std::uint32_t memTotalSize)
{
    VkResult res;
    VkDevice Device = pDevice->GetDevice();
    m_pDevice = pDevice;
    m_memTotalSize = memTotalSize;

    m_mem.OnCreate(numberOfBackBuffers, memTotalSize);

    m_pCommandBuffer = new VkCommandBuffer[memTotalSize];

    // Closing all the command buffers so we can call reset on them the first time we use them, otherwise the runtime would show a warning.
    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.queueFamilyIndex = pDevice->GetGraphicsQueueFamilyIndex();
    cmd_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    res = vkCreateCommandPool(Device, &cmd_pool_info, NULL, &m_commandPool);
    assert(res == VK_SUCCESS);

    VkCommandBufferAllocateInfo cmd = {};
    cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd.pNext = NULL;
    cmd.commandPool = m_commandPool;
    cmd.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd.commandBufferCount = memTotalSize;
    res = vkAllocateCommandBuffers(Device, &cmd, m_pCommandBuffer);
    assert(res == VK_SUCCESS);
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void CommandListRingVK::OnDestroy() {
    vkDestroyCommandPool(m_pDevice->GetDevice(), m_commandPool, NULL);

    vkFreeCommandBuffers(m_pDevice->GetDevice(), m_commandPool, m_memTotalSize, m_pCommandBuffer);
    delete m_pCommandBuffer;
}

//--------------------------------------------------------------------------------------
//
// GetNewCommandList
//
//--------------------------------------------------------------------------------------
VkCommandBuffer *CommandListRingVK::GetNewCommandList() {
    std::uint32_t index;
    if (m_mem.Alloc(1, &index) == false)
        return NULL;

    //ThrowIfFailed(m_ppCommandAllocator[index]->Reset());
    //ThrowIfFailed(m_ppCommandList[index]->Reset(m_ppCommandAllocator[index], nullptr));

    return &m_pCommandBuffer[index];
}

//--------------------------------------------------------------------------------------
//
// OnBeginFrame
//
//--------------------------------------------------------------------------------------
void CommandListRingVK::OnBeginFrame() {
    m_mem.OnBeginFrame();
}