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

#include "gltftechnique.h"

#include "texturevk.h"

#include <glm/matrix.hpp>

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

// Forward declarations
class DeviceVK;
class DynamicBufferRingVK;
class GLTFCommon;
class ResourceViewHeapsVK;
class StaticBufferPoolVK;
class UploadHeapVK;

// This class takes a GltfCommon class (that holds all the non-GPU specific data) as an input and loads all the GPU specific data
//
struct DepthMaterial {
    int m_textureCount = 0;

    VkDescriptorSet m_descriptorSet;
    VkDescriptorSetLayout m_descriptorSetLayout;

    std::map<std::string, std::string> m_defines;
    bool m_doubleSided;
};

struct DepthPrimitives : public Primitives {
    DepthMaterial* m_pMaterial = NULL;

    VkPipeline m_pipeline;
    VkPipelineCache m_pipelineCache;
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorPool m_descriptorPool;
    VkDescriptorSet m_descriptorSet;
    VkDescriptorSetLayout m_descriptorSetLayout;
};

struct DepthMesh {
    std::vector<DepthPrimitives> m_pPrimitives;
};

class GltfDepthPass : public GltfTechnique {
  public:
    struct per_batch {
        glm::mat4x4 mViewProj;
    };

    struct per_object {
        glm::mat4x4 mWorld;
    };

    void OnCreate(
        DeviceVK* pDevice,
        VkRenderPass renderPass,
        UploadHeapVK* pUploadHeap,
        ResourceViewHeapsVK* pHeaps,
        DynamicBufferRingVK* pDynamicBufferRing,
        StaticBufferPoolVK* pStaticBufferPool,
        GLTFCommon* pGLTFData, void* pluginManager, void* msghandler);

    void OnDestroy();
    GltfDepthPass::per_batch* SetPerBatchConstants();

  private:
    DeviceVK* m_pDevice;
    ResourceViewHeapsVK* m_pResourceViewHeaps;

    std::vector<DepthMesh> m_meshes;
    std::vector<Texture> m_textures;

    VkSampler m_sampler;

    VkDescriptorBufferInfo m_perBatchDesc;

    void DrawMesh(VkCommandBuffer cmd_buf, int meshIndex, const glm::mat4x4& worldMatrix);
    void CreatePipeline(DeviceVK* pDevice, VkRenderPass renderPass, std::vector<std::string> semanticName, std::vector<VkVertexInputAttributeDescription> layout, DepthPrimitives* pPrimitive);
};

