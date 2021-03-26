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

#include "gltfdepthpassvk.h"

#include "dynamicbufferringvk.h"
#include "gltfcommon.h"
#include "gltfhelpers.h"
#include "gltfhelpers_vulkan.h"
#include "resourceviewheapsvk.h"
#include "shadercompilerhelper.h"
#include "texturevk.h"
#include "threadpool.h"
#include "uploadheapvk.h"

#include <vector>

void GltfDepthPass::OnCreate(
    DeviceVK* pDevice,
    VkRenderPass renderPass,
    UploadHeapVK* pUploadHeap,
    ResourceViewHeapsVK *pHeaps,
    DynamicBufferRingVK *pDynamicBufferRing,
    StaticBufferPoolVK *pStaticBufferPool,
    GLTFCommon *pGLTFData, void *pluginManager, void *msghandler) {
    m_pDevice = pDevice;
    m_pGLTFData = pGLTFData;
    m_pDynamicBufferRing = pDynamicBufferRing;
    m_pResourceViewHeaps = pHeaps;
    m_pStaticBufferPool = pStaticBufferPool;

    json &j3 = pGLTFData->j3;

    // Load Textures (A cache should take care of deduplication)
    //
    auto images = j3["images"];
    if (images.size() > 0) {
        m_textures.resize(images.size());
        for (unsigned int i = 0; i<images.size(); i++) {
            std::string filename = images[i]["uri"];

            INT32 result = m_textures[i].InitFromFile(pDevice, pUploadHeap, (pGLTFData->m_path + filename).c_str(), pluginManager, msghandler);
        }
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

    // Create materials (in a depth pass materials are still needed to handle non opaque textures
    //
    std::vector<DepthMaterial *> materialsData;
    auto materials = j3["materials"];
    auto textures = j3["textures"];
    if ((materials.size() > 0) && (textures.size() > 0)) {
        for (unsigned int i = 0; i < materials.size(); i++) {
            json::object_t material = materials[i];

            DepthMaterial *tfmat = new DepthMaterial();
            materialsData.push_back(tfmat);

            // Load material constants. This is a depth pass and we are only interested in the txtures that are transparent
            //
            std::string opaque = GetElementString(material, "alphaMode", "OPAQUE");
            if (opaque != "OPAQUE") {
                tfmat->m_defines["DEF_alphaMode_" + GetElementString(material, "alphaMode", "OPAQUE")] = 1;
                tfmat->m_defines["DEF_alphaCutoff"] = std::to_string(GetElementFloat(material, "alphaCutoff", 1.0));
                tfmat->m_doubleSided = GetElementBoolean(material, "doubleSided", false);

                // If transparent create glTF 2.0 baseColorTexture SRV
                //
                if (textures.size() > 0) {
                    int id = GetElementInt(material, "pbrMetallicRoughness/baseColorTexture/index", -1);
                    if (id > 0) {
                        int tex = textures[id]["source"];
                        Texture *pTex = &m_textures[tex];

                        VkDescriptorSetLayoutBinding layout_binding;
                        layout_binding.binding = 0;
                        layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        layout_binding.descriptorCount = 1;
                        layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                        layout_binding.pImmutableSamplers = NULL;

                        VkWriteDescriptorSet writes;
                        VkDescriptorImageInfo desc_image;

                        desc_image.sampler = m_sampler;
                        desc_image.imageView = VK_NULL_HANDLE;
                        desc_image.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                        writes = {};
                        writes.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        writes.dstBinding = 0;
                        writes.pNext = NULL;
                        writes.dstSet = tfmat->m_descriptorSet;
                        writes.descriptorCount = 1;
                        writes.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                        writes.pImageInfo = &desc_image;
                        writes.dstArrayElement = 0;

                        pTex->CreateSRV(0, &desc_image.imageView);

                        tfmat->m_textureCount = 1;
                        tfmat->m_defines["ID_baseColorTexture"] = "0";
                    }
                }
            }
        }
    }

    // Load Meshes
    //
    auto accessors = j3["accessors"];
    auto bufferViews = j3["bufferViews"];
    auto meshes = j3["meshes"];
    m_meshes.resize(meshes.size());
    for (unsigned int i = 0; i < meshes.size(); i++) {
        DepthMesh* tfmesh = &m_meshes[i];

        auto primitives = meshes[i]["primitives"];
        tfmesh->m_pPrimitives.resize(primitives.size());
        for (unsigned int p = 0; p < primitives.size(); p++) {
            DepthPrimitives* pPrimitive = &tfmesh->m_pPrimitives[p];
            auto primitive = primitives[p];

            // Set Material
            //
            pPrimitive->m_pMaterial = materialsData[primitive["material"]];

            // Set Index buffer
            //
            tfAccessor indexBuffer;
            {
                auto indicesAccessor = accessors[primitive["indices"].get<int>()];
                GetBufferDetails(indicesAccessor, bufferViews, pGLTFData->buffersData, &indexBuffer);
            }

            // Get input layout from glTF attributes
            //
            std::vector<tfAccessor> vertexBuffers;
            std::vector<std::string> semanticNames;
            std::vector<VkVertexInputAttributeDescription> attributes;

            auto attribute = primitives[p]["attributes"];
            attributes.resize(attribute.size());
            vertexBuffers.resize(attribute.size());
            semanticNames.resize(attribute.size());

            int cnt = 0;
            for (auto it = attribute.begin(); it != attribute.end(); it++) {
                // the glTF attributes name may end in a number, DX12 doest like this and if this is the case we need to split the attribute name from the number
                //
                CMP_DWORD semanticIndex = 0;
                std::string semanticName;
                SplitGltfAttribute(it.key(), &semanticName, &semanticIndex);

                // We are only interested in the position or the texcoords (that is if the geoemtry uses a transparent material)
                //
                if (semanticName != "Position") {
                    if (pPrimitive->m_pMaterial->m_defines.find("DEF_alphaMode_OPAQUE") != pPrimitive->m_pMaterial->m_defines.end())
                        if (semanticName != "TEXCOORD")
                            continue;
                }

                semanticNames[cnt] = it.key();

                auto accessor = accessors[it.value().get<int>()];

                // Get VB accessors
                //
                GetBufferDetails(accessor, bufferViews, pGLTFData->buffersData, &vertexBuffers[cnt]);

                // Create Input Layout
                //
                VkVertexInputAttributeDescription l;
                l.location = (uint32_t)cnt;
                l.format = GetFormat_Vulkan(accessor["type"], accessor["componentType"]);
                l.offset = 0;
                l.binding = cnt;
                attributes[cnt] = l;

                cnt++;
            }

            CreateGeometry(indexBuffer, vertexBuffers, pPrimitive);
            //GetThreadPool()->Add_Job([=]()
            //{
            CreatePipeline(pDevice, renderPass, semanticNames, attributes, pPrimitive);
            //});
        }
    }
}

void GltfDepthPass::OnDestroy() {
    for (unsigned int i = 0; i < m_textures.size(); i++) {
        m_textures[i].OnDestroy();
    }
}

void GltfDepthPass::CreatePipeline(DeviceVK* pDevice, VkRenderPass renderPass, std::vector<std::string> semanticNames, std::vector<VkVertexInputAttributeDescription> layout, DepthPrimitives* pPrimitive) {
    VkResult res;

    // let vertex shader know what buffers are present
    std::map<std::string, std::string> attributeDefines;
    for (unsigned int i = 0; i < semanticNames.size(); i++) {
        attributeDefines[std::string("ID_") + semanticNames[i]] = "1";
    }

    /////////////////////////////////////////////
    // Compile and create shaders

    init_glslang();

    VkPipelineShaderStageCreateInfo m_vertexShader;
    res = VKCompileFromFile(pDevice->GetDevice(), SST_GLSL, VK_SHADER_STAGE_VERTEX_BIT, "./plugins/shaders/shadows-vert_vk.glsl", "main", attributeDefines, &m_vertexShader);
    //res = VKCompileFromFile(pDevice->GetDevice(), SST_HLSL, VK_SHADER_STAGE_VERTEX_BIT, "./plugins/shaders/glTF20_vk.hlsl", "mainVS", attributeDefines, &m_vertexShader);
    assert(res == VK_SUCCESS);

    VkPipelineShaderStageCreateInfo m_fragmentShader;
    res = VKCompileFromFile(pDevice->GetDevice(), SST_GLSL, VK_SHADER_STAGE_FRAGMENT_BIT, "./plugins/shaders/shadows-frag_vk.glsl", "main", attributeDefines, &m_fragmentShader);
    assert(res == VK_SUCCESS);

    finalize_glslang();

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { m_vertexShader, m_fragmentShader };

    bool bUsingTransparency = (pPrimitive->m_pMaterial->m_textureCount > 0);

    /////////////////////////////////////////////
    // Create pipeline layout

    std::vector<VkDescriptorSetLayoutBinding> layout_bindings(2);
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_bindings[0].pImmutableSamplers = NULL;

    layout_bindings[1].binding = 1;
    layout_bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    layout_bindings[1].descriptorCount = 1;
    layout_bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
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

    std::vector<VkDescriptorSetLayout> descriptorSetLayout = { pPrimitive->m_descriptorSetLayout };

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

    VkPipelineInputAssemblyStateCreateInfo ia = {};
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.pNext = NULL;
    ia.flags = 0;
    ia.primitiveRestartEnable = VK_FALSE;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // rasterizer state

    VkPipelineRasterizationStateCreateInfo rs = {};
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

    VkPipelineColorBlendStateCreateInfo cb = {};
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cb.flags = 0;
    cb.pNext = NULL;
    cb.attachmentCount = 0; //set to 1 when transparency
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

    VkPipelineDepthStencilStateCreateInfo ds = {};
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

    VkPipelineMultisampleStateCreateInfo ms = {};
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

    VkPipelineCacheCreateInfo pipelineCache = {};
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
}

GltfDepthPass::per_batch* GltfDepthPass::SetPerBatchConstants() {
    GltfDepthPass::per_batch* cbPerBatch;
    m_pDynamicBufferRing->AllocConstantBuffer(sizeof(GltfDepthPass::per_batch), (void**)&cbPerBatch, &m_perBatchDesc);

    return cbPerBatch;
}

void GltfDepthPass::DrawMesh(VkCommandBuffer cmd_buf, int meshIndex, const glm::mat4x4& worldMatrix) {
    DepthMesh* pMesh = &m_meshes[meshIndex];
    for (unsigned int p = 0; p < pMesh->m_pPrimitives.size(); p++) {
        DepthPrimitives* pPrimitive = &pMesh->m_pPrimitives[p];

        if (pPrimitive->m_pipeline == NULL)
            continue;

        // Set per Object constants
        //
        per_object* cbPerObject;
        VkDescriptorBufferInfo perObjectDesc = {};
        m_pDynamicBufferRing->AllocConstantBuffer(sizeof(per_object), (void**)&cbPerObject, &perObjectDesc);
        cbPerObject->mWorld = worldMatrix;

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

        VkDescriptorSet descritorSets[1] = { pPrimitive->m_descriptorSet };

        uint32_t uniformOffsets[2] = { (uint32_t)m_perBatchDesc.offset, (uint32_t)perObjectDesc.offset };
        vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pPrimitive->m_pipelineLayout, 0, 1, descritorSets, 2, uniformOffsets);
        vkCmdDrawIndexed(cmd_buf, pPrimitive->m_NumIndices, 1, 0, 0, 0);
    }
}
