//=====================================================================
// Copyright 2018 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#include "cmp_devicevk.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <cassert>


CMP_DeviceVK::CMP_DeviceVK()
{
}


CMP_DeviceVK::~CMP_DeviceVK()
{
}


void CMP_DeviceVK::OnCreate(VkInstance instance, void* hWnd)
{
    VkResult res;

    uint32_t gpu_count = 1;
    uint32_t const req_count = gpu_count;
    res = vkEnumeratePhysicalDevices(instance, &gpu_count, NULL);
    assert(gpu_count);

    std::vector<VkPhysicalDevice> gpus;
    gpus.resize(gpu_count);

    res = vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data());
    assert(!res && gpu_count >= req_count);

    m_physicaldevice = gpus[0];

    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicaldevice, &queue_family_count, NULL);
    assert(queue_family_count >= 1);

    std::vector<VkQueueFamilyProperties> queue_props;
    queue_props.resize(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicaldevice, &queue_family_count, queue_props.data());
    assert(queue_family_count >= 1);

    /* This is as good a place as any to do this */
    vkGetPhysicalDeviceMemoryProperties(m_physicaldevice, &m_memoryProperties);
    vkGetPhysicalDeviceProperties(m_physicaldevice, &m_deviceProperties);

#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.hinstance = NULL;
    createInfo.hwnd = *(static_cast<HWND*>(hWnd));
    res = vkCreateWin32SurfaceKHR(instance, &createInfo, NULL, &m_surface);
#endif

    // Iterate over each queue to learn whether it supports presenting:
    VkBool32 *pSupportsPresent = (VkBool32 *)malloc(queue_family_count * sizeof(VkBool32));
    for (uint32_t i = 0; i < queue_family_count; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(gpus[0], i, m_surface, &pSupportsPresent[i]);
    }

    // Search for a graphics and a present queue in the array of queue
    // families, try to find one that supports both
    graphics_queue_family_index = UINT32_MAX;
    present_queue_family_index = UINT32_MAX;
    for (uint32_t i = 0; i < queue_family_count; ++i) {
        if ((queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (graphics_queue_family_index == UINT32_MAX) graphics_queue_family_index = i;

            if (pSupportsPresent[i] == VK_TRUE) {
                graphics_queue_family_index = i;
                present_queue_family_index = i;
                break;
            }
        }
    }

    if (present_queue_family_index == UINT32_MAX) {
        // If didn't find a queue that supports both graphics and present, then
        // find a separate present queue.
        for (size_t i = 0; i < queue_family_count; ++i)
            if (pSupportsPresent[i] == VK_TRUE) {
                present_queue_family_index = (uint32_t)i;
                break;
            }
    }
    free(pSupportsPresent);

    // Create device
    //
    VkDeviceQueueCreateInfo queue_info = {};
    std::vector<const char *> device_extension_names;

    device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    device_extension_names.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);

    float queue_priorities[1] = { 0.0 };
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.pNext = NULL;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = queue_priorities;
    queue_info.queueFamilyIndex = graphics_queue_family_index;

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = NULL;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledExtensionCount = (uint32_t)device_extension_names.size();
    device_info.ppEnabledExtensionNames = device_info.enabledExtensionCount ? device_extension_names.data() : NULL;
    device_info.pEnabledFeatures = NULL;

    res = vkCreateDevice(gpus[0], &device_info, NULL, &m_device);
    assert(res == VK_SUCCESS);

    // create queues
    //
    vkGetDeviceQueue(m_device, graphics_queue_family_index, 0, &graphics_queue);
    if (graphics_queue_family_index == present_queue_family_index) {
        present_queue = graphics_queue;
    } else {
        vkGetDeviceQueue(m_device, present_queue_family_index, 0, &present_queue);
    }
}


void CMP_DeviceVK::OnDestroy()
{

}


bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties &memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}
