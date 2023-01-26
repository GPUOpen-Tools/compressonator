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

#include "cmp_gltftechnique.h"
//#include "cmp_skydome.h"

#include "cmp_texturevk.h"

#include <json/json.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <map>
#include <string>
#include <vector>

// Forward Declaration
class CMP_DeviceVK;
class CMP_UploadHeapVK;
class CMP_ResourceViewHeapsVK;
class CMP_DynamicBufferRingVK;
class CMP_StaticBufferPoolVK;
class CMP_GLTFCommon;

struct CMP_PBRMaterial
{
    int m_textureCount = 0;

    VkDescriptorSet m_descriptorSet;
    VkDescriptorSetLayout m_descriptorSetLayout;

    std::map<std::string, std::string> m_defines;

    glm::vec4 emissiveFactor;
    glm::vec4 baseColorFactor;
    float    metallicFactor;
    float    roughnessFactor;
};

struct CMP_PBRPrimitives : public CMP_Primitives
{
    CMP_PBRMaterial* m_pMaterial = NULL;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineCache m_pipelineCache;
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorPool m_descriptorPool;
    VkDescriptorSet m_descriptorSet;
    VkDescriptorSetLayout m_descriptorSetLayout;
    //SAMPLER                                    *m_sampler;
};

struct CMP_PBRMesh
{
    std::vector<CMP_PBRPrimitives> m_pPrimitives;
};

// This class takes a GltfCommon class (that holds all the non-GPU specific data) as an input and loads all the GPU specific data
//
class CMP_GltfPbrVK : public CMP_GltfTechnique
{
  public:
    struct per_batch {
        glm::mat4x4 mCameraViewProj;
        glm::vec4 cameraPos;
        glm::mat4x4 mLightViewProj;
        glm::vec4 lightDirection;
        glm::vec4 lightColor;
        float    depthBias;
        float    iblFactor;
    };

    struct per_object {
        glm::mat4x4 mWorld;
        glm::vec4 u_emissiveFactor;
        glm::vec4 u_baseColorFactor;
        glm::vec4 u_metallicRoughnessValues;
    };

    void OnCreate(CMP_DeviceVK*        pDevice,
        VkRenderPass renderPass,
                  CMP_UploadHeapVK*        pUploadHeap,
                  CMP_ResourceViewHeapsVK* pHeaps,
                  CMP_DynamicBufferRingVK* pDynamicBufferRing,
                  CMP_StaticBufferPoolVK*  pStaticBufferPool,
                  CMP_GLTFCommon*      pGLTFData,
        //SkyDome *pSkyDome,
        Texture* pShadowMap,
        void* pluginManager,
        void* msghandler
    );

    void OnDestroy();
    CMP_GltfPbrVK::per_batch* SetPerBatchConstants();

  private:
    CMP_DeviceVK*        m_pDevice;
      CMP_ResourceViewHeapsVK* m_pResourceViewHeaps;

    std::vector<CMP_PBRMesh> m_meshes;
    std::vector<Texture> m_textures;
    /*

        Texture *m_pCubeDiffuseTexture;
        Texture *m_pCubeSpecularTexture;
        Texture  m_BrdfTexture;
        SAMPLER m_sampler;
    */
    VkSampler m_sampler;

    VkDescriptorBufferInfo m_perBatchDesc;
    void DrawMesh(VkCommandBuffer cmd_buf, int meshIndex, const glm::mat4x4& worldMatrix);
    void AddTextureIfExists(nlohmann::json::object_t material, nlohmann::json::array_t textures, std::map<std::string, Texture*>& map, char* texturePath, char* textureName);
    void                   CreatePipeline(CMP_DeviceVK*                                  pDevice,
                                          VkRenderPass                                   renderPass,
                                          std::vector<std::string>                       semanticName,
                                          std::vector<VkVertexInputAttributeDescription> layout,
                                          CMP_PBRPrimitives*                             pPrimitive);
};

