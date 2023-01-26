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

#include "cmp_gltfcommon.h"
#include "cmp_gltftechnique.h"

#include <d3d12.h>
#include <wrl.h>

// This class takes a GltfCommon class (that holds all the non-GPU specific data) as an input and loads all the GPU specific data
//
struct DepthMaterial {
    CBV_SRV_UAV *m_pTransparency = NULL;
    std::map<std::string, std::string> m_defines;
};

struct DepthPrimitives {
    UINT m_NumIndices;
    D3D12_INDEX_BUFFER_VIEW m_IBV;
    std::vector<D3D12_VERTEX_BUFFER_VIEW> m_VBV;

    DepthMaterial *m_pMaterial = NULL;

    Microsoft::WRL::ComPtr<ID3D12RootSignature>    m_RootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>    m_PipelineRender;
    SAMPLER                                    *m_sampler;
};

struct DepthMesh {
    std::vector<DepthPrimitives> m_pPrimitives;
};

class GltfDepthPass : public CMP_GltfTechnique
{
  public:
    struct per_batch {
        DirectX::XMMATRIX mViewProj;
    };

    bool OnCreate(
        ID3D12Device* pDevice,
        UploadHeapDX12* pUploadHeap,
        ResourceViewHeapsDX12 *pHeaps,
        DynamicBufferRingDX12 *pDynamicBufferRing,
        StaticBufferPoolDX12 *pStaticBufferPool,
                  CMP_GLTFCommon*        pGLTFData,
                  void*                  pluginManager,
                  void*                  msghandler);

    void OnDestroy();
    GltfDepthPass::per_batch *SetPerBatchConstants();

  private:
    std::vector<TextureDX12> m_textures;
    std::vector<DepthMesh> m_meshes;

    SAMPLER m_sampler;

    D3D12_GPU_DESCRIPTOR_HANDLE m_perBatchDesc;
    void DrawMesh(ID3D12GraphicsCommandList* pCommandList, int meshIndex, DirectX::XMMATRIX worldMatrix);
    bool                        CreateGeometry(CMP_tfAccessor indexBuffer, std::vector<CMP_tfAccessor> vertexBuffers, DepthPrimitives* pPrimitive);
    void CreatePipeline(ID3D12Device* pDevice, UINT node, std::vector<std::string> semanticNames, std::vector<D3D12_INPUT_ELEMENT_DESC> layout, DepthPrimitives *pPrimitive);
};



