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
Inititalize Swapchain
*/

/* This is part of the draw cube progression */

#include <util_init.hpp>
#include <assert.h>
#include <cstdlib>

int main(int argc, char *argv[]) {
    VkResult U_ASSERT_ONLY res;
    struct sample_info info = {};
    char sample_title[] = "Swapchain Initialization Sample";

    /*
     * Set up swapchain:
     * - Get supported uses for all queues
     * - Try to find a queue that supports both graphics and present
     * - If no queue supports both, find a present queue and make sure we have a
     *   graphics queue
     * - Get a list of supported formats and use the first one
     * - Get surface properties and present modes and use them to create a swap
     *   chain
     * - Create swap chain buffers
     * - For each buffer, create a color attachment view and set its layout to
     *   color attachment
     */

    init_global_layer_properties(info);
    init_instance_extension_names(info);
    init_device_extension_names(info);
    init_instance(info, sample_title);
    init_enumerate_device(info);
    init_connection(info);
    init_window_size(info, 50, 50);
    init_window(info);

/* VULKAN_KEY_START */
// Construct the surface description:
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.hinstance = info.connection;
    createInfo.hwnd = info.window;
    res = vkCreateWin32SurfaceKHR(info.inst, &createInfo, NULL, &info.surface);
#else  // _WIN32
    VkXcbSurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.connection = info.connection;
    createInfo.window = info.window;
    res = vkCreateXcbSurfaceKHR(info.inst, &createInfo, NULL, &info.surface);
#endif // _WIN32
    assert(res == VK_SUCCESS);

    // Iterate over each queue to learn whether it supports presenting:
    VkBool32 *supportsPresent =
        (VkBool32 *)malloc(info.queue_count * sizeof(VkBool32));
    for (uint32_t i = 0; i < info.queue_count; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(info.gpus[0], i, info.surface,
                                             &supportsPresent[i]);
    }

    // Search for a graphics queue and a present queue in the array of queue
    // families, try to find one that supports both
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < info.queue_count; i++) {
        if ((info.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (supportsPresent[i] == VK_TRUE) {
                graphicsQueueNodeIndex = i;
                break;
            }
        }
    }
    free(supportsPresent);

    // Generate error if could not find a queue that supports both a graphics
    // and present
    if (graphicsQueueNodeIndex == UINT32_MAX) {
        std::cout << "Could not find a queue that supports both graphics and "
                     "present\n";
        exit(-1);
    }

    info.graphics_queue_family_index = graphicsQueueNodeIndex;

    init_device(info);

    // Get the list of VkFormats that are supported:
    uint32_t formatCount;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(info.gpus[0], info.surface,
                                               &formatCount, NULL);
    assert(res == VK_SUCCESS);
    VkSurfaceFormatKHR *surfFormats =
        (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(info.gpus[0], info.surface,
                                               &formatCount, surfFormats);
    assert(res == VK_SUCCESS);
    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {
        info.format = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
        assert(formatCount >= 1);
        info.format = surfFormats[0].format;
    }

    VkSurfaceCapabilitiesKHR surfCapabilities;

    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info.gpus[0], info.surface,
                                                    &surfCapabilities);
    assert(res == VK_SUCCESS);

    uint32_t presentModeCount;
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(info.gpus[0], info.surface,
                                                    &presentModeCount, NULL);
    assert(res == VK_SUCCESS);
    VkPresentModeKHR *presentModes =
        (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));

    res = vkGetPhysicalDeviceSurfacePresentModesKHR(
        info.gpus[0], info.surface, &presentModeCount, presentModes);
    assert(res == VK_SUCCESS);

    VkExtent2D swapChainExtent;
    // width and height are either both -1, or both not -1.
    if (surfCapabilities.currentExtent.width == (uint32_t)-1) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapChainExtent.width = info.width;
        swapChainExtent.height = info.height;
    } else {
        // If the surface size is defined, the swap chain size must match
        swapChainExtent = surfCapabilities.currentExtent;
    }

    // If mailbox mode is available, use it, as is the lowest-latency non-
    // tearing mode.  If not, try IMMEDIATE which will usually be available,
    // and is fastest (though it tears).  If not, fall back to FIFO which is
    // always available.
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < presentModeCount; i++) {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
            (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    // Determine the number of VkImage's to use in the swap chain (we desire to
    // own only 1 image at a time, besides the images being displayed and
    // queued for display):
    uint32_t desiredNumberOfSwapChainImages =
        surfCapabilities.minImageCount + 1;
    if ((surfCapabilities.maxImageCount > 0) &&
        (desiredNumberOfSwapChainImages > surfCapabilities.maxImageCount)) {
        // Application must settle for fewer images than desired:
        desiredNumberOfSwapChainImages = surfCapabilities.maxImageCount;
    }

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms &
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfCapabilities.currentTransform;
    }

    VkSwapchainCreateInfoKHR swap_chain = {};
    swap_chain.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_chain.pNext = NULL;
    swap_chain.surface = info.surface;
    swap_chain.minImageCount = desiredNumberOfSwapChainImages;
    swap_chain.imageFormat = info.format;
    swap_chain.imageExtent.width = swapChainExtent.width;
    swap_chain.imageExtent.height = swapChainExtent.height;
    swap_chain.preTransform = preTransform;
    swap_chain.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swap_chain.imageArrayLayers = 1;
    swap_chain.presentMode = swapchainPresentMode;
    swap_chain.oldSwapchain = NULL;
    swap_chain.clipped = true;
    swap_chain.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swap_chain.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swap_chain.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swap_chain.queueFamilyIndexCount = 0;
    swap_chain.pQueueFamilyIndices = NULL;

    res =
        vkCreateSwapchainKHR(info.device, &swap_chain, NULL, &info.swap_chain);
    assert(res == VK_SUCCESS);

    res = vkGetSwapchainImagesKHR(info.device, info.swap_chain,
                                  &info.swapchainImageCount, NULL);
    assert(res == VK_SUCCESS);

    VkImage *swapchainImages =
        (VkImage *)malloc(info.swapchainImageCount * sizeof(VkImage));
    assert(swapchainImages);
    res = vkGetSwapchainImagesKHR(info.device, info.swap_chain,
                                  &info.swapchainImageCount, swapchainImages);
    assert(res == VK_SUCCESS);

    info.buffers.resize(info.swapchainImageCount);

    // Going to need a command buffer to send the memory barriers in
    // set_image_layout but we couldn't have created one before we knew
    // what our graphics_queue_family_index is, but now that we have it,
    // create the command buffer

    init_command_pool(info);
    init_command_buffer(info);
    execute_begin_command_buffer(info);
    vkGetDeviceQueue(info.device, info.graphics_queue_family_index, 0,
                     &info.queue);

    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        VkImageViewCreateInfo color_image_view = {};
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        color_image_view.pNext = NULL;
        color_image_view.format = info.format;
        color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
        color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
        color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
        color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
        color_image_view.subresourceRange.aspectMask =
            VK_IMAGE_ASPECT_COLOR_BIT;
        color_image_view.subresourceRange.baseMipLevel = 0;
        color_image_view.subresourceRange.levelCount = 1;
        color_image_view.subresourceRange.baseArrayLayer = 0;
        color_image_view.subresourceRange.layerCount = 1;
        color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        color_image_view.flags = 0;

        info.buffers[i].image = swapchainImages[i];

        set_image_layout(info, info.buffers[i].image, VK_IMAGE_ASPECT_COLOR_BIT,
                         VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        color_image_view.image = info.buffers[i].image;

        res = vkCreateImageView(info.device, &color_image_view, NULL,
                                &info.buffers[i].view);
        assert(res == VK_SUCCESS);
    }
    free(swapchainImages);
    execute_end_command_buffer(info);
    execute_queue_command_buffer(info);
    /* VULKAN_KEY_END */

    /* Clean Up */
    VkCommandBuffer cmd_bufs[1] = {info.cmd};
    vkFreeCommandBuffers(info.device, info.cmd_pool, 1, cmd_bufs);
    vkDestroyCommandPool(info.device, info.cmd_pool, NULL);
    for (uint32_t i = 0; i < info.swapchainImageCount; i++) {
        vkDestroyImageView(info.device, info.buffers[i].view, NULL);
    }
    vkDestroySwapchainKHR(info.device, info.swap_chain, NULL);
    destroy_device(info);
    destroy_window(info);
    destroy_instance(info);

    return 0;
}
