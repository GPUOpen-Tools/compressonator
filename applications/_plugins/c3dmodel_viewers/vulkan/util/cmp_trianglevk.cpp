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

#include "cmp_trianglevk.h"

#include "cmp_shadercompilerhelpervk.h"

#include <glm/mat4x4.hpp>

#include <vector>


void TriangleVK::OnCreate(CMP_DeviceVK* pDevice, CMP_DynamicBufferRingVK* pConstantBufferRing, CMP_StaticBufferPoolVK* pStaticGeom, VkRenderPass renderPass)
{
    m_pDevice = pDevice;
    m_pConstantBufferRing = pConstantBufferRing;

    VkResult res;

    struct Vertex {
        float posX, posY, posZ, posW;  // Position data
        float r, g, b, a;              // Color
    };
#define XYZ1(_x_, _y_, _z_) (_x_), (_y_), (_z_), 1.f

    const Vertex g_vb_solid_face_colors_Data[] = {
        // red face
        { XYZ1(-1, -1, 1), XYZ1(1.f, 0.f, 0.f) },
        { XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f) },
        { XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f) },
        { XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f) },
        { XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f) },
        { XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 0.f) },
        // green face
        { XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 0.f) },
        { XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f) },
        { XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f) },
        { XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f) },
        { XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f) },
        { XYZ1(1, 1, -1), XYZ1(0.f, 1.f, 0.f) },
        // blue face
        { XYZ1(-1, 1, 1), XYZ1(0.f, 0.f, 1.f) },
        { XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f) },
        { XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f) },
        { XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f) },
        { XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f) },
        { XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 1.f) },
        // yellow face
        { XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 0.f) },
        { XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f) },
        { XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f) },
        { XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f) },
        { XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f) },
        { XYZ1(1, -1, -1), XYZ1(1.f, 1.f, 0.f) },
        // magenta face
        { XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 1.f) },
        { XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f) },
        { XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f) },
        { XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f) },
        { XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f) },
        { XYZ1(-1, 1, -1), XYZ1(1.f, 0.f, 1.f) },
        // cyan face
        { XYZ1(1, -1, 1), XYZ1(0.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f) },
        { XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f) },
        { XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f) },
        { XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f) },
        { XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 1.f) },
    };

    int vertices = 6 * 6;
    int vertexSize = 8 * sizeof(float);

    void *pData;
    pStaticGeom->AllocVertexBuffer(vertices, vertexSize, &pData, &m_geometry);
    memcpy(pData, g_vb_solid_face_colors_Data, sizeof(g_vb_solid_face_colors_Data));

    ///////////////////////////////////////////////
    // shaders
    const char *vertShaderText =
        "#version 400\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "layout (std140, binding = 0) uniform bufferVals {\n"
        "    mat4 mvp;\n"
        "} myBufferVals;\n"
        "layout (location = 0) in vec4 pos;\n"
        "layout (location = 1) in vec4 inColor;\n"
        "layout (location = 0) out vec4 outColor;\n"
        "void main() {\n"
        "   outColor = inColor;\n"
        "   gl_Position = myBufferVals.mvp * pos;\n"
        "}\n";

    const char *fragShaderText =
        "#version 400\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "layout (location = 0) in vec4 color;\n"
        "layout (location = 0) out vec4 outColor;\n"
        "void main() {\n"
        "   outColor = color;\n"
        "}\n";

    /////////////////////////////////////////////
    // Compile and create shaders

    init_glslang();

    std::map<std::string, std::string> attributeDefines;

    VkPipelineShaderStageCreateInfo m_vertexShader;
    res = VKCompile(pDevice->GetDevice(), SST_GLSL, VK_SHADER_STAGE_VERTEX_BIT, vertShaderText, "main", attributeDefines, &m_vertexShader);
    assert(res == VK_SUCCESS);

    VkPipelineShaderStageCreateInfo m_fragmentShader;
    res = VKCompile(pDevice->GetDevice(), SST_GLSL, VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderText, "main", attributeDefines, &m_fragmentShader);
    assert(res == VK_SUCCESS);

    finalize_glslang();

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { m_vertexShader, m_fragmentShader };

    /////////////////////////////////////////////
    // Create descriptor pool

    std::vector<VkDescriptorPoolSize> type_count = { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1} };

    VkDescriptorPoolCreateInfo descriptor_pool = {};
    descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool.pNext = NULL;
    descriptor_pool.maxSets = 1;
    descriptor_pool.poolSizeCount = (uint32_t)type_count.size();
    descriptor_pool.pPoolSizes = type_count.data();

    res = vkCreateDescriptorPool(pDevice->GetDevice(), &descriptor_pool, NULL, &m_descriptorPool);
    assert(res == VK_SUCCESS);

    /////////////////////////////////////////////
    // Create pipeline layout

    VkDescriptorSetLayoutBinding layout_bindings[1];
    layout_bindings[0].binding = 0;
    layout_bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_bindings[0].descriptorCount = 1;
    layout_bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_bindings[0].pImmutableSamplers = NULL;

    // Next take layout bindings and use them to create a descriptor set layout
    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
    descriptor_layout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.pNext = NULL;
    descriptor_layout.flags = 0;
    descriptor_layout.bindingCount = 1;
    descriptor_layout.pBindings = layout_bindings;

    std::vector<VkDescriptorSetLayout> desc_layout;
    desc_layout.resize(1);
    res = vkCreateDescriptorSetLayout(pDevice->GetDevice(), &descriptor_layout, NULL, desc_layout.data());
    assert(res == VK_SUCCESS);

    /* Now use the descriptor layout to create a pipeline layout */
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = NULL;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges = NULL;
    pPipelineLayoutCreateInfo.setLayoutCount = (uint32_t)desc_layout.size();
    pPipelineLayoutCreateInfo.pSetLayouts = desc_layout.data();

    res = vkCreatePipelineLayout(pDevice->GetDevice(), &pPipelineLayoutCreateInfo, NULL, &m_pipelineLayout);
    assert(res == VK_SUCCESS);

    /////////////////////////////////////////////
    // Create descriptor sets
    VkDescriptorSetAllocateInfo alloc_info[1];
    alloc_info[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info[0].pNext = NULL;
    alloc_info[0].descriptorPool = m_descriptorPool;
    alloc_info[0].descriptorSetCount = (uint32_t)desc_layout.size();
    alloc_info[0].pSetLayouts = desc_layout.data();

    m_descriptorSets.resize(desc_layout.size());
    res = vkAllocateDescriptorSets(pDevice->GetDevice(), alloc_info, m_descriptorSets.data());
    assert(res == VK_SUCCESS);

    /////////////////////////////////////////////
    // Create pipeline

    // vertex input state

    VkVertexInputBindingDescription vi_binding = {};
    vi_binding.binding = 0;
    vi_binding.stride = sizeof(float) * 8;
    vi_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> vi_attrs(2);
    // Position
    vi_attrs[0].location = 0;
    vi_attrs[0].binding = 0;
    vi_attrs[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vi_attrs[0].offset = 0;
    // Normal
    vi_attrs[1].location = 1;
    vi_attrs[1].binding = 0;
    vi_attrs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vi_attrs[1].offset = sizeof(float) * 4;

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
    att_state[0].blendEnable = VK_FALSE;
    att_state[0].alphaBlendOp = VK_BLEND_OP_ADD;
    att_state[0].colorBlendOp = VK_BLEND_OP_ADD;
    att_state[0].srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    att_state[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
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

void TriangleVK::OnDestroy() {
    m_geometry;
    m_pipelineLayout;
    m_pipelineCache;
    m_descriptorPool;
    m_pipeline;
}

void TriangleVK::Render(VkCommandBuffer cmd_buf, CMP_Camera* pCam)
{
    struct DATA {
        glm::mat4x4 mat;
    } *pData;
    VkDescriptorBufferInfo constantBuffer;
    m_pConstantBufferRing->AllocConstantBuffer(4*4 * sizeof(float), (void**)&pData, &constantBuffer);
    pData->mat = pCam->GetView() * pCam->GetProjection();


    VkWriteDescriptorSet writes[1];
    writes[0] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].pNext = NULL;
    writes[0].dstSet = m_descriptorSets[0];
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &constantBuffer;
    writes[0].dstArrayElement = 0;
    writes[0].dstBinding = 0;
    vkUpdateDescriptorSets(m_pDevice->GetDevice(), 1, writes, 0, NULL);

    const VkDeviceSize offsets[1] = { m_geometry.offset };
    vkCmdBindVertexBuffers(cmd_buf, 0, 1, &m_geometry.buffer, offsets);

    vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, (uint32_t)m_descriptorSets.size(), m_descriptorSets.data(), 0, NULL);
    vkCmdDraw(cmd_buf, 12 * 3, 1, 0, 0);
}
