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
#include "cmp_dynamicbufferringdx12.h"
#include "cmp_staticbufferpooldx12.h"
#include "cmp_uploadheapdx12.h"
#include "cmp_texturedx12.h"
#include "cmp_skydome.h"

using namespace DirectX;


void SkyDome::OnCreate(ID3D12Device* pDevice, UploadHeapDX12* pUploadHeap, DXGI_FORMAT outFormat, ResourceViewHeapsDX12 *pResourceViewHeaps, StaticBufferPoolDX12  *pStaticBufferPool, DynamicBufferRingDX12 *pDynamicBufferRing, UINT sampleDescCount, void *pluginManager, void *msghandler) {
    m_pDynamicBufferRing = pDynamicBufferRing;

    char buff[512];
    GetCurrentDirectoryA(512, buff);

    if (m_CubeDiffuseTexture.InitFromFile(pDevice, pUploadHeap, L"./plugins/media/envmap/diffuse.dds", pluginManager, msghandler) != 0)
        return;

    if (m_CubeSpecularTexture.InitFromFile(pDevice, pUploadHeap, L"./plugins/media/envmap/specular.dds", pluginManager, msghandler) != 0)
        return;

    pResourceViewHeaps->AllocCBV_SRV_UAVDescriptor(1, &m_CubeSpecularTextureSRV);
    m_CubeSpecularTexture.CreateCubeSRV(0, &m_CubeSpecularTextureSRV);

    D3D12_DEPTH_STENCIL_DESC DepthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    DepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    m_skydome.OnCreate(pDevice, pUploadHeap->GetNode(), "./plugins/shaders/", "SkyDome.hlsl", outFormat, pResourceViewHeaps, pStaticBufferPool, 1, NULL, NULL, sampleDescCount);

    m_initok = true;
}

void SkyDome::OnDestroy() {
    if (m_initok) {
        m_skydome.OnDestroy();
        m_CubeDiffuseTexture.OnDestroy();
        m_CubeSpecularTexture.OnDestroy();
        m_initok = false;
    }
}

void SkyDome::Draw(ID3D12GraphicsCommandList* pCommandList, XMMATRIX invViewProj) {
    if (!m_initok) return;

    XMMATRIX *cbPerDraw;
    D3D12_GPU_DESCRIPTOR_HANDLE constantBuffer;
    m_pDynamicBufferRing->AllocConstantBuffer(sizeof(XMMATRIX), (void **)&cbPerDraw, &constantBuffer);
    *cbPerDraw = invViewProj;

    m_skydome.Draw(pCommandList, 1, &m_CubeSpecularTextureSRV, constantBuffer);
}


void SkyDome::GenerateDiffuseMapFromEnvironmentMap() {

}

