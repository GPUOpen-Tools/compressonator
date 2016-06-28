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
Copy/blit image
*/

/* Create a checkerboard image, and blit a small area of it to the
 * presentation image. We should see bigger sqaures.  Then copy part of
 * the checkboard to the presentation image - we should see small squares
 */

#include <util_init.hpp>
#include <assert.h>
#include <string.h>
#include <cstdlib>

int main(int argc, char *argv[]) {
    VkResult U_ASSERT_ONLY res;
    struct sample_info info = {};
    char sample_title[] = "Copy/Blit Image";
    VkImageCreateInfo image_info;
    VkImage bltSrcImage;
    VkImage bltDstImage;
    VkMemoryRequirements memReq;
    VkMemoryAllocateInfo memAllocInfo;
    VkDeviceMemory dmem;
    unsigned char *pImgMem;

    process_command_line_args(info, argc, argv);
    init_global_layer_properties(info);
    init_instance_extension_names(info);
    init_device_extension_names(info);
    init_instance(info, sample_title);
    init_enumerate_device(info);
    init_window_size(info, 640, 640);
    init_connection(info);
    init_window(info);
    init_swapchain_extension(info);

    VkSurfaceCapabilitiesKHR surfCapabilities;
    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(info.gpus[0], info.surface,
                                                    &surfCapabilities);
    if (!(surfCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
        std::cout << "Surface cannot be destination of blit - abort \n";
        exit(-1);
    }

    init_device(info);
    init_command_pool(info);
    init_command_buffer(info);
    execute_begin_command_buffer(info);
    init_device_queue(info);
    init_swap_chain(info,  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                           VK_IMAGE_USAGE_TRANSFER_DST_BIT);

    /* VULKAN_KEY_START */

    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(info.gpus[0], info.format,
                                        &formatProps);
    assert(
        (formatProps.linearTilingFeatures & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) &&
        "Format cannot be used as transfer source");

    VkSemaphore presentCompleteSemaphore;
    VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
    presentCompleteSemaphoreCreateInfo.sType =
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    presentCompleteSemaphoreCreateInfo.pNext = NULL;
    presentCompleteSemaphoreCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    res = vkCreateSemaphore(info.device, &presentCompleteSemaphoreCreateInfo,
                            NULL, &presentCompleteSemaphore);
    assert(res == VK_SUCCESS);

    // Get the index of the next available swapchain image:
    res = vkAcquireNextImageKHR(info.device, info.swap_chain, UINT64_MAX,
                                presentCompleteSemaphore, NULL,
                                &info.current_buffer);
    // TODO: Deal with the VK_SUBOPTIMAL_KHR and VK_ERROR_OUT_OF_DATE_KHR
    // return codes
    assert(res == VK_SUCCESS);

    // Create an image, map it, and write some values to the image

    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = NULL;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = info.format;
    image_info.extent.width = info.width;
    image_info.extent.height = info.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = NUM_SAMPLES;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = NULL;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image_info.flags = 0;
    image_info.tiling = VK_IMAGE_TILING_LINEAR;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    res = vkCreateImage(info.device, &image_info, NULL, &bltSrcImage);

    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.pNext = NULL;

    vkGetImageMemoryRequirements(info.device, bltSrcImage, &memReq);
    bool pass = memory_type_from_properties(info, memReq.memoryTypeBits,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                            &memAllocInfo.memoryTypeIndex);
    assert(pass);
    memAllocInfo.allocationSize = memReq.size;
    res = vkAllocateMemory(info.device, &memAllocInfo, NULL, &dmem);
    res = vkBindImageMemory(info.device, bltSrcImage, dmem, 0);
    set_image_layout(info, bltSrcImage, VK_IMAGE_ASPECT_COLOR_BIT,
                     VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    res = vkEndCommandBuffer(info.cmd);
    assert(res == VK_SUCCESS);

    VkFence cmdFence;
    init_fence(info, cmdFence);
    VkPipelineStageFlags pipe_stage_flags =
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkSubmitInfo submit_info = {};
    submit_info.pNext = NULL;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &presentCompleteSemaphore;
    submit_info.pWaitDstStageMask = &pipe_stage_flags;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &info.cmd;
    submit_info.signalSemaphoreCount = 0;
    submit_info.pSignalSemaphores = NULL;

    /* Queue the command buffer for execution */
    res = vkQueueSubmit(info.queue, 1, &submit_info, cmdFence);
    assert(res == VK_SUCCESS);

    /* Make sure command buffer is finished before mapping */
    do {
        res =
            vkWaitForFences(info.device, 1, &cmdFence, VK_TRUE, FENCE_TIMEOUT);
    } while (res == VK_TIMEOUT);
    assert(res == VK_SUCCESS);
    vkDestroyFence(info.device, cmdFence, NULL);

    res = vkMapMemory(info.device, dmem, 0, memReq.size, 0, (void **)&pImgMem);
    // Checkerboard of 8x8 pixel squares
    for (int row = 0; row < info.height; row++) {
        for (int col = 0; col < info.width; col++) {
            unsigned char rgb = (((row & 0x8) == 0) ^ ((col & 0x8) == 0)) * 255;
            pImgMem[0] = rgb;
            pImgMem[1] = rgb;
            pImgMem[2] = rgb;
            pImgMem[3] = 255;
            pImgMem += 4;
        }
    }

    // Flush the mapped memory and then unmap it  Assume it isn't coherent since
    // we didn't really confirm
    VkMappedMemoryRange memRange;
    memRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    memRange.pNext = NULL;
    memRange.memory = dmem;
    memRange.offset = 0;
    memRange.size = memReq.size;
    res = vkFlushMappedMemoryRanges(info.device, 1, &memRange);

    vkUnmapMemory(info.device, dmem);

    vkResetCommandBuffer(info.cmd, 0);
    execute_begin_command_buffer(info);
    set_image_layout(info, bltSrcImage, VK_IMAGE_ASPECT_COLOR_BIT,
                     VK_IMAGE_LAYOUT_GENERAL,
                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    bltDstImage = info.buffers[info.current_buffer].image;
    // init_swap_chain will create the images as color attachment optimal
    // but we want transfer dst optimal
    set_image_layout(info, bltDstImage, VK_IMAGE_ASPECT_COLOR_BIT,
                     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Do a 32x32 blit to all of the dst image - should get big squares
    VkImageBlit region;
    region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.srcSubresource.mipLevel = 0;
    region.srcSubresource.baseArrayLayer = 0;
    region.srcSubresource.layerCount = 1;
    region.srcOffsets[0].x = 0;
    region.srcOffsets[0].y = 0;
    region.srcOffsets[0].z = 0;
    region.srcOffsets[1].x = 32;
    region.srcOffsets[1].y = 32;
    region.srcOffsets[1].z = 1;
    region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.dstSubresource.mipLevel = 0;
    region.dstSubresource.baseArrayLayer = 0;
    region.dstSubresource.layerCount = 1;
    region.dstOffsets[0].x = 0;
    region.dstOffsets[0].y = 0;
    region.dstOffsets[0].z = 0;
    region.dstOffsets[1].x = info.width;
    region.dstOffsets[1].y = info.height;
    region.dstOffsets[1].z = 1;

    vkCmdBlitImage(info.cmd, bltSrcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   bltDstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                   &region, VK_FILTER_LINEAR);

    // Do a image copy to part of the dst image - checks should stay small
    VkImageCopy cregion;
    cregion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    cregion.srcSubresource.mipLevel = 0;
    cregion.srcSubresource.baseArrayLayer = 0;
    cregion.srcSubresource.layerCount = 1;
    cregion.srcOffset.x = 0;
    cregion.srcOffset.y = 0;
    cregion.srcOffset.z = 0;
    cregion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    cregion.dstSubresource.mipLevel = 0;
    cregion.dstSubresource.baseArrayLayer = 0;
    cregion.dstSubresource.layerCount = 1;
    cregion.dstOffset.x = 256;
    cregion.dstOffset.y = 256;
    cregion.dstOffset.z = 0;
    cregion.extent.width = 128;
    cregion.extent.height = 128;
    cregion.extent.depth = 1;

    vkCmdCopyImage(info.cmd, bltSrcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   bltDstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                   &cregion);

    VkImageMemoryBarrier prePresentBarrier = {};
    prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    prePresentBarrier.pNext = NULL;
    prePresentBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    prePresentBarrier.subresourceRange.baseMipLevel = 0;
    prePresentBarrier.subresourceRange.levelCount = 1;
    prePresentBarrier.subresourceRange.baseArrayLayer = 0;
    prePresentBarrier.subresourceRange.layerCount = 1;
    prePresentBarrier.image = info.buffers[info.current_buffer].image;
    vkCmdPipelineBarrier(info.cmd, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0,
                         NULL, 1, &prePresentBarrier);

    res = vkEndCommandBuffer(info.cmd);
    VkFenceCreateInfo fenceInfo;
    VkFence drawFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, NULL, &drawFence);

    submit_info.pNext = NULL;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 0;
    submit_info.pWaitSemaphores = NULL;
    submit_info.pWaitDstStageMask = NULL;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &info.cmd;
    submit_info.signalSemaphoreCount = 0;
    submit_info.pSignalSemaphores = NULL;

    /* Queue the command buffer for execution */
    res = vkQueueSubmit(info.queue, 1, &submit_info, drawFence);
    assert(res == VK_SUCCESS);

    res = vkQueueWaitIdle(info.queue);
    assert(res == VK_SUCCESS);

    /* Now present the image in the window */

    VkPresentInfoKHR present;
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = NULL;
    present.swapchainCount = 1;
    present.pSwapchains = &info.swap_chain;
    present.pImageIndices = &info.current_buffer;
    present.pWaitSemaphores = NULL;
    present.waitSemaphoreCount = 0;
    present.pResults = NULL;

    /* Make sure command buffer is finished before presenting */
    do {
        res =
            vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
    } while (res == VK_TIMEOUT);
    assert(res == VK_SUCCESS);
    res = vkQueuePresentKHR(info.queue, &present);
    assert(res == VK_SUCCESS);

    wait_seconds(1);
    /* VULKAN_KEY_END */
    if (info.save_images)
        write_ppm(info, "copyblitimage");

    vkDestroySemaphore(info.device, presentCompleteSemaphore, NULL);
    vkDestroyFence(info.device, drawFence, NULL);
    vkDestroyImage(info.device, bltSrcImage, NULL);
    vkFreeMemory(info.device, dmem, NULL);
    destroy_swap_chain(info);
    destroy_command_buffer(info);
    destroy_command_pool(info);
    destroy_device(info);
    destroy_window(info);
    destroy_instance(info);
    return 0;
}
