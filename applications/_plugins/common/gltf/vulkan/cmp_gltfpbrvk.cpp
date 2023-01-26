// AMD AMDUtils code
//
// Copyright(c) 2018 Advanced Micro Devices, Inc.All rights reserved.
//
// Vulkan Samples
//
// Copyright (C) 2015-2016 Valve Corporation
// Copyright (C) 2015-2016 LunarG, Inc.
// Copyright (C) 2015-2016 Google, Inc.
//
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

#include "cmp_gltfpbrvk.h"

#include "cmp_devicevk.h"
#include "cmp_dynamicbufferringvk.h"
#include "cmp_gltfcommon.h"
#include "cmp_gltfhelpers.h"
#include "cmp_gltfhelpers_vulkan.h"
#include "cmp_resourceviewheapsvk.h"
#include "cmp_shadercompilerhelpervk.h"
#include "cmp_threadpoolvk.h"
#include "cmp_uploadheapvk.h"

#include <vector>


void CMP_GltfPbrVK::AddTextureIfExists(json::object_t                   material,
                                       json::array_t                    textures,
                                       std::map<std::string, Texture*>& map,
                                       char*                            texturePath,
                                       char*                            textureName)
{
    int id = GetElementInt(material, texturePath, -1);
    if (id >= 0) {
        int tex = textures[id]["source"];
        map[textureName] = &m_textures[tex];
    }
}

void CMP_GltfPbrVK::OnCreate(CMP_DeviceVK*        pDevice,
                             VkRenderPass renderPass,
                             CMP_UploadHeapVK*        pUploadHeap,
                             CMP_ResourceViewHeapsVK* pHeaps,
                             CMP_DynamicBufferRingVK* pDynamicBufferRing,
                             CMP_StaticBufferPoolVK*  pStaticBufferPool,
                             CMP_GLTFCommon*      pGLTFData,
                             //    SkyDome *pSkyDome,
                             Texture *pShadowMap,
                             void *pluginManager,
                             void *msghandler)
{
    m_pDevice = pDevice;
    m_pGLTFData = pGLTFData;
    m_pDynamicBufferRing = pDynamicBufferRing;
    m_pResourceViewHeaps = pHeaps;
    m_pStaticBufferPool = pStaticBufferPool;
    /*
        // Load cubemaps maps for IBL
        m_pCubeDiffuseTexture = pSkyDome->GetDiffuseCubeMap();
        m_pCubeSpecularTexture = pSkyDome->GetSpecularCubeMap();
        m_BrdfTexture.InitFromFile(pDevice, pUploadHeap, L"..\\media\\envmap\\brdf.dds");
        pUploadHeap->FlushAndFinish();
    */
    json &j3 = pGLTFData->j3;

    // Load Textures
    // ToDo - Change to use TextureIO in next release
    auto images = j3["images"];
    m_textures.resize(images.size());
    for (unsigned int i = 0; i<images.size(); i++) {
        std::string filename = images[i]["uri"];

        INT32 result = m_textures[i].InitFromFile(pDevice, pUploadHeap, (pGLTFData->m_path + filename).c_str(), pluginManager, msghandler);
    }
    pUploadHeap->FlushAndFinish();

    /////////////////////////////////////////////
    // Create Sampler

    {
        VkSamplerCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.magFilter = VK_FILTER_LINEAR;
        info.minFilter = VK_FILTER_LINEAR;
        info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.minLod = -1000;
        info.maxLod = 1000;
        info.maxAnisotropy = 1.0f;
        VkResult res = vkCreateSampler(pDevice->GetDevice(), &info, NULL, &m_sampler);
        assert(res == VK_SUCCESS);
    }

    // Load PBR 2.0 Materials
    //
    std::vector<CMP_PBRMaterial*> materialsData;
    auto materials = j3["materials"];
    auto textures = j3["textures"];
    for (unsigned int i = 0; i < materials.size(); i++) {
        json::object_t material = materials[i];

        CMP_PBRMaterial* tfmat = new CMP_PBRMaterial();
        materialsData.push_back(tfmat);

        // Load material constants
        //
        json::array_t ones = { 1.0, 1.0, 1.0, 1.0 };
        json::array_t zeroes = { 0.0, 0.0, 0.0, 0.0 };
        tfmat->emissiveFactor = GetVector(GetElementJsonArray(material, "emissiveFactor", zeroes));
        tfmat->baseColorFactor = GetVector(GetElementJsonArray(material, "pbrMetallicRoughness/baseColorFactor", ones));
        tfmat->metallicFactor = GetElementFloat(material, "pbrMetallicRoughness/metallicFactor", 1.0);
        tfmat->roughnessFactor = GetElementFloat(material, "pbrMetallicRoughness/roughnessFactor", 1.0);

        tfmat->m_defines["DEF_alphaMode_" + GetElementString(material, "alphaMode", "OPAQUE")] = std::to_string(1);
        tfmat->m_defines["DEF_alphaCutoff"] = std::to_string(GetElementFloat(material, "alphaCutoff", 1.0));

        // load glTF 2.0 material's textures (if present) and create descriptor set
        //
        std::map<std::string, Texture *> texturesBase;
        if (textures.size() > 0) {
            AddTextureIfExists(material, textures, texturesBase, "pbrMetallicRoughness/baseColorTexture/index", "baseColorTexture");
            AddTextureIfExists(material, textures, texturesBase, "pbrMetallicRoughness/metallicRoughnessTexture/index", "metallicRoughnessTexture");
            AddTextureIfExists(material, textures, texturesBase, "emissiveTexture/index", "emissiveTexture");
            AddTextureIfExists(material, textures, texturesBase, "normalTexture/index", "normalTexture");
            AddTextureIfExists(material, textures, texturesBase, "occlusionTexture/index", "occlusionTexture");
        }

        tfmat->m_textureCount = (int)texturesBase.size();
        /*
                if (m_pCubeDiffuseTexture)
                    tfmat->m_textureCount += 1;

                if (m_pCubeSpecularTexture)
                    tfmat->m_textureCount += 1;

                //+ 1 brdf lookup texture, add that to the total count of textures used
                tfmat->m_textureCount += 1;
        */
        // plus shadows
        if (pShadowMap != NULL)
            tfmat->m_textureCount += 1;

        if (tfmat->m_textureCount >= 0) {
            std::vector<VkDescriptorSetLayoutBinding> layout_bindings(tfmat->m_textureCount);

            int cnt = 0;

            //create SRVs and #defines so the shader compiler knows what the index of each texture is
            for (auto it = texturesBase.begin(); it != texturesBase.end(); it++) {
                tfmat->m_defines[std::string("ID_") + it->first] = std::to_string(cnt);

                layout_bindings[cnt].binding = cnt;
                layout_bindings[cnt].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                layout_bindings[cnt].descriptorCount = 1;
                layout_bindings[cnt].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                layout_bindings[cnt].pImmutableSamplers = NULL;

                cnt++;
            }

            if (pShadowMap != NULL) {
                tfmat->m_defines["ID_shadowMap"] = std::to_string(cnt);

                layout_bindings[cnt].binding = cnt;
                layout_bindings[cnt].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                layout_bindings[cnt].descriptorCount = 1;
                layout_bindings[cnt].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                layout_bindings[cnt].pImmutableSamplers = NULL;

                cnt++;
            }

            m_pResourceViewHeaps->AllocDescriptor(&layout_bindings, &tfmat->m_descriptorSetLayout, &tfmat->m_descriptorSet);

            cnt = 0;

            std::vector<VkWriteDescriptorSet> writes(tfmat->m_textureCount);
            std::vector<VkDescriptorImageInfo> desc_image(tfmat->m_textureCount);
            for (auto it = texturesBase.begin(); it != texturesBase.end(); it++) {
                desc_image[cnt].sampler = m_sampler;
                desc_image[cnt].imageView = VK_NULL_HANDLE;
                desc_image[cnt].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                writes[cnt] = {};
                writes[cnt].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes[cnt].dstBinding = cnt;
                writes[cnt].pNext = NULL;
                writes[cnt].dstSet = tfmat->m_descriptorSet;
                writes[cnt].descriptorCount = 1;
                writes[cnt].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                writes[cnt].pImageInfo = &desc_image[cnt];
                writes[cnt].dstArrayElement = 0;

                it->second->CreateSRV(0, &desc_image[cnt].imageView);

                cnt++;
            }

            /*
                        //create SRVs and #defines for the IBL resources
                        if (m_pCubeDiffuseTexture)
                        {
                            tfmat->m_defines["ID_diffuseCube"] = std::to_string(cnt);
                            m_pCubeDiffuseTexture->CreateCubeSRV(cnt++, tfmat->m_pTexturesTable);
                            tfmat->m_defines["USE_IBL"] = "1";
                        }

                        if (m_pCubeSpecularTexture)
                        {
                            tfmat->m_defines["ID_specularCube"] = std::to_string(cnt);
                            m_pCubeSpecularTexture->CreateCubeSRV(cnt++, tfmat->m_pTexturesTable);
                            tfmat->m_defines["USE_IBL"] = "1";
                        }

                        tfmat->m_defines["ID_brdfTexture"] = std::to_string(cnt);
                        m_BrdfTexture.CreateSRV(cnt++, tfmat->m_pTexturesTable);
                        */

            // add SRV for the shadowmap
            if (pShadowMap!=NULL) {
                desc_image[cnt].sampler = m_sampler;
                desc_image[cnt].imageView = VK_NULL_HANDLE;
                desc_image[cnt].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                writes[cnt] = {};
                writes[cnt].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes[cnt].dstBinding = cnt;
                writes[cnt].pNext = NULL;
                writes[cnt].dstSet = tfmat->m_descriptorSet;
                writes[cnt].descriptorCount = 1;
                writes[cnt].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                writes[cnt].pImageInfo = &desc_image[cnt];
                writes[cnt].dstArrayElement = 0;
                pShadowMap->CreateSRV(cnt, &desc_image[cnt].imageView);
                cnt++;
            }

            vkUpdateDescriptorSets(m_pDevice->GetDevice(), (uint32_t)writes.size(), writes.data(), 0, NULL);

        }
    }

    // Load Meshes
    //
    json::array_t accessors = j3["accessors"];
    json::array_t bufferViews = j3["bufferViews"];
    json::array_t meshes = j3["meshes"];
    m_meshes.resize(meshes.size());
    for (unsigned int i = 0; i < meshes.size(); i++) {
        CMP_PBRMesh* tfmesh = &m_meshes[i];

        auto primitives = meshes[i]["primitives"];
        tfmesh->m_pPrimitives.resize(primitives.size());
        for (unsigned int p = 0; p < primitives.size(); p++) {
            CMP_PBRPrimitives* pPrimitive = &tfmesh->m_pPrimitives[p];
            auto primitive = primitives[p];

            // Set Material
            //
            pPrimitive->m_pMaterial = materialsData[primitive["material"]];

            // Set Index buffer
            //
            CMP_tfAccessor indexBuffer;
            {
                auto indicesAccessor = accessors[primitive["indices"].get<int>()];
                GetBufferDetails(indicesAccessor, bufferViews, pGLTFData->buffersData, &indexBuffer);
            }

            // Get input layout
            //
            std::vector<CMP_tfAccessor>                    vertexBuffers;
            std::vector<std::string> semanticNames;
            std::vector<VkVertexInputAttributeDescription> attributes;      // std::vector<D3D12_INPUT_ELEMENT_DESC> layout;

            auto attribute = primitive["attributes"];
            attributes.resize(attribute.size());            // layout.reserve(attribute.size());
            vertexBuffers.resize(attribute.size());
            semanticNames.resize(attribute.size());         // vertexBuffers.resize(attribute.size());

            int cnt = 0;
            for (auto it = attribute.begin(); it != attribute.end(); it++) {
                int index = cnt; // std::uint32_t semanticIndex = 0;

                // Diff from DX12 code =====
                if (it.key() == "TANGENT") {
                    index = (int)attributes.size() - 1;
                    cnt--;
                }
//=================================

                semanticNames[index] = it.key();    //  semanticNames.push_back(semanticName);

                auto  accessor = accessors[it.value().get<int>()];

                // Get VB accessors
                //
                //................................................................................Index -> layout.size()
                GetBufferDetails(accessor, bufferViews, pGLTFData->buffersData, &vertexBuffers[index]);

                // Code is specific to VK here -------------
                // Create Input Layout
                //
                VkVertexInputAttributeDescription l;
                l.location = (uint32_t)index;
                l.format = GetFormat_Vulkan(accessor["type"], accessor["componentType"]);
                l.offset = 0;
                l.binding = index;
                attributes[index] = l;

                cnt++;          //   layout.push_back(l);
            }

            CreateGeometry(indexBuffer, vertexBuffers, pPrimitive);
            //GetThreadPool()->Add_Job([=]()
            //{
            CreatePipeline(pDevice, renderPass, semanticNames, attributes, pPrimitive);
            //});
        }
    }
}

void CMP_GltfPbrVK::OnDestroy()
{
    for (unsigned int i = 0; i < m_textures.size(); i++) {
        m_textures[i].OnDestroy();
    }
    /*
        m_BrdfTexture.OnDestroy();
    */
}

void CMP_GltfPbrVK::CreatePipeline(CMP_DeviceVK*                                  pDevice,
                               VkRenderPass                                   renderPass,
                               std::vector<std::string>                       semanticNames,
                               std::vector<VkVertexInputAttributeDescription> layout,
                               CMP_PBRPrimitives*                             pPrimitive)
{
    VkResult res;

    // let vertex shader know what buffers are present
    std::map<std::string, std::string> attributeDefines;
    for (unsigned int i = 0; i < semanticNames.size(); i++) {
        attributeDefines[std::string("ID_") + semanticNames[i]] = std::to_string(layout[i].binding);
    }

    if (attributeDefines.find("ID_TANGENT")!= attributeDefines.end()) {
        attributeDefines[std::string("ID_WORLDPOS")] = std::to_string(semanticNames.size() + 2);
    } else {
        attributeDefines[std::string("ID_WORLDPOS")] = std::to_string(semanticNames.size());
    }

    std::map<std::string, std::string> *pMatDefines = &pPrimitive->m_pMaterial->m_defines;
    for (auto it = pMatDefines->begin(); it != pMatDefines->end(); it++)
        attributeDefines[it->first] = it->second;

    /////////////////////////////////////////////
    // Compile and create shaders

    init_glslang();

    VkPipelineShaderStageCreateInfo m_vertexShader;
    res = VKCompileFromFile(pDevice->GetDevice(), SST_GLSL, VK_SHADER_STAGE_VERTEX_BIT, "./plugins/shaders/glTF20-vert_vk.glsl", "main", attributeDefines, &m_vertexShader);
    //res = VKCompileFromFile(pDevice->GetDevice(), SST_HLSL, VK_SHADER_STAGE_VERTEX_BIT, "./plugins/shaders/glTF20_vk.hlsl", "mainVS", attributeDefines, &m_vertexShader);
    assert(res == VK_SUCCESS);

    VkPipelineShaderStageCreateInfo m_fragmentShader;
    res = VKCompileFromFile(pDevice->GetDevice(), SST_GLSL, VK_SHADER_STAGE_FRAGMENT_BIT, "./plugins/shaders/glTF20-frag_vk.glsl", "main", attributeDefines, &m_fragmentShader);
    assert(res == VK_SUCCESS);

    finalize_glslang();

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { m_vertexShader, m_fragmentShader };

    /////////////////////////////////////////////
    // Create pipeline layout

    std::vector<VkDescriptorSetLayoutBinding> layout_bindings(2);
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_bindings[0].pImmutableSamplers = NULL;

    layout_bindings[1].binding = 1;
    layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    layout_bindings[1].descriptorCount = 1;
    layout_bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_bindings[1].pImmutableSamplers = NULL;

    m_pResourceViewHeaps->AllocDescriptor(&layout_bindings, &pPrimitive->m_descriptorSetLayout, &pPrimitive->m_descriptorSet);

    VkWriteDescriptorSet writes[2];
    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = NULL;
    writes[0].dstSet = pPrimitive->m_descriptorSet;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[0].pBufferInfo = &m_pDynamicBufferRing->GetMainBuffer(sizeof(per_batch));
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    writes[1] = {};
    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].pNext = NULL;
    writes[1].dstSet = pPrimitive->m_descriptorSet;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[1].pBufferInfo = &m_pDynamicBufferRing->GetMainBuffer(sizeof(per_object));
    writes[1].dstArrayElement = 0;
    writes[1].dstBinding = 1;
    vkUpdateDescriptorSets(m_pDevice->GetDevice(), 2, writes, 0, NULL);

    std::vector<VkDescriptorSetLayout> descriptorSetLayout = { pPrimitive->m_descriptorSetLayout, pPrimitive->m_pMaterial->m_descriptorSetLayout };

    /* Now use the descriptor layout to create a pipeline layout */
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = NULL;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges = NULL;
    pPipelineLayoutCreateInfo.setLayoutCount = (uint32_t)descriptorSetLayout.size();
    pPipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayout.data();

    res = vkCreatePipelineLayout(pDevice->GetDevice(), &pPipelineLayoutCreateInfo, NULL, &pPrimitive->m_pipelineLayout);
    assert(res == VK_SUCCESS);

    /////////////////////////////////////////////
    // Create pipeline

    // vertex input state

    std::vector<VkVertexInputBindingDescription> vi_binding(layout.size());
    for (unsigned int i = 0; i < layout.size(); i++) {
        vi_binding[i].binding = layout[i].binding;
        vi_binding[i].stride = SizeOfFormat_Vulkan(layout[i].format);
        vi_binding[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }

    VkPipelineVertexInputStateCreateInfo vi = {};
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vi.pNext = NULL;
    vi.flags = 0;
    vi.vertexBindingDescriptionCount = (uint32_t)vi_binding.size();
    vi.pVertexBindingDescriptions = vi_binding.data();
    vi.vertexAttributeDescriptionCount = (uint32_t)layout.size();
    vi.pVertexAttributeDescriptions = layout.data();

    // input assembly state

    VkPipelineInputAssemblyStateCreateInfo ia;
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.pNext = NULL;
    ia.flags = 0;
    ia.primitiveRestartEnable = VK_FALSE;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // rasterizer state

    VkPipelineRasterizationStateCreateInfo rs;
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.pNext = NULL;
    rs.flags = 0;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_BACK_BIT;
    rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.depthClampEnable = VK_FALSE;
    rs.rasterizerDiscardEnable = VK_FALSE;
    rs.depthBiasEnable = VK_FALSE;
    rs.depthBiasConstantFactor = 0;
    rs.depthBiasClamp = 0;
    rs.depthBiasSlopeFactor = 0;
    rs.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState att_state[1];
    att_state[0].colorWriteMask = 0xf;
    att_state[0].blendEnable = VK_TRUE;
    att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;
    att_state[0].colorBlendOp = VK_BLEND_OP_ADD;
    att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    att_state[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    // Color blend state

    VkPipelineColorBlendStateCreateInfo cb;
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.flags = 0;
    cb.pNext = NULL;
    cb.attachmentCount = 1;
    cb.pAttachments = att_state;
    cb.logicOpEnable = VK_FALSE;
    cb.logicOp = VK_LOGIC_OP_NO_OP;
    cb.blendConstants[0] = 1.0f;
    cb.blendConstants[1] = 1.0f;
    cb.blendConstants[2] = 1.0f;
    cb.blendConstants[3] = 1.0f;

    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pNext = NULL;
    dynamicState.pDynamicStates = dynamicStateEnables.data();
    dynamicState.dynamicStateCount = (uint32_t)dynamicStateEnables.size();

    // view port state

    VkPipelineViewportStateCreateInfo vp = {};
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.pNext = NULL;
    vp.flags = 0;
    vp.viewportCount = 1;
    vp.scissorCount = 1;
    vp.pScissors = NULL;
    vp.pViewports = NULL;

    // depth stencil state

    VkPipelineDepthStencilStateCreateInfo ds;
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.pNext = NULL;
    ds.flags = 0;
    ds.depthTestEnable = true;
    ds.depthWriteEnable = true;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable = VK_FALSE;
    ds.stencilTestEnable = VK_FALSE;
    ds.back.failOp = VK_STENCIL_OP_KEEP;
    ds.back.passOp = VK_STENCIL_OP_KEEP;
    ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
    ds.back.compareMask = 0;
    ds.back.reference = 0;
    ds.back.depthFailOp = VK_STENCIL_OP_KEEP;
    ds.back.writeMask = 0;
    ds.minDepthBounds = 0;
    ds.maxDepthBounds = 0;
    ds.stencilTestEnable = VK_FALSE;
    ds.front = ds.back;

    // multi sample state

    VkPipelineMultisampleStateCreateInfo ms;
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.pNext = NULL;
    ms.flags = 0;
    ms.pSampleMask = NULL;
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    ms.sampleShadingEnable = VK_FALSE;
    ms.alphaToCoverageEnable = VK_FALSE;
    ms.alphaToOneEnable = VK_FALSE;
    ms.minSampleShading = 0.0;

    // create pipeline cache

    VkPipelineCacheCreateInfo pipelineCache;
    pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCache.pNext = NULL;
    pipelineCache.initialDataSize = 0;
    pipelineCache.pInitialData = NULL;
    pipelineCache.flags = 0;
    res = vkCreatePipelineCache(pDevice->GetDevice(), &pipelineCache, NULL, &pPrimitive->m_pipelineCache);
    assert(res == VK_SUCCESS);

    // create pipeline

    VkGraphicsPipelineCreateInfo pipeline = {};
    pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline.pNext = NULL;
    pipeline.layout = pPrimitive->m_pipelineLayout;
    pipeline.basePipelineHandle = VK_NULL_HANDLE;
    pipeline.basePipelineIndex = 0;
    pipeline.flags = 0;
    pipeline.pVertexInputState = &vi;
    pipeline.pInputAssemblyState = &ia;
    pipeline.pRasterizationState = &rs;
    pipeline.pColorBlendState = &cb;
    pipeline.pTessellationState = NULL;
    pipeline.pMultisampleState = &ms;
    pipeline.pDynamicState = &dynamicState;
    pipeline.pViewportState = &vp;
    pipeline.pDepthStencilState = &ds;
    pipeline.pStages = shaderStages.data();
    pipeline.stageCount = (uint32_t)shaderStages.size();
    pipeline.renderPass = renderPass;
    pipeline.subpass = 0;

    res = vkCreateGraphicsPipelines(pDevice->GetDevice(), pPrimitive->m_pipelineCache, 1, &pipeline, NULL, &pPrimitive->m_pipeline);
    assert(res == VK_SUCCESS);
    /*
    // create samplers if not initialized (this should happen once)
    if (m_sampler.GetSize()==0)
    {
        m_pResourceViewHeaps->AllocSamplerDescriptor(5, &m_sampler);

        //for pbr materials
        D3D12_SAMPLER_DESC SamplerDesc;
        ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.BorderColor[0] = 0.0f;
        SamplerDesc.BorderColor[1] = 0.0f;
        SamplerDesc.BorderColor[2] = 0.0f;
        SamplerDesc.BorderColor[3] = 0.0f;
        SamplerDesc.MinLOD = 0.0f;
        SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
        SamplerDesc.MipLODBias = 0;
        SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        SamplerDesc.MaxAnisotropy = 1;
        pDevice->CreateSampler(&SamplerDesc, m_sampler.GetCPU(0));


        // diffuse env map sampler
        ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.BorderColor[0] = 0.0f;
        SamplerDesc.BorderColor[1] = 0.0f;
        SamplerDesc.BorderColor[2] = 0.0f;
        SamplerDesc.BorderColor[3] = 0.0f;
        SamplerDesc.MinLOD = 0.0f;
        SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
        SamplerDesc.MipLODBias = 0;
        SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        SamplerDesc.MaxAnisotropy = 1;
        pDevice->CreateSampler(&SamplerDesc, m_sampler.GetCPU(1));

        // specular env map sampler
        ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.BorderColor[0] = 0.0f;
        SamplerDesc.BorderColor[1] = 0.0f;
        SamplerDesc.BorderColor[2] = 0.0f;
        SamplerDesc.BorderColor[3] = 0.0f;
        SamplerDesc.MinLOD = 0.0f;
        SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
        SamplerDesc.MipLODBias = 0;
        SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        SamplerDesc.MaxAnisotropy = 1;
        pDevice->CreateSampler(&SamplerDesc, m_sampler.GetCPU(2));

        // specular BRDF lut sampler
        ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.BorderColor[0] = 0.0f;
        SamplerDesc.BorderColor[1] = 0.0f;
        SamplerDesc.BorderColor[2] = 0.0f;
        SamplerDesc.BorderColor[3] = 0.0f;
        SamplerDesc.MinLOD = 0.0f;
        SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
        SamplerDesc.MipLODBias = 0;
        SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        SamplerDesc.MaxAnisotropy = 1;
        pDevice->CreateSampler(&SamplerDesc, m_sampler.GetCPU(3));

        // specular BRDF lut sampler
        D3D12_SAMPLER_DESC samplerShadow = {
            D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            0.0f,
            1u,
            D3D12_COMPARISON_FUNC_LESS_EQUAL,
            { 0.0f, 0.0f, 0.0f, 0.0f },
            0.0f,
            D3D12_FLOAT32_MAX
        };

        pDevice->CreateSampler(&samplerShadow, m_sampler.GetCPU(4));
    }

    pPrimitive->m_sampler = &m_sampler;
    */
}

CMP_GltfPbrVK::per_batch* CMP_GltfPbrVK::SetPerBatchConstants()
{
    per_batch *cbPerBatch;
    m_pDynamicBufferRing->AllocConstantBuffer(sizeof(per_batch), (void **)&cbPerBatch, &m_perBatchDesc);

    return cbPerBatch;
}

void CMP_GltfPbrVK::DrawMesh(VkCommandBuffer cmd_buf, int meshIndex, const glm::mat4x4& worldMatrix)
{
    CMP_PBRMesh* pMesh = &m_meshes[meshIndex];
    for (unsigned int p = 0; p < pMesh->m_pPrimitives.size(); p++) {
        CMP_PBRPrimitives* pPrimitive = &pMesh->m_pPrimitives[p];

        if (pPrimitive->m_pipeline == VK_NULL_HANDLE)
            continue;

        // Set per Object constants
        //
        per_object *cbPerObject;
        VkDescriptorBufferInfo perObjectDesc;
        m_pDynamicBufferRing->AllocConstantBuffer(sizeof(per_object), (void**)&cbPerObject, &perObjectDesc);
        cbPerObject->mWorld = worldMatrix;
        cbPerObject->u_emissiveFactor = pPrimitive->m_pMaterial->emissiveFactor;
        cbPerObject->u_baseColorFactor = pPrimitive->m_pMaterial->baseColorFactor;
        cbPerObject->u_metallicRoughnessValues = glm::vec4(pPrimitive->m_pMaterial->metallicFactor, pPrimitive->m_pMaterial->roughnessFactor, 0, 0);

        // Compute offsets
        //
        std::uint32_t size = (std::uint32_t)pPrimitive->m_VBV.size();
        std::vector<VkBuffer> buffers(size);
        std::vector<VkDeviceSize> offsets(size);
        for (std::uint32_t i = 0; i < size; i++) {
            buffers[i] = pPrimitive->m_VBV[i].buffer;
            offsets[i] = pPrimitive->m_VBV[i].offset;
        }

        // Set state and draw
        //
        vkCmdBindVertexBuffers(cmd_buf, 0, size, buffers.data(), offsets.data());
        vkCmdBindIndexBuffer(cmd_buf, pPrimitive->m_IBV.buffer, pPrimitive->m_IBV.offset, pPrimitive->m_indexType);

        vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pPrimitive->m_pipeline);

        VkDescriptorSet descritorSets[2] = { pPrimitive->m_descriptorSet, pPrimitive->m_pMaterial->m_descriptorSet };

        uint32_t uniformOffsets[2] = { (uint32_t)m_perBatchDesc.offset,  (uint32_t)perObjectDesc.offset };
        vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pPrimitive->m_pipelineLayout, 0, 2, descritorSets, 2, uniformOffsets);
        vkCmdDrawIndexed(cmd_buf, pPrimitive->m_NumIndices, 1, 0, 0, 0);
    }
}

