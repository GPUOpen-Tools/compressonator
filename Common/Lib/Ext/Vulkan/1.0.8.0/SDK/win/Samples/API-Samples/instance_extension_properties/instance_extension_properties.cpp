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
Get global extension properties to know what
extension are available to enable at CreateInstance time.
*/

#include <util_init.hpp>
#include <cstdlib>

int main(int argc, char *argv[]) {
    VkResult res;
    VkExtensionProperties *vk_props = NULL;
    uint32_t instance_extension_count;

    /* VULKAN_KEY_START */

    /*
     * It's possible, though very rare, that the number of
     * instance layers could change. For example, installing something
     * could include new layers that the loader would pick up
     * between the initial query for the count and the
     * request for VkLayerProperties. If that happens,
     * the number of VkLayerProperties could exceed the count
     * previously given. To alert the app to this change
     * vkEnumerateInstanceExtensionProperties will return a VK_INCOMPLETE
     * status.
     * The count parameter will be updated with the number of
     * entries actually loaded into the data pointer.
     */

    do {
        res = vkEnumerateInstanceExtensionProperties(
            NULL, &instance_extension_count, NULL);
        if (res)
            break;

        if (instance_extension_count == 0) {
            break;
        }

        vk_props = (VkExtensionProperties *)realloc(
            vk_props, instance_extension_count * sizeof(VkExtensionProperties));

        res = vkEnumerateInstanceExtensionProperties(
            NULL, &instance_extension_count, vk_props);
    } while (res == VK_INCOMPLETE);

    std::cout << "Instance Extensions:" << std::endl;
    for (uint32_t i = 0; i < instance_extension_count; i++) {
        VkExtensionProperties *props = &vk_props[i];
        std::cout << props->extensionName << ":" << std::endl;
        std::cout << "\tVersion: " << props->specVersion << std::endl;
        std::cout << std::endl
                  << std::endl;
    }

    std::cout << std::endl;

    /* VULKAN_KEY_END */

    return 0;
}
