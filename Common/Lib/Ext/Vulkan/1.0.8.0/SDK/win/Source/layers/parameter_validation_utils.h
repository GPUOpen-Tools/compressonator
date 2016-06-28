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
 * The Materials are Confidential Information as defined by the Khronos
 * Membership Agreement until designated non-confidential by Khronos, at which
 * point this condition clause shall be removed.
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
 * Author: Dustin Graves <dustin@lunarg.com>
 */

#ifndef PARAMETER_VALIDATION_UTILS_H
#define PARAMETER_VALIDATION_UTILS_H

#include <algorithm>
#include <cstdlib>
#include <string>

#include "vulkan/vulkan.h"
#include "vk_enum_string_helper.h"
#include "vk_layer_logging.h"

namespace {
struct GenericHeader {
    VkStructureType sType;
    const void *pNext;
};
}

// Layer name string to be logged with validation messages.
const char ParameterValidationName[] = "ParameterValidation";

// String returned by string_VkStructureType for an unrecognized type.
const std::string UnsupportedStructureTypeString = "Unhandled VkStructureType";

// String returned by string_VkResult for an unrecognized type.
const std::string UnsupportedResultString = "Unhandled VkResult";

// The base value used when computing the offset for an enumeration token value that is added by an extension.
// When validating enumeration tokens, any value >= to this value is considered to be provided by an extension.
// See Appendix C.10 "Assigning Extension Token Values" from the Vulkan specification
const uint32_t ExtEnumBaseValue = 1000000000;

template <typename T> bool is_extension_added_token(T value) {
    return (std::abs(static_cast<int32_t>(value)) >= ExtEnumBaseValue);
}

// VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE token is a special case that was converted from a core token to an
// extension added token.  Its original value was intentionally preserved after the conversion, so it does not use
// the base value that other extension added tokens use, and it does not fall within the enum's begin/end range.
template <> bool is_extension_added_token(VkSamplerAddressMode value) {
    bool result = (std::abs(static_cast<int32_t>(value)) >= ExtEnumBaseValue);
    return (result || (value == VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE));
}

/**
 * Validate a required pointer.
 *
 * Verify that a required pointer is not NULL.
 *
 * @param report_data debug_report_data object for routing validation messages.
 * @param apiName Name of API call being validated.
 * @param parameterName Name of parameter being validated.
 * @param value Pointer to validate.
 * @return Boolean value indicating that the call should be skipped.
 */
static VkBool32 validate_required_pointer(debug_report_data *report_data, const char *apiName, const char *parameterName,
                                          const void *value) {
    VkBool32 skipCall = VK_FALSE;

    if (value == NULL) {
        skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                            ParameterValidationName, "%s: required parameter %s specified as NULL", apiName, parameterName);
    }

    return skipCall;
}

/**
 * Validate pointer to array count and pointer to array.
 *
 * Verify that required count and array parameters are not NULL.  If count
 * is not NULL and its value is not optional, verify that it is not 0.  If the
 * array parameter is NULL, and it is not optional, verify that count is 0.
 * The array parameter will typically be optional for this case (where count is
 * a pointer), allowing the caller to retrieve the available count.
 *
 * @param report_data debug_report_data object for routing validation messages.
 * @param apiName Name of API call being validated.
 * @param countName Name of count parameter.
 * @param arrayName Name of array parameter.
 * @param count Pointer to the number of elements in the array.
 * @param array Array to validate.
 * @param countPtrRequired The 'count' parameter may not be NULL when true.
 * @param countValueRequired The '*count' value may not be 0 when true.
 * @param arrayRequired The 'array' parameter may not be NULL when true.
 * @return Boolean value indicating that the call should be skipped.
 */
template <typename T>
VkBool32 validate_array(debug_report_data *report_data, const char *apiName, const char *countName, const char *arrayName,
                        const T *count, const void *array, VkBool32 countPtrRequired, VkBool32 countValueRequired,
                        VkBool32 arrayRequired) {
    VkBool32 skipCall = VK_FALSE;

    if (count == NULL) {
        if (countPtrRequired == VK_TRUE) {
            skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                                ParameterValidationName, "%s: required parameter %s specified as NULL", apiName, countName);
        }
    } else {
        skipCall |= validate_array(report_data, apiName, countName, arrayName, (*count), array, countValueRequired, arrayRequired);
    }

    return skipCall;
}

/**
 * Validate array count and pointer to array.
 *
 * Verify that required count and array parameters are not 0 or NULL.  If the
 * count parameter is not optional, verify that it is not 0.  If the array
 * parameter is NULL, and it is not optional, verify that count is 0.
 *
 * @param report_data debug_report_data object for routing validation messages.
 * @param apiName Name of API call being validated.
 * @param countName Name of count parameter.
 * @param arrayName Name of array parameter.
 * @param count Number of elements in the array.
 * @param array Array to validate.
 * @param countRequired The 'count' parameter may not be 0 when true.
 * @param arrayRequired The 'array' parameter may not be NULL when true.
 * @return Boolean value indicating that the call should be skipped.
 */
template <typename T>
VkBool32 validate_array(debug_report_data *report_data, const char *apiName, const char *countName, const char *arrayName, T count,
                        const void *array, VkBool32 countRequired, VkBool32 arrayRequired) {
    VkBool32 skipCall = VK_FALSE;

    // Count parameters not tagged as optional cannot be 0
    if ((count == 0) && (countRequired == VK_TRUE)) {
        skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                            ParameterValidationName, "%s: value of %s must be greater than 0", apiName, countName);
    }

    // Array parameters not tagged as optional cannot be NULL,
    // unless the count is 0
    if ((array == NULL) && (arrayRequired == VK_TRUE) && (count != 0)) {
        skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                            ParameterValidationName, "%s: required parameter %s specified as NULL", apiName, arrayName);
    }

    return skipCall;
}

/**
 * Validate an Vulkan structure type.
 *
 * @param report_data debug_report_data object for routing validation messages.
 * @param apiName Name of API call being validated.
 * @param parameterName Name of struct parameter being validated.
 * @param sTypeName Name of expected VkStructureType value.
 * @param value Pointer to the struct to validate.
 * @param sType VkStructureType for structure validation.
 * @param required The parameter may not be NULL when true.
 * @return Boolean value indicating that the call should be skipped.
 */
template <typename T>
VkBool32 validate_struct_type(debug_report_data *report_data, const char *apiName, const char *parameterName, const char *sTypeName,
                              const T *value, VkStructureType sType, VkBool32 required) {
    VkBool32 skipCall = VK_FALSE;

    if (value == NULL) {
        if (required == VK_TRUE) {
            skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                                ParameterValidationName, "%s: required parameter %s specified as NULL", apiName, parameterName);
        }
    } else if (value->sType != sType) {
        skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                            ParameterValidationName, "%s: parameter %s->sType must be %s", apiName, parameterName, sTypeName);
    }

    return skipCall;
}

/**
 * Validate an array of Vulkan structures.
 *
 * Verify that required count and array parameters are not NULL.  If count
 * is not NULL and its value is not optional, verify that it is not 0.
 * If the array contains 1 or more structures, verify that each structure's
 * sType field is set to the correct VkStructureType value.
 *
 * @param report_data debug_report_data object for routing validation messages.
 * @param apiName Name of API call being validated.
 * @param countName Name of count parameter.
 * @param arrayName Name of array parameter.
 * @param sTypeName Name of expected VkStructureType value.
 * @param count Pointer to the number of elements in the array.
 * @param array Array to validate.
 * @param sType VkStructureType for structure validation.
 * @param countPtrRequired The 'count' parameter may not be NULL when true.
 * @param countValueRequired The '*count' value may not be 0 when true.
 * @param arrayRequired The 'array' parameter may not be NULL when true.
 * @return Boolean value indicating that the call should be skipped.
 */
template <typename T>
VkBool32 validate_struct_type_array(debug_report_data *report_data, const char *apiName, const char *countName,
                                    const char *arrayName, const char *sTypeName, const uint32_t *count, const T *array,
                                    VkStructureType sType, VkBool32 countPtrRequired, VkBool32 countValueRequired,
                                    VkBool32 arrayRequired) {
    VkBool32 skipCall = VK_FALSE;

    if (count == NULL) {
        if (countPtrRequired == VK_TRUE) {
            skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                                ParameterValidationName, "%s: required parameter %s specified as NULL", apiName, countName);
        }
    } else {
        skipCall |= validate_struct_type_array(report_data, apiName, countName, arrayName, sTypeName, (*count), array, sType,
                                               countValueRequired, arrayRequired);
    }

    return skipCall;
}

/**
 * Validate an array of Vulkan structures
 *
 * Verify that required count and array parameters are not 0 or NULL.  If
 * the array contains 1 or more structures, verify that each structure's
 * sType field is set to the correct VkStructureType value.
 *
 * @param report_data debug_report_data object for routing validation messages.
 * @param apiName Name of API call being validated.
 * @param countName Name of count parameter.
 * @param arrayName Name of array parameter.
 * @param sTypeName Name of expected VkStructureType value.
 * @param count Number of elements in the array.
 * @param array Array to validate.
 * @param sType VkStructureType for structure validation.
 * @param countRequired The 'count' parameter may not be 0 when true.
 * @param arrayRequired The 'array' parameter may not be NULL when true.
 * @return Boolean value indicating that the call should be skipped.
 */
template <typename T>
VkBool32 validate_struct_type_array(debug_report_data *report_data, const char *apiName, const char *countName,
                                    const char *arrayName, const char *sTypeName, uint32_t count, const T *array,
                                    VkStructureType sType, VkBool32 countRequired, VkBool32 arrayRequired) {
    VkBool32 skipCall = VK_FALSE;

    if ((count == 0) || (array == NULL)) {
        // Count parameters not tagged as optional cannot be 0
        if ((count == 0) && (countRequired == VK_TRUE)) {
            skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                                ParameterValidationName, "%s: parameter %s must be greater than 0", apiName, countName);
        }

        // Array parameters not tagged as optional cannot be NULL,
        // unless the count is 0
        if ((array == NULL) && (arrayRequired == VK_TRUE) && (count != 0)) {
            skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                                ParameterValidationName, "%s: required parameter %s specified as NULL", apiName, arrayName);
        }
    } else {
        // Verify that all structs in the array have the correct type
        for (uint32_t i = 0; i < count; ++i) {
            if (array[i].sType != sType) {
                skipCall |=
                    log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                            ParameterValidationName, "%s: parameter %s[%d].sType must be %s", apiName, arrayName, i, sTypeName);
            }
        }
    }

    return skipCall;
}

/**
 * Validate string array count and content.
 *
 * Verify that required count and array parameters are not 0 or NULL.  If the
 * count parameter is not optional, verify that it is not 0.  If the array
 * parameter is NULL, and it is not optional, verify that count is 0.  If the
 * array parameter is not NULL, verify that none of the strings are NULL.
 *
 * @param report_data debug_report_data object for routing validation messages.
 * @param apiName Name of API call being validated.
 * @param countName Name of count parameter.
 * @param arrayName Name of array parameter.
 * @param count Number of strings in the array.
 * @param array Array of strings to validate.
 * @param countRequired The 'count' parameter may not be 0 when true.
 * @param arrayRequired The 'array' parameter may not be NULL when true.
 * @return Boolean value indicating that the call should be skipped.
 */
static VkBool32 validate_string_array(debug_report_data *report_data, const char *apiName, const char *countName,
                                      const char *arrayName, uint32_t count, const char *const *array, VkBool32 countRequired,
                                      VkBool32 arrayRequired) {
    VkBool32 skipCall = VK_FALSE;

    if ((count == 0) || (array == NULL)) {
        // Count parameters not tagged as optional cannot be 0
        if ((count == 0) && (countRequired == VK_TRUE)) {
            skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                                ParameterValidationName, "%s: parameter %s must be greater than 0", apiName, countName);
        }

        // Array parameters not tagged as optional cannot be NULL,
        // unless the count is 0
        if ((array == NULL) && (arrayRequired == VK_TRUE) && (count != 0)) {
            skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                                ParameterValidationName, "%s: required parameter %s specified as NULL", apiName, arrayName);
        }
    } else {
        // Verify that strings in the array not NULL
        for (uint32_t i = 0; i < count; ++i) {
            if (array[i] == NULL) {
                skipCall |=
                    log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                            ParameterValidationName, "%s: required parameter %s[%d] specified as NULL", apiName, arrayName, i);
            }
        }
    }

    return skipCall;
}

/**
 * Validate a structure's pNext member.
 *
 * Verify that the specified pNext value points to the head of a list of
 * allowed extension structures.  If no extension structures are allowed,
 * verify that pNext is null.
 *
 * @param report_data debug_report_data object for routing validation messages.
 * @param apiName Name of API call being validated.
 * @param parameterName Name of parameter being validated.
 * @param allowedStructNames Names of allowed structs.
 * @param next Pointer to validate.
 * @param allowedTypeCount total number of allowed structure types.
 * @param allowedTypes array of strcuture types allowed for pNext.
 * @return Boolean value indicating that the call should be skipped.
 */
static VkBool32 validate_struct_pnext(debug_report_data *report_data, const char *apiName, const char *parameterName,
                                      const char *allowedStructNames, const void *next, size_t allowedTypeCount,
                                      const VkStructureType *allowedTypes) {
    VkBool32 skipCall = VK_FALSE;

    if (next != NULL) {
        if (allowedTypeCount == 0) {
            skipCall |= log_msg(report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                                ParameterValidationName, "%s: value of %s must be NULL", apiName, parameterName);
        } else {
            const VkStructureType *start = allowedTypes;
            const VkStructureType *end = allowedTypes + allowedTypeCount;
            const GenericHeader *current = reinterpret_cast<const GenericHeader *>(next);

            while (current != NULL) {
                if (std::find(start, end, current->sType) == end) {
                    std::string typeName = string_VkStructureType(current->sType);

                    if (typeName == UnsupportedStructureTypeString) {
                        skipCall |= log_msg(
                            report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                            ParameterValidationName,
                            "%s: %s chain includes a structure with unexpected VkStructureType (%d); Allowed structures are [%s]",
                            apiName, parameterName, current->sType, allowedStructNames);
                    } else {
                        skipCall |= log_msg(
                            report_data, VK_DEBUG_REPORT_ERROR_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                            ParameterValidationName,
                            "%s: %s chain includes a structure with unexpected VkStructureType %s; Allowed structures are [%s]",
                            apiName, parameterName, typeName.c_str(), allowedStructNames);
                    }
                }

                current = reinterpret_cast<const GenericHeader *>(current->pNext);
            }
        }
    }

    return skipCall;
}

/**
* Validate a VkBool32 value.
*
* Generate a warning if a VkBool32 value is neither VK_TRUE nor VK_FALSE.
*
* @param report_data debug_report_data object for routing validation messages.
* @param apiName Name of API call being validated.
* @param parameterName Name of parameter being validated.
* @param value Boolean value to validate.
* @return Boolean value indicating that the call should be skipped.
*/
static VkBool32 validate_bool32(debug_report_data *report_data, const char *apiName, const char *parameterName, VkBool32 value) {
    VkBool32 skipCall = VK_FALSE;

    if ((value != VK_TRUE) && (value != VK_FALSE)) {
        skipCall |=
            log_msg(report_data, VK_DEBUG_REPORT_WARNING_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                    ParameterValidationName, "%s: value of %s (%d) is neither VK_TRUE nor VK_FALSE", apiName, parameterName, value);
    }

    return skipCall;
}

/**
* Validate a Vulkan enumeration value.
*
* Generate a warning if an enumeration token value does not fall within the core enumeration
* begin and end token values, and was not added to the enumeration by an extension.  Extension
* provided enumerations use the equation specified in Appendix C.10 of the Vulkan specification,
* with 1,000,000,000 as the base token value.
*
* @note This function does not expect to process enumerations defining bitmask flag bits.
*
* @param report_data debug_report_data object for routing validation messages.
* @param apiName Name of API call being validated.
* @param parameterName Name of parameter being validated.
* @param enumName Name of the enumeration being validated.
* @param begin The begin range value for the enumeration.
* @param end The end range value for the enumeration.
* @param value Boolean value to validate.
* @return Boolean value indicating that the call should be skipped.
*/
template <typename T>
VkBool32 validate_ranged_enum(debug_report_data *report_data, const char *apiName, const char *parameterName, const char *enumName,
                              T begin, T end, T value) {
    VkBool32 skipCall = VK_FALSE;

    if (((value < begin) || (value > end)) && !is_extension_added_token(value)) {
        skipCall |=
            log_msg(report_data, VK_DEBUG_REPORT_WARNING_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                    ParameterValidationName, "%s: value of %s (%d) does not fall within the begin..end range of the core %s "
                                             "enumeration tokens and is not an extension added token",
                    apiName, parameterName, value, enumName);
    }

    return skipCall;
}

/**
* Validate an array of Vulkan enumeration value.
*
* Process all enumeration token values in the specified array and generate a warning if a value
* does not fall within the core enumeration begin and end token values, and was not added to
* the enumeration by an extension.  Extension provided enumerations use the equation specified
* in Appendix C.10 of the Vulkan specification, with 1,000,000,000 as the base token value.
*
* @note This function does not expect to process enumerations defining bitmask flag bits.
*
* @param report_data debug_report_data object for routing validation messages.
* @param apiName Name of API call being validated.
* @param parameterName Name of parameter being validated.
* @param enumName Name of the enumeration being validated.
* @param begin The begin range value for the enumeration.
* @param end The end range value for the enumeration.
* @param value Boolean value to validate.
* @return Boolean value indicating that the call should be skipped.
*/
template <typename T>
static VkBool32 validate_ranged_enum_array(debug_report_data *report_data, const char *apiName, const char *parameterName,
                                           const char *enumName, T begin, T end, uint32_t count, const T *pValues) {
    VkBool32 skipCall = VK_FALSE;

    for (uint32_t i = 0; i < count; ++i) {
        if (((pValues[i] < begin) || (pValues[i] > end)) && !is_extension_added_token(pValues[i])) {
            skipCall |=
                log_msg(report_data, VK_DEBUG_REPORT_WARNING_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                ParameterValidationName, "%s: value of %s[%d] (%d) does not fall within the begin..end range of the core %s "
                "enumeration tokens and is not an extension added token",
                apiName, parameterName, i, pValues[i], enumName);
        }
    }

    return skipCall;
}

/**
* Get VkResult code description.
*
* Returns a string describing the specified VkResult code.  The description is based on the language in the Vulkan API specification.
*
* @param value VkResult code to process.
* @return String describing the specified VkResult code.
*/
static std::string get_result_description(VkResult result) {
    // clang-format off
    switch (result) {
        case VK_SUCCESS:                        return "a command completed successfully";
        case VK_NOT_READY:                      return "a fence or query has not yet completed";
        case VK_TIMEOUT:                        return "a wait operation has not completed in the specified time";
        case VK_EVENT_SET:                      return "an event is signaled";
        case VK_EVENT_RESET:                    return "an event is unsignalled";
        case VK_INCOMPLETE:                     return "a return array was too small for the result";
        case VK_ERROR_OUT_OF_HOST_MEMORY:       return "a host memory allocation has failed";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:     return "a device memory allocation has failed";
        case VK_ERROR_INITIALIZATION_FAILED:    return "the logical device has been lost";
        case VK_ERROR_DEVICE_LOST:              return "initialization of an object has failed";
        case VK_ERROR_MEMORY_MAP_FAILED:        return "mapping of a memory object has failed";
        case VK_ERROR_LAYER_NOT_PRESENT:        return "the specified layer does not exist";
        case VK_ERROR_EXTENSION_NOT_PRESENT:    return "the specified extension does not exist";
        case VK_ERROR_FEATURE_NOT_PRESENT:      return "the requested feature is not available on this device";
        case VK_ERROR_INCOMPATIBLE_DRIVER:      return "a Vulkan driver could not be found";
        case VK_ERROR_TOO_MANY_OBJECTS:         return "too many objects of the type have already been created";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:     return "the requested format is not supported on this device";
        case VK_ERROR_SURFACE_LOST_KHR:         return "a surface is no longer available";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "the requested window is already connected to another "
                                                       "VkSurfaceKHR object, or some other non-Vulkan surface object";
        case VK_SUBOPTIMAL_KHR:                 return "an image became available, and the swapchain no longer "
                                                       "matches the surface properties exactly, but can still be used to "
                                                       "present to the surface successfully.";
        case VK_ERROR_OUT_OF_DATE_KHR:          return "a surface has changed in such a way that it is no "
                                                       "longer compatible with the swapchain";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "the display used by a swapchain does not use the same "
                                                       "presentable image layout, or is incompatible in a way that prevents "
                                                       "sharing an image";
        case VK_ERROR_VALIDATION_FAILED_EXT:    return "API validation has detected an invalid use of the API";
        case VK_ERROR_INVALID_SHADER_NV:        return "one or more shaders failed to compile or link";
        default:                                return "an error has occured";
    };
    // clang-format on
}

/**
* Validate return code.
*
* Print a message describing the reason for failure when an error code is returned.
*
* @param report_data debug_report_data object for routing validation messages.
* @param apiName Name of API call being validated.
* @param value VkResult value to validate.
*/
static void validate_result(debug_report_data *report_data, const char *apiName, VkResult result) {
    if (result < 0) {
        std::string resultName = string_VkResult(result);

        if (resultName == UnsupportedResultString) {
            // Unrecognized result code
            log_msg(report_data, VK_DEBUG_REPORT_WARNING_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                    ParameterValidationName, "%s: returned a result code indicating that an error has occured", apiName);
        } else {
            std::string resultDesc = get_result_description(result);
            log_msg(report_data, VK_DEBUG_REPORT_WARNING_BIT_EXT, (VkDebugReportObjectTypeEXT)0, 0, __LINE__, 1,
                    ParameterValidationName, "%s: returned %s, indicating that %s", apiName, resultName.c_str(), resultDesc.c_str());
        }
    }
}

#endif // PARAMETER_VALIDATION_UTILS_H
