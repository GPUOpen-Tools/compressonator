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
create and destroy a Vulkan physical device
*/

/* This is part of the draw cube progression */

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <util_init.hpp>

int main(int argc, char *argv[]) {
    struct sample_info info = {};
    init_instance(info, "vulkansamples_device");

    init_enumerate_device(info);

    /* VULKAN_KEY_START */

    VkDeviceQueueCreateInfo queue_info = {};

    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_count,
                                             NULL);
    assert(info.queue_count >= 1);

    info.queue_props.resize(info.queue_count);
    vkGetPhysicalDeviceQueueFamilyProperties(info.gpus[0], &info.queue_count,
                                             info.queue_props.data());
    assert(info.queue_count >= 1);

    bool found = false;
    for (unsigned int i = 0; i < info.queue_count; i++) {
        if (info.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queue_info.queueFamilyIndex = i;
            found = true;
            break;
        }
    }
    assert(found);
    assert(info.queue_count >= 1);

    float queue_priorities[1] = {0.0};
    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.pNext = NULL;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = queue_priorities;

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = NULL;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledExtensionCount = 0;
    device_info.ppEnabledExtensionNames = NULL;
    device_info.enabledLayerCount = 0;
    device_info.ppEnabledLayerNames = NULL;
    device_info.pEnabledFeatures = NULL;

    VkDevice device;
    VkResult U_ASSERT_ONLY res =
        vkCreateDevice(info.gpus[0], &device_info, NULL, &device);
    assert(res == VK_SUCCESS);

    vkDestroyDevice(device, NULL);

    /* VULKAN_KEY_END */

    destroy_instance(info);

    return 0;
}
