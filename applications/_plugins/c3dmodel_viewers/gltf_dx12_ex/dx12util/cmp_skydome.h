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

#include <directxmath.h>

class SkyDome {
  public:
    void OnCreate(ID3D12Device* pDevice, UploadHeapDX12* pUploadHeap, DXGI_FORMAT outFormat, ResourceViewHeapsDX12 *pResourceViewHeaps, StaticBufferPoolDX12  *pStaticBufferPool, DynamicBufferRingDX12 *pDynamicBufferRing, UINT sampleDescCount, void *pluginManager, void *msghandler);
    void OnDestroy();
    void Draw(ID3D12GraphicsCommandList* pCommandList, DirectX::XMMATRIX invViewProj);
    void GenerateDiffuseMapFromEnvironmentMap();

    TextureDX12 *GetSpecularCubeMap() {
        return &m_CubeSpecularTexture;
    }
    TextureDX12 *GetDiffuseCubeMap() {
        return &m_CubeDiffuseTexture;
    }

  private:
    bool    m_initok = false;
    TextureDX12 m_CubeDiffuseTexture;
    TextureDX12 m_CubeSpecularTexture;

    CBV_SRV_UAV m_CubeSpecularTextureSRV;

    PostProcPS  m_skydome;

    DynamicBufferRingDX12 *m_pDynamicBufferRing = NULL;
};

