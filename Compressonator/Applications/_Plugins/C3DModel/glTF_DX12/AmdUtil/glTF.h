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
#include <string>
#include <vector>

#include "DynamicBufferRingDX12.h"
#include "StaticBufferPoolDX12.h"
#include "StaticConstantBufferPoolDX12.h"
#include "CommandListRingDX12.h"
#include "UploadHeapDX12.h"
#include "Texture.h"
#include "Camera.h"
#include ".\json\json.h"


class tfMaterial
{
public:
    int m_textureCount = 0;
    CBV_SRV_UAV *m_pTexturesTable;
    std::map<std::string, std::string> m_defines;

    XMVECTOR emissiveFactor;
    XMVECTOR baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    
};

class tfPrimitives
{    
public:
    UINT m_NumIndices;
    std::vector<D3D12_VERTEX_BUFFER_VIEW> m_VBV;
    D3D12_INDEX_BUFFER_VIEW m_IBV;
    std::map<std::string, std::string> m_defines;

    tfMaterial *m_pMaterial;

    CComPtr<ID3D12RootSignature>	m_RootSignature;
    CComPtr<ID3D12PipelineState>	m_PipelineRender;
    SAMPLER                         m_sampler;
};

class tfMesh
{
public:
    std::vector<tfPrimitives> m_pPrimitives;
};

class tfNode
{
public:
    std::vector<tfNode *> m_children;

    tfMesh *m_pMesh;
    
    XMMATRIX m_rotation;
    XMVECTOR m_translation;
    XMVECTOR m_scale;

    XMMATRIX GetWorldMat() { return XMMatrixScalingFromVector(m_scale)  * m_rotation  * XMMatrixTranslationFromVector(m_translation); }
};

class tfScene
{
public:
    std::vector<tfNode *> m_nodes;
};

class tfAccessor
{
public:
    void *m_data;
    int m_stride;
    int m_dimension;
    int m_type;
};

class tfSampler
{
public:
    tfAccessor m_time;
    tfAccessor m_value;
};

class tfChannel
{
public:
    tfSampler *m_pTranslation;
    tfSampler *m_pRotation;
    tfSampler *m_pScale;
};

class tfAnimation
{
public:
    std::map<int, tfChannel *> m_channels;
};


class GLTFLoader
{
public:
    std::string m_path;
    std::string m_filename;
    nlohmann::json j3;

    std::vector<char *> buffersData;

    bool Load(std::string path, std::string filename, CMP_Feedback_Proc pFeedbackProc = NULL);
    void Unload();
};

class GlTF
{
    std::vector<Texture *> m_textures;
    std::vector<tfMesh *> m_meshes;
    std::vector<tfNode> m_nodes;
    std::vector<tfScene *> m_scenes;
    std::vector<tfAnimation> m_animations;

    DynamicBufferRingDX12 *m_pDynamicBufferRing;
    ResourceViewHeapsDX12 *m_pResourceViewHeaps;

    Texture m_CubeDiffuseTexture;
    Texture m_CubeSpecularTexture;
    Texture m_BrdfTexture;

    void AddTextureIfExists(nlohmann::json::object_t material, nlohmann::json::array_t textures, std::map<std::string, Texture *> &map, char *texturePath, char *textureName);
    void OnCreatePrimitiveColorPass(ID3D12Device* pDevice, std::vector<D3D12_INPUT_ELEMENT_DESC> layout, UINT node, tfPrimitives *pPrimitives);

public:
    int LoadResources(std::string path, std::string filename);
    
    void OnCreate(
        ID3D12Device* pDevice,
        UploadHeapDX12* pUploadHeap,
        ResourceViewHeapsDX12 *pHeaps,
        DynamicBufferRingDX12 *pDynamicBufferRing,
        StaticConstantBufferPoolDX12 *pStaticConstantBufferPool,
        StaticBufferPoolDX12 *pStaticBufferPool,
        GLTFLoader *pGLTFData,
        void *pluginManager, void *msghandler);

    

    void OnDestroy();

    void Draw(ID3D12GraphicsCommandList* pCommandList, Camera *pCam);
};


