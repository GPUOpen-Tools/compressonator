/*
 * Vulkan Samples
 *
 * Copyright (C) 2015-2016 Valve Corporation
 * Copyright (C) 2015-2016 LunarG, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
VULKAN_SAMPLE_SHORT_DESCRIPTION
Initialize Vertex and Fragment Shaders
*/

/* This is part of the draw cube progression */

#include <util_init.hpp>
#include <assert.h>
#include <string.h>
#include <cstdlib>

int main(int argc, char *argv[]) {
    VkResult U_ASSERT_ONLY res;
    struct sample_info info = {};
    char sample_title[] = "Initialize Shaders Sample";
    bool U_ASSERT_ONLY retVal;

    init_global_layer_properties(info);
    init_instance(info, sample_title);
    init_enumerate_device(info);
    init_queue_family_index(info);
    init_device(info);

    /* For this sample, we'll start with GLSL so the shader function is plain */
    /* and then use the glslang GLSLtoSPV utility to convert it to SPIR-V for */
    /* the driver.  We do this for clarity rather than using pre-compiled     */
    /* SPIR-V                                                                 */

    /* VULKAN_KEY_START */
    static const char *vertShaderText =
        "#version 400\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "layout (std140, binding = 0) uniform bufferVals {\n"
        "    mat4 mvp;\n"
        "} myBufferVals;\n"
        "layout (location = 0) in vec4 pos;\n"
        "layout (location = 1) in vec4 inColor;\n"
        "layout (location = 0) out vec4 outColor;\n"
        "out gl_PerVertex { \n"
        "    vec4 gl_Position;\n"
        "};\n"
        "void main() {\n"
        "   outColor = inColor;\n"
        "   gl_Position = myBufferVals.mvp * pos;\n"
        "}\n";

    static const char *fragShaderText =
        "#version 400\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#extension GL_ARB_shading_language_420pack : enable\n"
        "layout (location = 0) in vec4 color;\n"
        "layout (location = 0) out vec4 outColor;\n"
        "void main() {\n"
        "   outColor = color;\n"
        "}\n";

    std::vector<unsigned int> vtx_spv;
    info.shaderStages[0].sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.shaderStages[0].pNext = NULL;
    info.shaderStages[0].pSpecializationInfo = NULL;
    info.shaderStages[0].flags = 0;
    info.shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    info.shaderStages[0].pName = "main";

    init_glslang();
    retVal = GLSLtoSPV(VK_SHADER_STAGE_VERTEX_BIT, vertShaderText, vtx_spv);
    assert(retVal);

    VkShaderModuleCreateInfo moduleCreateInfo;
    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = vtx_spv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = vtx_spv.data();
    res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL,
                               &info.shaderStages[0].module);
    assert(res == VK_SUCCESS);

    std::vector<unsigned int> frag_spv;
    info.shaderStages[1].sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.shaderStages[1].pNext = NULL;
    info.shaderStages[1].pSpecializationInfo = NULL;
    info.shaderStages[1].flags = 0;
    info.shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    info.shaderStages[1].pName = "main";

    retVal = GLSLtoSPV(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderText, frag_spv);
    assert(retVal);

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;
    moduleCreateInfo.flags = 0;
    moduleCreateInfo.codeSize = frag_spv.size() * sizeof(unsigned int);
    moduleCreateInfo.pCode = frag_spv.data();
    res = vkCreateShaderModule(info.device, &moduleCreateInfo, NULL,
                               &info.shaderStages[1].module);
    assert(res == VK_SUCCESS);

    finalize_glslang();
    /* VULKAN_KEY_END */

    vkDestroyShaderModule(info.device, info.shaderStages[0].module, NULL);
    vkDestroyShaderModule(info.device, info.shaderStages[1].module, NULL);
    destroy_device(info);
    destroy_instance(info);
    return 0;
}
