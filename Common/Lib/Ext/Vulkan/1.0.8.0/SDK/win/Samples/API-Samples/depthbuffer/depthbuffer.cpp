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
create Vulkan depth buffer
*/

/* This is part of the draw cube progression */

#include <util_init.hpp>
#include <assert.h>
#include <cstdlib>

int main(int argc, char *argv[]) {
    VkResult U_ASSERT_ONLY res;
    bool U_ASSERT_ONLY pass;

    struct sample_info info = {};
    char sample_title[] = "Depth Buffer Sample";

    /*
     * Make a depth buffer:
     * - Create an Image to be the depth buffer
     * - Find memory requirements
     * - Allocate and bind memory
     * - Set the image layout
     * - Create an attachment view
     */

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

    /* VULKAN_KEY_START */
    VkImageCreateInfo image_info = {};
    const VkFormat depth_format = VK_FORMAT_D16_UNORM;
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(info.gpus[0], depth_format, &props);
    if (props.linearTilingFeatures &
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_LINEAR;
    } else if (props.optimalTilingFeatures &
               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    } else {
        /* Try other depth formats? */
        std::cout << "VK_FORMAT_D16_UNORM Unsupported.\n";
        exit(-1);
    }

    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = NULL;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = depth_format;
    image_info.extent.width = info.width;
    image_info.extent.height = info.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = NUM_SAMPLES;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = NULL;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.flags = 0;

    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext = NULL;
    mem_alloc.allocationSize = 0;
    mem_alloc.memoryTypeIndex = 0;

    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.pNext = NULL;
    view_info.image = VK_NULL_HANDLE;
    view_info.format = depth_format;
    view_info.components.r = VK_COMPONENT_SWIZZLE_R;
    view_info.components.g = VK_COMPONENT_SWIZZLE_G;
    view_info.components.b = VK_COMPONENT_SWIZZLE_B;
    view_info.components.a = VK_COMPONENT_SWIZZLE_A;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.flags = 0;

    VkMemoryRequirements mem_reqs;

    info.depth.format = depth_format;

    /* Create image */
    res = vkCreateImage(info.device, &image_info, NULL, &info.depth.image);
    assert(res == VK_SUCCESS);

    vkGetImageMemoryRequirements(info.device, info.depth.image, &mem_reqs);

    mem_alloc.allocationSize = mem_reqs.size;
    /* Use the memory properties to determine the type of memory required */
    pass = memory_type_from_properties(info, mem_reqs.memoryTypeBits,
                                       0, /* No Requirements */
                                       &mem_alloc.memoryTypeIndex);
    assert(pass);

    /* Allocate memory */
    res = vkAllocateMemory(info.device, &mem_alloc, NULL, &info.depth.mem);
    assert(res == VK_SUCCESS);

    /* Bind memory */
    res = vkBindImageMemory(info.device, info.depth.image, info.depth.mem, 0);
    assert(res == VK_SUCCESS);

    /* Set the image layout to depth stencil optimal */
    set_image_layout(info, info.depth.image, VK_IMAGE_ASPECT_DEPTH_BIT,
                     VK_IMAGE_LAYOUT_UNDEFINED,
                     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    /* Create image view */
    view_info.image = info.depth.image;
    res = vkCreateImageView(info.device, &view_info, NULL, &info.depth.view);
    assert(res == VK_SUCCESS);
    execute_end_command_buffer(info);
    execute_queue_command_buffer(info);

    /* VULKAN_KEY_END */

    /* Clean Up */

    vkDestroyImageView(info.device, info.depth.view, NULL);
    vkDestroyImage(info.device, info.depth.image, NULL);
    vkFreeMemory(info.device, info.depth.mem, NULL);
    destroy_command_buffer(info);
    destroy_command_pool(info);
    destroy_device(info);
    destroy_window(info);
    destroy_instance(info);

    return 0;
}
