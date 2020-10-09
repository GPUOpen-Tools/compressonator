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


#include "gltfbboxpassvk.h"

#include "devicevk.h"
#include "staticbufferpoolvk.h"
#include "dynamicbufferringvk.h"
#include "resourceviewheapsvk.h"
#include "uploadheapvk.h"
#include "shadercompilerhelper.h"
#include "camera.h"
#include "gltfhelpers.h"
#include "gltfhelpers_vulkan.h"


void GltfBBoxPassVK::OnCreate(
    DeviceVK* pDevice,
    VkRenderPass renderPass,
    UploadHeapVK* pUploadHeap,
    ResourceViewHeapsVK *pHeaps,
    DynamicBufferRingVK *pDynamicBufferRing,
    StaticBufferPoolVK *pStaticBufferPool,
    GLTFCommon *pGLTFData) {
    VkResult res;

    m_pGLTFData = pGLTFData;
    m_pDynamicBufferRing = pDynamicBufferRing;
    m_pResourceViewHeaps = pHeaps;
    m_pStaticBufferPool = pStaticBufferPool;

    // set indices
    {
        short indices[] = {
            0,1, 1,2, 2,3, 3,0,
            4,5, 5,6, 6,7, 7,4,
            0,4,
            1,5,
            2,6,
            3,7
        };
        m_NumIndices = _countof(indices);

        m_indexType = VK_INDEX_TYPE_UINT16;

        void *pDest;
        m_pStaticBufferPool->AllocIndexBuffer(m_NumIndices, sizeof(short), &pDest, &m_IBV);
        memcpy(pDest, indices, sizeof(short)*m_NumIndices);
    }

    // set vertices
    {
        float vertices[] = {
            -1,  -1,  1,  1,
                1,  -1,  1,  1,
                1,   1,  1,  1,
                -1,   1,  1,  1,
                -1,  -1, -1,  1,
                1,  -1, -1,  1,
                1,   1, -1,  1,
                -1,   1, -1,  1,
            };

        void *pDest;
        m_pStaticBufferPool->AllocVertexBuffer(8, 4 * sizeof(float), &pDest, &m_VBV);
        memcpy(pDest, vertices, sizeof(float) * 4 * 8);
    }

    // the vertex shader
    static const char* vertexShader =
        "#version 400\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "layout (std140, binding = 0) uniform _cbPerObject\n"
        "{\n"
        "    mat4        u_mWorldViewProj;\n"
        "    vec4        u_Center;\n"
        "    vec4        u_Radius;\n"
        "    vec4        u_Color;\n"
        "} cbPerObject;\n"
        "layout(location = 0) in vec4 position; \n"
        "layout (location = 0) out vec4 outColor;\n"
        "void main() {\n"
        "   outColor = cbPerObject.u_Color;\n"
        "   gl_Position = cbPerObject.u_mWorldViewProj * (cbPerObject.u_Center + position * vec4(cbPerObject.u_Radius.xyz,1.0f));\n"
        "}\n";

    // the pixel shader
    static const char* pixelShader =
        "#version 400\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "layout (location = 0) in vec4 inColor;\n"
        "layout (location = 0) out vec4 outColor;\n"
        "void main() {\n"
        "   outColor = inColor;\n"
        "}";

    /////////////////////////////////////////////
    // Compile and create shaders

    init_glslang();

    std::map<std::string, std::string> attributeDefines;

    VkPipelineShaderStageCreateInfo m_vertexShader;
    res = VKCompile(pDevice->GetDevice(), SST_GLSL, VK_SHADER_STAGE_VERTEX_BIT, vertexShader, "main", attributeDefines, &m_vertexShader);
    assert(res == VK_SUCCESS);

    VkPipelineShaderStageCreateInfo m_fragmentShader;
    res = VKCompile(pDevice->GetDevice(), SST_GLSL, VK_SHADER_STAGE_FRAGMENT_BIT, pixelShader, "main", attributeDefines, &m_fragmentShader);
    assert(res == VK_SUCCESS);

    finalize_glslang();

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { m_vertexShader, m_fragmentShader };

    /////////////////////////////////////////////
    // Create descriptor set and the pipeline layout

    std::vector<VkDescriptorSetLayoutBinding> layout_bindings(1);
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    layout_bindings[0].pImmutableSamplers = NULL;

    m_pResourceViewHeaps->AllocDescriptor(&layout_bindings, &m_descriptorSetLayout, &m_descriptorSet);

    VkWriteDescriptorSet writes[1];
    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = NULL;
    writes[0].dstSet = m_descriptorSet;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    writes[0].pBufferInfo = &m_pDynamicBufferRing->GetMainBuffer(sizeof(per_object));
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;

    vkUpdateDescriptorSets(pDevice->GetDevice(), 1, writes, 0, NULL);

    std::vector<VkDescriptorSetLayout> descriptorSetLayout = { m_descriptorSetLayout };

    /* Now use the descriptor layout to create a pipeline layout */
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = NULL;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges = NULL;
    pPipelineLayoutCreateInfo.setLayoutCount = (uint32_t)descriptorSetLayout.size();
    pPipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayout.data();

    res = vkCreatePipelineLayout(pDevice->GetDevice(), &pPipelineLayoutCreateInfo, NULL, &m_pipelineLayout);
    assert(res == VK_SUCCESS);

    /////////////////////////////////////////////
    // Create pipeline

    // Create the input attribute description / input layout(in DX12 jargon)

    VkVertexInputBindingDescription vi_binding = {};
    vi_binding.binding = 0;
    vi_binding.stride = sizeof(float) * 4;
    vi_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> vi_attrs(1);
    // Position
    vi_attrs[0].location = 0;
    vi_attrs[0].binding = 0;
    vi_attrs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vi_attrs[0].offset = 0;

    VkPipelineVertexInputStateCreateInfo vi = {};
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vi.pNext = NULL;
    vi.flags = 0;
    vi.vertexBindingDescriptionCount = 1;
    vi.pVertexBindingDescriptions = &vi_binding;
    vi.vertexAttributeDescriptionCount = (uint32_t)vi_attrs.size();
    vi.pVertexAttributeDescriptions = vi_attrs.data();

    // input assembly state

    VkPipelineInputAssemblyStateCreateInfo ia = {};
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.pNext = NULL;
    ia.flags = 0;
    ia.primitiveRestartEnable = VK_FALSE;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

    // rasterizer state

    VkPipelineRasterizationStateCreateInfo rs = {};
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.pNext = NULL;
    rs.flags = 0;
    rs.polygonMode = VK_POLYGON_MODE_LINE;
    rs.cullMode = VK_CULL_MODE_NONE;
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
    res = vkCreatePipelineCache(pDevice->GetDevice(), &pipelineCache, NULL, &m_pipelineCache);
    assert(res == VK_SUCCESS);

    // create pipeline

    VkGraphicsPipelineCreateInfo pipeline = {};
    pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline.pNext = NULL;
    pipeline.layout = m_pipelineLayout;
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

    res = vkCreateGraphicsPipelines(pDevice->GetDevice(), m_pipelineCache, 1, &pipeline, NULL, &m_pipeline);
    assert(res == VK_SUCCESS);

}

void GltfBBoxPassVK::OnDestroy() {
}

void GltfBBoxPassVK::DrawMesh(VkCommandBuffer cmd_buf, int meshIndex, const glm::mat4x4& worldMatrix) {
    vkCmdBindVertexBuffers(cmd_buf, 0, 1, &m_VBV.buffer, &m_VBV.offset);
    vkCmdBindIndexBuffer(cmd_buf, m_IBV.buffer, m_IBV.offset, m_indexType);

    vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    VkDescriptorSet descritorSets[1] = { m_descriptorSet };

    glm::mat4x4 mWorldViewProj = worldMatrix * m_Camera;

    tfMesh *pMesh = &m_pGLTFData->m_meshes[meshIndex];
    for (unsigned int p = 0; p < pMesh->m_pPrimitives.size(); p++) {
        // Set per Object constants
        //
        per_object *cbPerObject;
        VkDescriptorBufferInfo perObjectDesc;
        m_pDynamicBufferRing->AllocConstantBuffer(sizeof(per_object), (void **)&cbPerObject, &perObjectDesc);
        cbPerObject->mWorldViewProj = mWorldViewProj;
        cbPerObject->vCenter = pMesh->m_pPrimitives[p].m_center;
        cbPerObject->vRadius = pMesh->m_pPrimitives[p].m_radius;
        cbPerObject->vColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

        uint32_t uniformOffsets[1] = { (uint32_t)perObjectDesc.offset };
        vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, descritorSets, 1, uniformOffsets);

        vkCmdDrawIndexed(cmd_buf, m_NumIndices, 1, 0, 0, 0);
    }

}

