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

#include <cassert>
#include <vector>

// In DX12 resource views are represented by handles(called also Descriptors handles). This handles live in a special type of array
// called Descriptor Heap. Placing a few views in contiguously in the same Descriptor Heap allows you
// to create a 'table', that is you can reference the whole table with just a offset(into the descriptor heap)
// and a length. This is a good practice to use tables since the harware runs more efficiently this way.
//
// We need then to allocate arrays of Descriptors into the descriptor heap. The following classes implement a very simple
// linear allocator. Also includes some functions to create Shader/Depth-Stencil/Samples views and assign it to a certain Descriptor.
//
// For every descriptor Heaps there are two types of Descriptor handles, CPU handles an GPU handles.
// To create a view you need a:
//      - resource
//      - a view description structure you need to fill
//      - a CPU handle (lets say the i-th one in your CPU Descritor heap)
//
// In order to bind that resource into the pipeline you'll need to use the i-th handle but from the GPU heap
// this GPU handle is used in SetGraphicsRootDescriptorTable.
//
//
// Since views are represented by just a pair of handles (one for the GPU another for the CPU) we can use a class for all of them.
// Just to avoid mistaking a Sample handle by a Shader Resource, later we'll be creating different types of views.
/*
class ResourceView
{
    friend class StaticResourceViewHeapDX12;
    friend class DynamicResourceViewHeapDX12;

    std::uint32_t m_dsvDescriptorSize;
    std::uint32_t m_Size;
    D3D12_CPU_DESCRIPTOR_HANDLE m_CPUDescriptor;
    D3D12_GPU_DESCRIPTOR_HANDLE m_GPUDescriptor;

protected:
    void SetResourceView(std::uint32_t size, std::uint32_t dsvDescriptorSize, D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptor)
    {
        m_Size = size;
        m_dsvDescriptorSize = dsvDescriptorSize;
        m_CPUDescriptor = CPUDescriptor;
        m_GPUDescriptor = GPUDescriptor;
    }

public:

    std::uint32_t GetSize()
    {
        return m_Size;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetCPU(std::uint32_t i=0)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptor = m_CPUDescriptor;
        CPUDescriptor.ptr += i * m_dsvDescriptorSize;
        return CPUDescriptor;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGPU(std::uint32_t i = 0)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptor = m_GPUDescriptor;
        GPUDescriptor.ptr += i * m_dsvDescriptorSize;
        return GPUDescriptor;
    }
};

//let's add some type safety as mentioned above
class RTV : public ResourceView { };
class DSV : public ResourceView { };
class CBV_SRV_UAV : public ResourceView { };
class SAMPLER : public ResourceView { };

//helper class to use a specific type of heap
class StaticResourceViewHeapVK
{
    CMP_DeviceVK *m_pDevice;
    std::uint32_t m_descriptorCount;
    std::uint32_t m_descriptorElementSize;
    std::uint32_t m_index;

    ID3D12DescriptorHeap *m_pHeap;

public:
    void OnCreate(CMP_DeviceVK* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE heapType, std::uint32_t descriptorCount, UINT nodeMask = 0);
    void OnDestroy();
    bool AllocDescriptor(std::uint32_t size, ResourceView *pRV)
    {
        if (m_index >= m_descriptorCount)
            return false;

        D3D12_CPU_DESCRIPTOR_HANDLE CPUView = m_pHeap->GetCPUDescriptorHandleForHeapStart();
        CPUView.ptr += m_index * m_descriptorElementSize;

        D3D12_GPU_DESCRIPTOR_HANDLE GPUView = m_pHeap->GetGPUDescriptorHandleForHeapStart();
        GPUView.ptr += m_index * m_descriptorElementSize;

        m_index += size;

        pRV->SetResourceView(size, m_descriptorElementSize, CPUView, GPUView);

        return true;
    }
};
*/


// This class will hold descriptor heaps for all the types of resources. We are going to need them all anyway.
class CMP_ResourceViewHeapsVK
{
    CMP_DeviceVK*    m_pDevice;
    VkDescriptorPool m_descriptorPool;
  public:
    void OnCreate(CMP_DeviceVK* pDevice,
                  std::uint32_t cbvDescriptorCount,
                  std::uint32_t srvDescriptorCount,
                  std::uint32_t uavDescriptorCount,
                  std::uint32_t samplerDescriptorCount)
    {
        m_pDevice = pDevice;

        VkResult res;

        std::vector<VkDescriptorPoolSize> type_count = {
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, cbvDescriptorCount },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, srvDescriptorCount },
            { VK_DESCRIPTOR_TYPE_SAMPLER, samplerDescriptorCount },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, uavDescriptorCount }
        };

        VkDescriptorPoolCreateInfo descriptor_pool = {};
        descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool.pNext = NULL;
        descriptor_pool.flags = 0;// VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        descriptor_pool.maxSets = 1000;
        descriptor_pool.poolSizeCount = (uint32_t)type_count.size();
        descriptor_pool.pPoolSizes = type_count.data();

        res = vkCreateDescriptorPool(pDevice->GetDevice(), &descriptor_pool, NULL, &m_descriptorPool);
        assert(res == VK_SUCCESS);
    }

    void OnDestroy() {
        vkDestroyDescriptorPool(m_pDevice->GetDevice(), m_descriptorPool, NULL);
    }

    bool AllocDescriptor(std::vector<VkDescriptorSetLayoutBinding> *pDescriptor, VkDescriptorSetLayout *pDescLayout, VkDescriptorSet *pDescriptorSet) {
        VkResult res;

        /* Next take layout bindings and use them to create a descriptor set layout
        */
        VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
        descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_layout.pNext = NULL;
        descriptor_layout.bindingCount = (uint32_t)pDescriptor->size();
        descriptor_layout.pBindings = pDescriptor->data();

        res = vkCreateDescriptorSetLayout(m_pDevice->GetDevice(), &descriptor_layout, NULL, pDescLayout);
        assert(res == VK_SUCCESS);

        VkDescriptorSetAllocateInfo alloc_info;
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.pNext = NULL;
        alloc_info.descriptorPool = m_descriptorPool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = pDescLayout;

        res = vkAllocateDescriptorSets(m_pDevice->GetDevice(), &alloc_info, pDescriptorSet);
        assert(res == VK_SUCCESS);

        return res == VK_SUCCESS;
    }

    VkDescriptorPool GetHeap() {
        return m_descriptorPool;
    }
};


