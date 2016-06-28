/*
 *
 * Copyright (C) 2015-2016 Valve Corporation
 * Copyright (C) 2015-2016 LunarG, Inc.
 * Copyright (C) 2015 Google Inc.
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
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Jon Ashburn <jon@lunarg.com>
 *
 */
#pragma once

#include "vulkan/vk_layer.h"

/*
 * This file contains static functions for the generated layer api_dump
 */

#define LAYER_PROPS_ARRAY_SIZE 1
static const VkLayerProperties layerProps[LAYER_PROPS_ARRAY_SIZE] = {{
    "VK_LAYER_LUNARG_api_dump",
    VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION), // specVersion
    1, "layer: api_dump",
}};

#define LAYER_DEV_PROPS_ARRAY_SIZE 1
static const VkLayerProperties layerDevProps[LAYER_DEV_PROPS_ARRAY_SIZE] = {{
    "VK_LAYER_LUNARG_api_dump",
    VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION), // specVersion
    1, "layer: api_dump",
}};

struct devExts {
    bool wsi_enabled;
};

struct instExts {
    bool wsi_enabled;
};

static std::unordered_map<void *, struct devExts> deviceExtMap;
static std::unordered_map<void *, struct instExts> instanceExtMap;

static void createDeviceRegisterExtensions(const VkDeviceCreateInfo *pCreateInfo, VkDevice device) {
    uint32_t i;
    VkLayerDispatchTable *pDisp = device_dispatch_table(device);
    PFN_vkGetDeviceProcAddr gpa = pDisp->GetDeviceProcAddr;
    pDisp->CreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)gpa(device, "vkCreateSwapchainKHR");
    pDisp->DestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)gpa(device, "vkDestroySwapchainKHR");
    pDisp->GetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)gpa(device, "vkGetSwapchainImagesKHR");
    pDisp->AcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)gpa(device, "vkAcquireNextImageKHR");
    pDisp->QueuePresentKHR = (PFN_vkQueuePresentKHR)gpa(device, "vkQueuePresentKHR");

    deviceExtMap[pDisp].wsi_enabled = false;
    for (i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
            deviceExtMap[pDisp].wsi_enabled = true;
    }
}

static void createInstanceRegisterExtensions(const VkInstanceCreateInfo *pCreateInfo, VkInstance instance) {
    uint32_t i;
    VkLayerInstanceDispatchTable *pDisp = instance_dispatch_table(instance);
    PFN_vkGetInstanceProcAddr gpa = pDisp->GetInstanceProcAddr;

    pDisp->DestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)gpa(instance, "vkDestroySurfaceKHR");
    pDisp->GetPhysicalDeviceSurfaceSupportKHR =
        (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)gpa(instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
    pDisp->GetPhysicalDeviceSurfaceCapabilitiesKHR =
        (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)gpa(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    pDisp->GetPhysicalDeviceSurfaceFormatsKHR =
        (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)gpa(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    pDisp->GetPhysicalDeviceSurfacePresentModesKHR =
        (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)gpa(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");

#if VK_USE_PLATFORM_WIN32_KHR
    pDisp->CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)gpa(instance, "vkCreateWin32SurfaceKHR");
    pDisp->GetPhysicalDeviceWin32PresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)gpa(instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
#endif // VK_USE_PLATFORM_WIN32_KHR
#ifdef VK_USE_PLATFORM_XCB_KHR
    pDisp->CreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR)gpa(instance, "vkCreateXcbSurfaceKHR");
    pDisp->GetPhysicalDeviceXcbPresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)gpa(instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR");
#endif // VK_USE_PLATFORM_XCB_KHR
#ifdef VK_USE_PLATFORM_XLIB_KHR
    pDisp->CreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR)gpa(instance, "vkCreateXlibSurfaceKHR");
    pDisp->GetPhysicalDeviceXlibPresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR)gpa(instance, "vkGetPhysicalDeviceXlibPresentationSupportKHR");
#endif // VK_USE_PLATFORM_XLIB_KHR
#ifdef VK_USE_PLATFORM_MIR_KHR
    pDisp->CreateMirSurfaceKHR = (PFN_vkCreateMirSurfaceKHR)gpa(instance, "vkCreateMirSurfaceKHR");
    pDisp->GetPhysicalDeviceMirPresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceMirPresentationSupportKHR)gpa(instance, "vkGetPhysicalDeviceMirPresentationSupportKHR");
#endif // VK_USE_PLATFORM_MIR_KHR
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    pDisp->CreateWaylandSurfaceKHR = (PFN_vkCreateWaylandSurfaceKHR)gpa(instance, "vkCreateWaylandSurfaceKHR");
    pDisp->GetPhysicalDeviceWaylandPresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR)gpa(instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR");
#endif //  VK_USE_PLATFORM_WAYLAND_KHR
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    pDisp->CreateAndroidSurfaceKHR = (PFN_vkCreateAndroidSurfaceKHR)gpa(instance, "vkCreateAndroidSurfaceKHR");
#endif // VK_USE_PLATFORM_ANDROID_KHR

    instanceExtMap[pDisp].wsi_enabled = false;
    for (i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
        if (strcmp(pCreateInfo->ppEnabledExtensionNames[i], VK_KHR_SURFACE_EXTENSION_NAME) == 0)
            instanceExtMap[pDisp].wsi_enabled = true;
    }
}

VkResult explicit_CreateDevice(VkPhysicalDevice gpu, const VkDeviceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                               VkDevice *pDevice) {
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

    initDeviceTable(*pDevice, fpGetDeviceProcAddr);

    createDeviceRegisterExtensions(pCreateInfo, *pDevice);

    return result;
}
