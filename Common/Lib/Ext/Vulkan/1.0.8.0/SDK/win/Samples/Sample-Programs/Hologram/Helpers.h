/*
 * Copyright (C) 2016 Google, Inc.
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

#ifndef HELPERS_H
#define HELPERS_H

#include <vector>
#include <sstream>
#include <stdexcept>
#include <vulkan/vulkan.h>

#include "HelpersDispatchTable.h"

namespace vk {

inline VkResult assert_success(VkResult res)
{
    if (res != VK_SUCCESS) {
        std::stringstream ss;
        ss << "VkResult " << res << " returned";
        throw std::runtime_error(ss.str());
    }

    return res;
}

inline VkResult enumerate(const char *layer, std::vector<VkExtensionProperties> &exts)
{
    uint32_t count = 0;
    vk::EnumerateInstanceExtensionProperties(layer, &count, nullptr);

    exts.resize(count);
    return vk::EnumerateInstanceExtensionProperties(layer, &count, exts.data());
}

inline VkResult enumerate(VkPhysicalDevice phy, const char *layer, std::vector<VkExtensionProperties> &exts)
{
    uint32_t count = 0;
    vk::EnumerateDeviceExtensionProperties(phy, layer, &count, nullptr);

    exts.resize(count);
    return vk::EnumerateDeviceExtensionProperties(phy, layer, &count, exts.data());
}

inline VkResult enumerate(VkInstance instance, std::vector<VkPhysicalDevice> &phys)
{
    uint32_t count = 0;
    vk::EnumeratePhysicalDevices(instance, &count, nullptr);

    phys.resize(count);
    return vk::EnumeratePhysicalDevices(instance, &count, phys.data());
}

inline VkResult enumerate(std::vector<VkLayerProperties> &layer_props)
{
    uint32_t count = 0;
    vk::EnumerateInstanceLayerProperties(&count, nullptr);

    layer_props.resize(count);
    return vk::EnumerateInstanceLayerProperties(&count, layer_props.data());
}

inline VkResult enumerate(VkPhysicalDevice phy, std::vector<VkLayerProperties> &layer_props)
{
    uint32_t count = 0;
    vk::EnumerateDeviceLayerProperties(phy, &count, nullptr);

    layer_props.resize(count);
    return vk::EnumerateDeviceLayerProperties(phy, &count, layer_props.data());
}

inline VkResult get(VkPhysicalDevice phy, std::vector<VkQueueFamilyProperties> &queues)
{
    uint32_t count = 0;
    vk::GetPhysicalDeviceQueueFamilyProperties(phy, &count, nullptr);

    queues.resize(count);
    vk::GetPhysicalDeviceQueueFamilyProperties(phy, &count, queues.data());

    return VK_SUCCESS;
}

inline VkResult get(VkPhysicalDevice phy, VkSurfaceKHR surface, std::vector<VkSurfaceFormatKHR> &formats)
{
    uint32_t count = 0;
    vk::GetPhysicalDeviceSurfaceFormatsKHR(phy, surface, &count, nullptr);

    formats.resize(count);
    return vk::GetPhysicalDeviceSurfaceFormatsKHR(phy, surface, &count, formats.data());
}

inline VkResult get(VkPhysicalDevice phy, VkSurfaceKHR surface, std::vector<VkPresentModeKHR> &modes)
{
    uint32_t count = 0;
    vk::GetPhysicalDeviceSurfacePresentModesKHR(phy, surface, &count, nullptr);

    modes.resize(count);
    return vk::GetPhysicalDeviceSurfacePresentModesKHR(phy, surface, &count, modes.data());
}

inline VkResult get(VkDevice dev, VkSwapchainKHR swapchain, std::vector<VkImage> &images)
{
    uint32_t count = 0;
    vk::GetSwapchainImagesKHR(dev, swapchain, &count, nullptr);

    images.resize(count);
    return vk::GetSwapchainImagesKHR(dev, swapchain, &count, images.data());
}

} // namespace vk

#endif // HELPERS_H
