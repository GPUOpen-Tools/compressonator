/*
 * Vulkan Samples
 *
 * Copyright (C) 2015-2016 Valve Corporation
 * Copyright (C) 2015-2016 LunarG, Inc.
 * Copyright (C) 2015-2016 Google, Inc.
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
Template sample to start from. Draw textured cube with mostly helpers.
*/

#include <util_init.hpp>
#include <assert.h>
#include <string.h>
#include <cstdlib>
#include "cube_data.h"

/* For this sample, we'll start with GLSL so the shader function is plain */
/* and then use the glslang GLSLtoSPV utility to convert it to SPIR-V for */
/* the driver.  We do this for clarity rather than using pre-compiled     */
/* SPIR-V                                                                 */

const char *vertShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (std140, binding = 0) uniform buf {\n"
    "        mat4 mvp;\n"
    "} ubuf;\n"
    "layout (location = 0) in vec4 pos;\n"
    "layout (location = 1) in vec2 inTexCoords;\n"
    "layout (location = 0) out vec2 texcoord;\n"
    "out gl_PerVertex { \n"
    "    vec4 gl_Position;\n"
    "};\n"
    "void main() {\n"
    "   texcoord = inTexCoords;\n"
    "   gl_Position = ubuf.mvp * pos;\n"
    "}\n";

const char *fragShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (binding = 1) uniform sampler2D tex;\n"
    "layout (location = 0) in vec2 texcoord;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "void main() {\n"
    "   outColor = textureLod(tex, texcoord, 0.0);\n"
    "}\n";

int main(int argc, char *argv[]) {
    VkResult U_ASSERT_ONLY res;
    struct sample_info info = {};
    char sample_title[] = "Draw Textured Cube";
    const bool depthPresent = true;

    process_command_line_args(info, argc, argv);
    init_global_layer_properties(info);
    init_instance_extension_names(info);
    init_device_extension_names(info);
    init_instance(info, sample_title);
    init_enumerate_device(info);
    init_window_size(info, 500, 500);
    init_connection(info);
    init_window(info);
    init_swapchain_extension(info);
    init_device(info);
    init_command_pool(info);
    init_command_buffer(info);
    execute_begin_command_buffer(info);
    init_device_queue(info);
    init_swap_chain(info);
    init_depth_buffer(info);
    init_texture(info);
    init_uniform_buffer(info);
    init_descriptor_and_pipeline_layouts(info, true);
    init_renderpass(info, depthPresent);
    init_shaders(info, vertShaderText, fragShaderText);
    init_framebuffers(info, depthPresent);
    init_vertex_buffer(info, g_vb_texture_Data, sizeof(g_vb_texture_Data),
                       sizeof(g_vb_texture_Data[0]), true);
    init_descriptor_pool(info, true);
    init_descriptor_set(info, true);
    init_pipeline_cache(info);
    init_pipeline(info, depthPresent);
    init_presentable_image(info);

    VkClearValue clear_values[2];
    init_clear_color_and_depth(info, clear_values);

    VkRenderPassBeginInfo rp_begin;
    init_render_pass_begin_info(info, rp_begin);
    rp_begin.clearValueCount = 2;
    rp_begin.pClearValues = clear_values;

    vkCmdBeginRenderPass(info.cmd, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline);
    vkCmdBindDescriptorSets(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            info.pipeline_layout, 0, NUM_DESCRIPTOR_SETS,
                            info.desc_set.data(), 0, NULL);

    const VkDeviceSize offsets[1] = {0};
    vkCmdBindVertexBuffers(info.cmd, 0, 1, &info.vertex_buffer.buf, offsets);

    init_viewports(info);
    init_scissors(info);

    vkCmdDraw(info.cmd, 12 * 3, 1, 0, 0);
    vkCmdEndRenderPass(info.cmd);

    execute_pre_present_barrier(info);

    res = vkEndCommandBuffer(info.cmd);
    assert(res == VK_SUCCESS);

    VkFence drawFence = {};
    init_fence(info, drawFence);
    VkPipelineStageFlags pipe_stage_flags =
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkSubmitInfo submit_info = {};
    init_submit_info(info, submit_info, pipe_stage_flags);

    /* Queue the command buffer for execution */
    res = vkQueueSubmit(info.queue, 1, &submit_info, drawFence);
    assert(res == VK_SUCCESS);

    /* Now present the image in the window */
    VkPresentInfoKHR present = {};
    init_present_info(info, present);

    /* Make sure command buffer is finished before presenting */
    do {
        res =
            vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
    } while (res == VK_TIMEOUT);
    assert(res == VK_SUCCESS);
    res = vkQueuePresentKHR(info.queue, &present);
    assert(res == VK_SUCCESS);

    wait_seconds(1);
    if (info.save_images)
        write_ppm(info, "template");

    vkDestroyFence(info.device, drawFence, NULL);
    vkDestroySemaphore(info.device, info.presentCompleteSemaphore, NULL);
    destroy_pipeline(info);
    destroy_pipeline_cache(info);
    destroy_textures(info);
    destroy_descriptor_pool(info);
    destroy_vertex_buffer(info);
    destroy_framebuffers(info);
    destroy_shaders(info);
    destroy_renderpass(info);
    destroy_descriptor_and_pipeline_layouts(info);
    destroy_uniform_buffer(info);
    destroy_depth_buffer(info);
    destroy_swap_chain(info);
    destroy_command_buffer(info);
    destroy_command_pool(info);
    destroy_device(info);
    destroy_window(info);
    destroy_instance(info);
    return 0;
}
