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
#include "cmp_postprocps.h"

class CMP_BlurPS
{
  public:
    void OnCreate(
        ID3D12Device* pDevice,
        UINT node,
        ResourceViewHeapsDX12       *m_pHeaps,
        StaticBufferPoolDX12        *m_pResourceViewHeaps,
        DynamicBufferRingDX12       *m_ConstantBufferRing
    );
    void OnDestroy();

    void OnCreateWindowSizeDependentResources(ID3D12Device* pDevice, DWORD Width, DWORD Height, UINT node, UINT nodemask);
    void OnDestroyWindowSizeDependentResources();

    void Draw(ID3D12GraphicsCommandList* pCommandList, CBV_SRV_UAV *pSrcResource, D3D12_CPU_DESCRIPTOR_HANDLE *pDestination);

    struct cbDownscale {
        float invWidth, invHeight;
    };

    struct cbBlur {
        float dirX, dirY;
    };

  private:
    ID3D12Device                *m_pDevice;

    TextureDX12                      m_temp;
    CBV_SRV_UAV                  m_tempSRV;
    RTV                          m_tempRTV;

    ResourceViewHeapsDX12       *m_pHeaps;
    DynamicBufferRingDX12       *m_pConstantBufferRing;

    DWORD                        m_Width;
    DWORD                        m_Height;

    PostProcPS                   m_blurX;
    PostProcPS                   m_blurY;
};


