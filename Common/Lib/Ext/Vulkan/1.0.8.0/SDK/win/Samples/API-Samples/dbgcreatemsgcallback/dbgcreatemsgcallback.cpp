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
Provide a callback routine to be called when debug or error messages are
reported
*/

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <string.h>
#include "util.hpp"

#define APP_SHORT_NAME "Debug Report Callback"

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
    VkExtensionProperties *vk_props = NULL;
    uint32_t instance_extension_count;
    VkResult res;

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

    bool found_extension = false;
    for (uint32_t i = 0; i < instance_extension_count; i++) {
        if (!strcmp(vk_props[i].extensionName,
                    VK_EXT_DEBUG_REPORT_EXTENSION_NAME)) {
            found_extension = true;
        }
    }

    if (!found_extension) {
        std::cout << "Something went very wrong, cannot find "
                     "VK_EXT_debug_report extension" << std::endl;
        exit(1);
    }

    const char *extension_names[1] = {VK_EXT_DEBUG_REPORT_EXTENSION_NAME};

    // initialize the VkApplicationInfo structure
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = APP_SHORT_NAME;
    app_info.applicationVersion = 1;
    app_info.pEngineName = APP_SHORT_NAME;
    app_info.engineVersion = 1;
    app_info.apiVersion = VK_API_VERSION_1_0;

    // initialize the VkInstanceCreateInfo structure
    VkInstanceCreateInfo inst_info = {};
    inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_info.pNext = NULL;
    inst_info.flags = 0;
    inst_info.pApplicationInfo = &app_info;
    inst_info.enabledExtensionCount = 1;
    inst_info.ppEnabledExtensionNames = extension_names;
    inst_info.enabledLayerCount = 0;
    inst_info.ppEnabledLayerNames = NULL;

    VkInstance inst;

    res = vkCreateInstance(&inst_info, NULL, &inst);
    if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
        std::cout << "cannot find a compatible Vulkan ICD\n";
        exit(-1);
    } else if (res) {
        std::cout << "unknown error\n";
        exit(-1);
    }

    PFN_vkCreateDebugReportCallbackEXT dbgCreateDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT dbgDestroyDebugReportCallback;
    VkDebugReportCallbackEXT debug_report_callback;

    dbgCreateDebugReportCallback =
        (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
            inst, "vkCreateDebugReportCallbackEXT");
    if (!dbgCreateDebugReportCallback) {
        std::cout << "GetInstanceProcAddr: Unable to find "
                     "vkCreateDebugReportCallbackEXT function." << std::endl;
        exit(1);
    }

    dbgDestroyDebugReportCallback =
        (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
            inst, "vkDestroyDebugReportCallbackEXT");
    if (!dbgDestroyDebugReportCallback) {
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

    res = dbgCreateDebugReportCallback(inst, &create_info, NULL,
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

    /* Clean up callback */
    dbgDestroyDebugReportCallback(inst, debug_report_callback, NULL);

    vkDestroyInstance(inst, NULL);

    /* VULKAN_KEY_END */

    return 0;
}
