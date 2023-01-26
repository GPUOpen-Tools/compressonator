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


#include <vector>
#include "cmp_devicevk.h"
#include "cmp_resourceviewheapsvk.h"
#include "misc.h"

#if 0
//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void StaticResourceViewHeapVK::OnCreate(CMP_DeviceVK* pDevice, D3D12_DESCRIPTOR_HEAP_TYPE heapType, std::uint32_t descriptorCount, UINT nodeMask) {
    m_pDevice = pDevice;
    m_descriptorCount = descriptorCount;
    m_index = 0;

    m_descriptorElementSize = m_pDevice->GetDescriptorHandleIncrementSize(heapType);

    D3D12_DESCRIPTOR_HEAP_DESC descHeap;
    descHeap.NumDescriptors = descriptorCount;
    descHeap.Type = heapType;
    descHeap.Flags = ( (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) || (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) ) ? D3D12_DESCRIPTOR_HEAP_FLAG_NONE : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    descHeap.NodeMask = nodeMask;
    ThrowIfFailed(
        m_pDevice->CreateDescriptorHeap(&descHeap, IID_PPV_ARGS(&m_pHeap))
    );
    m_pHeap->SetName(L"StaticHeapDX12");
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void StaticResourceViewHeapVK::OnDestroy() {
    m_pHeap->Release();
}

#endif