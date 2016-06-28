/*
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
 *
 * Author: Cody Northrop <cody@lunarg.com>
 * Author: David Pinedo <david@lunarg.com>
 * Author: Jon Ashburn <jon@lunarg.com>
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <list>
#include <map>
#include <vector>
#include <fstream>

using namespace std;

#include "vk_loader_platform.h"
#include "vk_dispatch_table_helper.h"
#include "vk_struct_string_helper_cpp.h"
#include "vk_layer_config.h"
#include "vk_layer_table.h"
#include "vk_layer_extension_utils.h"

static int globalLockInitialized = 0;
static loader_platform_thread_mutex globalLock;

// unordered map: associates a swap chain with a device, image extent, format, and
// list of images
typedef struct {
    VkDevice device;
    VkExtent2D imageExtent;
    VkFormat format;
    VkImage *imageList;
} SwapchainMapStruct;
static unordered_map<VkSwapchainKHR, SwapchainMapStruct *> swapchainMap;

// unordered map: associates an image with a device, image extent, and format
typedef struct {
    VkDevice device;
    VkExtent2D imageExtent;
    VkFormat format;
} ImageMapStruct;
static unordered_map<VkImage, ImageMapStruct *> imageMap;

// unordered map: associates a device with a queue, commandPool, and physical device
// also contains per device info including dispatch table
typedef struct {
    VkLayerDispatchTable *device_dispatch_table;
    bool wsi_enabled;
    VkQueue queue;
    VkCommandPool commandPool;
    VkPhysicalDevice physicalDevice;
    PFN_vkSetDeviceLoaderData pfn_dev_init;
} DeviceMapStruct;
static unordered_map<VkDevice, DeviceMapStruct *> deviceMap;

// unordered map: associates a physical device with an instance
typedef struct { VkInstance instance; } PhysDeviceMapStruct;
static unordered_map<VkPhysicalDevice, PhysDeviceMapStruct *> physDeviceMap;

// List of frames to we will get a screenshot of
static vector<int> screenshotFrames;

// Flag indicating we have queried _VK_SCREENSHOT env var
static bool screenshotEnvQueried = false;

static bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties *memory_properties, uint32_t typeBits,
                                        VkFlags requirements_mask, uint32_t *typeIndex) {
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((memory_properties->memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

static DeviceMapStruct *get_dev_info(VkDevice dev) {
    auto it = deviceMap.find(dev);
    if (it == deviceMap.end())
        return NULL;
    else
        return it->second;
}

static void init_screenshot() {
    if (!globalLockInitialized) {
        // TODO/TBD: Need to delete this mutex sometime.  How???  One
        // suggestion is to call this during vkCreateInstance(), and then we
        // can clean it up during vkDestroyInstance().  However, that requires
        // that the layer have per-instance locks.  We need to come back and
        // address this soon.
        loader_platform_thread_create_mutex(&globalLock);
        globalLockInitialized = 1;
    }
}

static void writePPM(const char *filename, VkImage image1) {
    VkImage image2;
    VkResult err;
    bool pass;
    uint32_t x, y;
    const char *ptr;
    VkDeviceMemory mem2;
    VkCommandBuffer commandBuffer;

    if (imageMap.empty() || imageMap.find(image1) == imageMap.end())
        return;

    VkDevice device = imageMap[image1]->device;
    VkPhysicalDevice physicalDevice = deviceMap[device]->physicalDevice;
    VkInstance instance = physDeviceMap[physicalDevice]->instance;
    VkQueue queue = deviceMap[device]->queue;
    uint32_t width = imageMap[image1]->imageExtent.width;
    uint32_t height = imageMap[image1]->imageExtent.height;
    VkFormat format = imageMap[image1]->format;
    const VkImageSubresource sr = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0};
    VkSubresourceLayout sr_layout;
    const VkImageCreateInfo imgCreateInfo = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        NULL,
        0,
        VK_IMAGE_TYPE_2D,
        format,
        {width, height, 1},
        1,
        1,
        VK_SAMPLE_COUNT_1_BIT,
        VK_IMAGE_TILING_LINEAR,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_SHARING_MODE_EXCLUSIVE,
        0,
        NULL,
        VK_IMAGE_LAYOUT_UNDEFINED,
    };
    VkMemoryAllocateInfo memAllocInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, NULL,
        0, // allocationSize, queried later
        0  // memoryTypeIndex, queried later
    };
    const VkCommandBufferAllocateInfo allocCommandBufferInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, NULL,
                                                                deviceMap[device]->commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1};
    const VkCommandBufferBeginInfo commandBufferBeginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, NULL, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    const VkImageCopy imageCopyRegion = {
        {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1}, {0, 0, 0}, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1}, {0, 0, 0}, {width, height, 1}};
    VkMemoryRequirements memRequirements;

    DeviceMapStruct *devMap = get_dev_info(device);
    assert(devMap);
    VkLayerDispatchTable *pTableDevice = devMap->device_dispatch_table;
    VkLayerDispatchTable *pTableQueue = get_dev_info(static_cast <VkDevice> (static_cast <void *> ( queue)))->device_dispatch_table;
    VkLayerInstanceDispatchTable *pInstanceTable;
    VkLayerDispatchTable *pTableCommandBuffer;
    VkPhysicalDeviceMemoryProperties memory_properties;

    // The VkImage image1 we are going to dump may not be mappable,
    // and/or it may have a tiling mode of optimal rather than linear.
    // To make sure we have an image that we can map and read linearly, we:
    //     create image2 that is mappable and linear
    //     copy image1 to image2
    //     map image2
    //     read from image2's mapped memory.

    err = pTableDevice->CreateImage(device, &imgCreateInfo, NULL, &image2);
    assert(!err);

    pTableDevice->GetImageMemoryRequirements(device, image2, &memRequirements);

    memAllocInfo.allocationSize = memRequirements.size;
    pInstanceTable = instance_dispatch_table(instance);
    pInstanceTable->GetPhysicalDeviceMemoryProperties(physicalDevice, &memory_properties);

    pass = memory_type_from_properties(&memory_properties, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                       &memAllocInfo.memoryTypeIndex);
    assert(pass);

    err = pTableDevice->AllocateMemory(device, &memAllocInfo, NULL, &mem2);
    assert(!err);

    err = pTableQueue->BindImageMemory(device, image2, mem2, 0);
    assert(!err);

    err = pTableDevice->AllocateCommandBuffers(device, &allocCommandBufferInfo, &commandBuffer);
    assert(!err);

    VkDevice cmdBuf = static_cast<VkDevice> (static_cast<void*> (commandBuffer));
    deviceMap.emplace(cmdBuf, devMap);
    pTableCommandBuffer = get_dev_info(cmdBuf)->device_dispatch_table;

    // We have just created a dispatchable object, but the dispatch table has not been placed
    // in the object yet.  When a "normal" application creates a command buffer, the dispatch
    // table is installed by the top-level api binding (trampoline.c).
    // But here, we have to do it ourselves.
    if (!devMap->pfn_dev_init) {
        *((const void **)commandBuffer) = *(void **)device;
    } else {
        err = devMap->pfn_dev_init(device, (void *) commandBuffer);
        assert(!err);
    }

    err = pTableCommandBuffer->BeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    assert(!err);

    // Transition the source image layout to prepare it for the copy.

    VkImageMemoryBarrier image1_memory_barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                  NULL,
                                                  VK_ACCESS_TRANSFER_WRITE_BIT,
                                                  VK_ACCESS_TRANSFER_READ_BIT,
                                                  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                  VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                  0,
                                                  0,
                                                  image1,
                                                  {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    VkImageMemoryBarrier image2_memory_barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                  NULL,
                                                  0,
                                                  VK_ACCESS_TRANSFER_WRITE_BIT,
                                                  VK_IMAGE_LAYOUT_UNDEFINED,
                                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                  0,
                                                  0,
                                                  image2,
                                                  {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkPipelineStageFlags dst_stages = VK_PIPELINE_STAGE_TRANSFER_BIT;

    // The source image needs to be transitioned from the potentially optimized swapchain format to optimum format for the copy.
    pTableCommandBuffer->CmdPipelineBarrier(commandBuffer, src_stages, dst_stages, 0, 0, NULL, 0, NULL, 1, &image1_memory_barrier);

    // The destination image needs to be transitioned from unknown to the optimum format for the copy.
    pTableCommandBuffer->CmdPipelineBarrier(commandBuffer, src_stages, dst_stages, 0, 0, NULL, 0, NULL, 1, &image2_memory_barrier);

    pTableCommandBuffer->CmdCopyImage(commandBuffer, image1, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image2,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageCopyRegion);

    // The destination needs to be transitioned from the optimum copy format to the format we can read with the CPU.
    image2_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    image2_memory_barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    image2_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    image2_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    pTableCommandBuffer->CmdPipelineBarrier(commandBuffer, src_stages, dst_stages, 0, 0, NULL, 0, NULL, 1, &image2_memory_barrier);

    // Restore the swap chain image layout to what it was before.
    // This may not be strictly needed, but it is generally good to restore things to original state.
    image1_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    image1_memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    image1_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    image1_memory_barrier.dstAccessMask = 0;

    pTableCommandBuffer->CmdPipelineBarrier(commandBuffer, src_stages, dst_stages, 0, 0, NULL, 0, NULL, 1, &image1_memory_barrier);

    err = pTableCommandBuffer->EndCommandBuffer(commandBuffer);
    assert(!err);

    VkFence nullFence = {VK_NULL_HANDLE};
    VkSubmitInfo submit_info;
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext = NULL;
    submit_info.waitSemaphoreCount = 0;
    submit_info.pWaitSemaphores = NULL;
    submit_info.pWaitDstStageMask = NULL;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &commandBuffer;
    submit_info.signalSemaphoreCount = 0;
    submit_info.pSignalSemaphores = NULL;

    err = pTableQueue->QueueSubmit(queue, 1, &submit_info, nullFence);
    assert(!err);

    err = pTableQueue->QueueWaitIdle(queue);
    assert(!err);

    err = pTableDevice->DeviceWaitIdle(device);
    assert(!err);

    pTableDevice->GetImageSubresourceLayout(device, image2, &sr, &sr_layout);

    err = pTableDevice->MapMemory(device, mem2, 0, VK_WHOLE_SIZE, 0, (void **)&ptr);
    assert(!err);

    ptr += sr_layout.offset;

    ofstream file(filename, ios::binary);

    file << "P6\n";
    file << width << "\n";
    file << height << "\n";
    file << 255 << "\n";

    for (y = 0; y < height; y++) {
        const unsigned int *row = (const unsigned int *)ptr;
        if (format == VK_FORMAT_B8G8R8A8_UNORM) {
            for (x = 0; x < width; x++) {
                unsigned int swapped;
                swapped = (*row & 0xff00ff00) | (*row & 0x000000ff) << 16 | (*row & 0x00ff0000) >> 16;
                file.write((char *)&swapped, 3);
                row++;
            }
        } else if (format == VK_FORMAT_R8G8B8A8_UNORM) {
            for (x = 0; x < width; x++) {
                file.write((char *)row, 3);
                row++;
            }
        } else {
            // TODO: add support for additional formats
            printf("Unrecognized image format\n");
            break;
        }
        ptr += sr_layout.rowPitch;
    }
    file.close();
    // Clean up
    pTableDevice->UnmapMemory(device, mem2);
    pTableDevice->FreeMemory(device, mem2, NULL);
    pTableDevice->DestroyImage(device, image2, NULL);
    pTableDevice->FreeCommandBuffers(device, deviceMap[device]->commandPool, 1, &commandBuffer);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkInstance *pInstance) {
    VkLayerInstanceCreateInfo *chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    assert(chain_info->u.pLayerInfo);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    assert(fpGetInstanceProcAddr);
    PFN_vkCreateInstance fpCreateInstance = (PFN_vkCreateInstance)fpGetInstanceProcAddr(*pInstance, "vkCreateInstance");
    if (fpCreateInstance == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Advance the link info for the next element on the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    VkResult result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS)
        return result;

    initInstanceTable(*pInstance, fpGetInstanceProcAddr);

    init_screenshot();

    return result;
}

// TODO hook DestroyInstance to cleanup

static void createDeviceRegisterExtensions(const VkDeviceCreateInfo *pCreateInfo, VkDevice device) {
    uint32_t i;
    DeviceMapStruct *devMap = get_dev_info(device);
    VkLayerDispatchTable *pDisp = devMap->device_dispatch_table;
    PFN_vkGetDeviceProcAddr gpa = pDisp->GetDeviceProcAddr;
    pDisp->CreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)gpa(device, "vkCreateSwapchainKHR");
    pDisp->GetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)gpa(device, "vkGetSwapchainImagesKHR");
    pDisp->AcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)gpa(device, "vkAcquireNextImageKHR");
    pDisp->QueuePresentKHR = (PFN_vkQueuePresentKHR)gpa(device, "vkQueuePresentKHR");
    devMap->wsi_enabled = false;
    for (i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
            devMap->wsi_enabled = true;
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(VkPhysicalDevice gpu, const VkDeviceCreateInfo *pCreateInfo,
                                                              const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
    VkLayerDeviceCreateInfo *chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);

    assert(chain_info->u.pLayerInfo);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = chain_info->u.pLayerInfo->pfnNextGetDeviceProcAddr;
    PFN_vkCreateDevice fpCreateDevice = (PFN_vkCreateDevice)fpGetInstanceProcAddr(NULL, "vkCreateDevice");
    if (fpCreateDevice == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Advance the link info for the next element on the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    VkResult result = fpCreateDevice(gpu, pCreateInfo, pAllocator, pDevice);
    if (result != VK_SUCCESS) {
        return result;
    }

    assert(deviceMap.find(*pDevice) == deviceMap.end());
    DeviceMapStruct *deviceMapElem = new DeviceMapStruct;
    deviceMap[*pDevice] = deviceMapElem;

    // Setup device dispatch table
    deviceMapElem->device_dispatch_table = new VkLayerDispatchTable;
    layer_init_device_dispatch_table(*pDevice, deviceMapElem->device_dispatch_table, fpGetDeviceProcAddr);

    createDeviceRegisterExtensions(pCreateInfo, *pDevice);
    // Create a mapping from a device to a physicalDevice
    deviceMapElem->physicalDevice = gpu;

    // store the loader callback for initializing created dispatchable objects
    chain_info = get_chain_info(pCreateInfo, VK_LOADER_DATA_CALLBACK);
    if (chain_info) {
        deviceMapElem->pfn_dev_init = chain_info->u.pfnSetDeviceLoaderData;
    } else {
        deviceMapElem->pfn_dev_init = NULL;
    }
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount, VkPhysicalDevice *pPhysicalDevices) {
    VkResult result;

    VkLayerInstanceDispatchTable *pTable = instance_dispatch_table(instance);
    result = pTable->EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
    if (result == VK_SUCCESS && *pPhysicalDeviceCount > 0 && pPhysicalDevices) {
        for (uint32_t i = 0; i < *pPhysicalDeviceCount; i++) {
            // Create a mapping from a physicalDevice to an instance
            if (physDeviceMap[pPhysicalDevices[i]] == NULL) {
                PhysDeviceMapStruct *physDeviceMapElem = new PhysDeviceMapStruct;
                physDeviceMap[pPhysicalDevices[i]] = physDeviceMapElem;
            }
            physDeviceMap[pPhysicalDevices[i]]->instance = instance;
        }
    }
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator) {
    DeviceMapStruct *devMap = get_dev_info(device);
    assert(devMap);
    VkLayerDispatchTable *pDisp = devMap->device_dispatch_table;
    pDisp->DestroyDevice(device, pAllocator);

    loader_platform_thread_lock_mutex(&globalLock);
    delete pDisp;
    delete devMap;

    deviceMap.erase(device);
    loader_platform_thread_unlock_mutex(&globalLock);
}

static const VkLayerProperties ss_device_layers[] = {{
    "VK_LAYER_LUNARG_screenshot", VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION), 1, "Layer: screenshot",
}};

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount, VkExtensionProperties *pProperties) {
    /* ScreenShot does not have any global extensions */
    return util_GetExtensionProperties(0, NULL, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t *pCount, VkLayerProperties *pProperties) {
    /* ScreenShot does not have any global layers */
    return util_GetLayerProperties(0, NULL, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                                                                    const char *pLayerName, uint32_t *pCount,
                                                                                    VkExtensionProperties *pProperties) {
    /* ScreenShot does not have any physical device extensions */
    if (pLayerName == NULL) {
        VkLayerInstanceDispatchTable *pTable = instance_dispatch_table(physicalDevice);
        return pTable->EnumerateDeviceExtensionProperties(physicalDevice, NULL, pCount, pProperties);
    } else {
        return util_GetExtensionProperties(0, NULL, pCount, pProperties);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pCount, VkLayerProperties *pProperties) {
    return util_GetLayerProperties(ARRAY_SIZE(ss_device_layers), ss_device_layers, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetDeviceQueue(VkDevice device, uint32_t queueNodeIndex, uint32_t queueIndex, VkQueue *pQueue) {
    DeviceMapStruct *devMap = get_dev_info(device);
    assert(devMap);
    VkLayerDispatchTable *pDisp = devMap->device_dispatch_table;
    pDisp->GetDeviceQueue(device, queueNodeIndex, queueIndex, pQueue);

    loader_platform_thread_lock_mutex(&globalLock);
    if (screenshotEnvQueried && screenshotFrames.empty()) {
        // We are all done taking screenshots, so don't do anything else
        loader_platform_thread_unlock_mutex(&globalLock);
        return;
    }

    VkDevice que = static_cast<VkDevice> (static_cast<void*> (*pQueue));
    deviceMap.emplace(que, devMap);

    // Create a mapping from a device to a queue
    devMap->queue = *pQueue;
    loader_platform_thread_unlock_mutex(&globalLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo *pCreateInfo,
                                                                   const VkAllocationCallbacks *pAllocator,
                                                                   VkCommandPool *pCommandPool) {
    DeviceMapStruct *devMap = get_dev_info(device);
    assert(devMap);
    VkLayerDispatchTable *pDisp = devMap->device_dispatch_table;
    VkResult result =
        pDisp->CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);

    loader_platform_thread_lock_mutex(&globalLock);
    if (screenshotEnvQueried && screenshotFrames.empty()) {
        // We are all done taking screenshots, so don't do anything else
        loader_platform_thread_unlock_mutex(&globalLock);
        return result;
    }

    // Create a mapping from a device to a commandPool
    devMap->commandPool = *pCommandPool;
    loader_platform_thread_unlock_mutex(&globalLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR *pCreateInfo,
                                                                    const VkAllocationCallbacks *pAllocator,
                                                                    VkSwapchainKHR *pSwapchain) {

    DeviceMapStruct *devMap = get_dev_info(device);
    assert(devMap);
    VkLayerDispatchTable *pDisp = devMap->device_dispatch_table;

    // This layer does an image copy later on, and the copy command expects the transfer src bit to be on.
    VkSwapchainCreateInfoKHR myCreateInfo = *pCreateInfo;
    myCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    VkResult result =
        pDisp->CreateSwapchainKHR(device, &myCreateInfo, pAllocator, pSwapchain);

    loader_platform_thread_lock_mutex(&globalLock);
    if (screenshotEnvQueried && screenshotFrames.empty()) {
        // We are all done taking screenshots, so don't do anything else
        loader_platform_thread_unlock_mutex(&globalLock);
        return result;
    }

    if (result == VK_SUCCESS) {
        // Create a mapping for a swapchain to a device, image extent, and format
        SwapchainMapStruct *swapchainMapElem = new SwapchainMapStruct;
        swapchainMapElem->device = device;
        swapchainMapElem->imageExtent = pCreateInfo->imageExtent;
        swapchainMapElem->format = pCreateInfo->imageFormat;
        swapchainMap.insert(make_pair(*pSwapchain, swapchainMapElem));

        // Create a mapping for the swapchain object into the dispatch table
        // TODO is this needed? screenshot_device_table_map.emplace((void *)pSwapchain, pTable);
    }
    loader_platform_thread_unlock_mutex(&globalLock);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t *pCount, VkImage *pSwapchainImages) {
    DeviceMapStruct *devMap = get_dev_info(device);
    assert(devMap);
    VkLayerDispatchTable *pDisp = devMap->device_dispatch_table;
    VkResult result =
        pDisp->GetSwapchainImagesKHR(device, swapchain, pCount, pSwapchainImages);

    loader_platform_thread_lock_mutex(&globalLock);
    if (screenshotEnvQueried && screenshotFrames.empty()) {
        // We are all done taking screenshots, so don't do anything else
        loader_platform_thread_unlock_mutex(&globalLock);
        return result;
    }

    if (result == VK_SUCCESS && pSwapchainImages && !swapchainMap.empty() && swapchainMap.find(swapchain) != swapchainMap.end()) {
        unsigned i;

        for (i = 0; i < *pCount; i++) {
            // Create a mapping for an image to a device, image extent, and format
            if (imageMap[pSwapchainImages[i]] == NULL) {
                ImageMapStruct *imageMapElem = new ImageMapStruct;
                imageMap[pSwapchainImages[i]] = imageMapElem;
            }
            imageMap[pSwapchainImages[i]]->device = swapchainMap[swapchain]->device;
            imageMap[pSwapchainImages[i]]->imageExtent = swapchainMap[swapchain]->imageExtent;
            imageMap[pSwapchainImages[i]]->format = swapchainMap[swapchain]->format;
        }

        // Add list of images to swapchain to image map
        SwapchainMapStruct *swapchainMapElem = swapchainMap[swapchain];
        if (i >= 1 && swapchainMapElem) {
            VkImage *imageList = new VkImage[i];
            swapchainMapElem->imageList = imageList;
            for (unsigned j = 0; j < i; j++) {
                swapchainMapElem->imageList[j] = pSwapchainImages[j];
            }
        }
    }
    loader_platform_thread_unlock_mutex(&globalLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR *pPresentInfo) {
    static int frameNumber = 0;
    if (frameNumber == 10) {
        fflush(stdout); /* *((int*)0)=0; */
    }
    DeviceMapStruct *devMap = get_dev_info((VkDevice) queue);
    assert(devMap);
    VkLayerDispatchTable *pDisp = devMap->device_dispatch_table;
    VkResult result = pDisp->QueuePresentKHR(queue, pPresentInfo);

    loader_platform_thread_lock_mutex(&globalLock);

    if (!screenshotEnvQueried) {
        const char *_vk_screenshot = loader_getenv("_VK_SCREENSHOT");
        if (_vk_screenshot && *_vk_screenshot) {
            string spec(_vk_screenshot), word;
            size_t start = 0, comma = 0;

            while (start < spec.size()) {
                int frameToAdd;
                comma = spec.find(',', start);
                if (comma == string::npos)
                    word = string(spec, start);
                else
                    word = string(spec, start, comma - start);
                frameToAdd = atoi(word.c_str());
                // Add the frame number to list, but only do it if the word started with a digit and if
                // it's not already in the list
                if (*(word.c_str()) >= '0' && *(word.c_str()) <= '9' &&
                    find(screenshotFrames.begin(), screenshotFrames.end(), frameToAdd) == screenshotFrames.end()) {
                    screenshotFrames.push_back(frameToAdd);
                }
                if (comma == string::npos)
                    break;
                start = comma + 1;
            }
        }
        loader_free_getenv(_vk_screenshot);
        screenshotEnvQueried = true;
    }

    if (result == VK_SUCCESS && !screenshotFrames.empty()) {
        vector<int>::iterator it;
        it = find(screenshotFrames.begin(), screenshotFrames.end(), frameNumber);
        if (it != screenshotFrames.end()) {
            string fileName;
            fileName = to_string(frameNumber) + ".ppm";

            VkImage image;
            VkSwapchainKHR swapchain;
            // We'll dump only one image: the first
            swapchain = pPresentInfo->pSwapchains[0];
            image = swapchainMap[swapchain]->imageList[pPresentInfo->pImageIndices[0]];
            writePPM(fileName.c_str(), image);
            screenshotFrames.erase(it);

            if (screenshotFrames.empty()) {
                // Free all our maps since we are done with them.
                for (auto it = swapchainMap.begin(); it != swapchainMap.end(); it++) {
                    SwapchainMapStruct *swapchainMapElem = it->second;
                    delete swapchainMapElem;
                }
                for (auto it = imageMap.begin(); it != imageMap.end(); it++) {
                    ImageMapStruct *imageMapElem = it->second;
                    delete imageMapElem;
                }
                for (auto it = physDeviceMap.begin(); it != physDeviceMap.end(); it++) {
                    PhysDeviceMapStruct *physDeviceMapElem = it->second;
                    delete physDeviceMapElem;
                }
                swapchainMap.clear();
                imageMap.clear();
                physDeviceMap.clear();
            }
        }
    }
    frameNumber++;
    loader_platform_thread_unlock_mutex(&globalLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice dev, const char *funcName) {
    if (!strcmp(funcName, "vkGetDeviceProcAddr"))
        return (PFN_vkVoidFunction)vkGetDeviceProcAddr;
    if (!strcmp(funcName, "vkGetDeviceQueue"))
        return (PFN_vkVoidFunction)vkGetDeviceQueue;
    if (!strcmp(funcName, "vkCreateCommandPool"))
        return (PFN_vkVoidFunction)vkCreateCommandPool;
    if (!strcmp(funcName, "vkDestroyDevice"))
        return (PFN_vkVoidFunction)vkDestroyDevice;

    if (dev == NULL) {
        return NULL;
    }

    DeviceMapStruct *devMap = get_dev_info(dev);
    assert(devMap);
    VkLayerDispatchTable *pDisp = devMap->device_dispatch_table;
    if (devMap->wsi_enabled) {
        if (!strcmp(funcName, "vkCreateSwapchainKHR"))
            return (PFN_vkVoidFunction)vkCreateSwapchainKHR;
        if (!strcmp(funcName, "vkGetSwapchainImagesKHR"))
            return (PFN_vkVoidFunction)vkGetSwapchainImagesKHR;
        if (!strcmp(funcName, "vkQueuePresentKHR"))
            return (PFN_vkVoidFunction)vkQueuePresentKHR;
    }

    if (pDisp->GetDeviceProcAddr == NULL)
        return NULL;
    return pDisp->GetDeviceProcAddr(dev, funcName);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *funcName) {

    if (!strcmp("vkGetInstanceProcAddr", funcName))
        return (PFN_vkVoidFunction)vkGetInstanceProcAddr;
    if (!strcmp(funcName, "vkCreateInstance"))
        return (PFN_vkVoidFunction)vkCreateInstance;
    if (!strcmp(funcName, "vkCreateDevice"))
        return (PFN_vkVoidFunction)vkCreateDevice;
    if (!strcmp(funcName, "vkEnumeratePhysicalDevices"))
        return (PFN_vkVoidFunction)vkEnumeratePhysicalDevices;
    if (!strcmp(funcName, "vkEnumerateDeviceExtensionProperties"))
        return (PFN_vkVoidFunction)vkEnumerateDeviceExtensionProperties;

    if (instance == VK_NULL_HANDLE) {
        return NULL;
    }
    VkLayerInstanceDispatchTable *pTable = instance_dispatch_table(instance);
    if (pTable->GetInstanceProcAddr == NULL)
        return NULL;
    return pTable->GetInstanceProcAddr(instance, funcName);
}
