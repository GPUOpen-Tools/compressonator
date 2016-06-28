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
Show how to enable validation layers and provide callback
*/

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <util_init.hpp>

VKAPI_ATTR VkBool32 VKAPI_CALL
dbgFunc(VkDebugReportFlagsEXT msgFlags, VkDebugReportObjectTypeEXT objType,
        uint64_t srcObject, size_t location, int32_t msgCode,
        const char *pLayerPrefix, const char *pMsg, void *pUserData) {
    std::ostringstream message;

    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        message << "ERROR: ";
    } else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        message << "WARNING: ";
    } else if (msgFlags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
        message << "PERFORMANCE WARNING: ";
    } else if (msgFlags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
        message << "INFO: ";
    } else if (msgFlags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
        message << "DEBUG: ";
    }
    message << "[" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg;

#ifdef _WIN32
    MessageBox(NULL, message.str().c_str(), "Alert", MB_OK);
#else
    std::cout << message.str() << std::endl;
#endif

    /*
     * false indicates that layer should not bail-out of an
     * API call that had validation failures. This may mean that the
     * app dies inside the driver due to invalid parameter(s).
     * That's what would happen without validation layers, so we'll
     * keep that behavior here.
     */
    return false;
}

int main(int argc, char *argv[]) {
    struct sample_info info = {};
    init_global_layer_properties(info);

    /* VULKAN_KEY_START */

    /* Use standard_validation meta layer that enables all
     * recommended validation layers
     */
    info.instance_layer_names.push_back("VK_LAYER_LUNARG_standard_validation");
    if (!demo_check_layers(info.instance_layer_properties,
                           info.instance_layer_names)) {
        exit(1);
    }

    /* Enable debug callback extension */
    info.instance_extension_names.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = "vulkansamples_enable_validation_with_callback";
    app_info.applicationVersion = 1;
    app_info.pEngineName = "Vulkan Samples";
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledLayerCount = info.instance_layer_names.size();
    inst_info.ppEnabledLayerNames = info.instance_layer_names.size()
                                        ? info.instance_layer_names.data()
                                        : NULL;
    inst_info.enabledExtensionCount = info.instance_extension_names.size();
    inst_info.ppEnabledExtensionNames = info.instance_extension_names.data();

    VkResult res = vkCreateInstance(&inst_info, NULL, &info.inst);
    assert(res == VK_SUCCESS);

    init_enumerate_device(info);

    init_device_layer_properties(info);

    /* Use standard_validation meta layer that enables all
     * recommended validation layers
     * Instance layers and Device layers are independent so
     * must enable validation layers for both to see everything.
     */
    info.device_layer_names.push_back("VK_LAYER_LUNARG_standard_validation");

    if (!demo_check_layers(info.device_layer_properties,
                           info.device_layer_names)) {
        exit(1);
    }
    float queue_priorities[1] = {0.0};
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

    queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_info.pNext = NULL;
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = queue_priorities;

    VkDeviceCreateInfo device_info = {};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.pNext = NULL;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledLayerCount = info.device_layer_names.size();
    device_info.ppEnabledLayerNames =
        device_info.enabledLayerCount ? info.device_layer_names.data() : NULL;
    device_info.enabledExtensionCount = info.device_extension_names.size();
    device_info.ppEnabledExtensionNames =
        device_info.enabledExtensionCount ? info.device_extension_names.data()
                                          : NULL;
    device_info.pEnabledFeatures = NULL;

    res = vkCreateDevice(info.gpus[0], &device_info, NULL, &info.device);
    assert(res == VK_SUCCESS);

    VkDebugReportCallbackEXT debug_report_callback;

    info.dbgCreateDebugReportCallback =
        (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
            info.inst, "vkCreateDebugReportCallbackEXT");
    if (!info.dbgCreateDebugReportCallback) {
        std::cout << "GetInstanceProcAddr: Unable to find "
                     "vkCreateDebugReportCallbackEXT function." << std::endl;
        exit(1);
    }

    info.dbgDestroyDebugReportCallback =
        (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
            info.inst, "vkDestroyDebugReportCallbackEXT");
    if (!info.dbgDestroyDebugReportCallback) {
        std::cout << "GetInstanceProcAddr: Unable to find "
                     "vkDestroyDebugReportCallbackEXT function." << std::endl;
        exit(1);
    }

    VkDebugReportCallbackCreateInfoEXT create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    create_info.pNext = NULL;
    create_info.flags =
        VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    create_info.pfnCallback = dbgFunc;
    create_info.pUserData = NULL;
    res = info.dbgCreateDebugReportCallback(info.inst, &create_info, NULL,
                                            &debug_report_callback);
    switch (res) {
    case VK_SUCCESS:
        break;
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        std::cout << "dbgCreateDebugReportCallback: out of host memory\n"
                  << std::endl;
        exit(1);
        break;
    default:
        std::cout << "dbgCreateDebugReportCallback: unknown failure\n"
                  << std::endl;
        exit(1);
        break;
    }

    /* Create a command pool */
    VkCommandPoolCreateInfo cmd_pool_info = {};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.pNext = NULL;
    cmd_pool_info.queueFamilyIndex = info.graphics_queue_family_index;
    cmd_pool_info.flags = 0;

    res =
        vkCreateCommandPool(info.device, &cmd_pool_info, NULL, &info.cmd_pool);
    assert(res == VK_SUCCESS);

    /*
     * Destroying the device before destroying the command pool above
     * will trigger a validation error.
     */
    std::cout << "calling vkDestroyDevice before destroying command pool\n";
    std::cout << "this should result in an error\n";
    vkDestroyDevice(info.device, NULL);

    /* Clean up callback */
    info.dbgDestroyDebugReportCallback(info.inst, debug_report_callback, NULL);

    /* VULKAN_KEY_END */

    vkDestroyInstance(info.inst, NULL);

    return 0;
}
