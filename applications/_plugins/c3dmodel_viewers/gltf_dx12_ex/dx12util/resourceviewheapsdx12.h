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

#include <d3d12.h>

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
class ResourceView {
  public:
    DWORD GetSize() {
        return m_Size;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetCPU(DWORD i=0) {
        D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptor = m_CPUDescriptor;
        CPUDescriptor.ptr += i * m_dsvDescriptorSize;
        return CPUDescriptor;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGPU(DWORD i = 0) {
        D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptor = m_GPUDescriptor;
        GPUDescriptor.ptr += i * m_dsvDescriptorSize;
        return GPUDescriptor;
    }
  private:
    friend class StaticResourceViewHeapDX12;
    friend class DynamicResourceViewHeapDX12;

    DWORD m_Size = 0;
    DWORD m_dsvDescriptorSize = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE m_CPUDescriptor;
    D3D12_GPU_DESCRIPTOR_HANDLE m_GPUDescriptor;

  protected:
    void SetResourceView(DWORD size, DWORD dsvDescriptorSize, D3D12_CPU_DESCRIPTOR_HANDLE CPUDescriptor, D3D12_GPU_DESCRIPTOR_HANDLE GPUDescriptor) {
        m_Size = size;
        m_CPUDescriptor = CPUDescriptor;
        m_GPUDescriptor = GPUDescriptor;
        m_dsvDescriptorSize = dsvDescriptorSize;
    }
};

//let's add some type safety as mentioned above
class RTV : public ResourceView { };
class DSV : public ResourceView { };
class CBV_SRV_UAV : public ResourceView { };
class SAMPLER : public ResourceView { };

//helper class to use a specific type of heap
class StaticResourceViewHeapDX12 {
  public:
    void OnCreate(ID3D12Device* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE heapType, DWORD descriptorCount, UINT nodeMask = 0);
    void OnDestroy();
    bool AllocDescriptor(DWORD size, ResourceView *pRV) {
        if ((m_index + size) >= m_descriptorCount) {
            char *msg = "StaticResourceViewHeapDX12 heap ran of memory, increase its size";
            MessageBoxA(0, msg, "", 0);
            return false;
        }

        D3D12_CPU_DESCRIPTOR_HANDLE CPUView = m_pHeap->GetCPUDescriptorHandleForHeapStart();
        CPUView.ptr += m_index * m_descriptorElementSize;

        D3D12_GPU_DESCRIPTOR_HANDLE GPUView = m_pHeap->GetGPUDescriptorHandleForHeapStart();
        GPUView.ptr += m_index * m_descriptorElementSize;

        m_index += size;

        pRV->SetResourceView(size, m_descriptorElementSize, CPUView, GPUView);

        return true;
    }

    ID3D12DescriptorHeap *GetHeap() {
        return m_pHeap;
    }

  private:
    DWORD m_index;
    DWORD m_descriptorCount;
    DWORD m_descriptorElementSize;

    ID3D12Device *m_pDevice;
    ID3D12DescriptorHeap *m_pHeap;
};

// This class will hold descriptor heaps for all the types of resources. We are going to need them all anyway.
class ResourceViewHeapsDX12 {
  public:
    void OnCreate(ID3D12Device* pDevice, DWORD cbvDescriptorCount, DWORD srvDescriptorCount, DWORD uavDescriptorCount, DWORD dsvDescriptorCount, DWORD rtvDescriptorCount, DWORD samplerDescriptorCount, UINT nodemask = 0) {
        m_pDevice = pDevice;
        m_nodemask = nodemask;
        m_DSV_Heap.OnCreate(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, dsvDescriptorCount, nodemask);
        m_RTV_Heap.OnCreate(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtvDescriptorCount, nodemask);
        m_Sampler_Heap.OnCreate(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, samplerDescriptorCount, nodemask);
        m_CBV_SRV_UAV_Heap.OnCreate(pDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, cbvDescriptorCount + srvDescriptorCount + uavDescriptorCount, nodemask);
    }

    void OnDestroy() {
        m_DSV_Heap.OnDestroy();
        m_RTV_Heap.OnDestroy();
        m_Sampler_Heap.OnDestroy();
        m_CBV_SRV_UAV_Heap.OnDestroy();
    }

    bool AllocCBV_SRV_UAVDescriptor(DWORD size, CBV_SRV_UAV *pRV) {
        return m_CBV_SRV_UAV_Heap.AllocDescriptor(size, pRV);
    }

    bool AllocDSVDescriptor(DWORD size, DSV *pRV) {
        return m_DSV_Heap.AllocDescriptor(size, pRV);
    }

    bool AllocRTVDescriptor(DWORD size, RTV *pRV) {
        return m_RTV_Heap.AllocDescriptor(size, pRV);
    }

    bool AllocSamplerDescriptor(DWORD size, SAMPLER *pRV) {
        return m_Sampler_Heap.AllocDescriptor(size, pRV);
    }

    ID3D12DescriptorHeap* GetDSVHeap() {
        return m_DSV_Heap.GetHeap();
    }
    ID3D12DescriptorHeap* GetRTVHeap() {
        return m_RTV_Heap.GetHeap();
    }
    ID3D12DescriptorHeap* GetSamplerHeap() {
        return m_Sampler_Heap.GetHeap();
    }
    ID3D12DescriptorHeap* GetCBV_SRV_UAVHeap() {
        return m_CBV_SRV_UAV_Heap.GetHeap();
    }

    UINT GetNodeMask() {
        return m_nodemask;
    }

  private:
    UINT m_nodemask;
    ID3D12Device* m_pDevice;
    StaticResourceViewHeapDX12 m_DSV_Heap;
    StaticResourceViewHeapDX12 m_RTV_Heap;
    StaticResourceViewHeapDX12 m_Sampler_Heap;
    StaticResourceViewHeapDX12 m_CBV_SRV_UAV_Heap;
};


