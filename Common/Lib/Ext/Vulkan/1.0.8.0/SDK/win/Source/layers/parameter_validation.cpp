/* Copyright (c) 2015-2016 The Khronos Group Inc.
 * Copyright (c) 2015-2016 Valve Corporation
 * Copyright (c) 2015-2016 LunarG, Inc.
 * Copyright (C) 2015-2016 Google Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and/or associated documentation files (the "Materials"), to
 * deal in the Materials without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Materials, and to permit persons to whom the Materials
 * are furnished to do so, subject to the following conditions:
 *
 * The above copyright notice(s) and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR THE
 * USE OR OTHER DEALINGS IN THE MATERIALS
 *
 * Author: Jeremy Hayes <jeremy@lunarg.com>
 * Author: Tony Barbour <tony@LunarG.com>
 * Author: Mark Lobodzinski <mark@LunarG.com>
 * Author: Dustin Graves <dustin@lunarg.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "vk_loader_platform.h"
#include "vulkan/vk_layer.h"
#include "vk_layer_config.h"
#include "vk_enum_validate_helper.h"
#include "vk_struct_validate_helper.h"

#include "vk_layer_table.h"
#include "vk_layer_data.h"
#include "vk_layer_logging.h"
#include "vk_layer_extension_utils.h"
#include "vk_layer_utils.h"

#include "parameter_validation.h"

struct layer_data {
    debug_report_data *report_data;
    std::vector<VkDebugReportCallbackEXT> logging_callback;

    // TODO: Split instance/device structs
    // Device Data
    // Map for queue family index to queue count
    std::unordered_map<uint32_t, uint32_t> queueFamilyIndexMap;

    layer_data() : report_data(nullptr){};
};

static std::unordered_map<void *, layer_data *> layer_data_map;
static device_table_map pc_device_table_map;
static instance_table_map pc_instance_table_map;

// "my instance data"
debug_report_data *mid(VkInstance object) {
    dispatch_key key = get_dispatch_key(object);
    layer_data *data = get_my_data_ptr(key, layer_data_map);
#if DISPATCH_MAP_DEBUG
    fprintf(stderr, "MID: map: %p, object: %p, key: %p, data: %p\n", &layer_data_map, object, key, data);
#endif
    assert(data != NULL);

    return data->report_data;
}

// "my device data"
debug_report_data *mdd(void *object) {
    dispatch_key key = get_dispatch_key(object);
    layer_data *data = get_my_data_ptr(key, layer_data_map);
#if DISPATCH_MAP_DEBUG
    fprintf(stderr, "MDD: map: %p, object: %p, key: %p, data: %p\n", &layer_data_map, object, key, data);
#endif
    assert(data != NULL);
    return data->report_data;
}

static void init_parameter_validation(layer_data *my_data, const VkAllocationCallbacks *pAllocator) {

    layer_debug_actions(my_data->report_data, my_data->logging_callback, pAllocator, "lunarg_parameter_validation");
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
                               const VkAllocationCallbacks *pAllocator, VkDebugReportCallbackEXT *pMsgCallback) {
    VkLayerInstanceDispatchTable *pTable = get_dispatch_table(pc_instance_table_map, instance);
    VkResult result = pTable->CreateDebugReportCallbackEXT(instance, pCreateInfo, pAllocator, pMsgCallback);

    if (result == VK_SUCCESS) {
        layer_data *data = get_my_data_ptr(get_dispatch_key(instance), layer_data_map);
        result = layer_create_msg_callback(data->report_data, pCreateInfo, pAllocator, pMsgCallback);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDebugReportCallbackEXT(VkInstance instance,
                                                                           VkDebugReportCallbackEXT msgCallback,
                                                                           const VkAllocationCallbacks *pAllocator) {
    VkLayerInstanceDispatchTable *pTable = get_dispatch_table(pc_instance_table_map, instance);
    pTable->DestroyDebugReportCallbackEXT(instance, msgCallback, pAllocator);

    layer_data *data = get_my_data_ptr(get_dispatch_key(instance), layer_data_map);
    layer_destroy_msg_callback(data->report_data, msgCallback, pAllocator);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDebugReportMessageEXT(VkInstance instance, VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t object,
                        size_t location, int32_t msgCode, const char *pLayerPrefix, const char *pMsg) {
    VkLayerInstanceDispatchTable *pTable = get_dispatch_table(pc_instance_table_map, instance);
    pTable->DebugReportMessageEXT(instance, flags, objType, object, location, msgCode, pLayerPrefix, pMsg);
}

static const VkExtensionProperties instance_extensions[] = {{VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_SPEC_VERSION}};

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount, VkExtensionProperties *pProperties) {
    return util_GetExtensionProperties(1, instance_extensions, pCount, pProperties);
}

static const VkLayerProperties pc_global_layers[] = {{
    "VK_LAYER_LUNARG_parameter_validation", VK_LAYER_API_VERSION, 1, "LunarG Validation Layer",
}};

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateInstanceLayerProperties(uint32_t *pCount, VkLayerProperties *pProperties) {
    return util_GetLayerProperties(ARRAY_SIZE(pc_global_layers), pc_global_layers, pCount, pProperties);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                                                                    const char *pLayerName, uint32_t *pCount,
                                                                                    VkExtensionProperties *pProperties) {
    /* parameter_validation does not have any physical device extensions */
    if (pLayerName == NULL) {
        return get_dispatch_table(pc_instance_table_map, physicalDevice)
            ->EnumerateDeviceExtensionProperties(physicalDevice, NULL, pCount, pProperties);
    } else {
        return util_GetExtensionProperties(0, NULL, pCount, pProperties);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pCount, VkLayerProperties *pProperties) {

    /* parameter_validation's physical device layers are the same as global */
    return util_GetLayerProperties(ARRAY_SIZE(pc_global_layers), pc_global_layers, pCount, pProperties);
}

static bool ValidateEnumerator(VkFormatFeatureFlagBits const &enumerator) {
    VkFormatFeatureFlagBits allFlags = (VkFormatFeatureFlagBits)(
        VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT |
        VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT |
        VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT |
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkFormatFeatureFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_BLIT_SRC_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_BLIT_SRC_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_BLIT_DST_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_BLIT_DST_BIT");
    }
    if (enumerator & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) {
        strings.push_back("VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkImageUsageFlagBits const &enumerator) {
    VkImageUsageFlagBits allFlags = (VkImageUsageFlagBits)(
        VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkImageUsageFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
        strings.push_back("VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT");
    }
    if (enumerator & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        strings.push_back("VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT");
    }
    if (enumerator & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        strings.push_back("VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT");
    }
    if (enumerator & VK_IMAGE_USAGE_STORAGE_BIT) {
        strings.push_back("VK_IMAGE_USAGE_STORAGE_BIT");
    }
    if (enumerator & VK_IMAGE_USAGE_SAMPLED_BIT) {
        strings.push_back("VK_IMAGE_USAGE_SAMPLED_BIT");
    }
    if (enumerator & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        strings.push_back("VK_IMAGE_USAGE_TRANSFER_DST_BIT");
    }
    if (enumerator & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
        strings.push_back("VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT");
    }
    if (enumerator & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        strings.push_back("VK_IMAGE_USAGE_TRANSFER_SRC_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkQueueFlagBits const &enumerator) {
    VkQueueFlagBits allFlags =
        (VkQueueFlagBits)(VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_SPARSE_BINDING_BIT | VK_QUEUE_GRAPHICS_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkQueueFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_QUEUE_TRANSFER_BIT) {
        strings.push_back("VK_QUEUE_TRANSFER_BIT");
    }
    if (enumerator & VK_QUEUE_COMPUTE_BIT) {
        strings.push_back("VK_QUEUE_COMPUTE_BIT");
    }
    if (enumerator & VK_QUEUE_SPARSE_BINDING_BIT) {
        strings.push_back("VK_QUEUE_SPARSE_BINDING_BIT");
    }
    if (enumerator & VK_QUEUE_GRAPHICS_BIT) {
        strings.push_back("VK_QUEUE_GRAPHICS_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkMemoryPropertyFlagBits const &enumerator) {
    VkMemoryPropertyFlagBits allFlags = (VkMemoryPropertyFlagBits)(
        VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkMemoryPropertyFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) {
        strings.push_back("VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT");
    }
    if (enumerator & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {
        strings.push_back("VK_MEMORY_PROPERTY_HOST_COHERENT_BIT");
    }
    if (enumerator & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        strings.push_back("VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT");
    }
    if (enumerator & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) {
        strings.push_back("VK_MEMORY_PROPERTY_HOST_CACHED_BIT");
    }
    if (enumerator & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
        strings.push_back("VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkMemoryHeapFlagBits const &enumerator) {
    VkMemoryHeapFlagBits allFlags = (VkMemoryHeapFlagBits)(VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkMemoryHeapFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
        strings.push_back("VK_MEMORY_HEAP_DEVICE_LOCAL_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkSparseImageFormatFlagBits const &enumerator) {
    VkSparseImageFormatFlagBits allFlags =
        (VkSparseImageFormatFlagBits)(VK_SPARSE_IMAGE_FORMAT_NONSTANDARD_BLOCK_SIZE_BIT |
                                      VK_SPARSE_IMAGE_FORMAT_ALIGNED_MIP_SIZE_BIT | VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkSparseImageFormatFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_SPARSE_IMAGE_FORMAT_NONSTANDARD_BLOCK_SIZE_BIT) {
        strings.push_back("VK_SPARSE_IMAGE_FORMAT_NONSTANDARD_BLOCK_SIZE_BIT");
    }
    if (enumerator & VK_SPARSE_IMAGE_FORMAT_ALIGNED_MIP_SIZE_BIT) {
        strings.push_back("VK_SPARSE_IMAGE_FORMAT_ALIGNED_MIP_SIZE_BIT");
    }
    if (enumerator & VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT) {
        strings.push_back("VK_SPARSE_IMAGE_FORMAT_SINGLE_MIPTAIL_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkFenceCreateFlagBits const &enumerator) {
    VkFenceCreateFlagBits allFlags = (VkFenceCreateFlagBits)(VK_FENCE_CREATE_SIGNALED_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkFenceCreateFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_FENCE_CREATE_SIGNALED_BIT) {
        strings.push_back("VK_FENCE_CREATE_SIGNALED_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkQueryPipelineStatisticFlagBits const &enumerator) {
    VkQueryPipelineStatisticFlagBits allFlags = (VkQueryPipelineStatisticFlagBits)(
        VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT | VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
        VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT | VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
        VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT | VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT |
        VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT | VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
        VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT |
        VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT |
        VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkQueryPipelineStatisticFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS_BIT");
    }
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT");
    }
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT");
    }
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT");
    }
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT");
    }
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES_BIT");
    }
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT");
    }
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT");
    }
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT");
    }
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS_BIT");
    }
    if (enumerator & VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT) {
        strings.push_back("VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkQueryResultFlagBits const &enumerator) {
    VkQueryResultFlagBits allFlags = (VkQueryResultFlagBits)(VK_QUERY_RESULT_PARTIAL_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT |
                                                             VK_QUERY_RESULT_WAIT_BIT | VK_QUERY_RESULT_64_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkQueryResultFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_QUERY_RESULT_PARTIAL_BIT) {
        strings.push_back("VK_QUERY_RESULT_PARTIAL_BIT");
    }
    if (enumerator & VK_QUERY_RESULT_WITH_AVAILABILITY_BIT) {
        strings.push_back("VK_QUERY_RESULT_WITH_AVAILABILITY_BIT");
    }
    if (enumerator & VK_QUERY_RESULT_WAIT_BIT) {
        strings.push_back("VK_QUERY_RESULT_WAIT_BIT");
    }
    if (enumerator & VK_QUERY_RESULT_64_BIT) {
        strings.push_back("VK_QUERY_RESULT_64_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkBufferUsageFlagBits const &enumerator) {
    VkBufferUsageFlagBits allFlags = (VkBufferUsageFlagBits)(
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT |
        VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkBufferUsageFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
        strings.push_back("VK_BUFFER_USAGE_VERTEX_BUFFER_BIT");
    }
    if (enumerator & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        strings.push_back("VK_BUFFER_USAGE_INDEX_BUFFER_BIT");
    }
    if (enumerator & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT) {
        strings.push_back("VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT");
    }
    if (enumerator & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT) {
        strings.push_back("VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT");
    }
    if (enumerator & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        strings.push_back("VK_BUFFER_USAGE_STORAGE_BUFFER_BIT");
    }
    if (enumerator & VK_BUFFER_USAGE_TRANSFER_DST_BIT) {
        strings.push_back("VK_BUFFER_USAGE_TRANSFER_DST_BIT");
    }
    if (enumerator & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT) {
        strings.push_back("VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT");
    }
    if (enumerator & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) {
        strings.push_back("VK_BUFFER_USAGE_TRANSFER_SRC_BIT");
    }
    if (enumerator & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        strings.push_back("VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkBufferCreateFlagBits const &enumerator) {
    VkBufferCreateFlagBits allFlags = (VkBufferCreateFlagBits)(
        VK_BUFFER_CREATE_SPARSE_ALIASED_BIT | VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT | VK_BUFFER_CREATE_SPARSE_BINDING_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkBufferCreateFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_BUFFER_CREATE_SPARSE_ALIASED_BIT) {
        strings.push_back("VK_BUFFER_CREATE_SPARSE_ALIASED_BIT");
    }
    if (enumerator & VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT) {
        strings.push_back("VK_BUFFER_CREATE_SPARSE_RESIDENCY_BIT");
    }
    if (enumerator & VK_BUFFER_CREATE_SPARSE_BINDING_BIT) {
        strings.push_back("VK_BUFFER_CREATE_SPARSE_BINDING_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkImageCreateFlagBits const &enumerator) {
    VkImageCreateFlagBits allFlags = (VkImageCreateFlagBits)(
        VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT | VK_IMAGE_CREATE_SPARSE_ALIASED_BIT | VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT |
        VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_SPARSE_BINDING_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkImageCreateFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) {
        strings.push_back("VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT");
    }
    if (enumerator & VK_IMAGE_CREATE_SPARSE_ALIASED_BIT) {
        strings.push_back("VK_IMAGE_CREATE_SPARSE_ALIASED_BIT");
    }
    if (enumerator & VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT) {
        strings.push_back("VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT");
    }
    if (enumerator & VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT) {
        strings.push_back("VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT");
    }
    if (enumerator & VK_IMAGE_CREATE_SPARSE_BINDING_BIT) {
        strings.push_back("VK_IMAGE_CREATE_SPARSE_BINDING_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkColorComponentFlagBits const &enumerator) {
    VkColorComponentFlagBits allFlags = (VkColorComponentFlagBits)(VK_COLOR_COMPONENT_A_BIT | VK_COLOR_COMPONENT_B_BIT |
                                                                   VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_R_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkColorComponentFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_COLOR_COMPONENT_A_BIT) {
        strings.push_back("VK_COLOR_COMPONENT_A_BIT");
    }
    if (enumerator & VK_COLOR_COMPONENT_B_BIT) {
        strings.push_back("VK_COLOR_COMPONENT_B_BIT");
    }
    if (enumerator & VK_COLOR_COMPONENT_G_BIT) {
        strings.push_back("VK_COLOR_COMPONENT_G_BIT");
    }
    if (enumerator & VK_COLOR_COMPONENT_R_BIT) {
        strings.push_back("VK_COLOR_COMPONENT_R_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkPipelineCreateFlagBits const &enumerator) {
    VkPipelineCreateFlagBits allFlags = (VkPipelineCreateFlagBits)(
        VK_PIPELINE_CREATE_DERIVATIVE_BIT | VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT | VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkPipelineCreateFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_PIPELINE_CREATE_DERIVATIVE_BIT) {
        strings.push_back("VK_PIPELINE_CREATE_DERIVATIVE_BIT");
    }
    if (enumerator & VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT) {
        strings.push_back("VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT");
    }
    if (enumerator & VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT) {
        strings.push_back("VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkShaderStageFlagBits const &enumerator) {
    VkShaderStageFlagBits allFlags = (VkShaderStageFlagBits)(
        VK_SHADER_STAGE_ALL | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_COMPUTE_BIT |
        VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_VERTEX_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkShaderStageFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_SHADER_STAGE_ALL) {
        strings.push_back("VK_SHADER_STAGE_ALL");
    }
    if (enumerator & VK_SHADER_STAGE_FRAGMENT_BIT) {
        strings.push_back("VK_SHADER_STAGE_FRAGMENT_BIT");
    }
    if (enumerator & VK_SHADER_STAGE_GEOMETRY_BIT) {
        strings.push_back("VK_SHADER_STAGE_GEOMETRY_BIT");
    }
    if (enumerator & VK_SHADER_STAGE_COMPUTE_BIT) {
        strings.push_back("VK_SHADER_STAGE_COMPUTE_BIT");
    }
    if (enumerator & VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT) {
        strings.push_back("VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT");
    }
    if (enumerator & VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT) {
        strings.push_back("VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT");
    }
    if (enumerator & VK_SHADER_STAGE_VERTEX_BIT) {
        strings.push_back("VK_SHADER_STAGE_VERTEX_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkPipelineStageFlagBits const &enumerator) {
    VkPipelineStageFlagBits allFlags = (VkPipelineStageFlagBits)(
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT | VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_HOST_BIT |
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
        VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
        VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT |
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkPipelineStageFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_PIPELINE_STAGE_ALL_COMMANDS_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_ALL_COMMANDS_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_HOST_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_HOST_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_TRANSFER_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_TRANSFER_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_VERTEX_SHADER_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_VERTEX_SHADER_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_VERTEX_INPUT_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_VERTEX_INPUT_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT");
    }
    if (enumerator & VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT) {
        strings.push_back("VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkAccessFlagBits const &enumerator) {
    VkAccessFlagBits allFlags = (VkAccessFlagBits)(
        VK_ACCESS_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
        VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT |
        VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT);

    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkAccessFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_ACCESS_INDIRECT_COMMAND_READ_BIT) {
        strings.push_back("VK_ACCESS_INDIRECT_COMMAND_READ_BIT");
    }
    if (enumerator & VK_ACCESS_INDEX_READ_BIT) {
        strings.push_back("VK_ACCESS_INDEX_READ_BIT");
    }
    if (enumerator & VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT) {
        strings.push_back("VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT");
    }
    if (enumerator & VK_ACCESS_UNIFORM_READ_BIT) {
        strings.push_back("VK_ACCESS_UNIFORM_READ_BIT");
    }
    if (enumerator & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) {
        strings.push_back("VK_ACCESS_INPUT_ATTACHMENT_READ_BIT");
    }
    if (enumerator & VK_ACCESS_SHADER_READ_BIT) {
        strings.push_back("VK_ACCESS_SHADER_READ_BIT");
    }
    if (enumerator & VK_ACCESS_SHADER_WRITE_BIT) {
        strings.push_back("VK_ACCESS_SHADER_WRITE_BIT");
    }
    if (enumerator & VK_ACCESS_COLOR_ATTACHMENT_READ_BIT) {
        strings.push_back("VK_ACCESS_COLOR_ATTACHMENT_READ_BIT");
    }
    if (enumerator & VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT) {
        strings.push_back("VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT");
    }
    if (enumerator & VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT) {
        strings.push_back("VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT");
    }
    if (enumerator & VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT) {
        strings.push_back("VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT");
    }
    if (enumerator & VK_ACCESS_TRANSFER_READ_BIT) {
        strings.push_back("VK_ACCESS_TRANSFER_READ_BIT");
    }
    if (enumerator & VK_ACCESS_TRANSFER_WRITE_BIT) {
        strings.push_back("VK_ACCESS_TRANSFER_WRITE_BIT");
    }
    if (enumerator & VK_ACCESS_HOST_READ_BIT) {
        strings.push_back("VK_ACCESS_HOST_READ_BIT");
    }
    if (enumerator & VK_ACCESS_HOST_WRITE_BIT) {
        strings.push_back("VK_ACCESS_HOST_WRITE_BIT");
    }
    if (enumerator & VK_ACCESS_MEMORY_READ_BIT) {
        strings.push_back("VK_ACCESS_MEMORY_READ_BIT");
    }
    if (enumerator & VK_ACCESS_MEMORY_WRITE_BIT) {
        strings.push_back("VK_ACCESS_MEMORY_WRITE_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkCommandPoolCreateFlagBits const &enumerator) {
    VkCommandPoolCreateFlagBits allFlags =
        (VkCommandPoolCreateFlagBits)(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkCommandPoolCreateFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) {
        strings.push_back("VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT");
    }
    if (enumerator & VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) {
        strings.push_back("VK_COMMAND_POOL_CREATE_TRANSIENT_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkCommandPoolResetFlagBits const &enumerator) {
    VkCommandPoolResetFlagBits allFlags = (VkCommandPoolResetFlagBits)(VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkCommandPoolResetFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT) {
        strings.push_back("VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkCommandBufferUsageFlags const &enumerator) {
    VkCommandBufferUsageFlags allFlags =
        (VkCommandBufferUsageFlags)(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT |
                                    VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkCommandBufferUsageFlags const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT) {
        strings.push_back("VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT");
    }
    if (enumerator & VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) {
        strings.push_back("VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT");
    }
    if (enumerator & VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT) {
        strings.push_back("VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkCommandBufferResetFlagBits const &enumerator) {
    VkCommandBufferResetFlagBits allFlags = (VkCommandBufferResetFlagBits)(VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkCommandBufferResetFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) {
        strings.push_back("VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool ValidateEnumerator(VkImageAspectFlagBits const &enumerator) {
    VkImageAspectFlagBits allFlags = (VkImageAspectFlagBits)(VK_IMAGE_ASPECT_METADATA_BIT | VK_IMAGE_ASPECT_STENCIL_BIT |
                                                             VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_COLOR_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkImageAspectFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_IMAGE_ASPECT_METADATA_BIT) {
        strings.push_back("VK_IMAGE_ASPECT_METADATA_BIT");
    }
    if (enumerator & VK_IMAGE_ASPECT_STENCIL_BIT) {
        strings.push_back("VK_IMAGE_ASPECT_STENCIL_BIT");
    }
    if (enumerator & VK_IMAGE_ASPECT_DEPTH_BIT) {
        strings.push_back("VK_IMAGE_ASPECT_DEPTH_BIT");
    }
    if (enumerator & VK_IMAGE_ASPECT_COLOR_BIT) {
        strings.push_back("VK_IMAGE_ASPECT_COLOR_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static bool validate_queue_family_indices(VkDevice device, const char *function_name, const uint32_t count,
                                          const uint32_t *indices) {
    bool skipCall = false;
    layer_data *my_device_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);

    for (auto i = 0u; i < count; i++) {
        if (indices[i] == VK_QUEUE_FAMILY_IGNORED) {
            skipCall |=
                log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                        "%s: the specified queueFamilyIndex cannot be VK_QUEUE_FAMILY_IGNORED.", function_name);
        } else {
            const auto &queue_data = my_device_data->queueFamilyIndexMap.find(indices[i]);
            if (queue_data == my_device_data->queueFamilyIndexMap.end()) {
                skipCall |= log_msg(
                    mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "VkGetDeviceQueue parameter, uint32_t queueFamilyIndex %d, must have been given when the device was created.",
                    indices[i]);
                return false;
            }
        }
    }
    return skipCall;
}

static bool ValidateEnumerator(VkQueryControlFlagBits const &enumerator) {
    VkQueryControlFlagBits allFlags = (VkQueryControlFlagBits)(VK_QUERY_CONTROL_PRECISE_BIT);
    if (enumerator & (~allFlags)) {
        return false;
    }

    return true;
}

static std::string EnumeratorString(VkQueryControlFlagBits const &enumerator) {
    if (!ValidateEnumerator(enumerator)) {
        return "unrecognized enumerator";
    }

    std::vector<std::string> strings;
    if (enumerator & VK_QUERY_CONTROL_PRECISE_BIT) {
        strings.push_back("VK_QUERY_CONTROL_PRECISE_BIT");
    }

    std::string enumeratorString;
    for (auto const &string : strings) {
        enumeratorString += string;

        if (string != strings.back()) {
            enumeratorString += '|';
        }
    }

    return enumeratorString;
}

static const int MaxParamCheckerStringLength = 256;

static VkBool32 validate_string(debug_report_data *report_data, const char *apiName, const char *stringName,
                                const char *validateString) {
    assert(apiName != nullptr);
    assert(stringName != nullptr);
    assert(validateString != nullptr);

    VkBool32 skipCall = VK_FALSE;

    VkStringErrorFlags result = vk_string_validate(MaxParamCheckerStringLength, validateString);

    if (result == VK_STRING_ERROR_NONE) {
        return skipCall;
    } else if (result & VK_STRING_ERROR_LENGTH) {
        skipCall = log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                           "%s: string %s exceeds max length %d", apiName, stringName, MaxParamCheckerStringLength);
    } else if (result & VK_STRING_ERROR_BAD_DATA) {
        skipCall = log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                           "%s: string %s contains invalid characters or is badly formed", apiName, stringName);
    }
    return skipCall;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateInstance(const VkInstanceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkInstance *pInstance) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;

    VkLayerInstanceCreateInfo *chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);
    assert(chain_info != nullptr);
    assert(chain_info->u.pLayerInfo != nullptr);

    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateInstance fpCreateInstance = (PFN_vkCreateInstance)fpGetInstanceProcAddr(NULL, "vkCreateInstance");
    if (fpCreateInstance == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    // Advance the link info for the next element on the chain
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

    result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);

    if (result == VK_SUCCESS) {
        layer_data *my_instance_data = get_my_data_ptr(get_dispatch_key(*pInstance), layer_data_map);
        assert(my_instance_data != nullptr);

        VkLayerInstanceDispatchTable *pTable = initInstanceTable(*pInstance, fpGetInstanceProcAddr, pc_instance_table_map);

        my_instance_data->report_data = debug_report_create_instance(pTable, *pInstance, pCreateInfo->enabledExtensionCount,
                                                                     pCreateInfo->ppEnabledExtensionNames);

        init_parameter_validation(my_instance_data, pAllocator);

        // Ordinarily we'd check these before calling down the chain, but none of the layer
        // support is in place until now, if we survive we can report the issue now.
        parameter_validation_vkCreateInstance(my_instance_data->report_data, pCreateInfo, pAllocator, pInstance);

        if (pCreateInfo->pApplicationInfo) {
            if (pCreateInfo->pApplicationInfo->pApplicationName) {
                validate_string(my_instance_data->report_data, "vkCreateInstance",
                                "pCreateInfo->VkApplicationInfo->pApplicationName",
                                pCreateInfo->pApplicationInfo->pApplicationName);
            }

            if (pCreateInfo->pApplicationInfo->pEngineName) {
                validate_string(my_instance_data->report_data, "vkCreateInstance", "pCreateInfo->VkApplicationInfo->pEngineName",
                                pCreateInfo->pApplicationInfo->pEngineName);
            }
        }
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks *pAllocator) {
    // Grab the key before the instance is destroyed.
    dispatch_key key = get_dispatch_key(instance);
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(key, layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyInstance(my_data->report_data, pAllocator);

    if (skipCall == VK_FALSE) {
        VkLayerInstanceDispatchTable *pTable = get_dispatch_table(pc_instance_table_map, instance);
        pTable->DestroyInstance(instance, pAllocator);

        // Clean up logging callback, if any
        while (my_data->logging_callback.size() > 0) {
            VkDebugReportCallbackEXT callback = my_data->logging_callback.back();
            layer_destroy_msg_callback(my_data->report_data, callback, pAllocator);
            my_data->logging_callback.pop_back();
        }

        layer_debug_report_destroy_instance(mid(instance));
        layer_data_map.erase(pTable);

        pc_instance_table_map.erase(key);
        layer_data_map.erase(key);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDevices(VkInstance instance, uint32_t *pPhysicalDeviceCount, VkPhysicalDevice *pPhysicalDevices) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(instance), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkEnumeratePhysicalDevices(my_data->report_data, pPhysicalDeviceCount, pPhysicalDevices);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_instance_table_map, instance)
                     ->EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);

        validate_result(my_data->report_data, "vkEnumeratePhysicalDevices", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures *pFeatures) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(physicalDevice), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetPhysicalDeviceFeatures(my_data->report_data, pFeatures);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_instance_table_map, physicalDevice)->GetPhysicalDeviceFeatures(physicalDevice, pFeatures);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties *pFormatProperties) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(physicalDevice), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetPhysicalDeviceFormatProperties(my_data->report_data, format, pFormatProperties);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_instance_table_map, physicalDevice)
            ->GetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling,
                                         VkImageUsageFlags usage, VkImageCreateFlags flags,
                                         VkImageFormatProperties *pImageFormatProperties) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(physicalDevice), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetPhysicalDeviceImageFormatProperties(my_data->report_data, format, type, tiling, usage, flags,
                                                                     pImageFormatProperties);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_instance_table_map, physicalDevice)
                     ->GetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags,
                                                              pImageFormatProperties);

        validate_result(my_data->report_data, "vkGetPhysicalDeviceImageFormatProperties", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *pProperties) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(physicalDevice), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetPhysicalDeviceProperties(my_data->report_data, pProperties);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_instance_table_map, physicalDevice)->GetPhysicalDeviceProperties(physicalDevice, pProperties);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t *pQueueFamilyPropertyCount,
                                         VkQueueFamilyProperties *pQueueFamilyProperties) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(physicalDevice), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetPhysicalDeviceQueueFamilyProperties(my_data->report_data, pQueueFamilyPropertyCount,
                                                                     pQueueFamilyProperties);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_instance_table_map, physicalDevice)
            ->GetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties *pMemoryProperties) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(physicalDevice), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetPhysicalDeviceMemoryProperties(my_data->report_data, pMemoryProperties);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_instance_table_map, physicalDevice)
            ->GetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
    }
}

void validateDeviceCreateInfo(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo *pCreateInfo,
                              const std::vector<VkQueueFamilyProperties> properties) {
    std::unordered_set<uint32_t> set;

    if ((pCreateInfo != nullptr) && (pCreateInfo->pQueueCreateInfos != nullptr)) {
        for (uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; ++i) {
            if (set.count(pCreateInfo->pQueueCreateInfos[i].queueFamilyIndex)) {
                log_msg(mdd(physicalDevice), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                        "PARAMCHECK",
                        "VkDeviceCreateInfo parameter, uint32_t pQueueCreateInfos[%d]->queueFamilyIndex, is not unique within this "
                        "structure.",
                        i);
            } else {
                set.insert(pCreateInfo->pQueueCreateInfos[i].queueFamilyIndex);
            }

            if (pCreateInfo->pQueueCreateInfos[i].queueCount == 0) {
                log_msg(mdd(physicalDevice), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                        "PARAMCHECK", "VkDeviceCreateInfo parameter, uint32_t pQueueCreateInfos[%d]->queueCount, cannot be zero.",
                        i);
            }

            if (pCreateInfo->pQueueCreateInfos[i].pQueuePriorities != nullptr) {
                for (uint32_t j = 0; j < pCreateInfo->pQueueCreateInfos[i].queueCount; ++j) {
                    if ((pCreateInfo->pQueueCreateInfos[i].pQueuePriorities[j] < 0.f) ||
                        (pCreateInfo->pQueueCreateInfos[i].pQueuePriorities[j] > 1.f)) {
                        log_msg(mdd(physicalDevice), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                                "PARAMCHECK",
                                "VkDeviceCreateInfo parameter, uint32_t pQueueCreateInfos[%d]->pQueuePriorities[%d], must be "
                                "between 0 and 1. Actual value is %f",
                                i, j, pCreateInfo->pQueueCreateInfos[i].pQueuePriorities[j]);
                    }
                }
            }

            if (pCreateInfo->pQueueCreateInfos[i].queueFamilyIndex >= properties.size()) {
                log_msg(
                    mdd(physicalDevice), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "VkDeviceCreateInfo parameter, uint32_t pQueueCreateInfos[%d]->queueFamilyIndex cannot be more than the number "
                    "of queue families.",
                    i);
            } else if (pCreateInfo->pQueueCreateInfos[i].queueCount >
                       properties[pCreateInfo->pQueueCreateInfos[i].queueFamilyIndex].queueCount) {
                log_msg(
                    mdd(physicalDevice), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "VkDeviceCreateInfo parameter, uint32_t pQueueCreateInfos[%d]->queueCount cannot be more than the number of "
                    "queues for the given family index.",
                    i);
            }
        }
    }
}

void storeCreateDeviceData(VkDevice device, const VkDeviceCreateInfo *pCreateInfo) {
    layer_data *my_device_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);

    if ((pCreateInfo != nullptr) && (pCreateInfo->pQueueCreateInfos != nullptr)) {
        for (uint32_t i = 0; i < pCreateInfo->queueCreateInfoCount; ++i) {
            my_device_data->queueFamilyIndexMap.insert(
                std::make_pair(pCreateInfo->pQueueCreateInfos[i].queueFamilyIndex, pCreateInfo->pQueueCreateInfos[i].queueCount));
        }
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(VkPhysicalDevice physicalDevice,
                                                              const VkDeviceCreateInfo *pCreateInfo,
                                                              const VkAllocationCallbacks *pAllocator, VkDevice *pDevice) {
    /*
     * NOTE: We do not validate physicalDevice or any dispatchable
     * object as the first parameter. We couldn't get here if it was wrong!
     */

    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_instance_data = get_my_data_ptr(get_dispatch_key(physicalDevice), layer_data_map);
    assert(my_instance_data != nullptr);

    skipCall |= parameter_validation_vkCreateDevice(my_instance_data->report_data, pCreateInfo, pAllocator, pDevice);

    if (pCreateInfo != NULL) {
        if ((pCreateInfo->enabledLayerCount > 0) && (pCreateInfo->ppEnabledLayerNames != NULL)) {
            for (size_t i = 0; i < pCreateInfo->enabledLayerCount; i++) {
                skipCall |= validate_string(my_instance_data->report_data, "vkCreateDevice", "pCreateInfo->ppEnabledLayerNames",
                                            pCreateInfo->ppEnabledLayerNames[i]);
            }
        }

        if ((pCreateInfo->enabledExtensionCount > 0) && (pCreateInfo->ppEnabledExtensionNames != NULL)) {
            for (size_t i = 0; i < pCreateInfo->enabledExtensionCount; i++) {
                skipCall |= validate_string(my_instance_data->report_data, "vkCreateDevice", "pCreateInfo->ppEnabledExtensionNames",
                                            pCreateInfo->ppEnabledExtensionNames[i]);
            }
        }
    }

    if (skipCall == VK_FALSE) {
        VkLayerDeviceCreateInfo *chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);
        assert(chain_info != nullptr);
        assert(chain_info->u.pLayerInfo != nullptr);

        PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
        PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr = chain_info->u.pLayerInfo->pfnNextGetDeviceProcAddr;
        PFN_vkCreateDevice fpCreateDevice = (PFN_vkCreateDevice)fpGetInstanceProcAddr(NULL, "vkCreateDevice");
        if (fpCreateDevice == NULL) {
            return VK_ERROR_INITIALIZATION_FAILED;
        }

        // Advance the link info for the next element on the chain
        chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;

        result = fpCreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);

        validate_result(my_instance_data->report_data, "vkCreateDevice", result);

        if (result == VK_SUCCESS) {
            layer_data *my_device_data = get_my_data_ptr(get_dispatch_key(*pDevice), layer_data_map);
            assert(my_device_data != nullptr);

            my_device_data->report_data = layer_debug_report_create_device(my_instance_data->report_data, *pDevice);
            initDeviceTable(*pDevice, fpGetDeviceProcAddr, pc_device_table_map);

            uint32_t count;
            get_dispatch_table(pc_instance_table_map, physicalDevice)
                ->GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
            std::vector<VkQueueFamilyProperties> properties(count);
            get_dispatch_table(pc_instance_table_map, physicalDevice)
                ->GetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, &properties[0]);

            validateDeviceCreateInfo(physicalDevice, pCreateInfo, properties);
            storeCreateDeviceData(*pDevice, pCreateInfo);
        }
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(VkDevice device, const VkAllocationCallbacks *pAllocator) {
    dispatch_key key = get_dispatch_key(device);
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(key, layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyDevice(my_data->report_data, pAllocator);

    if (skipCall == VK_FALSE) {
        layer_debug_report_destroy_device(device);

#if DISPATCH_MAP_DEBUG
        fprintf(stderr, "Device: %p, key: %p\n", device, key);
#endif

        get_dispatch_table(pc_device_table_map, device)->DestroyDevice(device, pAllocator);
        pc_device_table_map.erase(key);
        layer_data_map.erase(key);
    }
}

bool PreGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex) {
    layer_data *my_device_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_device_data != nullptr);

    validate_queue_family_indices(device, "vkGetDeviceQueue", 1, &queueFamilyIndex);

    const auto &queue_data = my_device_data->queueFamilyIndexMap.find(queueFamilyIndex);
    if (queue_data->second <= queueIndex) {
        log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                "VkGetDeviceQueue parameter, uint32_t queueIndex %d, must be less than the number of queues given when the device "
                "was created.",
                queueIndex);
        return false;
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue *pQueue) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetDeviceQueue(my_data->report_data, queueFamilyIndex, queueIndex, pQueue);

    if (skipCall == VK_FALSE) {
        PreGetDeviceQueue(device, queueFamilyIndex, queueIndex);

        get_dispatch_table(pc_device_table_map, device)->GetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo *pSubmits, VkFence fence) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(queue), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkQueueSubmit(my_data->report_data, submitCount, pSubmits, fence);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, queue)->QueueSubmit(queue, submitCount, pSubmits, fence);

        validate_result(my_data->report_data, "vkQueueSubmit", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueWaitIdle(VkQueue queue) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(queue), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, queue)->QueueWaitIdle(queue);

    validate_result(my_data->report_data, "vkQueueWaitIdle", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(VkDevice device) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, device)->DeviceWaitIdle(device);

    validate_result(my_data->report_data, "vkDeviceWaitIdle", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo *pAllocateInfo,
                                                                const VkAllocationCallbacks *pAllocator, VkDeviceMemory *pMemory) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkAllocateMemory(my_data->report_data, pAllocateInfo, pAllocator, pMemory);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->AllocateMemory(device, pAllocateInfo, pAllocator, pMemory);

        validate_result(my_data->report_data, "vkAllocateMemory", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkFreeMemory(my_data->report_data, memory, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->FreeMemory(device, memory, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void **ppData) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkMapMemory(my_data->report_data, memory, offset, size, flags, ppData);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->MapMemory(device, memory, offset, size, flags, ppData);

        validate_result(my_data->report_data, "vkMapMemory", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange *pMemoryRanges) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkFlushMappedMemoryRanges(my_data->report_data, memoryRangeCount, pMemoryRanges);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->FlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);

        validate_result(my_data->report_data, "vkFlushMappedMemoryRanges", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange *pMemoryRanges) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkInvalidateMappedMemoryRanges(my_data->report_data, memoryRangeCount, pMemoryRanges);

    if (skipCall == VK_FALSE) {
        result =
            get_dispatch_table(pc_device_table_map, device)->InvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);

        validate_result(my_data->report_data, "vkInvalidateMappedMemoryRanges", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory, VkDeviceSize *pCommittedMemoryInBytes) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetDeviceMemoryCommitment(my_data->report_data, memory, pCommittedMemoryInBytes);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->GetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory mem, VkDeviceSize memoryOffset) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, device)->BindBufferMemory(device, buffer, mem, memoryOffset);

    validate_result(my_data->report_data, "vkBindBufferMemory", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory mem, VkDeviceSize memoryOffset) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, device)->BindImageMemory(device, image, mem, memoryOffset);

    validate_result(my_data->report_data, "vkBindImageMemory", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements *pMemoryRequirements) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetBufferMemoryRequirements(my_data->report_data, buffer, pMemoryRequirements);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->GetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements *pMemoryRequirements) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetImageMemoryRequirements(my_data->report_data, image, pMemoryRequirements);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->GetImageMemoryRequirements(device, image, pMemoryRequirements);
    }
}

bool PostGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t *pNumRequirements,
                                          VkSparseImageMemoryRequirements *pSparseMemoryRequirements) {
    if (pSparseMemoryRequirements != nullptr) {
        if ((pSparseMemoryRequirements->formatProperties.aspectMask &
             (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT |
              VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "vkGetImageSparseMemoryRequirements parameter, VkImageAspect "
                    "pSparseMemoryRequirements->formatProperties.aspectMask, is an unrecognized enumerator");
            return false;
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t *pSparseMemoryRequirementCount,
                                   VkSparseImageMemoryRequirements *pSparseMemoryRequirements) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetImageSparseMemoryRequirements(my_data->report_data, image, pSparseMemoryRequirementCount,
                                                               pSparseMemoryRequirements);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)
            ->GetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);

        PostGetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    }
}

bool PostGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type,
                                                      VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling,
                                                      uint32_t *pNumProperties, VkSparseImageFormatProperties *pProperties) {
    if (pProperties != nullptr) {
        if ((pProperties->aspectMask & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT |
                                        VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(mdd(physicalDevice), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "vkGetPhysicalDeviceSparseImageFormatProperties parameter, VkImageAspect pProperties->aspectMask, is an "
                    "unrecognized enumerator");
            return false;
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type,
                                               VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling,
                                               uint32_t *pPropertyCount, VkSparseImageFormatProperties *pProperties) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(physicalDevice), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetPhysicalDeviceSparseImageFormatProperties(my_data->report_data, format, type, samples, usage,
                                                                           tiling, pPropertyCount, pProperties);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_instance_table_map, physicalDevice)
            ->GetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount,
                                                           pProperties);

        PostGetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount,
                                                         pProperties);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo *pBindInfo, VkFence fence) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(queue), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkQueueBindSparse(my_data->report_data, bindInfoCount, pBindInfo, fence);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, queue)->QueueBindSparse(queue, bindInfoCount, pBindInfo, fence);

        validate_result(my_data->report_data, "vkQueueBindSparse", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateFence(VkDevice device, const VkFenceCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkFence *pFence) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateFence(my_data->report_data, pCreateInfo, pAllocator, pFence);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->CreateFence(device, pCreateInfo, pAllocator, pFence);

        validate_result(my_data->report_data, "vkCreateFence", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyFence(my_data->report_data, fence, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyFence(device, fence, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkResetFences(my_data->report_data, fenceCount, pFences);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->ResetFences(device, fenceCount, pFences);

        validate_result(my_data->report_data, "vkResetFences", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceStatus(VkDevice device, VkFence fence) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, device)->GetFenceStatus(device, fence);

    validate_result(my_data->report_data, "vkGetFenceStatus", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence *pFences, VkBool32 waitAll, uint64_t timeout) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkWaitForFences(my_data->report_data, fenceCount, pFences, waitAll, timeout);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->WaitForFences(device, fenceCount, pFences, waitAll, timeout);

        validate_result(my_data->report_data, "vkWaitForFences", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo *pCreateInfo,
                                                                 const VkAllocationCallbacks *pAllocator, VkSemaphore *pSemaphore) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateSemaphore(my_data->report_data, pCreateInfo, pAllocator, pSemaphore);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->CreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);

        validate_result(my_data->report_data, "vkCreateSemaphore", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroySemaphore(my_data->report_data, semaphore, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroySemaphore(device, semaphore, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateEvent(VkDevice device, const VkEventCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkEvent *pEvent) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateEvent(my_data->report_data, pCreateInfo, pAllocator, pEvent);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->CreateEvent(device, pCreateInfo, pAllocator, pEvent);

        validate_result(my_data->report_data, "vkCreateEvent", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyEvent(my_data->report_data, event, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyEvent(device, event, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetEventStatus(VkDevice device, VkEvent event) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, device)->GetEventStatus(device, event);

    validate_result(my_data->report_data, "vkGetEventStatus", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkSetEvent(VkDevice device, VkEvent event) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, device)->SetEvent(device, event);

    validate_result(my_data->report_data, "vkSetEvent", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetEvent(VkDevice device, VkEvent event) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, device)->ResetEvent(device, event);

    validate_result(my_data->report_data, "vkSetEvent", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo *pCreateInfo,
                                                                 const VkAllocationCallbacks *pAllocator, VkQueryPool *pQueryPool) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateQueryPool(my_data->report_data, pCreateInfo, pAllocator, pQueryPool);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->CreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);

        validate_result(my_data->report_data, "vkCreateQueryPool", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyQueryPool(my_data->report_data, queryPool, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyQueryPool(device, queryPool, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery,
                                                                     uint32_t queryCount, size_t dataSize, void *pData,
                                                                     VkDeviceSize stride, VkQueryResultFlags flags) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |=
        parameter_validation_vkGetQueryPoolResults(my_data->report_data, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)
                     ->GetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);

        validate_result(my_data->report_data, "vkGetQueryPoolResults", result);
    }

    return result;
}

bool PreCreateBuffer(VkDevice device, const VkBufferCreateInfo *pCreateInfo) {
    if (pCreateInfo != nullptr) {
        if (pCreateInfo->sharingMode == VK_SHARING_MODE_CONCURRENT) {
            validate_queue_family_indices(device, "vkCreateBuffer", pCreateInfo->queueFamilyIndexCount,
                                          pCreateInfo->pQueueFamilyIndices);
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateBuffer(VkDevice device, const VkBufferCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkBuffer *pBuffer) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateBuffer(my_data->report_data, pCreateInfo, pAllocator, pBuffer);

    if (skipCall == VK_FALSE) {
        PreCreateBuffer(device, pCreateInfo);

        result = get_dispatch_table(pc_device_table_map, device)->CreateBuffer(device, pCreateInfo, pAllocator, pBuffer);

        validate_result(my_data->report_data, "vkCreateBuffer", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyBuffer(my_data->report_data, buffer, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyBuffer(device, buffer, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateBufferView(VkDevice device, const VkBufferViewCreateInfo *pCreateInfo,
                                                                  const VkAllocationCallbacks *pAllocator, VkBufferView *pView) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateBufferView(my_data->report_data, pCreateInfo, pAllocator, pView);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->CreateBufferView(device, pCreateInfo, pAllocator, pView);

        validate_result(my_data->report_data, "vkCreateBufferView", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyBufferView(my_data->report_data, bufferView, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyBufferView(device, bufferView, pAllocator);
    }
}

bool PreCreateImage(VkDevice device, const VkImageCreateInfo *pCreateInfo) {
    if (pCreateInfo != nullptr) {
        if (pCreateInfo->sharingMode == VK_SHARING_MODE_CONCURRENT) {
            validate_queue_family_indices(device, "vkCreateImage", pCreateInfo->queueFamilyIndexCount,
                                          pCreateInfo->pQueueFamilyIndices);
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateImage(VkDevice device, const VkImageCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkImage *pImage) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateImage(my_data->report_data, pCreateInfo, pAllocator, pImage);

    if (skipCall == VK_FALSE) {
        PreCreateImage(device, pCreateInfo);

        result = get_dispatch_table(pc_device_table_map, device)->CreateImage(device, pCreateInfo, pAllocator, pImage);

        validate_result(my_data->report_data, "vkCreateImage", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyImage(my_data->report_data, image, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyImage(device, image, pAllocator);
    }
}

bool PreGetImageSubresourceLayout(VkDevice device, const VkImageSubresource *pSubresource) {
    if (pSubresource != nullptr) {
        if ((pSubresource->aspectMask & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT |
                                         VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "vkGetImageSubresourceLayout parameter, VkImageAspect pSubresource->aspectMask, is an unrecognized enumerator");
            return false;
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource *pSubresource, VkSubresourceLayout *pLayout) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetImageSubresourceLayout(my_data->report_data, image, pSubresource, pLayout);

    if (skipCall == VK_FALSE) {
        PreGetImageSubresourceLayout(device, pSubresource);

        get_dispatch_table(pc_device_table_map, device)->GetImageSubresourceLayout(device, image, pSubresource, pLayout);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateImageView(VkDevice device, const VkImageViewCreateInfo *pCreateInfo,
                                                                 const VkAllocationCallbacks *pAllocator, VkImageView *pView) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateImageView(my_data->report_data, pCreateInfo, pAllocator, pView);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->CreateImageView(device, pCreateInfo, pAllocator, pView);

        validate_result(my_data->report_data, "vkCreateImageView", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyImageView(my_data->report_data, imageView, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyImageView(device, imageView, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo *pCreateInfo,
                                                                    const VkAllocationCallbacks *pAllocator,
                                                                    VkShaderModule *pShaderModule) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateShaderModule(my_data->report_data, pCreateInfo, pAllocator, pShaderModule);

    if (skipCall == VK_FALSE) {
        result =
            get_dispatch_table(pc_device_table_map, device)->CreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);

        validate_result(my_data->report_data, "vkCreateShaderModule", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyShaderModule(my_data->report_data, shaderModule, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyShaderModule(device, shaderModule, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo *pCreateInfo,
                                                                     const VkAllocationCallbacks *pAllocator,
                                                                     VkPipelineCache *pPipelineCache) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreatePipelineCache(my_data->report_data, pCreateInfo, pAllocator, pPipelineCache);

    if (skipCall == VK_FALSE) {
        result =
            get_dispatch_table(pc_device_table_map, device)->CreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);

        validate_result(my_data->report_data, "vkCreatePipelineCache", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyPipelineCache(my_data->report_data, pipelineCache, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyPipelineCache(device, pipelineCache, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t *pDataSize, void *pData) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetPipelineCacheData(my_data->report_data, pipelineCache, pDataSize, pData);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->GetPipelineCacheData(device, pipelineCache, pDataSize, pData);

        validate_result(my_data->report_data, "vkGetPipelineCacheData", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache *pSrcCaches) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkMergePipelineCaches(my_data->report_data, dstCache, srcCacheCount, pSrcCaches);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->MergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);

        validate_result(my_data->report_data, "vkMergePipelineCaches", result);
    }

    return result;
}

bool PreCreateGraphicsPipelines(VkDevice device, const VkGraphicsPipelineCreateInfo *pCreateInfos) {
    layer_data *data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);

    // TODO: Handle count
    if (pCreateInfos != nullptr) {
        if (pCreateInfos->flags | VK_PIPELINE_CREATE_DERIVATIVE_BIT) {
            if (pCreateInfos->basePipelineIndex != -1) {
                if (pCreateInfos->basePipelineHandle != VK_NULL_HANDLE) {
                    log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                            "vkCreateGraphicsPipelines parameter, pCreateInfos->basePipelineHandle, must be VK_NULL_HANDLE if "
                            "pCreateInfos->flags "
                            "contains the VK_PIPELINE_CREATE_DERIVATIVE_BIT flag and pCreateInfos->basePipelineIndex is not -1");
                    return false;
                }
            }

            if (pCreateInfos->basePipelineHandle != VK_NULL_HANDLE) {
                if (pCreateInfos->basePipelineIndex != -1) {
                    log_msg(
                        mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                        "vkCreateGraphicsPipelines parameter, pCreateInfos->basePipelineIndex, must be -1 if pCreateInfos->flags "
                        "contains the VK_PIPELINE_CREATE_DERIVATIVE_BIT flag and pCreateInfos->basePipelineHandle is not "
                        "VK_NULL_HANDLE");
                    return false;
                }
            }
        }

        if (pCreateInfos->pRasterizationState != nullptr) {
            if (pCreateInfos->pRasterizationState->cullMode & ~VK_CULL_MODE_FRONT_AND_BACK) {
                log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                        "vkCreateGraphicsPipelines parameter, VkCullMode pCreateInfos->pRasterizationState->cullMode, is an "
                        "unrecognized enumerator");
                return false;
            }
        }
        if (pCreateInfos->pColorBlendState != nullptr) {
            if (pCreateInfos->pColorBlendState->logicOpEnable == VK_TRUE &&
                (pCreateInfos->pColorBlendState->logicOp < VK_LOGIC_OP_BEGIN_RANGE ||
                 pCreateInfos->pColorBlendState->logicOp > VK_LOGIC_OP_END_RANGE)) {
                log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                        "vkCreateGraphicsPipelines parameter, VkLogicOp pCreateInfos->pColorBlendState->logicOp, is an "
                        "unrecognized enumerator");
                return false;
            }
            if (pCreateInfos->pColorBlendState->pAttachments != nullptr &&
                pCreateInfos->pColorBlendState->pAttachments->blendEnable == VK_TRUE) {
                if (pCreateInfos->pColorBlendState->pAttachments->srcColorBlendFactor < VK_BLEND_FACTOR_BEGIN_RANGE ||
                    pCreateInfos->pColorBlendState->pAttachments->srcColorBlendFactor > VK_BLEND_FACTOR_END_RANGE) {
                    log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                            "vkCreateGraphicsPipelines parameter, VkBlendFactor "
                            "pCreateInfos->pColorBlendState->pAttachments->srcColorBlendFactor, is an unrecognized enumerator");
                    return false;
                }
                if (pCreateInfos->pColorBlendState->pAttachments->dstColorBlendFactor < VK_BLEND_FACTOR_BEGIN_RANGE ||
                    pCreateInfos->pColorBlendState->pAttachments->dstColorBlendFactor > VK_BLEND_FACTOR_END_RANGE) {
                    log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                            "vkCreateGraphicsPipelines parameter, VkBlendFactor "
                            "pCreateInfos->pColorBlendState->pAttachments->dstColorBlendFactor, is an unrecognized enumerator");
                    return false;
                }
                if (pCreateInfos->pColorBlendState->pAttachments->colorBlendOp < VK_BLEND_OP_BEGIN_RANGE ||
                    pCreateInfos->pColorBlendState->pAttachments->colorBlendOp > VK_BLEND_OP_END_RANGE) {
                    log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                            "vkCreateGraphicsPipelines parameter, VkBlendOp "
                            "pCreateInfos->pColorBlendState->pAttachments->colorBlendOp, is an unrecognized enumerator");
                    return false;
                }
                if (pCreateInfos->pColorBlendState->pAttachments->srcAlphaBlendFactor < VK_BLEND_FACTOR_BEGIN_RANGE ||
                    pCreateInfos->pColorBlendState->pAttachments->srcAlphaBlendFactor > VK_BLEND_FACTOR_END_RANGE) {
                    log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                            "vkCreateGraphicsPipelines parameter, VkBlendFactor "
                            "pCreateInfos->pColorBlendState->pAttachments->srcAlphaBlendFactor, is an unrecognized enumerator");
                    return false;
                }
                if (pCreateInfos->pColorBlendState->pAttachments->dstAlphaBlendFactor < VK_BLEND_FACTOR_BEGIN_RANGE ||
                    pCreateInfos->pColorBlendState->pAttachments->dstAlphaBlendFactor > VK_BLEND_FACTOR_END_RANGE) {
                    log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                            "vkCreateGraphicsPipelines parameter, VkBlendFactor "
                            "pCreateInfos->pColorBlendState->pAttachments->dstAlphaBlendFactor, is an unrecognized enumerator");
                    return false;
                }
                if (pCreateInfos->pColorBlendState->pAttachments->alphaBlendOp < VK_BLEND_OP_BEGIN_RANGE ||
                    pCreateInfos->pColorBlendState->pAttachments->alphaBlendOp > VK_BLEND_OP_END_RANGE) {
                    log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                            "vkCreateGraphicsPipelines parameter, VkBlendOp "
                            "pCreateInfos->pColorBlendState->pAttachments->alphaBlendOp, is an unrecognized enumerator");
                    return false;
                }
            }
        }
        if (pCreateInfos->renderPass == VK_NULL_HANDLE) {
            log_msg(mdd(device), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "vkCreateGraphicsPipelines parameter, VkRenderPass pCreateInfos->renderPass, is null pointer");
        }

        int i = 0;
        for (size_t j = 0; j < pCreateInfos[i].stageCount; j++) {
            validate_string(data->report_data, "vkCreateGraphicsPipelines", "pCreateInfos[i].pStages[j].pName",
                            pCreateInfos[i].pStages[j].pName);
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
                          const VkGraphicsPipelineCreateInfo *pCreateInfos, const VkAllocationCallbacks *pAllocator,
                          VkPipeline *pPipelines) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateGraphicsPipelines(my_data->report_data, pipelineCache, createInfoCount, pCreateInfos,
                                                      pAllocator, pPipelines);

    if (skipCall == VK_FALSE) {
        PreCreateGraphicsPipelines(device, pCreateInfos);

        result = get_dispatch_table(pc_device_table_map, device)
                     ->CreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);

        validate_result(my_data->report_data, "vkCreateGraphicsPipelines", result);
    }

    return result;
}

bool PreCreateComputePipelines(VkDevice device, const VkComputePipelineCreateInfo *pCreateInfos) {
    layer_data *data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);

    if (pCreateInfos != nullptr) {
        // TODO: Handle count!
        int i = 0;
        validate_string(data->report_data, "vkCreateComputePipelines", "pCreateInfos[i].stage.pName", pCreateInfos[i].stage.pName);
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount,
                         const VkComputePipelineCreateInfo *pCreateInfos, const VkAllocationCallbacks *pAllocator,
                         VkPipeline *pPipelines) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateComputePipelines(my_data->report_data, pipelineCache, createInfoCount, pCreateInfos, pAllocator,
                                                     pPipelines);

    if (skipCall == VK_FALSE) {
        PreCreateComputePipelines(device, pCreateInfos);

        result = get_dispatch_table(pc_device_table_map, device)
                     ->CreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);

        validate_result(my_data->report_data, "vkCreateComputePipelines", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyPipeline(my_data->report_data, pipeline, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyPipeline(device, pipeline, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                       VkPipelineLayout *pPipelineLayout) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreatePipelineLayout(my_data->report_data, pCreateInfo, pAllocator, pPipelineLayout);

    if (skipCall == VK_FALSE) {
        result =
            get_dispatch_table(pc_device_table_map, device)->CreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);

        validate_result(my_data->report_data, "vkCreatePipelineLayout", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyPipelineLayout(my_data->report_data, pipelineLayout, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyPipelineLayout(device, pipelineLayout, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSampler(VkDevice device, const VkSamplerCreateInfo *pCreateInfo,
                                                               const VkAllocationCallbacks *pAllocator, VkSampler *pSampler) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateSampler(my_data->report_data, pCreateInfo, pAllocator, pSampler);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->CreateSampler(device, pCreateInfo, pAllocator, pSampler);

        validate_result(my_data->report_data, "vkCreateSampler", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroySampler(my_data->report_data, sampler, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroySampler(device, sampler, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo *pCreateInfo,
                            const VkAllocationCallbacks *pAllocator, VkDescriptorSetLayout *pSetLayout) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateDescriptorSetLayout(my_data->report_data, pCreateInfo, pAllocator, pSetLayout);

    if (skipCall == VK_FALSE) {
        result =
            get_dispatch_table(pc_device_table_map, device)->CreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);

        validate_result(my_data->report_data, "vkCreateDescriptorSetLayout", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyDescriptorSetLayout(my_data->report_data, descriptorSetLayout, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo *pCreateInfo, const VkAllocationCallbacks *pAllocator,
                       VkDescriptorPool *pDescriptorPool) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateDescriptorPool(my_data->report_data, pCreateInfo, pAllocator, pDescriptorPool);

    /* TODOVV: How do we validate maxSets? Probably belongs in the limits layer? */

    if (skipCall == VK_FALSE) {
        result =
            get_dispatch_table(pc_device_table_map, device)->CreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);

        validate_result(my_data->report_data, "vkCreateDescriptorPool", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyDescriptorPool(my_data->report_data, descriptorPool, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyDescriptorPool(device, descriptorPool, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, device)->ResetDescriptorPool(device, descriptorPool, flags);

    validate_result(my_data->report_data, "vkResetDescriptorPool", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo *pAllocateInfo, VkDescriptorSet *pDescriptorSets) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkAllocateDescriptorSets(my_data->report_data, pAllocateInfo, pDescriptorSets);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->AllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);

        validate_result(my_data->report_data, "vkAllocateDescriptorSets", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool,
                                                                    uint32_t descriptorSetCount,
                                                                    const VkDescriptorSet *pDescriptorSets) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkFreeDescriptorSets(my_data->report_data, descriptorPool, descriptorSetCount, pDescriptorSets);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)
                     ->FreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);

        validate_result(my_data->report_data, "vkFreeDescriptorSets", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet *pDescriptorWrites,
                       uint32_t descriptorCopyCount, const VkCopyDescriptorSet *pDescriptorCopies) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkUpdateDescriptorSets(my_data->report_data, descriptorWriteCount, pDescriptorWrites,
                                                   descriptorCopyCount, pDescriptorCopies);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)
            ->UpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo *pCreateInfo,
                                                                   const VkAllocationCallbacks *pAllocator,
                                                                   VkFramebuffer *pFramebuffer) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateFramebuffer(my_data->report_data, pCreateInfo, pAllocator, pFramebuffer);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->CreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);

        validate_result(my_data->report_data, "vkCreateFramebuffer", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyFramebuffer(my_data->report_data, framebuffer, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyFramebuffer(device, framebuffer, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo *pCreateInfo,
                                                                  const VkAllocationCallbacks *pAllocator,
                                                                  VkRenderPass *pRenderPass) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCreateRenderPass(my_data->report_data, pCreateInfo, pAllocator, pRenderPass);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);

        validate_result(my_data->report_data, "vkCreateRenderPass", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyRenderPass(my_data->report_data, renderPass, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyRenderPass(device, renderPass, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D *pGranularity) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkGetRenderAreaGranularity(my_data->report_data, renderPass, pGranularity);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->GetRenderAreaGranularity(device, renderPass, pGranularity);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo *pCreateInfo,
                                                                   const VkAllocationCallbacks *pAllocator,
                                                                   VkCommandPool *pCommandPool) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    bool skipCall = false;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= validate_queue_family_indices(device, "vkCreateCommandPool", 1, &(pCreateInfo->queueFamilyIndex));

    skipCall |= parameter_validation_vkCreateCommandPool(my_data->report_data, pCreateInfo, pAllocator, pCommandPool);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);

        validate_result(my_data->report_data, "vkCreateCommandPool", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks *pAllocator) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkDestroyCommandPool(my_data->report_data, commandPool, pAllocator);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)->DestroyCommandPool(device, commandPool, pAllocator);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, device)->ResetCommandPool(device, commandPool, flags);

    validate_result(my_data->report_data, "vkResetCommandPool", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo *pAllocateInfo, VkCommandBuffer *pCommandBuffers) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkAllocateCommandBuffers(my_data->report_data, pAllocateInfo, pCommandBuffers);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, device)->AllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);

        validate_result(my_data->report_data, "vkAllocateCommandBuffers", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool,
                                                                uint32_t commandBufferCount,
                                                                const VkCommandBuffer *pCommandBuffers) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkFreeCommandBuffers(my_data->report_data, commandPool, commandBufferCount, pCommandBuffers);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, device)
            ->FreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo *pBeginInfo) {
    VkResult result = VK_ERROR_VALIDATION_FAILED_EXT;
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkBeginCommandBuffer(my_data->report_data, pBeginInfo);

    if (skipCall == VK_FALSE) {
        result = get_dispatch_table(pc_device_table_map, commandBuffer)->BeginCommandBuffer(commandBuffer, pBeginInfo);

        validate_result(my_data->report_data, "vkBeginCommandBuffer", result);
    }

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEndCommandBuffer(VkCommandBuffer commandBuffer) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, commandBuffer)->EndCommandBuffer(commandBuffer);

    validate_result(my_data->report_data, "vkEndCommandBuffer", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL
vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) {
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    VkResult result = get_dispatch_table(pc_device_table_map, commandBuffer)->ResetCommandBuffer(commandBuffer, flags);

    validate_result(my_data->report_data, "vkResetCommandBuffer", result);

    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdBindPipeline(my_data->report_data, pipelineBindPoint, pipeline);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)->CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport *pViewports) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdSetViewport(my_data->report_data, firstViewport, viewportCount, pViewports);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D *pScissors) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdSetScissor(my_data->report_data, firstScissor, scissorCount, pScissors);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)->CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdSetLineWidth(commandBuffer, lineWidth);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) {
    get_dispatch_table(pc_device_table_map, commandBuffer)
        ->CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4]) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdSetBlendConstants(my_data->report_data, blendConstants);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)->CmdSetBlendConstants(commandBuffer, blendConstants);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdSetStencilReference(commandBuffer, faceMask, reference);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout,
                        uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets,
                        uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdBindDescriptorSets(my_data->report_data, pipelineBindPoint, layout, firstSet, descriptorSetCount,
                                                    pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets,
                                    dynamicOffsetCount, pDynamicOffsets);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdBindIndexBuffer(my_data->report_data, buffer, offset, indexType);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)->CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding,
                                                                  uint32_t bindingCount, const VkBuffer *pBuffers,
                                                                  const VkDeviceSize *pOffsets) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdBindVertexBuffers(my_data->report_data, firstBinding, bindingCount, pBuffers, pOffsets);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
    }
}

bool PreCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
                uint32_t firstInstance) {
    if (vertexCount == 0) {
        // TODO: Verify against Valid Usage section. I don't see a non-zero vertexCount listed, may need to add that and make
        // this an error or leave as is.
        log_msg(mdd(commandBuffer), VK_DEBUG_REPORT_WARNING_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                "vkCmdDraw parameter, uint32_t vertexCount, is 0");
        return false;
    }

    if (instanceCount == 0) {
        // TODO: Verify against Valid Usage section. I don't see a non-zero instanceCount listed, may need to add that and make
        // this an error or leave as is.
        log_msg(mdd(commandBuffer), VK_DEBUG_REPORT_WARNING_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                "vkCmdDraw parameter, uint32_t instanceCount, is 0");
        return false;
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount,
                                                     uint32_t firstVertex, uint32_t firstInstance) {
    PreCmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);

    get_dispatch_table(pc_device_table_map, commandBuffer)
        ->CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount,
                                                            uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
                                                            uint32_t firstInstance) {
    get_dispatch_table(pc_device_table_map, commandBuffer)
        ->CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t count, uint32_t stride) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdDrawIndirect(commandBuffer, buffer, offset, count, stride);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t count, uint32_t stride) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdDrawIndexedIndirect(commandBuffer, buffer, offset, count, stride);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t x, uint32_t y, uint32_t z) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdDispatch(commandBuffer, x, y, z);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdDispatchIndirect(commandBuffer, buffer, offset);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer,
                                                           uint32_t regionCount, const VkBufferCopy *pRegions) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdCopyBuffer(my_data->report_data, srcBuffer, dstBuffer, regionCount, pRegions);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
    }
}

bool PreCmdCopyImage(VkCommandBuffer commandBuffer, const VkImageCopy *pRegions) {
    if (pRegions != nullptr) {
        if ((pRegions->srcSubresource.aspectMask & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT |
                                                    VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(mdd(commandBuffer), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "vkCmdCopyImage parameter, VkImageAspect pRegions->srcSubresource.aspectMask, is an unrecognized enumerator");
            return false;
        }
        if ((pRegions->dstSubresource.aspectMask & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT |
                                                    VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(mdd(commandBuffer), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "vkCmdCopyImage parameter, VkImageAspect pRegions->dstSubresource.aspectMask, is an unrecognized enumerator");
            return false;
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage,
               VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy *pRegions) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |=
        parameter_validation_vkCmdCopyImage(my_data->report_data, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);

    if (skipCall == VK_FALSE) {
        PreCmdCopyImage(commandBuffer, pRegions);

        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    }
}

bool PreCmdBlitImage(VkCommandBuffer commandBuffer, const VkImageBlit *pRegions) {
    if (pRegions != nullptr) {
        if ((pRegions->srcSubresource.aspectMask & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT |
                                                    VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(mdd(commandBuffer), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "vkCmdCopyImage parameter, VkImageAspect pRegions->srcSubresource.aspectMask, is an unrecognized enumerator");
            return false;
        }
        if ((pRegions->dstSubresource.aspectMask & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT |
                                                    VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(mdd(commandBuffer), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "vkCmdCopyImage parameter, VkImageAspect pRegions->dstSubresource.aspectMask, is an unrecognized enumerator");
            return false;
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage,
               VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit *pRegions, VkFilter filter) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdBlitImage(my_data->report_data, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount,
                                           pRegions, filter);

    if (skipCall == VK_FALSE) {
        PreCmdBlitImage(commandBuffer, pRegions);

        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
    }
}

bool PreCmdCopyBufferToImage(VkCommandBuffer commandBuffer, const VkBufferImageCopy *pRegions) {
    if (pRegions != nullptr) {
        if ((pRegions->imageSubresource.aspectMask & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT |
                                                      VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(mdd(commandBuffer), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "vkCmdCopyBufferToImage parameter, VkImageAspect pRegions->imageSubresource.aspectMask, is an unrecognized "
                    "enumerator");
            return false;
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer,
                                                                  VkImage dstImage, VkImageLayout dstImageLayout,
                                                                  uint32_t regionCount, const VkBufferImageCopy *pRegions) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |=
        parameter_validation_vkCmdCopyBufferToImage(my_data->report_data, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);

    if (skipCall == VK_FALSE) {
        PreCmdCopyBufferToImage(commandBuffer, pRegions);

        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
    }
}

bool PreCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, const VkBufferImageCopy *pRegions) {
    if (pRegions != nullptr) {
        if ((pRegions->imageSubresource.aspectMask & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT |
                                                      VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(mdd(commandBuffer), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                    "vkCmdCopyImageToBuffer parameter, VkImageAspect pRegions->imageSubresource.aspectMask, is an unrecognized "
                    "enumerator");
            return false;
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage,
                                                                  VkImageLayout srcImageLayout, VkBuffer dstBuffer,
                                                                  uint32_t regionCount, const VkBufferImageCopy *pRegions) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |=
        parameter_validation_vkCmdCopyImageToBuffer(my_data->report_data, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);

    if (skipCall == VK_FALSE) {
        PreCmdCopyImageToBuffer(commandBuffer, pRegions);

        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer,
                                                             VkDeviceSize dstOffset, VkDeviceSize dataSize, const uint32_t *pData) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdUpdateBuffer(my_data->report_data, dstBuffer, dstOffset, dataSize, pData);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image,
                                                                VkImageLayout imageLayout, const VkClearColorValue *pColor,
                                                                uint32_t rangeCount, const VkImageSubresourceRange *pRanges) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdClearColorImage(my_data->report_data, image, imageLayout, pColor, rangeCount, pRanges);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout,
                            const VkClearDepthStencilValue *pDepthStencil, uint32_t rangeCount,
                            const VkImageSubresourceRange *pRanges) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |=
        parameter_validation_vkCmdClearDepthStencilImage(my_data->report_data, image, imageLayout, pDepthStencil, rangeCount, pRanges);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount,
                                                                 const VkClearAttachment *pAttachments, uint32_t rectCount,
                                                                 const VkClearRect *pRects) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdClearAttachments(my_data->report_data, attachmentCount, pAttachments, rectCount, pRects);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
    }
}

bool PreCmdResolveImage(VkCommandBuffer commandBuffer, const VkImageResolve *pRegions) {
    if (pRegions != nullptr) {
        if ((pRegions->srcSubresource.aspectMask & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT |
                                                    VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(
                mdd(commandBuffer), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                "vkCmdResolveImage parameter, VkImageAspect pRegions->srcSubresource.aspectMask, is an unrecognized enumerator");
            return false;
        }
        if ((pRegions->dstSubresource.aspectMask & (VK_IMAGE_ASPECT_COLOR_BIT | VK_IMAGE_ASPECT_DEPTH_BIT |
                                                    VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_METADATA_BIT)) == 0) {
            log_msg(
                mdd(commandBuffer), VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1, "PARAMCHECK",
                "vkCmdResolveImage parameter, VkImageAspect pRegions->dstSubresource.aspectMask, is an unrecognized enumerator");
            return false;
        }
    }

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage,
                  VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve *pRegions) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdResolveImage(my_data->report_data, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount,
                                              pRegions);

    if (skipCall == VK_FALSE) {
        PreCmdResolveImage(commandBuffer, pRegions);

        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdSetEvent(commandBuffer, event, stageMask);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdResetEvent(commandBuffer, event, stageMask);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent *pEvents, VkPipelineStageFlags srcStageMask,
                VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers,
                uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers,
                uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdWaitEvents(my_data->report_data, eventCount, pEvents, srcStageMask, dstStageMask,
                                            memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers,
                                            imageMemoryBarrierCount, pImageMemoryBarriers);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers,
                            bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
                     VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier *pMemoryBarriers,
                     uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier *pBufferMemoryBarriers,
                     uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier *pImageMemoryBarriers) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdPipelineBarrier(my_data->report_data, srcStageMask, dstStageMask, dependencyFlags,
                                                 memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount,
                                                 pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers,
                                 bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t slot, VkQueryControlFlags flags) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdBeginQuery(commandBuffer, queryPool, slot, flags);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t slot) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdEndQuery(commandBuffer, queryPool, slot);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
}

bool PostCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool,
                           uint32_t slot) {

    ValidateEnumerator(pipelineStage);

    return true;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t slot) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, slot);

    PostCmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, slot);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount,
                          VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags) {
    get_dispatch_table(pc_device_table_map, commandBuffer)
        ->CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout,
                                                              VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size,
                                                              const void *pValues) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdPushConstants(my_data->report_data, layout, stageFlags, offset, size, pValues);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo *pRenderPassBegin, VkSubpassContents contents) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdBeginRenderPass(my_data->report_data, pRenderPassBegin, contents);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)->CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdNextSubpass(my_data->report_data, contents);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)->CmdNextSubpass(commandBuffer, contents);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass(VkCommandBuffer commandBuffer) {
    get_dispatch_table(pc_device_table_map, commandBuffer)->CmdEndRenderPass(commandBuffer);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL
vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer *pCommandBuffers) {
    VkBool32 skipCall = VK_FALSE;
    layer_data *my_data = get_my_data_ptr(get_dispatch_key(commandBuffer), layer_data_map);
    assert(my_data != NULL);

    skipCall |= parameter_validation_vkCmdExecuteCommands(my_data->report_data, commandBufferCount, pCommandBuffers);

    if (skipCall == VK_FALSE) {
        get_dispatch_table(pc_device_table_map, commandBuffer)
            ->CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
    }
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char *funcName) {
    layer_data *data = get_my_data_ptr(get_dispatch_key(device), layer_data_map);

    if (validate_string(data->report_data, "vkGetDeviceProcAddr", "funcName", funcName) == VK_TRUE) {
        return NULL;
    }

    if (!strcmp(funcName, "vkGetDeviceProcAddr"))
        return (PFN_vkVoidFunction)vkGetDeviceProcAddr;
    if (!strcmp(funcName, "vkDestroyDevice"))
        return (PFN_vkVoidFunction)vkDestroyDevice;
    if (!strcmp(funcName, "vkGetDeviceQueue"))
        return (PFN_vkVoidFunction)vkGetDeviceQueue;
    if (!strcmp(funcName, "vkQueueSubmit"))
        return (PFN_vkVoidFunction)vkQueueSubmit;
    if (!strcmp(funcName, "vkQueueWaitIdle"))
        return (PFN_vkVoidFunction)vkQueueWaitIdle;
    if (!strcmp(funcName, "vkDeviceWaitIdle"))
        return (PFN_vkVoidFunction)vkDeviceWaitIdle;
    if (!strcmp(funcName, "vkAllocateMemory"))
        return (PFN_vkVoidFunction)vkAllocateMemory;
    if (!strcmp(funcName, "vkFreeMemory"))
        return (PFN_vkVoidFunction)vkFreeMemory;
    if (!strcmp(funcName, "vkMapMemory"))
        return (PFN_vkVoidFunction)vkMapMemory;
    if (!strcmp(funcName, "vkFlushMappedMemoryRanges"))
        return (PFN_vkVoidFunction)vkFlushMappedMemoryRanges;
    if (!strcmp(funcName, "vkInvalidateMappedMemoryRanges"))
        return (PFN_vkVoidFunction)vkInvalidateMappedMemoryRanges;
    if (!strcmp(funcName, "vkCreateFence"))
        return (PFN_vkVoidFunction)vkCreateFence;
    if (!strcmp(funcName, "vkDestroyFence"))
        return (PFN_vkVoidFunction)vkDestroyFence;
    if (!strcmp(funcName, "vkResetFences"))
        return (PFN_vkVoidFunction)vkResetFences;
    if (!strcmp(funcName, "vkGetFenceStatus"))
        return (PFN_vkVoidFunction)vkGetFenceStatus;
    if (!strcmp(funcName, "vkWaitForFences"))
        return (PFN_vkVoidFunction)vkWaitForFences;
    if (!strcmp(funcName, "vkCreateSemaphore"))
        return (PFN_vkVoidFunction)vkCreateSemaphore;
    if (!strcmp(funcName, "vkDestroySemaphore"))
        return (PFN_vkVoidFunction)vkDestroySemaphore;
    if (!strcmp(funcName, "vkCreateEvent"))
        return (PFN_vkVoidFunction)vkCreateEvent;
    if (!strcmp(funcName, "vkDestroyEvent"))
        return (PFN_vkVoidFunction)vkDestroyEvent;
    if (!strcmp(funcName, "vkGetEventStatus"))
        return (PFN_vkVoidFunction)vkGetEventStatus;
    if (!strcmp(funcName, "vkSetEvent"))
        return (PFN_vkVoidFunction)vkSetEvent;
    if (!strcmp(funcName, "vkResetEvent"))
        return (PFN_vkVoidFunction)vkResetEvent;
    if (!strcmp(funcName, "vkCreateQueryPool"))
        return (PFN_vkVoidFunction)vkCreateQueryPool;
    if (!strcmp(funcName, "vkDestroyQueryPool"))
        return (PFN_vkVoidFunction)vkDestroyQueryPool;
    if (!strcmp(funcName, "vkGetQueryPoolResults"))
        return (PFN_vkVoidFunction)vkGetQueryPoolResults;
    if (!strcmp(funcName, "vkCreateBuffer"))
        return (PFN_vkVoidFunction)vkCreateBuffer;
    if (!strcmp(funcName, "vkDestroyBuffer"))
        return (PFN_vkVoidFunction)vkDestroyBuffer;
    if (!strcmp(funcName, "vkCreateBufferView"))
        return (PFN_vkVoidFunction)vkCreateBufferView;
    if (!strcmp(funcName, "vkDestroyBufferView"))
        return (PFN_vkVoidFunction)vkDestroyBufferView;
    if (!strcmp(funcName, "vkCreateImage"))
        return (PFN_vkVoidFunction)vkCreateImage;
    if (!strcmp(funcName, "vkDestroyImage"))
        return (PFN_vkVoidFunction)vkDestroyImage;
    if (!strcmp(funcName, "vkGetImageSubresourceLayout"))
        return (PFN_vkVoidFunction)vkGetImageSubresourceLayout;
    if (!strcmp(funcName, "vkCreateImageView"))
        return (PFN_vkVoidFunction)vkCreateImageView;
    if (!strcmp(funcName, "vkDestroyImageView"))
        return (PFN_vkVoidFunction)vkDestroyImageView;
    if (!strcmp(funcName, "vkCreateShaderModule"))
        return (PFN_vkVoidFunction)vkCreateShaderModule;
    if (!strcmp(funcName, "vkDestroyShaderModule"))
        return (PFN_vkVoidFunction)vkDestroyShaderModule;
    if (!strcmp(funcName, "vkCreatePipelineCache"))
        return (PFN_vkVoidFunction)vkCreatePipelineCache;
    if (!strcmp(funcName, "vkDestroyPipelineCache"))
        return (PFN_vkVoidFunction)vkDestroyPipelineCache;
    if (!strcmp(funcName, "vkGetPipelineCacheData"))
        return (PFN_vkVoidFunction)vkGetPipelineCacheData;
    if (!strcmp(funcName, "vkMergePipelineCaches"))
        return (PFN_vkVoidFunction)vkMergePipelineCaches;
    if (!strcmp(funcName, "vkCreateGraphicsPipelines"))
        return (PFN_vkVoidFunction)vkCreateGraphicsPipelines;
    if (!strcmp(funcName, "vkCreateComputePipelines"))
        return (PFN_vkVoidFunction)vkCreateComputePipelines;
    if (!strcmp(funcName, "vkDestroyPipeline"))
        return (PFN_vkVoidFunction)vkDestroyPipeline;
    if (!strcmp(funcName, "vkCreatePipelineLayout"))
        return (PFN_vkVoidFunction)vkCreatePipelineLayout;
    if (!strcmp(funcName, "vkDestroyPipelineLayout"))
        return (PFN_vkVoidFunction)vkDestroyPipelineLayout;
    if (!strcmp(funcName, "vkCreateSampler"))
        return (PFN_vkVoidFunction)vkCreateSampler;
    if (!strcmp(funcName, "vkDestroySampler"))
        return (PFN_vkVoidFunction)vkDestroySampler;
    if (!strcmp(funcName, "vkCreateDescriptorSetLayout"))
        return (PFN_vkVoidFunction)vkCreateDescriptorSetLayout;
    if (!strcmp(funcName, "vkDestroyDescriptorSetLayout"))
        return (PFN_vkVoidFunction)vkDestroyDescriptorSetLayout;
    if (!strcmp(funcName, "vkCreateDescriptorPool"))
        return (PFN_vkVoidFunction)vkCreateDescriptorPool;
    if (!strcmp(funcName, "vkDestroyDescriptorPool"))
        return (PFN_vkVoidFunction)vkDestroyDescriptorPool;
    if (!strcmp(funcName, "vkResetDescriptorPool"))
        return (PFN_vkVoidFunction)vkResetDescriptorPool;
    if (!strcmp(funcName, "vkAllocateDescriptorSets"))
        return (PFN_vkVoidFunction)vkAllocateDescriptorSets;
    if (!strcmp(funcName, "vkCmdSetViewport"))
        return (PFN_vkVoidFunction)vkCmdSetViewport;
    if (!strcmp(funcName, "vkCmdSetScissor"))
        return (PFN_vkVoidFunction)vkCmdSetScissor;
    if (!strcmp(funcName, "vkCmdSetLineWidth"))
        return (PFN_vkVoidFunction)vkCmdSetLineWidth;
    if (!strcmp(funcName, "vkCmdSetDepthBias"))
        return (PFN_vkVoidFunction)vkCmdSetDepthBias;
    if (!strcmp(funcName, "vkCmdSetBlendConstants"))
        return (PFN_vkVoidFunction)vkCmdSetBlendConstants;
    if (!strcmp(funcName, "vkCmdSetDepthBounds"))
        return (PFN_vkVoidFunction)vkCmdSetDepthBounds;
    if (!strcmp(funcName, "vkCmdSetStencilCompareMask"))
        return (PFN_vkVoidFunction)vkCmdSetStencilCompareMask;
    if (!strcmp(funcName, "vkCmdSetStencilWriteMask"))
        return (PFN_vkVoidFunction)vkCmdSetStencilWriteMask;
    if (!strcmp(funcName, "vkCmdSetStencilReference"))
        return (PFN_vkVoidFunction)vkCmdSetStencilReference;
    if (!strcmp(funcName, "vkAllocateCommandBuffers"))
        return (PFN_vkVoidFunction)vkAllocateCommandBuffers;
    if (!strcmp(funcName, "vkFreeCommandBuffers"))
        return (PFN_vkVoidFunction)vkFreeCommandBuffers;
    if (!strcmp(funcName, "vkBeginCommandBuffer"))
        return (PFN_vkVoidFunction)vkBeginCommandBuffer;
    if (!strcmp(funcName, "vkEndCommandBuffer"))
        return (PFN_vkVoidFunction)vkEndCommandBuffer;
    if (!strcmp(funcName, "vkResetCommandBuffer"))
        return (PFN_vkVoidFunction)vkResetCommandBuffer;
    if (!strcmp(funcName, "vkCmdBindPipeline"))
        return (PFN_vkVoidFunction)vkCmdBindPipeline;
    if (!strcmp(funcName, "vkCmdBindDescriptorSets"))
        return (PFN_vkVoidFunction)vkCmdBindDescriptorSets;
    if (!strcmp(funcName, "vkCmdBindVertexBuffers"))
        return (PFN_vkVoidFunction)vkCmdBindVertexBuffers;
    if (!strcmp(funcName, "vkCmdBindIndexBuffer"))
        return (PFN_vkVoidFunction)vkCmdBindIndexBuffer;
    if (!strcmp(funcName, "vkCmdDraw"))
        return (PFN_vkVoidFunction)vkCmdDraw;
    if (!strcmp(funcName, "vkCmdDrawIndexed"))
        return (PFN_vkVoidFunction)vkCmdDrawIndexed;
    if (!strcmp(funcName, "vkCmdDrawIndirect"))
        return (PFN_vkVoidFunction)vkCmdDrawIndirect;
    if (!strcmp(funcName, "vkCmdDrawIndexedIndirect"))
        return (PFN_vkVoidFunction)vkCmdDrawIndexedIndirect;
    if (!strcmp(funcName, "vkCmdDispatch"))
        return (PFN_vkVoidFunction)vkCmdDispatch;
    if (!strcmp(funcName, "vkCmdDispatchIndirect"))
        return (PFN_vkVoidFunction)vkCmdDispatchIndirect;
    if (!strcmp(funcName, "vkCmdCopyBuffer"))
        return (PFN_vkVoidFunction)vkCmdCopyBuffer;
    if (!strcmp(funcName, "vkCmdCopyImage"))
        return (PFN_vkVoidFunction)vkCmdCopyImage;
    if (!strcmp(funcName, "vkCmdBlitImage"))
        return (PFN_vkVoidFunction)vkCmdBlitImage;
    if (!strcmp(funcName, "vkCmdCopyBufferToImage"))
        return (PFN_vkVoidFunction)vkCmdCopyBufferToImage;
    if (!strcmp(funcName, "vkCmdCopyImageToBuffer"))
        return (PFN_vkVoidFunction)vkCmdCopyImageToBuffer;
    if (!strcmp(funcName, "vkCmdUpdateBuffer"))
        return (PFN_vkVoidFunction)vkCmdUpdateBuffer;
    if (!strcmp(funcName, "vkCmdFillBuffer"))
        return (PFN_vkVoidFunction)vkCmdFillBuffer;
    if (!strcmp(funcName, "vkCmdClearColorImage"))
        return (PFN_vkVoidFunction)vkCmdClearColorImage;
    if (!strcmp(funcName, "vkCmdResolveImage"))
        return (PFN_vkVoidFunction)vkCmdResolveImage;
    if (!strcmp(funcName, "vkCmdSetEvent"))
        return (PFN_vkVoidFunction)vkCmdSetEvent;
    if (!strcmp(funcName, "vkCmdResetEvent"))
        return (PFN_vkVoidFunction)vkCmdResetEvent;
    if (!strcmp(funcName, "vkCmdWaitEvents"))
        return (PFN_vkVoidFunction)vkCmdWaitEvents;
    if (!strcmp(funcName, "vkCmdPipelineBarrier"))
        return (PFN_vkVoidFunction)vkCmdPipelineBarrier;
    if (!strcmp(funcName, "vkCmdBeginQuery"))
        return (PFN_vkVoidFunction)vkCmdBeginQuery;
    if (!strcmp(funcName, "vkCmdEndQuery"))
        return (PFN_vkVoidFunction)vkCmdEndQuery;
    if (!strcmp(funcName, "vkCmdResetQueryPool"))
        return (PFN_vkVoidFunction)vkCmdResetQueryPool;
    if (!strcmp(funcName, "vkCmdWriteTimestamp"))
        return (PFN_vkVoidFunction)vkCmdWriteTimestamp;
    if (!strcmp(funcName, "vkCmdCopyQueryPoolResults"))
        return (PFN_vkVoidFunction)vkCmdCopyQueryPoolResults;
    if (!strcmp(funcName, "vkCreateFramebuffer"))
        return (PFN_vkVoidFunction)vkCreateFramebuffer;
    if (!strcmp(funcName, "vkDestroyFramebuffer"))
        return (PFN_vkVoidFunction)vkDestroyFramebuffer;
    if (!strcmp(funcName, "vkCreateRenderPass"))
        return (PFN_vkVoidFunction)vkCreateRenderPass;
    if (!strcmp(funcName, "vkDestroyRenderPass"))
        return (PFN_vkVoidFunction)vkDestroyRenderPass;
    if (!strcmp(funcName, "vkGetRenderAreaGranularity"))
        return (PFN_vkVoidFunction)vkGetRenderAreaGranularity;
    if (!strcmp(funcName, "vkCreateCommandPool"))
        return (PFN_vkVoidFunction)vkCreateCommandPool;
    if (!strcmp(funcName, "vkDestroyCommandPool"))
        return (PFN_vkVoidFunction)vkDestroyCommandPool;
    if (!strcmp(funcName, "vkCmdBeginRenderPass"))
        return (PFN_vkVoidFunction)vkCmdBeginRenderPass;
    if (!strcmp(funcName, "vkCmdNextSubpass"))
        return (PFN_vkVoidFunction)vkCmdNextSubpass;

    if (device == NULL) {
        return NULL;
    }

    if (get_dispatch_table(pc_device_table_map, device)->GetDeviceProcAddr == NULL)
        return NULL;
    return get_dispatch_table(pc_device_table_map, device)->GetDeviceProcAddr(device, funcName);
}

VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char *funcName) {
    if (!strcmp(funcName, "vkGetInstanceProcAddr"))
        return (PFN_vkVoidFunction)vkGetInstanceProcAddr;
    if (!strcmp(funcName, "vkCreateInstance"))
        return (PFN_vkVoidFunction)vkCreateInstance;
    if (!strcmp(funcName, "vkDestroyInstance"))
        return (PFN_vkVoidFunction)vkDestroyInstance;
    if (!strcmp(funcName, "vkCreateDevice"))
        return (PFN_vkVoidFunction)vkCreateDevice;
    if (!strcmp(funcName, "vkEnumeratePhysicalDevices"))
        return (PFN_vkVoidFunction)vkEnumeratePhysicalDevices;
    if (!strcmp(funcName, "vkGetPhysicalDeviceProperties"))
        return (PFN_vkVoidFunction)vkGetPhysicalDeviceProperties;
    if (!strcmp(funcName, "vkGetPhysicalDeviceFeatures"))
        return (PFN_vkVoidFunction)vkGetPhysicalDeviceFeatures;
    if (!strcmp(funcName, "vkGetPhysicalDeviceFormatProperties"))
        return (PFN_vkVoidFunction)vkGetPhysicalDeviceFormatProperties;
    if (!strcmp(funcName, "vkEnumerateInstanceLayerProperties"))
        return (PFN_vkVoidFunction)vkEnumerateInstanceLayerProperties;
    if (!strcmp(funcName, "vkEnumerateInstanceExtensionProperties"))
        return (PFN_vkVoidFunction)vkEnumerateInstanceExtensionProperties;
    if (!strcmp(funcName, "vkEnumerateDeviceLayerProperties"))
        return (PFN_vkVoidFunction)vkEnumerateDeviceLayerProperties;
    if (!strcmp(funcName, "vkEnumerateDeviceExtensionProperties"))
        return (PFN_vkVoidFunction)vkEnumerateDeviceExtensionProperties;

    if (instance == NULL) {
        return NULL;
    }

    layer_data *data = get_my_data_ptr(get_dispatch_key(instance), layer_data_map);

    PFN_vkVoidFunction fptr = debug_report_get_instance_proc_addr(data->report_data, funcName);
    if (fptr)
        return fptr;

    if (get_dispatch_table(pc_instance_table_map, instance)->GetInstanceProcAddr == NULL)
        return NULL;
    return get_dispatch_table(pc_instance_table_map, instance)->GetInstanceProcAddr(instance, funcName);
}
