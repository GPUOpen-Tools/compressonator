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


#include "cmp_staticbufferpoolvk.h"
#include "misc.h"

#include <cassert>

void CMP_StaticBufferPoolVK::OnCreate(CMP_DeviceVK* pDevice, std::uint32_t totalMemSize)
{
    VkResult res;
    m_pDevice = pDevice;

    m_totalMemSize = totalMemSize;
    m_memOffset = 0;
    m_pData = NULL;

    // create the buffer, allocate memory and bind it
    VkBufferCreateInfo buf_info = {};
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.pNext = NULL;
    buf_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buf_info.size = m_totalMemSize;
    buf_info.queueFamilyIndexCount = 0;
    buf_info.pQueueFamilyIndices = NULL;
    buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buf_info.flags = 0;
    res = vkCreateBuffer(m_pDevice->GetDevice(), &buf_info, NULL, &m_buffer);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(m_pDevice->GetDevice(), m_buffer, &mem_reqs);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.pNext = NULL;
    alloc_info.memoryTypeIndex = 0;
    alloc_info.memoryTypeIndex = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    alloc_info.allocationSize = mem_reqs.size;
    alloc_info.memoryTypeIndex = 0;

    bool pass = memory_type_from_properties(m_pDevice->GetPhysicalDeviceMemoryProperties(), mem_reqs.memoryTypeBits,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            &alloc_info.memoryTypeIndex);
    assert(pass && "No mappable, coherent memory");

    res = vkAllocateMemory(m_pDevice->GetDevice(), &alloc_info, NULL, &m_deviceMemory);
    assert(res == VK_SUCCESS);

    res = vkMapMemory(m_pDevice->GetDevice(), m_deviceMemory, 0, mem_reqs.size, 0, (void **)&m_pData);
    assert(res == VK_SUCCESS);

    res = vkBindBufferMemory(m_pDevice->GetDevice(), m_buffer, m_deviceMemory, 0);
    assert(res == VK_SUCCESS);
}

void CMP_StaticBufferPoolVK::OnDestroy()
{
    vkUnmapMemory(m_pDevice->GetDevice(), m_deviceMemory);
    vkFreeMemory(m_pDevice->GetDevice(), m_deviceMemory, NULL);
    vkDestroyBuffer(m_pDevice->GetDevice(), m_buffer, NULL);
}


bool CMP_StaticBufferPoolVK::AllocVertexBuffer(std::uint32_t numbeOfVertices, UINT strideInBytes, void** pData, VkDescriptorBufferInfo* pOut)
{
    std::uint32_t size = (std::uint32_t)Align(numbeOfVertices* strideInBytes, 256);
    assert(m_memOffset + size < m_totalMemSize);

    *pData = (void *)(m_pData + m_memOffset);

    pOut->buffer = m_buffer;
    pOut->offset = m_memOffset;
    pOut->range = size;

    m_memOffset += size;

    return true;
}

bool CMP_StaticBufferPoolVK::AllocIndexBuffer(std::uint32_t numbeOfIndices, UINT strideInBytes, void** pData, VkDescriptorBufferInfo* pOut)
{
    std::uint32_t size = (std::uint32_t)Align(numbeOfIndices*strideInBytes, 256);
    assert(m_memOffset + size < m_totalMemSize);


    *pData = (void *)(m_pData + m_memOffset);

    // fill buffer view structure with the right pointer
    pOut->buffer = m_buffer;
    pOut->offset = m_memOffset;
    pOut->range = size;

    m_memOffset += size;

    return true;
}

void CMP_StaticBufferPoolVK::UploadData(VkCommandBuffer cmd_buf)
{
}

void CMP_StaticBufferPoolVK::FreeUploadHeap()
{
}




