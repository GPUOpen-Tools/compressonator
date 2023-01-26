//=====================================================================
// Copyright 2018 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "cmp_tonemapping.h"

#include "cmp_dynamicbufferringdx12.h"
#include "cmp_staticbufferpooldx12.h"
#include "cmp_uploadheapdx12.h"


void ToneMapping::OnCreate(ID3D12Device* pDevice, UINT node, DXGI_FORMAT outFormat, ResourceViewHeapsDX12 *pResourceViewHeaps, StaticBufferPoolDX12  *pStaticBufferPool, DynamicBufferRingDX12 *pDynamicBufferRing) {
    m_pDynamicBufferRing = pDynamicBufferRing;

    m_toneMapping.OnCreate(pDevice, node, "./plugins/shaders/", "Tonemapping.hlsl", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, pResourceViewHeaps, pStaticBufferPool, 1);
}

void ToneMapping::OnDestroy() {
    m_toneMapping.OnDestroy();
}

void ToneMapping::Draw(ID3D12GraphicsCommandList* pCommandList, CBV_SRV_UAV *pHDRSRV, float exposure, int toneMapper, int gammaTestPattern) {
    D3D12_GPU_DESCRIPTOR_HANDLE cbTonemappingHandle;
    struct ToneMapping {
        float exposure;
        int toneMapper;
        int gammaTestPattern;
    } *cbToneMapping;
    m_pDynamicBufferRing->AllocConstantBuffer(sizeof(ToneMapping), (void **)&cbToneMapping, &cbTonemappingHandle);
    cbToneMapping->exposure = exposure;
    cbToneMapping->toneMapper = toneMapper;
    cbToneMapping->gammaTestPattern = gammaTestPattern;

    m_toneMapping.Draw(pCommandList, 1, pHDRSRV, cbTonemappingHandle);
}
