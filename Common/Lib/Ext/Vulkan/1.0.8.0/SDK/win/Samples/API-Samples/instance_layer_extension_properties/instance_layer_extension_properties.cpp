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
Get list of global layers and their associated extensions, if any.
*/

#include <util_init.hpp>
#include <cstdlib>

int main(int argc, char *argv[]) {
    VkResult res;
    uint32_t instance_layer_count;
    VkLayerProperties *vk_props = NULL;
    std::vector<layer_properties> instance_layer_properties;

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

    /* VULKAN_KEY_START */

    /*
     * Now gather the extension list for each instance layer.
     */
    for (uint32_t i = 0; i < instance_layer_count; i++) {
        layer_properties layer_props;
        layer_props.properties = vk_props[i];
        VkResult res;

        {
            VkExtensionProperties *instance_extensions;
            uint32_t instance_extension_count;
            char *layer_name = NULL;

            layer_name = layer_props.properties.layerName;

            do {
                res = vkEnumerateInstanceExtensionProperties(
                    layer_name, &instance_extension_count, NULL);

                if (res)
                    break;

                if (instance_extension_count == 0) {
                    break;
                }

                layer_props.extensions.resize(instance_extension_count);
                instance_extensions = layer_props.extensions.data();
                res = vkEnumerateInstanceExtensionProperties(
                    layer_name, &instance_extension_count, instance_extensions);
            } while (res == VK_INCOMPLETE);
        }

        if (res)
            break;

        instance_layer_properties.push_back(layer_props);
    }
    free(vk_props);

    /* VULKAN_KEY_END */

    std::cout << "Instance Layers:" << std::endl;
    for (std::vector<layer_properties>::iterator it =
             instance_layer_properties.begin();
         it != instance_layer_properties.end(); it++) {
        layer_properties *props = &(*it);
        std::cout << props->properties.layerName << std::endl;
        if (props->extensions.size() > 0) {
            for (uint32_t j = 0; j < props->extensions.size(); j++) {
                if (j > 0) {
                    std::cout << ", ";
                }
                std::cout << props->extensions[j].extensionName << " Version "
                          << props->extensions[j].specVersion;
            }
        } else {
            std::cout << "Layer Extensions: None";
        }
        std::cout << std::endl
                  << std::endl;
    }

    std::cout << std::endl;

    return 0;
}
