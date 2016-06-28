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
Get global layer properties to know what
layers are available to enable at CreateInstance time.
*/

#include <util_init.hpp>
#include <cstdlib>

int main(int argc, char *argv[]) {
    VkResult res;
    struct sample_info info;
    uint32_t instance_layer_count;
    VkLayerProperties *vk_props = NULL;

    /* VULKAN_KEY_START */

    /*
     * It's possible, though very rare, that the number of
     * instance layers could change. For example, installing something
     * could include new layers that the loader would pick up
     * between the initial query for the count and the
     * request for VkLayerProperties. The loader indicates that
     * by returning a VK_INCOMPLETE status and will update the
     * the count parameter.
     * The count parameter will be updated with the number of
     * entries loaded into the data pointer - in case the number
     * of layers went down or is smaller than the size given.
     */
    do {
        res = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
        if (res)
            break;

        if (instance_layer_count == 0) {
            break;
        }

        vk_props = (VkLayerProperties *)realloc(
            vk_props, instance_layer_count * sizeof(VkLayerProperties));

        res =
            vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props);
    } while (res == VK_INCOMPLETE);

    std::cout << "Instance Layers:" << std::endl;
    for (uint32_t i = 0; i < instance_layer_count; i++) {
        VkLayerProperties *props = &vk_props[i];
        uint32_t major, minor, patch;
        std::cout << props->layerName << ":" << std::endl;
        extract_version(props->specVersion, major, minor, patch);
        std::cout << "\tVersion: " << props->implementationVersion << std::endl;
        std::cout << "\tAPI Version: "
                  << "(" << major << "." << minor << "." << patch << ")"
                  << std::endl;
        std::cout << "\tDescription: " << props->description << std::endl;
        std::cout << std::endl
                  << std::endl;
    }

    std::cout << std::endl;

    free(vk_props);

    /* VULKAN_KEY_END */

    return 0;
}
