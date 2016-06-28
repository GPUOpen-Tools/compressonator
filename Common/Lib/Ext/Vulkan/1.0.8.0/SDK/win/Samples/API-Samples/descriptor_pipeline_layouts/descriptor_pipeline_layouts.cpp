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
Create Descriptor Layout and Pipeline Layout
*/

/* This is part of the draw cube progression */

#include <util_init.hpp>
#include <assert.h>
#include <cstdlib>

int main(int argc, char *argv[]) {
    VkResult U_ASSERT_ONLY res;
    struct sample_info info = {};
    char sample_title[] = "Descriptor / Pipeline Layout Sample";

    init_global_layer_properties(info);
    init_instance(info, sample_title);
    init_enumerate_device(info);
    init_queue_family_index(info);
    init_device(info);

    /* VULKAN_KEY_START */
    /* Start with just our uniform buffer that has our transformation matrices
     * (for the vertex shader). The fragment shader we intend to use needs no
     * external resources, so nothing else is necessary
     */

    /* Note that when we start using textures, this is where our sampler will
     * need to be specified
     */
    VkDescriptorSetLayoutBinding layout_binding = {};
    layout_binding.binding = 0;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layout_binding.pImmutableSamplers = NULL;

    /* Next take layout bindings and use them to create a descriptor set layout
     */
    VkDescriptorSetLayoutCreateInfo descriptor_layout = {};
    descriptor_layout.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_layout.pNext = NULL;
    descriptor_layout.bindingCount = 1;
    descriptor_layout.pBindings = &layout_binding;

    info.desc_layout.resize(NUM_DESCRIPTOR_SETS);
    res = vkCreateDescriptorSetLayout(info.device, &descriptor_layout, NULL,
                                      info.desc_layout.data());
    assert(res == VK_SUCCESS);

    /* Now use the descriptor layout to create a pipeline layout */
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.pNext = NULL;
    pPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pPipelineLayoutCreateInfo.pPushConstantRanges = NULL;
    pPipelineLayoutCreateInfo.setLayoutCount = NUM_DESCRIPTOR_SETS;
    pPipelineLayoutCreateInfo.pSetLayouts = info.desc_layout.data();

    res = vkCreatePipelineLayout(info.device, &pPipelineLayoutCreateInfo, NULL,
                                 &info.pipeline_layout);
    assert(res == VK_SUCCESS);
    /* VULKAN_KEY_END */

    for (int i = 0; i < NUM_DESCRIPTOR_SETS; i++)
        vkDestroyDescriptorSetLayout(info.device, info.desc_layout[i], NULL);
    vkDestroyPipelineLayout(info.device, info.pipeline_layout, NULL);
    destroy_device(info);
    destroy_instance(info);
    return 0;
}
