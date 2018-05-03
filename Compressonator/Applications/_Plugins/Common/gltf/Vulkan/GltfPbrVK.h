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

#include "GltfCommon.h"
#include "GltfTechnique.h"
//#include "SkyDome.h"

struct PBRMaterial
{
    int m_textureCount = 0;

    VkDescriptorSet m_descriptorSet;
    VkDescriptorSetLayout m_descriptorSetLayout;

    std::map<std::string, std::string> m_defines;

    XMVECTOR emissiveFactor;
    XMVECTOR baseColorFactor;
    float    metallicFactor;
    float    roughnessFactor;
};

struct PBRPrimitives : public Primitives
{
    PBRMaterial *m_pMaterial = NULL;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineCache m_pipelineCache;
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorPool m_descriptorPool;    
    VkDescriptorSet m_descriptorSet;
    VkDescriptorSetLayout m_descriptorSetLayout;
    //SAMPLER                                    *m_sampler;
};

struct PBRMesh
{
    std::vector<PBRPrimitives> m_pPrimitives;
};

// This class takes a GltfCommon class (that holds all the non-GPU specific data) as an input and loads all the GPU specific data
//
class GltfPbrVK : public GltfTechnique
{
public:
    struct per_batch
    {
        XMMATRIX mCameraViewProj; 
        XMVECTOR cameraPos;
        XMMATRIX mLightViewProj;
        XMVECTOR lightDirection;
        XMVECTOR lightColor;
        float    depthBias;
        float    iblFactor;
    };

    struct per_object
    {
        XMMATRIX mWorld;
        XMVECTOR u_emissiveFactor;
        XMVECTOR u_baseColorFactor;
        XMVECTOR u_metallicRoughnessValues;
    };

    void OnCreate(
        DeviceVK* pDevice,
        VkRenderPass renderPass,
        UploadHeapVK* pUploadHeap,
        ResourceViewHeapsVK *pHeaps,
        DynamicBufferRingVK *pDynamicBufferRing,
        StaticBufferPoolVK *pStaticBufferPool,
        GLTFCommon *pGLTFData,
        //SkyDome *pSkyDome,
        Texture *pShadowMap,
        void *pluginManager,
        void *msghandler
    );

    void OnDestroy();
    GltfPbrVK::per_batch *SetPerBatchConstants();

private:
    DeviceVK* m_pDevice;
    ResourceViewHeapsVK *m_pResourceViewHeaps;

    std::vector<PBRMesh> m_meshes;
    std::vector<Texture> m_textures;
/*
    
    Texture *m_pCubeDiffuseTexture;
    Texture *m_pCubeSpecularTexture;
    Texture  m_BrdfTexture;
    SAMPLER m_sampler;
*/
    VkSampler m_sampler;

    VkDescriptorBufferInfo m_perBatchDesc;
    void DrawMesh(VkCommandBuffer cmd_buf, int meshIndex, XMMATRIX worldMatrix);
    void AddTextureIfExists(nlohmann::json::object_t material, nlohmann::json::array_t textures, std::map<std::string, Texture *> &map, char *texturePath, char *textureName);
    void CreatePipeline(DeviceVK* pDevice, VkRenderPass renderPass, std::vector<std::string> semanticName, std::vector<VkVertexInputAttributeDescription> layout, PBRPrimitives *pPrimitive);
};



