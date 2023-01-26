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
class PostProcCS {
  public:
    PostProcCS();
    ~PostProcCS();

    void OnCreate(
        ID3D12Device* pDevice,
        UINT node,
        ResourceViewHeapsDX12 *pResourceViewHeaps,
        DynamicBufferRingDX12 *pDynamicBufferRing,
        DWORD UAVTableSize,
        CBV_SRV_UAV *pUAVTable,
        DWORD dwTileX, DWORD dwTileY, DWORD dwTileZ,
        DWORD dwWidth, DWORD dwHeigth, DWORD dwDepth
    );
    void OnDestroy();
    void Draw(ID3D12GraphicsCommandList* pCommandList, D3D12_GPU_DESCRIPTOR_HANDLE constantBuffer);
    /*
        T *SetPerBatchConstants()
        {
            void *pData;
            m_pDynamicBufferRing->AllocConstantBuffer(sizeof(T), &pData, &m_pGpuDescriptor);
            return pData;
        };
    */
  private:
    ResourceViewHeapsDX12       *m_pHeaps;
    ID3D12Device                *m_pDevice;
    DynamicBufferRingDX12       *m_pDynamicBufferRing;
    ResourceViewHeapsDX12       *m_pResourceViewHeaps;

    D3D12_GPU_DESCRIPTOR_HANDLE *m_pGpuDescriptor;

    ID3D12RootSignature            *m_pRootSignature;

    CBV_SRV_UAV                 *m_pUAVTable;

    DWORD                       m_dwThreadGroupCountX;
    DWORD                       m_dwThreadGroupCountY;
    DWORD                       m_dwThreadGroupCountZ;
};

