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
#include "cmp_skydome.h"

#include <directxmath.h>
#include <wrl.h>

extern CMIPS *DX12_CMips;

struct CMP_PBRMaterial
{
    int m_textureCount = 0;
    CBV_SRV_UAV *m_pTexturesTable;
    std::map<std::string, std::string> m_defines;

    DirectX::XMVECTOR emissiveFactor;
    DirectX::XMVECTOR baseColorFactor;
    float    metallicFactor;
    float    roughnessFactor;
};

struct CMP_PBRPrimitives
{
    UINT m_NumIndices;
    D3D12_INDEX_BUFFER_VIEW m_IBV;
    std::vector<D3D12_VERTEX_BUFFER_VIEW> m_VBV;

    CMP_PBRMaterial* m_pMaterial = NULL;

    Microsoft::WRL::ComPtr<ID3D12RootSignature>    m_RootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState>    m_PipelineRender;
    SAMPLER                                    *m_sampler;
};

struct CMP_PBRMesh
{
    std::vector<CMP_PBRPrimitives> m_pPrimitives;
};

// This class takes a GltfCommon class (that holds all the non-GPU specific data) as an input and loads all the GPU specific data
//
class CMP_GltfPbr : public CMP_GltfTechnique
{
  public:
    struct per_batch {
        DirectX::XMMATRIX mCameraViewProj;
        DirectX::XMVECTOR cameraPos;
        DirectX::XMMATRIX mLightViewProj;
        DirectX::XMVECTOR lightDirection;
        DirectX::XMVECTOR lightColor;
        float    depthBias;
        float    iblFactor;
    };

    bool OnCreate(
        ID3D12Device* pDevice,
        UploadHeapDX12* pUploadHeap,
        ResourceViewHeapsDX12 *pHeaps,
        DynamicBufferRingDX12 *pDynamicBufferRing,
        StaticBufferPoolDX12 *pStaticBufferPool,
                  CMP_GLTFCommon*        pGLTFData,
        SkyDome *pSkyDome,
#ifdef USE_SHADOWMAPS
        Texture *pShadowMap,
#endif
        void *pluginManager, void *msghandler);

    void OnDestroy();
    CMP_GltfPbr::per_batch* SetPerBatchConstants();

    UINT m_TotalNumIndices = 0;

  private:

    std::vector<TextureDX12> m_textures;
      std::vector<CMP_PBRMesh> m_meshes;

    TextureDX12 *m_pCubeDiffuseTexture;
    TextureDX12 *m_pCubeSpecularTexture;
    TextureDX12  m_BrdfTexture;

    SAMPLER m_sampler;

    D3D12_GPU_DESCRIPTOR_HANDLE m_perBatchDesc;
    void DrawMesh(ID3D12GraphicsCommandList* pCommandList, int meshIndex, DirectX::XMMATRIX worldMatrix);
    void AddTextureIfExists(nlohmann::json::object_t material, nlohmann::json::array_t textures, std::map<std::string, TextureDX12 *> &map, char *texturePath, char *textureName);
    bool                        CreateGeometry(CMP_tfAccessor indexBuffer, std::vector<CMP_tfAccessor> vertexBuffers, CMP_PBRPrimitives* pPrimitive);
    void CreatePipeline(ID3D12Device*                         pDevice,
                        UINT                                  node,
                        std::vector<std::string>              semanticName,
                        std::vector<D3D12_INPUT_ELEMENT_DESC> layout,
                        CMP_PBRPrimitives*                    pPrimitive);
};



