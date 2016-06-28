/* THIS FILE IS GENERATED.  DO NOT EDIT. */

/*
 * Copyright (c) 2015-2016 Valve Corporation
 * Copyright (c) 2015-2016 LunarG, Inc.
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
 * Author: Tobin Ehlis <tobin@lunarg.com>
 * Author: Courtney Goeltzenleuchter <courtney@lunarg.com>
 * Author: Jon Ashburn <jon@lunarg.com>
 * Author: Mark Lobodzinski <mark@lunarg.com>
 * Author: Mike Stroyan <mike@lunarg.com>
 * Author: Tony Barbour <tony@LunarG.com>
 */

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #869
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>

#include "vk_loader_platform.h"
#include "vulkan/vk_layer.h"
#include "vk_struct_string_helper_cpp.h"
#include "vk_layer_table.h"
#include "vk_layer_extension_utils.h"
#include "vk_layer_config.h"
#include "vk_layer_utils.h"
#include <unordered_map>
#include "api_dump.h"

static std::ofstream fileStream;
static std::string fileName = "vk_apidump.txt";
std::ostream* outputStream = NULL;
void ConfigureOutputStream(bool writeToFile, bool flushAfterWrite)
{
    if(writeToFile)
    {
        if (fileName == "stdout")
        {
            outputStream = &std::cout;
            (*outputStream) << endl << "api_dump output filename 'stdout' specified. Writing to STDOUT instead of a file." << endl << endl;
        } else {
            fileStream.open(fileName);
            if ((fileStream.rdstate() & fileStream.failbit) != 0) {
                outputStream = &std::cout;
                (*outputStream) << endl << "api_dump ERROR: Bad output filename specified: " << fileName << ". Writing to STDOUT instead" << endl << endl;
            }
            else
                outputStream = &fileStream;
        }
    }
    else
    {
        outputStream = &std::cout;
    }

    if(flushAfterWrite)
    {
        outputStream->sync_with_stdio(true);
    }
    else
    {
        outputStream->sync_with_stdio(false);
    }
}

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #921
static bool g_ApiDumpDetailed = true;

static LOADER_PLATFORM_THREAD_ONCE_DECLARATION(initOnce);

static int printLockInitialized = 0;
static loader_platform_thread_mutex printLock;

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #929
#define LAYER_EXT_ARRAY_SIZE 1
#define LAYER_DEV_EXT_ARRAY_SIZE 1
#define MAX_TID 513
static loader_platform_thread_id tidMapping[MAX_TID] = {0};
static uint32_t maxTID = 0;
// Map actual TID to an index value and return that index
//  This keeps TIDs in range from 0-MAX_TID and simplifies compares between runs
static uint32_t getTIDIndex() {
    loader_platform_thread_id tid = loader_platform_get_thread_id();
    for (uint32_t i = 0; i < maxTID; i++) {
        if (tid == tidMapping[i])
            return i;
    }
    // Don't yet have mapping, set it and return newly set index
    uint32_t retVal = (uint32_t) maxTID;
    tidMapping[maxTID++] = tid;
    assert(maxTID < MAX_TID);
    return retVal;
}


// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #954
#include "vk_dispatch_table_helper.h"
#include "vk_layer_config.h"

static void initapi_dump(void)
{
    using namespace StreamControl;

    char const*const logName = getLayerOption("lunarg_api_dump.log_filename");
    if(logName != NULL)
    {
        fileName = logName;
    }

    char const*const detailedStr = getLayerOption("lunarg_api_dump.detailed");
    if(detailedStr != NULL)
    {
        if(strcmp(detailedStr, "TRUE") == 0)
        {
            g_ApiDumpDetailed = true;
        }
        else if(strcmp(detailedStr, "FALSE") == 0)
        {
            g_ApiDumpDetailed = false;
        }
    }

    char const*const writeToFileStr = getLayerOption("lunarg_api_dump.file");
    bool writeToFile = false;
    if(writeToFileStr != NULL)
    {
        if(strcmp(writeToFileStr, "TRUE") == 0)
        {
            writeToFile = true;
        }
        else if(strcmp(writeToFileStr, "FALSE") == 0)
        {
            writeToFile = false;
        }
    }

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #995
    char const*const noAddrStr = getLayerOption("lunarg_api_dump.no_addr");
    if(noAddrStr != NULL)
    {
        if(strcmp(noAddrStr, "FALSE") == 0)
        {
            StreamControl::writeAddress = true;
        }
        else if(strcmp(noAddrStr, "TRUE") == 0)
        {
            StreamControl::writeAddress = false;
        }
    }

    char const*const flushAfterWriteStr = getLayerOption("lunarg_api_dump.flush");
    bool flushAfterWrite = false;
    if(flushAfterWriteStr != NULL)
    {
        if(strcmp(flushAfterWriteStr, "TRUE") == 0)
        {
            flushAfterWrite = true;
        }
        else if(strcmp(flushAfterWriteStr, "FALSE") == 0)
        {
            flushAfterWrite = false;
        }
    }

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1023
    ConfigureOutputStream(writeToFile, flushAfterWrite);

    if (!printLockInitialized)
    {
        // TODO/TBD: Need to delete this mutex sometime.  How???
        loader_platform_thread_create_mutex(&printLock);
        printLockInitialized = 1;
    }
}


VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance)
{
    using namespace StreamControl;
    loader_platform_thread_once(&initOnce, initapi_dump);
    VkLayerInstanceCreateInfo *chain_info = get_chain_info(pCreateInfo, VK_LAYER_LINK_INFO);
    PFN_vkGetInstanceProcAddr fpGetInstanceProcAddr = chain_info->u.pLayerInfo->pfnNextGetInstanceProcAddr;
    PFN_vkCreateInstance fpCreateInstance = (PFN_vkCreateInstance) fpGetInstanceProcAddr(NULL, "vkCreateInstance");
    if (fpCreateInstance == NULL) {
                return VK_ERROR_INITIALIZATION_FAILED;
    }
    // Advance the link info for the next element on the chain
    assert(chain_info->u.pLayerInfo);
    chain_info->u.pLayerInfo = chain_info->u.pLayerInfo->pNext;
    VkResult result = fpCreateInstance(pCreateInfo, pAllocator, pInstance);
    if (result != VK_SUCCESS)
        return result;
    initInstanceTable(*pInstance, fpGetInstanceProcAddr);
    createInstanceRegisterExtensions(pCreateInfo, *pInstance);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateInstance(pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pInstance = " << (void*)*pInstance << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateInstance(pCreateInfo = address, pAllocator = address, pInstance = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkinstancecreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}


VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyInstance(VkInstance instance, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    dispatch_key key = get_dispatch_key(instance);
    VkLayerInstanceDispatchTable *pDisp  = instance_dispatch_table(instance);
    pDisp->DestroyInstance(instance, pAllocator);
    instanceExtMap.erase(pDisp);
    destroy_instance_dispatch_table(key);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyInstance(instance = " << (void*)(instance) << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyInstance(instance = address, pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumeratePhysicalDevices(VkInstance instance, uint32_t* pPhysicalDeviceCount, VkPhysicalDevice* pPhysicalDevices)
{
    using namespace StreamControl;
    VkResult result = instance_dispatch_table(instance)->EnumeratePhysicalDevices(instance, pPhysicalDeviceCount, pPhysicalDevices);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkEnumeratePhysicalDevices(instance = " << (void*)(instance) << ", pPhysicalDeviceCount = " << *(pPhysicalDeviceCount) << ", pPhysicalDevices = " << (void*)(pPhysicalDevices) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkEnumeratePhysicalDevices(instance = address, pPhysicalDeviceCount = " << *(pPhysicalDeviceCount) << ", pPhysicalDevices = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pPhysicalDevices) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < *pPhysicalDeviceCount; i++) {
                tmp_str = string_convert_helper(pPhysicalDevices[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pPhysicalDevices[" << i << "] (" << &pPhysicalDevices[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pPhysicalDevices[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFeatures(VkPhysicalDevice physicalDevice, VkPhysicalDeviceFeatures* pFeatures)
{
    using namespace StreamControl;
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceFeatures(physicalDevice, pFeatures);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceFeatures(physicalDevice = " << (void*)(physicalDevice) << ", pFeatures = " << (void*)(pFeatures) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceFeatures(physicalDevice = address, pFeatures = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pFeatures) {
            tmp_str = vk_print_vkphysicaldevicefeatures(pFeatures, "    ");
            (*outputStream) << "   pFeatures (" << pFeatures << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkFormatProperties* pFormatProperties)
{
    using namespace StreamControl;
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceFormatProperties(physicalDevice, format, pFormatProperties);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceFormatProperties(physicalDevice = " << (void*)(physicalDevice) << ", format = " << string_VkFormat(format) << ", pFormatProperties = " << (void*)(pFormatProperties) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceFormatProperties(physicalDevice = address, format = " << string_VkFormat(format) << ", pFormatProperties = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pFormatProperties) {
            tmp_str = vk_print_vkformatproperties(pFormatProperties, "    ");
            (*outputStream) << "   pFormatProperties (" << pFormatProperties << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags, VkImageFormatProperties* pImageFormatProperties)
{
    using namespace StreamControl;
    VkResult result = instance_dispatch_table(physicalDevice)->GetPhysicalDeviceImageFormatProperties(physicalDevice, format, type, tiling, usage, flags, pImageFormatProperties);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceImageFormatProperties(physicalDevice = " << (void*)(physicalDevice) << ", format = " << string_VkFormat(format) << ", type = " << string_VkImageType(type) << ", tiling = " << string_VkImageTiling(tiling) << ", usage = " << usage << ", flags = " << flags << ", pImageFormatProperties = " << (void*)(pImageFormatProperties) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceImageFormatProperties(physicalDevice = address, format = " << string_VkFormat(format) << ", type = " << string_VkImageType(type) << ", tiling = " << string_VkImageTiling(tiling) << ", usage = " << usage << ", flags = " << flags << ", pImageFormatProperties = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pImageFormatProperties) {
            tmp_str = vk_print_vkimageformatproperties(pImageFormatProperties, "    ");
            (*outputStream) << "   pImageFormatProperties (" << pImageFormatProperties << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties* pProperties)
{
    using namespace StreamControl;
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceProperties(physicalDevice, pProperties);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceProperties(physicalDevice = " << (void*)(physicalDevice) << ", pProperties = " << (void*)(pProperties) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceProperties(physicalDevice = address, pProperties = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pProperties) {
            tmp_str = vk_print_vkphysicaldeviceproperties(pProperties, "    ");
            (*outputStream) << "   pProperties (" << pProperties << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice, uint32_t* pQueueFamilyPropertyCount, VkQueueFamilyProperties* pQueueFamilyProperties)
{
    using namespace StreamControl;
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceQueueFamilyProperties(physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice = " << (void*)(physicalDevice) << ", pQueueFamilyPropertyCount = " << *(pQueueFamilyPropertyCount) << ", pQueueFamilyProperties = " << (void*)(pQueueFamilyProperties) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice = address, pQueueFamilyPropertyCount = " << *(pQueueFamilyPropertyCount) << ", pQueueFamilyProperties = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pQueueFamilyProperties) {
            tmp_str = vk_print_vkqueuefamilyproperties(pQueueFamilyProperties, "    ");
            (*outputStream) << "   pQueueFamilyProperties (" << pQueueFamilyProperties << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice physicalDevice, VkPhysicalDeviceMemoryProperties* pMemoryProperties)
{
    using namespace StreamControl;
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceMemoryProperties(physicalDevice, pMemoryProperties);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceMemoryProperties(physicalDevice = " << (void*)(physicalDevice) << ", pMemoryProperties = " << (void*)(pMemoryProperties) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceMemoryProperties(physicalDevice = address, pMemoryProperties = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pMemoryProperties) {
            tmp_str = vk_print_vkphysicaldevicememoryproperties(pMemoryProperties, "    ");
            (*outputStream) << "   pMemoryProperties (" << pMemoryProperties << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1195


VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDevice(VkPhysicalDevice physicalDevice, const VkDeviceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDevice* pDevice)
{
    using namespace StreamControl;
    VkResult result = explicit_CreateDevice(physicalDevice, pCreateInfo, pAllocator, pDevice);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateDevice(physicalDevice = " << (void*)(physicalDevice) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pDevice = " << (void*)*pDevice << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateDevice(physicalDevice = address, pCreateInfo = address, pAllocator = address, pDevice = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkdevicecreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDevice(VkDevice device, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    dispatch_key key = get_dispatch_key(device);
    VkLayerDispatchTable *pDisp  = device_dispatch_table(device);
    pDisp->DestroyDevice(device, pAllocator);
    deviceExtMap.erase(pDisp);
    destroy_device_dispatch_table(key);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyDevice(device = " << (void*)(device) << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyDevice(device = address, pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #315

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char *pLayerName, uint32_t *pCount,  VkExtensionProperties* pProperties)
{
    return util_GetExtensionProperties(0, NULL, pCount, pProperties);
}

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #342
static const VkLayerProperties globalLayerProps[] = {
    {
        "VK_LAYER_LUNARG_api_dump",
        VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION), // specVersion
        VK_MAKE_VERSION(0, 1, 0), // implementationVersion
        "layer: api_dump",
    }
};

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #352

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t *pCount,  VkLayerProperties* pProperties)
{
    return util_GetLayerProperties(ARRAY_SIZE(globalLayerProps), globalLayerProps, pCount, pProperties);
}

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #366
static const VkLayerProperties deviceLayerProps[] = {
    {
        "VK_LAYER_LUNARG_api_dump",
        VK_MAKE_VERSION(1, 0, VK_HEADER_VERSION),
        VK_MAKE_VERSION(0, 1, 0),
        "layer: api_dump",
    }
};
VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateDeviceLayerProperties(VkPhysicalDevice physicalDevice, uint32_t *pCount, VkLayerProperties* pProperties)
{
    return util_GetLayerProperties(ARRAY_SIZE(deviceLayerProps), deviceLayerProps, pCount, pProperties);
}


VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceQueue(VkDevice device, uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue* pQueue)
{
    using namespace StreamControl;
    device_dispatch_table(device)->GetDeviceQueue(device, queueFamilyIndex, queueIndex, pQueue);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetDeviceQueue(device = " << (void*)(device) << ", queueFamilyIndex = " << queueFamilyIndex << ", queueIndex = " << queueIndex << ", pQueue = " << (void*)(pQueue) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetDeviceQueue(device = address, queueFamilyIndex = " << queueFamilyIndex << ", queueIndex = " << queueIndex << ", pQueue = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueSubmit(VkQueue queue, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(queue)->QueueSubmit(queue, submitCount, pSubmits, fence);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkQueueSubmit(queue = " << (void*)(queue) << ", submitCount = " << submitCount << ", pSubmits = " << (void*)(pSubmits) << ", fence = " << fence << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkQueueSubmit(queue = address, submitCount = " << submitCount << ", pSubmits = address, fence = " << fence << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pSubmits) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < submitCount; i++) {
                tmp_str = vk_print_vksubmitinfo(&pSubmits[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pSubmits[" << i << "] (" << &pSubmits[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pSubmits[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueWaitIdle(VkQueue queue)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(queue)->QueueWaitIdle(queue);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkQueueWaitIdle(queue = " << (void*)(queue) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkQueueWaitIdle(queue = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkDeviceWaitIdle(VkDevice device)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->DeviceWaitIdle(device);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDeviceWaitIdle(device = " << (void*)(device) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDeviceWaitIdle(device = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAllocateMemory(VkDevice device, const VkMemoryAllocateInfo* pAllocateInfo, const VkAllocationCallbacks* pAllocator, VkDeviceMemory* pMemory)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->AllocateMemory(device, pAllocateInfo, pAllocator, pMemory);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkAllocateMemory(device = " << (void*)(device) << ", pAllocateInfo = " << (void*)(pAllocateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pMemory = " << pMemory << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkAllocateMemory(device = address, pAllocateInfo = address, pAllocator = address, pMemory = " << pMemory << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocateInfo) {
            tmp_str = vk_print_vkmemoryallocateinfo(pAllocateInfo, "    ");
            (*outputStream) << "   pAllocateInfo (" << pAllocateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkFreeMemory(VkDevice device, VkDeviceMemory memory, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->FreeMemory(device, memory, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkFreeMemory(device = " << (void*)(device) << ", memory = " << memory << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkFreeMemory(device = address, memory = " << memory << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkMapMemory(VkDevice device, VkDeviceMemory memory, VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags, void** ppData)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->MapMemory(device, memory, offset, size, flags, ppData);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkMapMemory(device = " << (void*)(device) << ", memory = " << memory << ", offset = " << (void*)(offset) << ", size = " << (void*)(size) << ", flags = " << flags << ", ppData = " << (void*)*ppData << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkMapMemory(device = address, memory = " << memory << ", offset = address, size = address, flags = " << flags << ", ppData = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkUnmapMemory(VkDevice device, VkDeviceMemory memory)
{
    using namespace StreamControl;
    device_dispatch_table(device)->UnmapMemory(device, memory);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkUnmapMemory(device = " << (void*)(device) << ", memory = " << memory << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkUnmapMemory(device = address, memory = " << memory << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkFlushMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->FlushMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkFlushMappedMemoryRanges(device = " << (void*)(device) << ", memoryRangeCount = " << memoryRangeCount << ", pMemoryRanges = " << (void*)(pMemoryRanges) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkFlushMappedMemoryRanges(device = address, memoryRangeCount = " << memoryRangeCount << ", pMemoryRanges = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pMemoryRanges) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < memoryRangeCount; i++) {
                tmp_str = vk_print_vkmappedmemoryrange(&pMemoryRanges[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pMemoryRanges[" << i << "] (" << &pMemoryRanges[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pMemoryRanges[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkInvalidateMappedMemoryRanges(VkDevice device, uint32_t memoryRangeCount, const VkMappedMemoryRange* pMemoryRanges)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->InvalidateMappedMemoryRanges(device, memoryRangeCount, pMemoryRanges);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkInvalidateMappedMemoryRanges(device = " << (void*)(device) << ", memoryRangeCount = " << memoryRangeCount << ", pMemoryRanges = " << (void*)(pMemoryRanges) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkInvalidateMappedMemoryRanges(device = address, memoryRangeCount = " << memoryRangeCount << ", pMemoryRanges = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pMemoryRanges) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < memoryRangeCount; i++) {
                tmp_str = vk_print_vkmappedmemoryrange(&pMemoryRanges[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pMemoryRanges[" << i << "] (" << &pMemoryRanges[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pMemoryRanges[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetDeviceMemoryCommitment(VkDevice device, VkDeviceMemory memory, VkDeviceSize* pCommittedMemoryInBytes)
{
    using namespace StreamControl;
    device_dispatch_table(device)->GetDeviceMemoryCommitment(device, memory, pCommittedMemoryInBytes);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetDeviceMemoryCommitment(device = " << (void*)(device) << ", memory = " << memory << ", pCommittedMemoryInBytes = " << (void*)(pCommittedMemoryInBytes) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetDeviceMemoryCommitment(device = address, memory = " << memory << ", pCommittedMemoryInBytes = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBindBufferMemory(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->BindBufferMemory(device, buffer, memory, memoryOffset);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkBindBufferMemory(device = " << (void*)(device) << ", buffer = " << buffer << ", memory = " << memory << ", memoryOffset = " << (void*)(memoryOffset) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkBindBufferMemory(device = address, buffer = " << buffer << ", memory = " << memory << ", memoryOffset = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBindImageMemory(VkDevice device, VkImage image, VkDeviceMemory memory, VkDeviceSize memoryOffset)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->BindImageMemory(device, image, memory, memoryOffset);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkBindImageMemory(device = " << (void*)(device) << ", image = " << image << ", memory = " << memory << ", memoryOffset = " << (void*)(memoryOffset) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkBindImageMemory(device = address, image = " << image << ", memory = " << memory << ", memoryOffset = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetBufferMemoryRequirements(VkDevice device, VkBuffer buffer, VkMemoryRequirements* pMemoryRequirements)
{
    using namespace StreamControl;
    device_dispatch_table(device)->GetBufferMemoryRequirements(device, buffer, pMemoryRequirements);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetBufferMemoryRequirements(device = " << (void*)(device) << ", buffer = " << buffer << ", pMemoryRequirements = " << (void*)(pMemoryRequirements) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetBufferMemoryRequirements(device = address, buffer = " << buffer << ", pMemoryRequirements = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pMemoryRequirements) {
            tmp_str = vk_print_vkmemoryrequirements(pMemoryRequirements, "    ");
            (*outputStream) << "   pMemoryRequirements (" << pMemoryRequirements << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageMemoryRequirements(VkDevice device, VkImage image, VkMemoryRequirements* pMemoryRequirements)
{
    using namespace StreamControl;
    device_dispatch_table(device)->GetImageMemoryRequirements(device, image, pMemoryRequirements);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetImageMemoryRequirements(device = " << (void*)(device) << ", image = " << image << ", pMemoryRequirements = " << (void*)(pMemoryRequirements) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetImageMemoryRequirements(device = address, image = " << image << ", pMemoryRequirements = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pMemoryRequirements) {
            tmp_str = vk_print_vkmemoryrequirements(pMemoryRequirements, "    ");
            (*outputStream) << "   pMemoryRequirements (" << pMemoryRequirements << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageSparseMemoryRequirements(VkDevice device, VkImage image, uint32_t* pSparseMemoryRequirementCount, VkSparseImageMemoryRequirements* pSparseMemoryRequirements)
{
    using namespace StreamControl;
    device_dispatch_table(device)->GetImageSparseMemoryRequirements(device, image, pSparseMemoryRequirementCount, pSparseMemoryRequirements);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetImageSparseMemoryRequirements(device = " << (void*)(device) << ", image = " << image << ", pSparseMemoryRequirementCount = " << *(pSparseMemoryRequirementCount) << ", pSparseMemoryRequirements = " << (void*)(pSparseMemoryRequirements) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetImageSparseMemoryRequirements(device = address, image = " << image << ", pSparseMemoryRequirementCount = " << *(pSparseMemoryRequirementCount) << ", pSparseMemoryRequirements = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pSparseMemoryRequirements) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < *pSparseMemoryRequirementCount; i++) {
                tmp_str = vk_print_vksparseimagememoryrequirements(&pSparseMemoryRequirements[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pSparseMemoryRequirements[" << i << "] (" << &pSparseMemoryRequirements[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pSparseMemoryRequirements[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetPhysicalDeviceSparseImageFormatProperties(VkPhysicalDevice physicalDevice, VkFormat format, VkImageType type, VkSampleCountFlagBits samples, VkImageUsageFlags usage, VkImageTiling tiling, uint32_t* pPropertyCount, VkSparseImageFormatProperties* pProperties)
{
    using namespace StreamControl;
    instance_dispatch_table(physicalDevice)->GetPhysicalDeviceSparseImageFormatProperties(physicalDevice, format, type, samples, usage, tiling, pPropertyCount, pProperties);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice = " << (void*)(physicalDevice) << ", format = " << string_VkFormat(format) << ", type = " << string_VkImageType(type) << ", samples = " << string_VkSampleCountFlagBits(samples) << ", usage = " << usage << ", tiling = " << string_VkImageTiling(tiling) << ", pPropertyCount = " << *(pPropertyCount) << ", pProperties = " << (void*)(pProperties) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceSparseImageFormatProperties(physicalDevice = address, format = " << string_VkFormat(format) << ", type = " << string_VkImageType(type) << ", samples = " << string_VkSampleCountFlagBits(samples) << ", usage = " << usage << ", tiling = " << string_VkImageTiling(tiling) << ", pPropertyCount = " << *(pPropertyCount) << ", pProperties = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pProperties) {
            tmp_str = vk_print_vksparseimageformatproperties(pProperties, "    ");
            (*outputStream) << "   pProperties (" << pProperties << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueueBindSparse(VkQueue queue, uint32_t bindInfoCount, const VkBindSparseInfo* pBindInfo, VkFence fence)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(queue)->QueueBindSparse(queue, bindInfoCount, pBindInfo, fence);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkQueueBindSparse(queue = " << (void*)(queue) << ", bindInfoCount = " << bindInfoCount << ", pBindInfo = " << (void*)(pBindInfo) << ", fence = " << fence << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkQueueBindSparse(queue = address, bindInfoCount = " << bindInfoCount << ", pBindInfo = address, fence = " << fence << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pBindInfo) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < bindInfoCount; i++) {
                tmp_str = vk_print_vkbindsparseinfo(&pBindInfo[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pBindInfo[" << i << "] (" << &pBindInfo[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pBindInfo[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateFence(VkDevice device, const VkFenceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFence* pFence)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateFence(device, pCreateInfo, pAllocator, pFence);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateFence(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pFence = " << pFence << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateFence(device = address, pCreateInfo = address, pAllocator = address, pFence = " << pFence << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkfencecreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyFence(VkDevice device, VkFence fence, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyFence(device, fence, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyFence(device = " << (void*)(device) << ", fence = " << fence << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyFence(device = address, fence = " << fence << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->ResetFences(device, fenceCount, pFences);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkResetFences(device = " << (void*)(device) << ", fenceCount = " << fenceCount << ", pFences = " << (void*)(pFences) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkResetFences(device = address, fenceCount = " << fenceCount << ", pFences = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pFences) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < fenceCount; i++) {
                tmp_str = string_convert_helper(pFences[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pFences[" << i << "] (" << &pFences[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pFences[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetFenceStatus(VkDevice device, VkFence fence)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->GetFenceStatus(device, fence);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetFenceStatus(device = " << (void*)(device) << ", fence = " << fence << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetFenceStatus(device = address, fence = " << fence << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkWaitForFences(VkDevice device, uint32_t fenceCount, const VkFence* pFences, VkBool32 waitAll, uint64_t timeout)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->WaitForFences(device, fenceCount, pFences, waitAll, timeout);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkWaitForFences(device = " << (void*)(device) << ", fenceCount = " << fenceCount << ", pFences = " << (void*)(pFences) << ", waitAll = " << waitAll << ", timeout = " << timeout << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkWaitForFences(device = address, fenceCount = " << fenceCount << ", pFences = address, waitAll = " << waitAll << ", timeout = " << timeout << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pFences) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < fenceCount; i++) {
                tmp_str = string_convert_helper(pFences[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pFences[" << i << "] (" << &pFences[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pFences[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSemaphore(VkDevice device, const VkSemaphoreCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSemaphore* pSemaphore)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateSemaphore(device, pCreateInfo, pAllocator, pSemaphore);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateSemaphore(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pSemaphore = " << pSemaphore << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateSemaphore(device = address, pCreateInfo = address, pAllocator = address, pSemaphore = " << pSemaphore << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vksemaphorecreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySemaphore(VkDevice device, VkSemaphore semaphore, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroySemaphore(device, semaphore, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroySemaphore(device = " << (void*)(device) << ", semaphore = " << semaphore << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroySemaphore(device = address, semaphore = " << semaphore << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateEvent(VkDevice device, const VkEventCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkEvent* pEvent)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateEvent(device, pCreateInfo, pAllocator, pEvent);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateEvent(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pEvent = " << pEvent << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateEvent(device = address, pCreateInfo = address, pAllocator = address, pEvent = " << pEvent << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkeventcreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyEvent(VkDevice device, VkEvent event, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyEvent(device, event, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyEvent(device = " << (void*)(device) << ", event = " << event << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyEvent(device = address, event = " << event << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetEventStatus(VkDevice device, VkEvent event)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->GetEventStatus(device, event);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetEventStatus(device = " << (void*)(device) << ", event = " << event << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetEventStatus(device = address, event = " << event << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkSetEvent(VkDevice device, VkEvent event)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->SetEvent(device, event);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkSetEvent(device = " << (void*)(device) << ", event = " << event << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkSetEvent(device = address, event = " << event << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetEvent(VkDevice device, VkEvent event)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->ResetEvent(device, event);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkResetEvent(device = " << (void*)(device) << ", event = " << event << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkResetEvent(device = address, event = " << event << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateQueryPool(VkDevice device, const VkQueryPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkQueryPool* pQueryPool)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateQueryPool(device, pCreateInfo, pAllocator, pQueryPool);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateQueryPool(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pQueryPool = " << pQueryPool << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateQueryPool(device = address, pCreateInfo = address, pAllocator = address, pQueryPool = " << pQueryPool << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkquerypoolcreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyQueryPool(VkDevice device, VkQueryPool queryPool, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyQueryPool(device, queryPool, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyQueryPool(device = " << (void*)(device) << ", queryPool = " << queryPool << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyQueryPool(device = address, queryPool = " << queryPool << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetQueryPoolResults(VkDevice device, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, size_t dataSize, void* pData, VkDeviceSize stride, VkQueryResultFlags flags)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->GetQueryPoolResults(device, queryPool, firstQuery, queryCount, dataSize, pData, stride, flags);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetQueryPoolResults(device = " << (void*)(device) << ", queryPool = " << queryPool << ", firstQuery = " << firstQuery << ", queryCount = " << queryCount << ", dataSize = " << (unsigned long)dataSize << ", pData = " << (void*)(pData) << ", stride = " << (void*)(stride) << ", flags = " << flags << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetQueryPoolResults(device = address, queryPool = " << queryPool << ", firstQuery = " << firstQuery << ", queryCount = " << queryCount << ", dataSize = " << (unsigned long)dataSize << ", pData = address, stride = address, flags = " << flags << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateBuffer(VkDevice device, const VkBufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBuffer* pBuffer)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateBuffer(device, pCreateInfo, pAllocator, pBuffer);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateBuffer(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pBuffer = " << pBuffer << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateBuffer(device = address, pCreateInfo = address, pAllocator = address, pBuffer = " << pBuffer << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkbuffercreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyBuffer(VkDevice device, VkBuffer buffer, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyBuffer(device, buffer, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyBuffer(device = " << (void*)(device) << ", buffer = " << buffer << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyBuffer(device = address, buffer = " << buffer << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateBufferView(VkDevice device, const VkBufferViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkBufferView* pView)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateBufferView(device, pCreateInfo, pAllocator, pView);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateBufferView(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pView = " << pView << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateBufferView(device = address, pCreateInfo = address, pAllocator = address, pView = " << pView << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkbufferviewcreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyBufferView(VkDevice device, VkBufferView bufferView, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyBufferView(device, bufferView, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyBufferView(device = " << (void*)(device) << ", bufferView = " << bufferView << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyBufferView(device = address, bufferView = " << bufferView << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateImage(VkDevice device, const VkImageCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImage* pImage)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateImage(device, pCreateInfo, pAllocator, pImage);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateImage(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pImage = " << pImage << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateImage(device = address, pCreateInfo = address, pAllocator = address, pImage = " << pImage << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkimagecreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyImage(VkDevice device, VkImage image, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyImage(device, image, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyImage(device = " << (void*)(device) << ", image = " << image << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyImage(device = address, image = " << image << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetImageSubresourceLayout(VkDevice device, VkImage image, const VkImageSubresource* pSubresource, VkSubresourceLayout* pLayout)
{
    using namespace StreamControl;
    device_dispatch_table(device)->GetImageSubresourceLayout(device, image, pSubresource, pLayout);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetImageSubresourceLayout(device = " << (void*)(device) << ", image = " << image << ", pSubresource = " << (void*)(pSubresource) << ", pLayout = " << (void*)(pLayout) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetImageSubresourceLayout(device = address, image = " << image << ", pSubresource = address, pLayout = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pSubresource) {
            tmp_str = vk_print_vkimagesubresource(pSubresource, "    ");
            (*outputStream) << "   pSubresource (" << pSubresource << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pLayout) {
            tmp_str = vk_print_vksubresourcelayout(pLayout, "    ");
            (*outputStream) << "   pLayout (" << pLayout << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateImageView(VkDevice device, const VkImageViewCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkImageView* pView)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateImageView(device, pCreateInfo, pAllocator, pView);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateImageView(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pView = " << pView << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateImageView(device = address, pCreateInfo = address, pAllocator = address, pView = " << pView << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkimageviewcreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyImageView(VkDevice device, VkImageView imageView, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyImageView(device, imageView, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyImageView(device = " << (void*)(device) << ", imageView = " << imageView << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyImageView(device = address, imageView = " << imageView << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateShaderModule(VkDevice device, const VkShaderModuleCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkShaderModule* pShaderModule)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateShaderModule(device, pCreateInfo, pAllocator, pShaderModule);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateShaderModule(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pShaderModule = " << pShaderModule << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateShaderModule(device = address, pCreateInfo = address, pAllocator = address, pShaderModule = " << pShaderModule << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkshadermodulecreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyShaderModule(VkDevice device, VkShaderModule shaderModule, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyShaderModule(device, shaderModule, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyShaderModule(device = " << (void*)(device) << ", shaderModule = " << shaderModule << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyShaderModule(device = address, shaderModule = " << shaderModule << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineCache(VkDevice device, const VkPipelineCacheCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineCache* pPipelineCache)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreatePipelineCache(device, pCreateInfo, pAllocator, pPipelineCache);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreatePipelineCache(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pPipelineCache = " << pPipelineCache << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreatePipelineCache(device = address, pCreateInfo = address, pAllocator = address, pPipelineCache = " << pPipelineCache << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkpipelinecachecreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineCache(VkDevice device, VkPipelineCache pipelineCache, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyPipelineCache(device, pipelineCache, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyPipelineCache(device = " << (void*)(device) << ", pipelineCache = " << pipelineCache << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyPipelineCache(device = address, pipelineCache = " << pipelineCache << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPipelineCacheData(VkDevice device, VkPipelineCache pipelineCache, size_t* pDataSize, void* pData)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->GetPipelineCacheData(device, pipelineCache, pDataSize, pData);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPipelineCacheData(device = " << (void*)(device) << ", pipelineCache = " << pipelineCache << ", pDataSize = " << (unsigned long)*pDataSize << ", pData = " << (void*)(pData) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPipelineCacheData(device = address, pipelineCache = " << pipelineCache << ", pDataSize = " << (unsigned long)*pDataSize << ", pData = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkMergePipelineCaches(VkDevice device, VkPipelineCache dstCache, uint32_t srcCacheCount, const VkPipelineCache* pSrcCaches)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->MergePipelineCaches(device, dstCache, srcCacheCount, pSrcCaches);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkMergePipelineCaches(device = " << (void*)(device) << ", dstCache = " << dstCache << ", srcCacheCount = " << srcCacheCount << ", pSrcCaches = " << (void*)(pSrcCaches) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkMergePipelineCaches(device = address, dstCache = " << dstCache << ", srcCacheCount = " << srcCacheCount << ", pSrcCaches = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pSrcCaches) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < srcCacheCount; i++) {
                tmp_str = string_convert_helper(pSrcCaches[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pSrcCaches[" << i << "] (" << &pSrcCaches[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pSrcCaches[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateGraphicsPipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkGraphicsPipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateGraphicsPipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateGraphicsPipelines(device = " << (void*)(device) << ", pipelineCache = " << pipelineCache << ", createInfoCount = " << createInfoCount << ", pCreateInfos = " << (void*)(pCreateInfos) << ", pAllocator = " << (void*)(pAllocator) << ", pPipelines = " << pPipelines << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateGraphicsPipelines(device = address, pipelineCache = " << pipelineCache << ", createInfoCount = " << createInfoCount << ", pCreateInfos = address, pAllocator = address, pPipelines = " << pPipelines << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pCreateInfos) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < createInfoCount; i++) {
                tmp_str = vk_print_vkgraphicspipelinecreateinfo(&pCreateInfos[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pCreateInfos[" << i << "] (" << &pCreateInfos[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pCreateInfos[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateComputePipelines(VkDevice device, VkPipelineCache pipelineCache, uint32_t createInfoCount, const VkComputePipelineCreateInfo* pCreateInfos, const VkAllocationCallbacks* pAllocator, VkPipeline* pPipelines)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateComputePipelines(device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, pPipelines);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateComputePipelines(device = " << (void*)(device) << ", pipelineCache = " << pipelineCache << ", createInfoCount = " << createInfoCount << ", pCreateInfos = " << (void*)(pCreateInfos) << ", pAllocator = " << (void*)(pAllocator) << ", pPipelines = " << pPipelines << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateComputePipelines(device = address, pipelineCache = " << pipelineCache << ", createInfoCount = " << createInfoCount << ", pCreateInfos = address, pAllocator = address, pPipelines = " << pPipelines << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pCreateInfos) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < createInfoCount; i++) {
                tmp_str = vk_print_vkcomputepipelinecreateinfo(&pCreateInfos[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pCreateInfos[" << i << "] (" << &pCreateInfos[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pCreateInfos[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPipeline(VkDevice device, VkPipeline pipeline, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyPipeline(device, pipeline, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyPipeline(device = " << (void*)(device) << ", pipeline = " << pipeline << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyPipeline(device = address, pipeline = " << pipeline << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreatePipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkPipelineLayout* pPipelineLayout)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreatePipelineLayout(device, pCreateInfo, pAllocator, pPipelineLayout);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreatePipelineLayout(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pPipelineLayout = " << pPipelineLayout << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreatePipelineLayout(device = address, pCreateInfo = address, pAllocator = address, pPipelineLayout = " << pPipelineLayout << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkpipelinelayoutcreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyPipelineLayout(VkDevice device, VkPipelineLayout pipelineLayout, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyPipelineLayout(device, pipelineLayout, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyPipelineLayout(device = " << (void*)(device) << ", pipelineLayout = " << pipelineLayout << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyPipelineLayout(device = address, pipelineLayout = " << pipelineLayout << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSampler(VkDevice device, const VkSamplerCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSampler* pSampler)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateSampler(device, pCreateInfo, pAllocator, pSampler);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateSampler(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pSampler = " << pSampler << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateSampler(device = address, pCreateInfo = address, pAllocator = address, pSampler = " << pSampler << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vksamplercreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySampler(VkDevice device, VkSampler sampler, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroySampler(device, sampler, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroySampler(device = " << (void*)(device) << ", sampler = " << sampler << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroySampler(device = address, sampler = " << sampler << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorSetLayout(VkDevice device, const VkDescriptorSetLayoutCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorSetLayout* pSetLayout)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateDescriptorSetLayout(device, pCreateInfo, pAllocator, pSetLayout);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateDescriptorSetLayout(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pSetLayout = " << pSetLayout << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateDescriptorSetLayout(device = address, pCreateInfo = address, pAllocator = address, pSetLayout = " << pSetLayout << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkdescriptorsetlayoutcreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyDescriptorSetLayout(device, descriptorSetLayout, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyDescriptorSetLayout(device = " << (void*)(device) << ", descriptorSetLayout = " << descriptorSetLayout << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyDescriptorSetLayout(device = address, descriptorSetLayout = " << descriptorSetLayout << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateDescriptorPool(VkDevice device, const VkDescriptorPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDescriptorPool* pDescriptorPool)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateDescriptorPool(device, pCreateInfo, pAllocator, pDescriptorPool);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateDescriptorPool(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pDescriptorPool = " << pDescriptorPool << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateDescriptorPool(device = address, pCreateInfo = address, pAllocator = address, pDescriptorPool = " << pDescriptorPool << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkdescriptorpoolcreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyDescriptorPool(device, descriptorPool, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyDescriptorPool(device = " << (void*)(device) << ", descriptorPool = " << descriptorPool << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyDescriptorPool(device = address, descriptorPool = " << descriptorPool << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetDescriptorPool(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorPoolResetFlags flags)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->ResetDescriptorPool(device, descriptorPool, flags);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkResetDescriptorPool(device = " << (void*)(device) << ", descriptorPool = " << descriptorPool << ", flags = " << flags << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkResetDescriptorPool(device = address, descriptorPool = " << descriptorPool << ", flags = " << flags << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo* pAllocateInfo, VkDescriptorSet* pDescriptorSets)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->AllocateDescriptorSets(device, pAllocateInfo, pDescriptorSets);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkAllocateDescriptorSets(device = " << (void*)(device) << ", pAllocateInfo = " << (void*)(pAllocateInfo) << ", pDescriptorSets = " << pDescriptorSets << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkAllocateDescriptorSets(device = address, pAllocateInfo = address, pDescriptorSets = " << pDescriptorSets << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocateInfo) {
            tmp_str = vk_print_vkdescriptorsetallocateinfo(pAllocateInfo, "    ");
            (*outputStream) << "   pAllocateInfo (" << pAllocateInfo << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkFreeDescriptorSets(VkDevice device, VkDescriptorPool descriptorPool, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->FreeDescriptorSets(device, descriptorPool, descriptorSetCount, pDescriptorSets);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkFreeDescriptorSets(device = " << (void*)(device) << ", descriptorPool = " << descriptorPool << ", descriptorSetCount = " << descriptorSetCount << ", pDescriptorSets = " << (void*)(pDescriptorSets) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkFreeDescriptorSets(device = address, descriptorPool = " << descriptorPool << ", descriptorSetCount = " << descriptorSetCount << ", pDescriptorSets = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pDescriptorSets) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < descriptorSetCount; i++) {
                tmp_str = string_convert_helper(pDescriptorSets[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pDescriptorSets[" << i << "] (" << &pDescriptorSets[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pDescriptorSets[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkUpdateDescriptorSets(VkDevice device, uint32_t descriptorWriteCount, const VkWriteDescriptorSet* pDescriptorWrites, uint32_t descriptorCopyCount, const VkCopyDescriptorSet* pDescriptorCopies)
{
    using namespace StreamControl;
    device_dispatch_table(device)->UpdateDescriptorSets(device, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkUpdateDescriptorSets(device = " << (void*)(device) << ", descriptorWriteCount = " << descriptorWriteCount << ", pDescriptorWrites = " << (void*)(pDescriptorWrites) << ", descriptorCopyCount = " << descriptorCopyCount << ", pDescriptorCopies = " << (void*)(pDescriptorCopies) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkUpdateDescriptorSets(device = address, descriptorWriteCount = " << descriptorWriteCount << ", pDescriptorWrites = address, descriptorCopyCount = " << descriptorCopyCount << ", pDescriptorCopies = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pDescriptorWrites) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < descriptorWriteCount; i++) {
                tmp_str = vk_print_vkwritedescriptorset(&pDescriptorWrites[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pDescriptorWrites[" << i << "] (" << &pDescriptorWrites[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pDescriptorWrites[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pDescriptorCopies) {
            tmp_str = vk_print_vkcopydescriptorset(pDescriptorCopies, "    ");
            (*outputStream) << "   pDescriptorCopies (" << pDescriptorCopies << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateFramebuffer(VkDevice device, const VkFramebufferCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkFramebuffer* pFramebuffer)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateFramebuffer(device, pCreateInfo, pAllocator, pFramebuffer);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateFramebuffer(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pFramebuffer = " << pFramebuffer << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateFramebuffer(device = address, pCreateInfo = address, pAllocator = address, pFramebuffer = " << pFramebuffer << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkframebuffercreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyFramebuffer(VkDevice device, VkFramebuffer framebuffer, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyFramebuffer(device, framebuffer, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyFramebuffer(device = " << (void*)(device) << ", framebuffer = " << framebuffer << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyFramebuffer(device = address, framebuffer = " << framebuffer << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateRenderPass(VkDevice device, const VkRenderPassCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateRenderPass(device, pCreateInfo, pAllocator, pRenderPass);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateRenderPass(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pRenderPass = " << pRenderPass << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateRenderPass(device = address, pCreateInfo = address, pAllocator = address, pRenderPass = " << pRenderPass << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkrenderpasscreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyRenderPass(VkDevice device, VkRenderPass renderPass, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyRenderPass(device, renderPass, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyRenderPass(device = " << (void*)(device) << ", renderPass = " << renderPass << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyRenderPass(device = address, renderPass = " << renderPass << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkGetRenderAreaGranularity(VkDevice device, VkRenderPass renderPass, VkExtent2D* pGranularity)
{
    using namespace StreamControl;
    device_dispatch_table(device)->GetRenderAreaGranularity(device, renderPass, pGranularity);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetRenderAreaGranularity(device = " << (void*)(device) << ", renderPass = " << renderPass << ", pGranularity = " << (void*)(pGranularity) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetRenderAreaGranularity(device = address, renderPass = " << renderPass << ", pGranularity = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pGranularity) {
            tmp_str = vk_print_vkextent2d(pGranularity, "    ");
            (*outputStream) << "   pGranularity (" << pGranularity << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateCommandPool(VkDevice device, const VkCommandPoolCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkCommandPool* pCommandPool)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateCommandPool(device, pCreateInfo, pAllocator, pCommandPool);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateCommandPool(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pCommandPool = " << pCommandPool << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateCommandPool(device = address, pCreateInfo = address, pAllocator = address, pCommandPool = " << pCommandPool << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkcommandpoolcreateinfo(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroyCommandPool(device, commandPool, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyCommandPool(device = " << (void*)(device) << ", commandPool = " << commandPool << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroyCommandPool(device = address, commandPool = " << commandPool << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->ResetCommandPool(device, commandPool, flags);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkResetCommandPool(device = " << (void*)(device) << ", commandPool = " << commandPool << ", flags = " << flags << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkResetCommandPool(device = address, commandPool = " << commandPool << ", flags = " << flags << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo* pAllocateInfo, VkCommandBuffer* pCommandBuffers)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->AllocateCommandBuffers(device, pAllocateInfo, pCommandBuffers);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkAllocateCommandBuffers(device = " << (void*)(device) << ", pAllocateInfo = " << (void*)(pAllocateInfo) << ", pCommandBuffers = " << (void*)*pCommandBuffers << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkAllocateCommandBuffers(device = address, pAllocateInfo = address, pCommandBuffers = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocateInfo) {
            tmp_str = vk_print_vkcommandbufferallocateinfo(pAllocateInfo, "    ");
            (*outputStream) << "   pAllocateInfo (" << pAllocateInfo << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
    using namespace StreamControl;
    device_dispatch_table(device)->FreeCommandBuffers(device, commandPool, commandBufferCount, pCommandBuffers);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkFreeCommandBuffers(device = " << (void*)(device) << ", commandPool = " << commandPool << ", commandBufferCount = " << commandBufferCount << ", pCommandBuffers = " << (void*)(pCommandBuffers) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkFreeCommandBuffers(device = address, commandPool = " << commandPool << ", commandBufferCount = " << commandBufferCount << ", pCommandBuffers = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pCommandBuffers) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < commandBufferCount; i++) {
                tmp_str = string_convert_helper(pCommandBuffers[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pCommandBuffers[" << i << "] (" << &pCommandBuffers[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pCommandBuffers[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkBeginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo* pBeginInfo)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(commandBuffer)->BeginCommandBuffer(commandBuffer, pBeginInfo);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkBeginCommandBuffer(commandBuffer = " << (void*)(commandBuffer) << ", pBeginInfo = " << (void*)(pBeginInfo) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkBeginCommandBuffer(commandBuffer = address, pBeginInfo = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pBeginInfo) {
            tmp_str = vk_print_vkcommandbufferbegininfo(pBeginInfo, "    ");
            (*outputStream) << "   pBeginInfo (" << pBeginInfo << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkEndCommandBuffer(VkCommandBuffer commandBuffer)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(commandBuffer)->EndCommandBuffer(commandBuffer);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkEndCommandBuffer(commandBuffer = " << (void*)(commandBuffer) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkEndCommandBuffer(commandBuffer = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkResetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(commandBuffer)->ResetCommandBuffer(commandBuffer, flags);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkResetCommandBuffer(commandBuffer = " << (void*)(commandBuffer) << ", flags = " << flags << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkResetCommandBuffer(commandBuffer = address, flags = " << flags << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindPipeline(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBindPipeline(commandBuffer = " << (void*)(commandBuffer) << ", pipelineBindPoint = " << string_VkPipelineBindPoint(pipelineBindPoint) << ", pipeline = " << pipeline << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBindPipeline(commandBuffer = address, pipelineBindPoint = " << string_VkPipelineBindPoint(pipelineBindPoint) << ", pipeline = " << pipeline << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetViewport(VkCommandBuffer commandBuffer, uint32_t firstViewport, uint32_t viewportCount, const VkViewport* pViewports)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdSetViewport(commandBuffer, firstViewport, viewportCount, pViewports);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetViewport(commandBuffer = " << (void*)(commandBuffer) << ", firstViewport = " << firstViewport << ", viewportCount = " << viewportCount << ", pViewports = " << (void*)(pViewports) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetViewport(commandBuffer = address, firstViewport = " << firstViewport << ", viewportCount = " << viewportCount << ", pViewports = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pViewports) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < viewportCount; i++) {
                tmp_str = vk_print_vkviewport(&pViewports[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pViewports[" << i << "] (" << &pViewports[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pViewports[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetScissor(VkCommandBuffer commandBuffer, uint32_t firstScissor, uint32_t scissorCount, const VkRect2D* pScissors)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdSetScissor(commandBuffer, firstScissor, scissorCount, pScissors);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetScissor(commandBuffer = " << (void*)(commandBuffer) << ", firstScissor = " << firstScissor << ", scissorCount = " << scissorCount << ", pScissors = " << (void*)(pScissors) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetScissor(commandBuffer = address, firstScissor = " << firstScissor << ", scissorCount = " << scissorCount << ", pScissors = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pScissors) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < scissorCount; i++) {
                tmp_str = vk_print_vkrect2d(&pScissors[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pScissors[" << i << "] (" << &pScissors[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pScissors[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetLineWidth(VkCommandBuffer commandBuffer, float lineWidth)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdSetLineWidth(commandBuffer, lineWidth);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetLineWidth(commandBuffer = " << (void*)(commandBuffer) << ", lineWidth = " << lineWidth << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetLineWidth(commandBuffer = address, lineWidth = " << lineWidth << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBias(VkCommandBuffer commandBuffer, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdSetDepthBias(commandBuffer, depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetDepthBias(commandBuffer = " << (void*)(commandBuffer) << ", depthBiasConstantFactor = " << depthBiasConstantFactor << ", depthBiasClamp = " << depthBiasClamp << ", depthBiasSlopeFactor = " << depthBiasSlopeFactor << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetDepthBias(commandBuffer = address, depthBiasConstantFactor = " << depthBiasConstantFactor << ", depthBiasClamp = " << depthBiasClamp << ", depthBiasSlopeFactor = " << depthBiasSlopeFactor << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetBlendConstants(VkCommandBuffer commandBuffer, const float blendConstants[4])
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdSetBlendConstants(commandBuffer, blendConstants);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetBlendConstants(commandBuffer = " << (void*)(commandBuffer) << ", blendConstants = " << "[" << blendConstants[0] << "," << blendConstants[1] << "," << blendConstants[2] << "," << blendConstants[3] << "]" << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetBlendConstants(commandBuffer = address, blendConstants = " << "[" << blendConstants[0] << "," << blendConstants[1] << "," << blendConstants[2] << "," << blendConstants[3] << "]" << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetDepthBounds(VkCommandBuffer commandBuffer, float minDepthBounds, float maxDepthBounds)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdSetDepthBounds(commandBuffer, minDepthBounds, maxDepthBounds);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetDepthBounds(commandBuffer = " << (void*)(commandBuffer) << ", minDepthBounds = " << minDepthBounds << ", maxDepthBounds = " << maxDepthBounds << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetDepthBounds(commandBuffer = address, minDepthBounds = " << minDepthBounds << ", maxDepthBounds = " << maxDepthBounds << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilCompareMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t compareMask)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdSetStencilCompareMask(commandBuffer, faceMask, compareMask);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetStencilCompareMask(commandBuffer = " << (void*)(commandBuffer) << ", faceMask = " << faceMask << ", compareMask = " << compareMask << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetStencilCompareMask(commandBuffer = address, faceMask = " << faceMask << ", compareMask = " << compareMask << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilWriteMask(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t writeMask)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdSetStencilWriteMask(commandBuffer, faceMask, writeMask);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetStencilWriteMask(commandBuffer = " << (void*)(commandBuffer) << ", faceMask = " << faceMask << ", writeMask = " << writeMask << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetStencilWriteMask(commandBuffer = address, faceMask = " << faceMask << ", writeMask = " << writeMask << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetStencilReference(VkCommandBuffer commandBuffer, VkStencilFaceFlags faceMask, uint32_t reference)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdSetStencilReference(commandBuffer, faceMask, reference);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetStencilReference(commandBuffer = " << (void*)(commandBuffer) << ", faceMask = " << faceMask << ", reference = " << reference << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetStencilReference(commandBuffer = address, faceMask = " << faceMask << ", reference = " << reference << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindDescriptorSets(VkCommandBuffer commandBuffer, VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdBindDescriptorSets(commandBuffer, pipelineBindPoint, layout, firstSet, descriptorSetCount, pDescriptorSets, dynamicOffsetCount, pDynamicOffsets);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBindDescriptorSets(commandBuffer = " << (void*)(commandBuffer) << ", pipelineBindPoint = " << string_VkPipelineBindPoint(pipelineBindPoint) << ", layout = " << layout << ", firstSet = " << firstSet << ", descriptorSetCount = " << descriptorSetCount << ", pDescriptorSets = " << (void*)(pDescriptorSets) << ", dynamicOffsetCount = " << dynamicOffsetCount << ", pDynamicOffsets = " << (void*)(pDynamicOffsets) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBindDescriptorSets(commandBuffer = address, pipelineBindPoint = " << string_VkPipelineBindPoint(pipelineBindPoint) << ", layout = " << layout << ", firstSet = " << firstSet << ", descriptorSetCount = " << descriptorSetCount << ", pDescriptorSets = address, dynamicOffsetCount = " << dynamicOffsetCount << ", pDynamicOffsets = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pDescriptorSets) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < descriptorSetCount; i++) {
                tmp_str = string_convert_helper(pDescriptorSets[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pDescriptorSets[" << i << "] (" << &pDescriptorSets[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pDescriptorSets[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
        if (pDynamicOffsets) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < dynamicOffsetCount; i++) {
                tmp_str = string_convert_helper(pDynamicOffsets[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pDynamicOffsets[" << i << "] (" << &pDynamicOffsets[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pDynamicOffsets[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindIndexBuffer(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdBindIndexBuffer(commandBuffer, buffer, offset, indexType);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBindIndexBuffer(commandBuffer = " << (void*)(commandBuffer) << ", buffer = " << buffer << ", offset = " << (void*)(offset) << ", indexType = " << string_VkIndexType(indexType) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBindIndexBuffer(commandBuffer = address, buffer = " << buffer << ", offset = address, indexType = " << string_VkIndexType(indexType) << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBindVertexBuffers(VkCommandBuffer commandBuffer, uint32_t firstBinding, uint32_t bindingCount, const VkBuffer* pBuffers, const VkDeviceSize* pOffsets)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdBindVertexBuffers(commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBindVertexBuffers(commandBuffer = " << (void*)(commandBuffer) << ", firstBinding = " << firstBinding << ", bindingCount = " << bindingCount << ", pBuffers = " << (void*)(pBuffers) << ", pOffsets = " << (void*)(pOffsets) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBindVertexBuffers(commandBuffer = address, firstBinding = " << firstBinding << ", bindingCount = " << bindingCount << ", pBuffers = address, pOffsets = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDraw(VkCommandBuffer commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdDraw(commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDraw(commandBuffer = " << (void*)(commandBuffer) << ", vertexCount = " << vertexCount << ", instanceCount = " << instanceCount << ", firstVertex = " << firstVertex << ", firstInstance = " << firstInstance << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDraw(commandBuffer = address, vertexCount = " << vertexCount << ", instanceCount = " << instanceCount << ", firstVertex = " << firstVertex << ", firstInstance = " << firstInstance << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexed(VkCommandBuffer commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdDrawIndexed(commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDrawIndexed(commandBuffer = " << (void*)(commandBuffer) << ", indexCount = " << indexCount << ", instanceCount = " << instanceCount << ", firstIndex = " << firstIndex << ", vertexOffset = " << vertexOffset << ", firstInstance = " << firstInstance << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDrawIndexed(commandBuffer = address, indexCount = " << indexCount << ", instanceCount = " << instanceCount << ", firstIndex = " << firstIndex << ", vertexOffset = " << vertexOffset << ", firstInstance = " << firstInstance << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdDrawIndirect(commandBuffer, buffer, offset, drawCount, stride);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDrawIndirect(commandBuffer = " << (void*)(commandBuffer) << ", buffer = " << buffer << ", offset = " << (void*)(offset) << ", drawCount = " << drawCount << ", stride = " << stride << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDrawIndirect(commandBuffer = address, buffer = " << buffer << ", offset = address, drawCount = " << drawCount << ", stride = " << stride << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDrawIndexedIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdDrawIndexedIndirect(commandBuffer, buffer, offset, drawCount, stride);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDrawIndexedIndirect(commandBuffer = " << (void*)(commandBuffer) << ", buffer = " << buffer << ", offset = " << (void*)(offset) << ", drawCount = " << drawCount << ", stride = " << stride << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDrawIndexedIndirect(commandBuffer = address, buffer = " << buffer << ", offset = address, drawCount = " << drawCount << ", stride = " << stride << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDispatch(VkCommandBuffer commandBuffer, uint32_t x, uint32_t y, uint32_t z)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdDispatch(commandBuffer, x, y, z);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDispatch(commandBuffer = " << (void*)(commandBuffer) << ", x = " << x << ", y = " << y << ", z = " << z << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDispatch(commandBuffer = address, x = " << x << ", y = " << y << ", z = " << z << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdDispatchIndirect(VkCommandBuffer commandBuffer, VkBuffer buffer, VkDeviceSize offset)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdDispatchIndirect(commandBuffer, buffer, offset);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDispatchIndirect(commandBuffer = " << (void*)(commandBuffer) << ", buffer = " << buffer << ", offset = " << (void*)(offset) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdDispatchIndirect(commandBuffer = address, buffer = " << buffer << ", offset = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBuffer(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferCopy* pRegions)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, regionCount, pRegions);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdCopyBuffer(commandBuffer = " << (void*)(commandBuffer) << ", srcBuffer = " << srcBuffer << ", dstBuffer = " << dstBuffer << ", regionCount = " << regionCount << ", pRegions = " << (void*)(pRegions) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdCopyBuffer(commandBuffer = address, srcBuffer = " << srcBuffer << ", dstBuffer = " << dstBuffer << ", regionCount = " << regionCount << ", pRegions = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pRegions) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < regionCount; i++) {
                tmp_str = vk_print_vkbuffercopy(&pRegions[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pRegions[" << i << "] (" << &pRegions[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pRegions[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageCopy* pRegions)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdCopyImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdCopyImage(commandBuffer = " << (void*)(commandBuffer) << ", srcImage = " << srcImage << ", srcImageLayout = " << string_VkImageLayout(srcImageLayout) << ", dstImage = " << dstImage << ", dstImageLayout = " << string_VkImageLayout(dstImageLayout) << ", regionCount = " << regionCount << ", pRegions = " << (void*)(pRegions) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdCopyImage(commandBuffer = address, srcImage = " << srcImage << ", srcImageLayout = " << string_VkImageLayout(srcImageLayout) << ", dstImage = " << dstImage << ", dstImageLayout = " << string_VkImageLayout(dstImageLayout) << ", regionCount = " << regionCount << ", pRegions = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pRegions) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < regionCount; i++) {
                tmp_str = vk_print_vkimagecopy(&pRegions[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pRegions[" << i << "] (" << &pRegions[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pRegions[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBlitImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageBlit* pRegions, VkFilter filter)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdBlitImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions, filter);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBlitImage(commandBuffer = " << (void*)(commandBuffer) << ", srcImage = " << srcImage << ", srcImageLayout = " << string_VkImageLayout(srcImageLayout) << ", dstImage = " << dstImage << ", dstImageLayout = " << string_VkImageLayout(dstImageLayout) << ", regionCount = " << regionCount << ", pRegions = " << (void*)(pRegions) << ", filter = " << string_VkFilter(filter) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBlitImage(commandBuffer = address, srcImage = " << srcImage << ", srcImageLayout = " << string_VkImageLayout(srcImageLayout) << ", dstImage = " << dstImage << ", dstImageLayout = " << string_VkImageLayout(dstImageLayout) << ", regionCount = " << regionCount << ", pRegions = address, filter = " << string_VkFilter(filter) << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pRegions) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < regionCount; i++) {
                tmp_str = vk_print_vkimageblit(&pRegions[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pRegions[" << i << "] (" << &pRegions[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pRegions[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyBufferToImage(VkCommandBuffer commandBuffer, VkBuffer srcBuffer, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdCopyBufferToImage(commandBuffer, srcBuffer, dstImage, dstImageLayout, regionCount, pRegions);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdCopyBufferToImage(commandBuffer = " << (void*)(commandBuffer) << ", srcBuffer = " << srcBuffer << ", dstImage = " << dstImage << ", dstImageLayout = " << string_VkImageLayout(dstImageLayout) << ", regionCount = " << regionCount << ", pRegions = " << (void*)(pRegions) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdCopyBufferToImage(commandBuffer = address, srcBuffer = " << srcBuffer << ", dstImage = " << dstImage << ", dstImageLayout = " << string_VkImageLayout(dstImageLayout) << ", regionCount = " << regionCount << ", pRegions = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pRegions) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < regionCount; i++) {
                tmp_str = vk_print_vkbufferimagecopy(&pRegions[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pRegions[" << i << "] (" << &pRegions[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pRegions[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyImageToBuffer(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkBuffer dstBuffer, uint32_t regionCount, const VkBufferImageCopy* pRegions)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdCopyImageToBuffer(commandBuffer, srcImage, srcImageLayout, dstBuffer, regionCount, pRegions);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdCopyImageToBuffer(commandBuffer = " << (void*)(commandBuffer) << ", srcImage = " << srcImage << ", srcImageLayout = " << string_VkImageLayout(srcImageLayout) << ", dstBuffer = " << dstBuffer << ", regionCount = " << regionCount << ", pRegions = " << (void*)(pRegions) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdCopyImageToBuffer(commandBuffer = address, srcImage = " << srcImage << ", srcImageLayout = " << string_VkImageLayout(srcImageLayout) << ", dstBuffer = " << dstBuffer << ", regionCount = " << regionCount << ", pRegions = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pRegions) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < regionCount; i++) {
                tmp_str = vk_print_vkbufferimagecopy(&pRegions[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pRegions[" << i << "] (" << &pRegions[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pRegions[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdUpdateBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize dataSize, const uint32_t* pData)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdUpdateBuffer(commandBuffer, dstBuffer, dstOffset, dataSize, pData);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdUpdateBuffer(commandBuffer = " << (void*)(commandBuffer) << ", dstBuffer = " << dstBuffer << ", dstOffset = " << (void*)(dstOffset) << ", dataSize = " << (void*)(dataSize) << ", pData = " << (void*)(pData) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdUpdateBuffer(commandBuffer = address, dstBuffer = " << dstBuffer << ", dstOffset = address, dataSize = address, pData = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdFillBuffer(VkCommandBuffer commandBuffer, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdFillBuffer(commandBuffer, dstBuffer, dstOffset, size, data);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdFillBuffer(commandBuffer = " << (void*)(commandBuffer) << ", dstBuffer = " << dstBuffer << ", dstOffset = " << (void*)(dstOffset) << ", size = " << (void*)(size) << ", data = " << data << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdFillBuffer(commandBuffer = address, dstBuffer = " << dstBuffer << ", dstOffset = address, size = address, data = " << data << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearColorImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearColorValue* pColor, uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdClearColorImage(commandBuffer, image, imageLayout, pColor, rangeCount, pRanges);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdClearColorImage(commandBuffer = " << (void*)(commandBuffer) << ", image = " << image << ", imageLayout = " << string_VkImageLayout(imageLayout) << ", pColor = " << (void*)(pColor) << ", rangeCount = " << rangeCount << ", pRanges = " << (void*)(pRanges) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdClearColorImage(commandBuffer = address, image = " << image << ", imageLayout = " << string_VkImageLayout(imageLayout) << ", pColor = address, rangeCount = " << rangeCount << ", pRanges = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pColor) {
            tmp_str = vk_print_vkclearcolorvalue(pColor, "    ");
            (*outputStream) << "   pColor (" << pColor << ")" << endl << tmp_str << endl;
        }
        uint32_t i;
        if (pRanges) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < rangeCount; i++) {
                tmp_str = vk_print_vkimagesubresourcerange(&pRanges[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pRanges[" << i << "] (" << &pRanges[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pRanges[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearDepthStencilImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout imageLayout, const VkClearDepthStencilValue* pDepthStencil, uint32_t rangeCount, const VkImageSubresourceRange* pRanges)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdClearDepthStencilImage(commandBuffer, image, imageLayout, pDepthStencil, rangeCount, pRanges);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdClearDepthStencilImage(commandBuffer = " << (void*)(commandBuffer) << ", image = " << image << ", imageLayout = " << string_VkImageLayout(imageLayout) << ", pDepthStencil = " << (void*)(pDepthStencil) << ", rangeCount = " << rangeCount << ", pRanges = " << (void*)(pRanges) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdClearDepthStencilImage(commandBuffer = address, image = " << image << ", imageLayout = " << string_VkImageLayout(imageLayout) << ", pDepthStencil = address, rangeCount = " << rangeCount << ", pRanges = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pDepthStencil) {
            tmp_str = vk_print_vkcleardepthstencilvalue(pDepthStencil, "    ");
            (*outputStream) << "   pDepthStencil (" << pDepthStencil << ")" << endl << tmp_str << endl;
        }
        uint32_t i;
        if (pRanges) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < rangeCount; i++) {
                tmp_str = vk_print_vkimagesubresourcerange(&pRanges[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pRanges[" << i << "] (" << &pRanges[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pRanges[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdClearAttachments(VkCommandBuffer commandBuffer, uint32_t attachmentCount, const VkClearAttachment* pAttachments, uint32_t rectCount, const VkClearRect* pRects)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdClearAttachments(commandBuffer, attachmentCount, pAttachments, rectCount, pRects);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdClearAttachments(commandBuffer = " << (void*)(commandBuffer) << ", attachmentCount = " << attachmentCount << ", pAttachments = " << (void*)(pAttachments) << ", rectCount = " << rectCount << ", pRects = " << (void*)(pRects) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdClearAttachments(commandBuffer = address, attachmentCount = " << attachmentCount << ", pAttachments = address, rectCount = " << rectCount << ", pRects = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pAttachments) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < attachmentCount; i++) {
                tmp_str = vk_print_vkclearattachment(&pAttachments[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pAttachments[" << i << "] (" << &pAttachments[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pAttachments[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
        if (pRects) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < rectCount; i++) {
                tmp_str = vk_print_vkclearrect(&pRects[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pRects[" << i << "] (" << &pRects[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pRects[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResolveImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImageLayout srcImageLayout, VkImage dstImage, VkImageLayout dstImageLayout, uint32_t regionCount, const VkImageResolve* pRegions)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdResolveImage(commandBuffer, srcImage, srcImageLayout, dstImage, dstImageLayout, regionCount, pRegions);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdResolveImage(commandBuffer = " << (void*)(commandBuffer) << ", srcImage = " << srcImage << ", srcImageLayout = " << string_VkImageLayout(srcImageLayout) << ", dstImage = " << dstImage << ", dstImageLayout = " << string_VkImageLayout(dstImageLayout) << ", regionCount = " << regionCount << ", pRegions = " << (void*)(pRegions) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdResolveImage(commandBuffer = address, srcImage = " << srcImage << ", srcImageLayout = " << string_VkImageLayout(srcImageLayout) << ", dstImage = " << dstImage << ", dstImageLayout = " << string_VkImageLayout(dstImageLayout) << ", regionCount = " << regionCount << ", pRegions = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pRegions) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < regionCount; i++) {
                tmp_str = vk_print_vkimageresolve(&pRegions[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pRegions[" << i << "] (" << &pRegions[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pRegions[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdSetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdSetEvent(commandBuffer, event, stageMask);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetEvent(commandBuffer = " << (void*)(commandBuffer) << ", event = " << event << ", stageMask = " << stageMask << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdSetEvent(commandBuffer = address, event = " << event << ", stageMask = " << stageMask << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResetEvent(VkCommandBuffer commandBuffer, VkEvent event, VkPipelineStageFlags stageMask)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdResetEvent(commandBuffer, event, stageMask);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdResetEvent(commandBuffer = " << (void*)(commandBuffer) << ", event = " << event << ", stageMask = " << stageMask << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdResetEvent(commandBuffer = address, event = " << event << ", stageMask = " << stageMask << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdWaitEvents(VkCommandBuffer commandBuffer, uint32_t eventCount, const VkEvent* pEvents, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdWaitEvents(commandBuffer, eventCount, pEvents, srcStageMask, dstStageMask, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdWaitEvents(commandBuffer = " << (void*)(commandBuffer) << ", eventCount = " << eventCount << ", pEvents = " << (void*)(pEvents) << ", srcStageMask = " << srcStageMask << ", dstStageMask = " << dstStageMask << ", memoryBarrierCount = " << memoryBarrierCount << ", pMemoryBarriers = " << (void*)(pMemoryBarriers) << ", bufferMemoryBarrierCount = " << bufferMemoryBarrierCount << ", pBufferMemoryBarriers = " << (void*)(pBufferMemoryBarriers) << ", imageMemoryBarrierCount = " << imageMemoryBarrierCount << ", pImageMemoryBarriers = " << (void*)(pImageMemoryBarriers) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdWaitEvents(commandBuffer = address, eventCount = " << eventCount << ", pEvents = address, srcStageMask = " << srcStageMask << ", dstStageMask = " << dstStageMask << ", memoryBarrierCount = " << memoryBarrierCount << ", pMemoryBarriers = address, bufferMemoryBarrierCount = " << bufferMemoryBarrierCount << ", pBufferMemoryBarriers = address, imageMemoryBarrierCount = " << imageMemoryBarrierCount << ", pImageMemoryBarriers = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pBufferMemoryBarriers) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < bufferMemoryBarrierCount; i++) {
                tmp_str = vk_print_vkbuffermemorybarrier(&pBufferMemoryBarriers[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pBufferMemoryBarriers[" << i << "] (" << &pBufferMemoryBarriers[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pBufferMemoryBarriers[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
        if (pEvents) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < eventCount; i++) {
                tmp_str = string_convert_helper(pEvents[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pEvents[" << i << "] (" << &pEvents[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pEvents[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
        if (pImageMemoryBarriers) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < imageMemoryBarrierCount; i++) {
                tmp_str = vk_print_vkimagememorybarrier(&pImageMemoryBarriers[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pImageMemoryBarriers[" << i << "] (" << &pImageMemoryBarriers[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pImageMemoryBarriers[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
        if (pMemoryBarriers) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < memoryBarrierCount; i++) {
                tmp_str = vk_print_vkmemorybarrier(&pMemoryBarriers[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pMemoryBarriers[" << i << "] (" << &pMemoryBarriers[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pMemoryBarriers[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPipelineBarrier(VkCommandBuffer commandBuffer, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, uint32_t memoryBarrierCount, const VkMemoryBarrier* pMemoryBarriers, uint32_t bufferMemoryBarrierCount, const VkBufferMemoryBarrier* pBufferMemoryBarriers, uint32_t imageMemoryBarrierCount, const VkImageMemoryBarrier* pImageMemoryBarriers)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, dependencyFlags, memoryBarrierCount, pMemoryBarriers, bufferMemoryBarrierCount, pBufferMemoryBarriers, imageMemoryBarrierCount, pImageMemoryBarriers);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdPipelineBarrier(commandBuffer = " << (void*)(commandBuffer) << ", srcStageMask = " << srcStageMask << ", dstStageMask = " << dstStageMask << ", dependencyFlags = " << dependencyFlags << ", memoryBarrierCount = " << memoryBarrierCount << ", pMemoryBarriers = " << (void*)(pMemoryBarriers) << ", bufferMemoryBarrierCount = " << bufferMemoryBarrierCount << ", pBufferMemoryBarriers = " << (void*)(pBufferMemoryBarriers) << ", imageMemoryBarrierCount = " << imageMemoryBarrierCount << ", pImageMemoryBarriers = " << (void*)(pImageMemoryBarriers) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdPipelineBarrier(commandBuffer = address, srcStageMask = " << srcStageMask << ", dstStageMask = " << dstStageMask << ", dependencyFlags = " << dependencyFlags << ", memoryBarrierCount = " << memoryBarrierCount << ", pMemoryBarriers = address, bufferMemoryBarrierCount = " << bufferMemoryBarrierCount << ", pBufferMemoryBarriers = address, imageMemoryBarrierCount = " << imageMemoryBarrierCount << ", pImageMemoryBarriers = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pImageMemoryBarriers) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < imageMemoryBarrierCount; i++) {
                tmp_str = vk_print_vkimagememorybarrier(&pImageMemoryBarriers[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pImageMemoryBarriers[" << i << "] (" << &pImageMemoryBarriers[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pImageMemoryBarriers[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
        if (pMemoryBarriers) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < memoryBarrierCount; i++) {
                tmp_str = vk_print_vkmemorybarrier(&pMemoryBarriers[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pMemoryBarriers[" << i << "] (" << &pMemoryBarriers[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pMemoryBarriers[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
        if (pBufferMemoryBarriers) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < bufferMemoryBarrierCount; i++) {
                tmp_str = vk_print_vkbuffermemorybarrier(&pBufferMemoryBarriers[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pBufferMemoryBarriers[" << i << "] (" << &pBufferMemoryBarriers[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pBufferMemoryBarriers[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBeginQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query, VkQueryControlFlags flags)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdBeginQuery(commandBuffer, queryPool, query, flags);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBeginQuery(commandBuffer = " << (void*)(commandBuffer) << ", queryPool = " << queryPool << ", query = " << query << ", flags = " << flags << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBeginQuery(commandBuffer = address, queryPool = " << queryPool << ", query = " << query << ", flags = " << flags << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndQuery(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t query)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdEndQuery(commandBuffer, queryPool, query);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdEndQuery(commandBuffer = " << (void*)(commandBuffer) << ", queryPool = " << queryPool << ", query = " << query << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdEndQuery(commandBuffer = address, queryPool = " << queryPool << ", query = " << query << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdResetQueryPool(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdResetQueryPool(commandBuffer, queryPool, firstQuery, queryCount);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdResetQueryPool(commandBuffer = " << (void*)(commandBuffer) << ", queryPool = " << queryPool << ", firstQuery = " << firstQuery << ", queryCount = " << queryCount << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdResetQueryPool(commandBuffer = address, queryPool = " << queryPool << ", firstQuery = " << firstQuery << ", queryCount = " << queryCount << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdWriteTimestamp(VkCommandBuffer commandBuffer, VkPipelineStageFlagBits pipelineStage, VkQueryPool queryPool, uint32_t query)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdWriteTimestamp(commandBuffer, pipelineStage, queryPool, query);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdWriteTimestamp(commandBuffer = " << (void*)(commandBuffer) << ", pipelineStage = " << string_VkPipelineStageFlagBits(pipelineStage) << ", queryPool = " << queryPool << ", query = " << query << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdWriteTimestamp(commandBuffer = address, pipelineStage = " << string_VkPipelineStageFlagBits(pipelineStage) << ", queryPool = " << queryPool << ", query = " << query << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdCopyQueryPoolResults(VkCommandBuffer commandBuffer, VkQueryPool queryPool, uint32_t firstQuery, uint32_t queryCount, VkBuffer dstBuffer, VkDeviceSize dstOffset, VkDeviceSize stride, VkQueryResultFlags flags)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdCopyQueryPoolResults(commandBuffer, queryPool, firstQuery, queryCount, dstBuffer, dstOffset, stride, flags);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdCopyQueryPoolResults(commandBuffer = " << (void*)(commandBuffer) << ", queryPool = " << queryPool << ", firstQuery = " << firstQuery << ", queryCount = " << queryCount << ", dstBuffer = " << dstBuffer << ", dstOffset = " << (void*)(dstOffset) << ", stride = " << (void*)(stride) << ", flags = " << flags << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdCopyQueryPoolResults(commandBuffer = address, queryPool = " << queryPool << ", firstQuery = " << firstQuery << ", queryCount = " << queryCount << ", dstBuffer = " << dstBuffer << ", dstOffset = address, stride = address, flags = " << flags << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdPushConstants(VkCommandBuffer commandBuffer, VkPipelineLayout layout, VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size, const void* pValues)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdPushConstants(commandBuffer, layout, stageFlags, offset, size, pValues);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdPushConstants(commandBuffer = " << (void*)(commandBuffer) << ", layout = " << layout << ", stageFlags = " << stageFlags << ", offset = " << offset << ", size = " << size << ", pValues = " << (void*)(pValues) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdPushConstants(commandBuffer = address, layout = " << layout << ", stageFlags = " << stageFlags << ", offset = " << offset << ", size = " << size << ", pValues = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdBeginRenderPass(VkCommandBuffer commandBuffer, const VkRenderPassBeginInfo* pRenderPassBegin, VkSubpassContents contents)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdBeginRenderPass(commandBuffer, pRenderPassBegin, contents);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBeginRenderPass(commandBuffer = " << (void*)(commandBuffer) << ", pRenderPassBegin = " << (void*)(pRenderPassBegin) << ", contents = " << string_VkSubpassContents(contents) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdBeginRenderPass(commandBuffer = address, pRenderPassBegin = address, contents = " << string_VkSubpassContents(contents) << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pRenderPassBegin) {
            tmp_str = vk_print_vkrenderpassbegininfo(pRenderPassBegin, "    ");
            (*outputStream) << "   pRenderPassBegin (" << pRenderPassBegin << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdNextSubpass(VkCommandBuffer commandBuffer, VkSubpassContents contents)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdNextSubpass(commandBuffer, contents);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdNextSubpass(commandBuffer = " << (void*)(commandBuffer) << ", contents = " << string_VkSubpassContents(contents) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdNextSubpass(commandBuffer = address, contents = " << string_VkSubpassContents(contents) << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdEndRenderPass(VkCommandBuffer commandBuffer)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdEndRenderPass(commandBuffer);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdEndRenderPass(commandBuffer = " << (void*)(commandBuffer) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdEndRenderPass(commandBuffer = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkCmdExecuteCommands(VkCommandBuffer commandBuffer, uint32_t commandBufferCount, const VkCommandBuffer* pCommandBuffers)
{
    using namespace StreamControl;
    device_dispatch_table(commandBuffer)->CmdExecuteCommands(commandBuffer, commandBufferCount, pCommandBuffers);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdExecuteCommands(commandBuffer = " << (void*)(commandBuffer) << ", commandBufferCount = " << commandBufferCount << ", pCommandBuffers = " << (void*)(pCommandBuffers) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCmdExecuteCommands(commandBuffer = address, commandBufferCount = " << commandBufferCount << ", pCommandBuffers = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pCommandBuffers) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < commandBufferCount; i++) {
                tmp_str = string_convert_helper(pCommandBuffers[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pCommandBuffers[" << i << "] (" << &pCommandBuffers[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pCommandBuffers[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySurfaceKHR(VkInstance instance, VkSurfaceKHR surface, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    instance_dispatch_table(instance)->DestroySurfaceKHR(instance, surface, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroySurfaceKHR(instance = " << (void*)(instance) << ", surface = " << surface << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroySurfaceKHR(instance = address, surface = " << surface << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface, VkBool32* pSupported)
{
    using namespace StreamControl;
    VkResult result = instance_dispatch_table(physicalDevice)->GetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, pSupported);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice = " << (void*)(physicalDevice) << ", queueFamilyIndex = " << queueFamilyIndex << ", surface = " << surface << ", pSupported = " << pSupported << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice = address, queueFamilyIndex = " << queueFamilyIndex << ", surface = " << surface << ", pSupported = " << pSupported << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR* pSurfaceCapabilities)
{
    using namespace StreamControl;
    VkResult result = instance_dispatch_table(physicalDevice)->GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, pSurfaceCapabilities);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice = " << (void*)(physicalDevice) << ", surface = " << surface << ", pSurfaceCapabilities = " << (void*)(pSurfaceCapabilities) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice = address, surface = " << surface << ", pSurfaceCapabilities = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pSurfaceCapabilities) {
            tmp_str = vk_print_vksurfacecapabilitieskhr(pSurfaceCapabilities, "    ");
            (*outputStream) << "   pSurfaceCapabilities (" << pSurfaceCapabilities << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pSurfaceFormatCount, VkSurfaceFormatKHR* pSurfaceFormats)
{
    using namespace StreamControl;
    VkResult result = instance_dispatch_table(physicalDevice)->GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, pSurfaceFormatCount, pSurfaceFormats);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice = " << (void*)(physicalDevice) << ", surface = " << surface << ", pSurfaceFormatCount = " << *(pSurfaceFormatCount) << ", pSurfaceFormats = " << (void*)(pSurfaceFormats) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice = address, surface = " << surface << ", pSurfaceFormatCount = " << *(pSurfaceFormatCount) << ", pSurfaceFormats = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pSurfaceFormats) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < *pSurfaceFormatCount; i++) {
                tmp_str = vk_print_vksurfaceformatkhr(&pSurfaceFormats[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pSurfaceFormats[" << i << "] (" << &pSurfaceFormats[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pSurfaceFormats[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, uint32_t* pPresentModeCount, VkPresentModeKHR* pPresentModes)
{
    using namespace StreamControl;
    VkResult result = instance_dispatch_table(physicalDevice)->GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, pPresentModeCount, pPresentModes);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice = " << (void*)(physicalDevice) << ", surface = " << surface << ", pPresentModeCount = " << *(pPresentModeCount) << ", pPresentModes = " << (void*)(pPresentModes) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice = address, surface = " << surface << ", pPresentModeCount = " << *(pPresentModeCount) << ", pPresentModes = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pPresentModes) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < *pPresentModeCount; i++) {
                tmp_str = string_convert_helper(pPresentModes[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pPresentModes[" << i << "] (" << &pPresentModes[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pPresentModes[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->CreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateSwapchainKHR(device = " << (void*)(device) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pSwapchain = " << pSwapchain << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateSwapchainKHR(device = address, pCreateInfo = address, pAllocator = address, pSwapchain = " << pSwapchain << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkswapchaincreateinfokhr(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR void VKAPI_CALL vkDestroySwapchainKHR(VkDevice device, VkSwapchainKHR swapchain, const VkAllocationCallbacks* pAllocator)
{
    using namespace StreamControl;
    device_dispatch_table(device)->DestroySwapchainKHR(device, swapchain, pAllocator);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroySwapchainKHR(device = " << (void*)(device) << ", swapchain = " << swapchain << ", pAllocator = " << (void*)(pAllocator) << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkDestroySwapchainKHR(device = address, swapchain = " << swapchain << ", pAllocator = address)\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t* pSwapchainImageCount, VkImage* pSwapchainImages)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->GetSwapchainImagesKHR(device, swapchain, pSwapchainImageCount, pSwapchainImages);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetSwapchainImagesKHR(device = " << (void*)(device) << ", swapchain = " << swapchain << ", pSwapchainImageCount = " << *(pSwapchainImageCount) << ", pSwapchainImages = " << pSwapchainImages << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetSwapchainImagesKHR(device = address, swapchain = " << swapchain << ", pSwapchainImageCount = " << *(pSwapchainImageCount) << ", pSwapchainImages = " << pSwapchainImages << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
        uint32_t i;
        if (pSwapchainImages) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1144
            for (i = 0; i < *pSwapchainImageCount; i++) {
                tmp_str = string_convert_helper(pSwapchainImages[i], "    ");
                if (StreamControl::writeAddress == true) {
                    (*outputStream) << "   pSwapchainImages[" << i << "] (" << &pSwapchainImages[i] << ")" << endl << tmp_str << endl;
                } else {
                    (*outputStream) << "   pSwapchainImages[" << i << "] (address)" << endl << "    address" << endl;
                }
            }
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(device)->AcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkAcquireNextImageKHR(device = " << (void*)(device) << ", swapchain = " << swapchain << ", timeout = " << timeout << ", semaphore = " << semaphore << ", fence = " << fence << ", pImageIndex = " << *(pImageIndex) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkAcquireNextImageKHR(device = address, swapchain = " << swapchain << ", timeout = " << timeout << ", semaphore = " << semaphore << ", fence = " << fence << ", pImageIndex = " << *(pImageIndex) << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
{
    using namespace StreamControl;
    VkResult result = device_dispatch_table(queue)->QueuePresentKHR(queue, pPresentInfo);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkQueuePresentKHR(queue = " << (void*)(queue) << ", pPresentInfo = " << (void*)(pPresentInfo) << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkQueuePresentKHR(queue = address, pPresentInfo = address) = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pPresentInfo) {
            tmp_str = vk_print_vkpresentinfokhr(pPresentInfo, "    ");
            (*outputStream) << "   pPresentInfo (" << pPresentInfo << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

#ifdef VK_USE_PLATFORM_WIN32_KHR

VK_LAYER_EXPORT VKAPI_ATTR VkResult VKAPI_CALL vkCreateWin32SurfaceKHR(VkInstance instance, const VkWin32SurfaceCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface)
{
    using namespace StreamControl;
    VkResult result = instance_dispatch_table(instance)->CreateWin32SurfaceKHR(instance, pCreateInfo, pAllocator, pSurface);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateWin32SurfaceKHR(instance = " << (void*)(instance) << ", pCreateInfo = " << (void*)(pCreateInfo) << ", pAllocator = " << (void*)(pAllocator) << ", pSurface = " << pSurface << ") = " << string_VkResult((VkResult)result) << endl;
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkCreateWin32SurfaceKHR(instance = address, pCreateInfo = address, pAllocator = address, pSurface = " << pSurface << ") = " << string_VkResult((VkResult)result) << endl;
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    if (g_ApiDumpDetailed) {
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1112
        string tmp_str;
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pCreateInfo) {
            tmp_str = vk_print_vkwin32surfacecreateinfokhr(pCreateInfo, "    ");
            (*outputStream) << "   pCreateInfo (" << pCreateInfo << ")" << endl << tmp_str << endl;
        }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1121
        if (pAllocator) {
            tmp_str = vk_print_vkallocationcallbacks(pAllocator, "    ");
            (*outputStream) << "   pAllocator (" << pAllocator << ")" << endl << tmp_str << endl;
        }
    }
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

#endif  // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR

VK_LAYER_EXPORT VKAPI_ATTR VkBool32 VKAPI_CALL vkGetPhysicalDeviceWin32PresentationSupportKHR(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
    using namespace StreamControl;
    VkBool32 result = instance_dispatch_table(physicalDevice)->GetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
    loader_platform_thread_lock_mutex(&printLock);
    // CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1053
    if (StreamControl::writeAddress == true) {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice = " << (void*)(physicalDevice) << ", queueFamilyIndex = " << queueFamilyIndex << ")\n";
    }
    else {
        (*outputStream) << "t{" << getTIDIndex() << "} vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice = address, queueFamilyIndex = " << queueFamilyIndex << ")\n";
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #1105
    loader_platform_thread_unlock_mutex(&printLock);
    return result;
}

#endif  // VK_USE_PLATFORM_WIN32_KHR

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #424
static inline PFN_vkVoidFunction layer_intercept_proc(const char *name)
{
    if (!name || name[0] != 'v' || name[1] != 'k')
        return NULL;

    name += 2;
    if (!strcmp(name, "CreateInstance"))
        return (PFN_vkVoidFunction) vkCreateInstance;
    if (!strcmp(name, "DestroyInstance"))
        return (PFN_vkVoidFunction) vkDestroyInstance;
    if (!strcmp(name, "EnumeratePhysicalDevices"))
        return (PFN_vkVoidFunction) vkEnumeratePhysicalDevices;
    if (!strcmp(name, "GetPhysicalDeviceFeatures"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceFeatures;
    if (!strcmp(name, "GetPhysicalDeviceFormatProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceFormatProperties;
    if (!strcmp(name, "GetPhysicalDeviceImageFormatProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceImageFormatProperties;
    if (!strcmp(name, "GetPhysicalDeviceProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceProperties;
    if (!strcmp(name, "GetPhysicalDeviceQueueFamilyProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceQueueFamilyProperties;
    if (!strcmp(name, "GetPhysicalDeviceMemoryProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceMemoryProperties;
    if (!strcmp(name, "CreateDevice"))
        return (PFN_vkVoidFunction) vkCreateDevice;
    if (!strcmp(name, "DestroyDevice"))
        return (PFN_vkVoidFunction) vkDestroyDevice;
    if (!strcmp(name, "EnumerateInstanceExtensionProperties"))
        return (PFN_vkVoidFunction) vkEnumerateInstanceExtensionProperties;
    if (!strcmp(name, "EnumerateInstanceLayerProperties"))
        return (PFN_vkVoidFunction) vkEnumerateInstanceLayerProperties;
    if (!strcmp(name, "EnumerateDeviceLayerProperties"))
        return (PFN_vkVoidFunction) vkEnumerateDeviceLayerProperties;
    if (!strcmp(name, "GetDeviceQueue"))
        return (PFN_vkVoidFunction) vkGetDeviceQueue;
    if (!strcmp(name, "QueueSubmit"))
        return (PFN_vkVoidFunction) vkQueueSubmit;
    if (!strcmp(name, "QueueWaitIdle"))
        return (PFN_vkVoidFunction) vkQueueWaitIdle;
    if (!strcmp(name, "DeviceWaitIdle"))
        return (PFN_vkVoidFunction) vkDeviceWaitIdle;
    if (!strcmp(name, "AllocateMemory"))
        return (PFN_vkVoidFunction) vkAllocateMemory;
    if (!strcmp(name, "FreeMemory"))
        return (PFN_vkVoidFunction) vkFreeMemory;
    if (!strcmp(name, "MapMemory"))
        return (PFN_vkVoidFunction) vkMapMemory;
    if (!strcmp(name, "UnmapMemory"))
        return (PFN_vkVoidFunction) vkUnmapMemory;
    if (!strcmp(name, "FlushMappedMemoryRanges"))
        return (PFN_vkVoidFunction) vkFlushMappedMemoryRanges;
    if (!strcmp(name, "InvalidateMappedMemoryRanges"))
        return (PFN_vkVoidFunction) vkInvalidateMappedMemoryRanges;
    if (!strcmp(name, "GetDeviceMemoryCommitment"))
        return (PFN_vkVoidFunction) vkGetDeviceMemoryCommitment;
    if (!strcmp(name, "BindBufferMemory"))
        return (PFN_vkVoidFunction) vkBindBufferMemory;
    if (!strcmp(name, "BindImageMemory"))
        return (PFN_vkVoidFunction) vkBindImageMemory;
    if (!strcmp(name, "GetBufferMemoryRequirements"))
        return (PFN_vkVoidFunction) vkGetBufferMemoryRequirements;
    if (!strcmp(name, "GetImageMemoryRequirements"))
        return (PFN_vkVoidFunction) vkGetImageMemoryRequirements;
    if (!strcmp(name, "GetImageSparseMemoryRequirements"))
        return (PFN_vkVoidFunction) vkGetImageSparseMemoryRequirements;
    if (!strcmp(name, "GetPhysicalDeviceSparseImageFormatProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceSparseImageFormatProperties;
    if (!strcmp(name, "QueueBindSparse"))
        return (PFN_vkVoidFunction) vkQueueBindSparse;
    if (!strcmp(name, "CreateFence"))
        return (PFN_vkVoidFunction) vkCreateFence;
    if (!strcmp(name, "DestroyFence"))
        return (PFN_vkVoidFunction) vkDestroyFence;
    if (!strcmp(name, "ResetFences"))
        return (PFN_vkVoidFunction) vkResetFences;
    if (!strcmp(name, "GetFenceStatus"))
        return (PFN_vkVoidFunction) vkGetFenceStatus;
    if (!strcmp(name, "WaitForFences"))
        return (PFN_vkVoidFunction) vkWaitForFences;
    if (!strcmp(name, "CreateSemaphore"))
        return (PFN_vkVoidFunction) vkCreateSemaphore;
    if (!strcmp(name, "DestroySemaphore"))
        return (PFN_vkVoidFunction) vkDestroySemaphore;
    if (!strcmp(name, "CreateEvent"))
        return (PFN_vkVoidFunction) vkCreateEvent;
    if (!strcmp(name, "DestroyEvent"))
        return (PFN_vkVoidFunction) vkDestroyEvent;
    if (!strcmp(name, "GetEventStatus"))
        return (PFN_vkVoidFunction) vkGetEventStatus;
    if (!strcmp(name, "SetEvent"))
        return (PFN_vkVoidFunction) vkSetEvent;
    if (!strcmp(name, "ResetEvent"))
        return (PFN_vkVoidFunction) vkResetEvent;
    if (!strcmp(name, "CreateQueryPool"))
        return (PFN_vkVoidFunction) vkCreateQueryPool;
    if (!strcmp(name, "DestroyQueryPool"))
        return (PFN_vkVoidFunction) vkDestroyQueryPool;
    if (!strcmp(name, "GetQueryPoolResults"))
        return (PFN_vkVoidFunction) vkGetQueryPoolResults;
    if (!strcmp(name, "CreateBuffer"))
        return (PFN_vkVoidFunction) vkCreateBuffer;
    if (!strcmp(name, "DestroyBuffer"))
        return (PFN_vkVoidFunction) vkDestroyBuffer;
    if (!strcmp(name, "CreateBufferView"))
        return (PFN_vkVoidFunction) vkCreateBufferView;
    if (!strcmp(name, "DestroyBufferView"))
        return (PFN_vkVoidFunction) vkDestroyBufferView;
    if (!strcmp(name, "CreateImage"))
        return (PFN_vkVoidFunction) vkCreateImage;
    if (!strcmp(name, "DestroyImage"))
        return (PFN_vkVoidFunction) vkDestroyImage;
    if (!strcmp(name, "GetImageSubresourceLayout"))
        return (PFN_vkVoidFunction) vkGetImageSubresourceLayout;
    if (!strcmp(name, "CreateImageView"))
        return (PFN_vkVoidFunction) vkCreateImageView;
    if (!strcmp(name, "DestroyImageView"))
        return (PFN_vkVoidFunction) vkDestroyImageView;
    if (!strcmp(name, "CreateShaderModule"))
        return (PFN_vkVoidFunction) vkCreateShaderModule;
    if (!strcmp(name, "DestroyShaderModule"))
        return (PFN_vkVoidFunction) vkDestroyShaderModule;
    if (!strcmp(name, "CreatePipelineCache"))
        return (PFN_vkVoidFunction) vkCreatePipelineCache;
    if (!strcmp(name, "DestroyPipelineCache"))
        return (PFN_vkVoidFunction) vkDestroyPipelineCache;
    if (!strcmp(name, "GetPipelineCacheData"))
        return (PFN_vkVoidFunction) vkGetPipelineCacheData;
    if (!strcmp(name, "MergePipelineCaches"))
        return (PFN_vkVoidFunction) vkMergePipelineCaches;
    if (!strcmp(name, "CreateGraphicsPipelines"))
        return (PFN_vkVoidFunction) vkCreateGraphicsPipelines;
    if (!strcmp(name, "CreateComputePipelines"))
        return (PFN_vkVoidFunction) vkCreateComputePipelines;
    if (!strcmp(name, "DestroyPipeline"))
        return (PFN_vkVoidFunction) vkDestroyPipeline;
    if (!strcmp(name, "CreatePipelineLayout"))
        return (PFN_vkVoidFunction) vkCreatePipelineLayout;
    if (!strcmp(name, "DestroyPipelineLayout"))
        return (PFN_vkVoidFunction) vkDestroyPipelineLayout;
    if (!strcmp(name, "CreateSampler"))
        return (PFN_vkVoidFunction) vkCreateSampler;
    if (!strcmp(name, "DestroySampler"))
        return (PFN_vkVoidFunction) vkDestroySampler;
    if (!strcmp(name, "CreateDescriptorSetLayout"))
        return (PFN_vkVoidFunction) vkCreateDescriptorSetLayout;
    if (!strcmp(name, "DestroyDescriptorSetLayout"))
        return (PFN_vkVoidFunction) vkDestroyDescriptorSetLayout;
    if (!strcmp(name, "CreateDescriptorPool"))
        return (PFN_vkVoidFunction) vkCreateDescriptorPool;
    if (!strcmp(name, "DestroyDescriptorPool"))
        return (PFN_vkVoidFunction) vkDestroyDescriptorPool;
    if (!strcmp(name, "ResetDescriptorPool"))
        return (PFN_vkVoidFunction) vkResetDescriptorPool;
    if (!strcmp(name, "AllocateDescriptorSets"))
        return (PFN_vkVoidFunction) vkAllocateDescriptorSets;
    if (!strcmp(name, "FreeDescriptorSets"))
        return (PFN_vkVoidFunction) vkFreeDescriptorSets;
    if (!strcmp(name, "UpdateDescriptorSets"))
        return (PFN_vkVoidFunction) vkUpdateDescriptorSets;
    if (!strcmp(name, "CreateFramebuffer"))
        return (PFN_vkVoidFunction) vkCreateFramebuffer;
    if (!strcmp(name, "DestroyFramebuffer"))
        return (PFN_vkVoidFunction) vkDestroyFramebuffer;
    if (!strcmp(name, "CreateRenderPass"))
        return (PFN_vkVoidFunction) vkCreateRenderPass;
    if (!strcmp(name, "DestroyRenderPass"))
        return (PFN_vkVoidFunction) vkDestroyRenderPass;
    if (!strcmp(name, "GetRenderAreaGranularity"))
        return (PFN_vkVoidFunction) vkGetRenderAreaGranularity;
    if (!strcmp(name, "CreateCommandPool"))
        return (PFN_vkVoidFunction) vkCreateCommandPool;
    if (!strcmp(name, "DestroyCommandPool"))
        return (PFN_vkVoidFunction) vkDestroyCommandPool;
    if (!strcmp(name, "ResetCommandPool"))
        return (PFN_vkVoidFunction) vkResetCommandPool;
    if (!strcmp(name, "AllocateCommandBuffers"))
        return (PFN_vkVoidFunction) vkAllocateCommandBuffers;
    if (!strcmp(name, "FreeCommandBuffers"))
        return (PFN_vkVoidFunction) vkFreeCommandBuffers;
    if (!strcmp(name, "BeginCommandBuffer"))
        return (PFN_vkVoidFunction) vkBeginCommandBuffer;
    if (!strcmp(name, "EndCommandBuffer"))
        return (PFN_vkVoidFunction) vkEndCommandBuffer;
    if (!strcmp(name, "ResetCommandBuffer"))
        return (PFN_vkVoidFunction) vkResetCommandBuffer;
    if (!strcmp(name, "CmdBindPipeline"))
        return (PFN_vkVoidFunction) vkCmdBindPipeline;
    if (!strcmp(name, "CmdSetViewport"))
        return (PFN_vkVoidFunction) vkCmdSetViewport;
    if (!strcmp(name, "CmdSetScissor"))
        return (PFN_vkVoidFunction) vkCmdSetScissor;
    if (!strcmp(name, "CmdSetLineWidth"))
        return (PFN_vkVoidFunction) vkCmdSetLineWidth;
    if (!strcmp(name, "CmdSetDepthBias"))
        return (PFN_vkVoidFunction) vkCmdSetDepthBias;
    if (!strcmp(name, "CmdSetBlendConstants"))
        return (PFN_vkVoidFunction) vkCmdSetBlendConstants;
    if (!strcmp(name, "CmdSetDepthBounds"))
        return (PFN_vkVoidFunction) vkCmdSetDepthBounds;
    if (!strcmp(name, "CmdSetStencilCompareMask"))
        return (PFN_vkVoidFunction) vkCmdSetStencilCompareMask;
    if (!strcmp(name, "CmdSetStencilWriteMask"))
        return (PFN_vkVoidFunction) vkCmdSetStencilWriteMask;
    if (!strcmp(name, "CmdSetStencilReference"))
        return (PFN_vkVoidFunction) vkCmdSetStencilReference;
    if (!strcmp(name, "CmdBindDescriptorSets"))
        return (PFN_vkVoidFunction) vkCmdBindDescriptorSets;
    if (!strcmp(name, "CmdBindIndexBuffer"))
        return (PFN_vkVoidFunction) vkCmdBindIndexBuffer;
    if (!strcmp(name, "CmdBindVertexBuffers"))
        return (PFN_vkVoidFunction) vkCmdBindVertexBuffers;
    if (!strcmp(name, "CmdDraw"))
        return (PFN_vkVoidFunction) vkCmdDraw;
    if (!strcmp(name, "CmdDrawIndexed"))
        return (PFN_vkVoidFunction) vkCmdDrawIndexed;
    if (!strcmp(name, "CmdDrawIndirect"))
        return (PFN_vkVoidFunction) vkCmdDrawIndirect;
    if (!strcmp(name, "CmdDrawIndexedIndirect"))
        return (PFN_vkVoidFunction) vkCmdDrawIndexedIndirect;
    if (!strcmp(name, "CmdDispatch"))
        return (PFN_vkVoidFunction) vkCmdDispatch;
    if (!strcmp(name, "CmdDispatchIndirect"))
        return (PFN_vkVoidFunction) vkCmdDispatchIndirect;
    if (!strcmp(name, "CmdCopyBuffer"))
        return (PFN_vkVoidFunction) vkCmdCopyBuffer;
    if (!strcmp(name, "CmdCopyImage"))
        return (PFN_vkVoidFunction) vkCmdCopyImage;
    if (!strcmp(name, "CmdBlitImage"))
        return (PFN_vkVoidFunction) vkCmdBlitImage;
    if (!strcmp(name, "CmdCopyBufferToImage"))
        return (PFN_vkVoidFunction) vkCmdCopyBufferToImage;
    if (!strcmp(name, "CmdCopyImageToBuffer"))
        return (PFN_vkVoidFunction) vkCmdCopyImageToBuffer;
    if (!strcmp(name, "CmdUpdateBuffer"))
        return (PFN_vkVoidFunction) vkCmdUpdateBuffer;
    if (!strcmp(name, "CmdFillBuffer"))
        return (PFN_vkVoidFunction) vkCmdFillBuffer;
    if (!strcmp(name, "CmdClearColorImage"))
        return (PFN_vkVoidFunction) vkCmdClearColorImage;
    if (!strcmp(name, "CmdClearDepthStencilImage"))
        return (PFN_vkVoidFunction) vkCmdClearDepthStencilImage;
    if (!strcmp(name, "CmdClearAttachments"))
        return (PFN_vkVoidFunction) vkCmdClearAttachments;
    if (!strcmp(name, "CmdResolveImage"))
        return (PFN_vkVoidFunction) vkCmdResolveImage;
    if (!strcmp(name, "CmdSetEvent"))
        return (PFN_vkVoidFunction) vkCmdSetEvent;
    if (!strcmp(name, "CmdResetEvent"))
        return (PFN_vkVoidFunction) vkCmdResetEvent;
    if (!strcmp(name, "CmdWaitEvents"))
        return (PFN_vkVoidFunction) vkCmdWaitEvents;
    if (!strcmp(name, "CmdPipelineBarrier"))
        return (PFN_vkVoidFunction) vkCmdPipelineBarrier;
    if (!strcmp(name, "CmdBeginQuery"))
        return (PFN_vkVoidFunction) vkCmdBeginQuery;
    if (!strcmp(name, "CmdEndQuery"))
        return (PFN_vkVoidFunction) vkCmdEndQuery;
    if (!strcmp(name, "CmdResetQueryPool"))
        return (PFN_vkVoidFunction) vkCmdResetQueryPool;
    if (!strcmp(name, "CmdWriteTimestamp"))
        return (PFN_vkVoidFunction) vkCmdWriteTimestamp;
    if (!strcmp(name, "CmdCopyQueryPoolResults"))
        return (PFN_vkVoidFunction) vkCmdCopyQueryPoolResults;
    if (!strcmp(name, "CmdPushConstants"))
        return (PFN_vkVoidFunction) vkCmdPushConstants;
    if (!strcmp(name, "CmdBeginRenderPass"))
        return (PFN_vkVoidFunction) vkCmdBeginRenderPass;
    if (!strcmp(name, "CmdNextSubpass"))
        return (PFN_vkVoidFunction) vkCmdNextSubpass;
    if (!strcmp(name, "CmdEndRenderPass"))
        return (PFN_vkVoidFunction) vkCmdEndRenderPass;
    if (!strcmp(name, "CmdExecuteCommands"))
        return (PFN_vkVoidFunction) vkCmdExecuteCommands;

    return NULL;
}
static inline PFN_vkVoidFunction layer_intercept_instance_proc(const char *name)
{
    if (!name || name[0] != 'v' || name[1] != 'k')
        return NULL;

    name += 2;
    if (!strcmp(name, "DestroyInstance"))
        return (PFN_vkVoidFunction) vkDestroyInstance;
    if (!strcmp(name, "EnumeratePhysicalDevices"))
        return (PFN_vkVoidFunction) vkEnumeratePhysicalDevices;
    if (!strcmp(name, "GetPhysicalDeviceFeatures"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceFeatures;
    if (!strcmp(name, "GetPhysicalDeviceFormatProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceFormatProperties;
    if (!strcmp(name, "GetPhysicalDeviceImageFormatProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceImageFormatProperties;
    if (!strcmp(name, "GetPhysicalDeviceProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceProperties;
    if (!strcmp(name, "GetPhysicalDeviceQueueFamilyProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceQueueFamilyProperties;
    if (!strcmp(name, "GetPhysicalDeviceMemoryProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceMemoryProperties;
    if (!strcmp(name, "EnumerateInstanceExtensionProperties"))
        return (PFN_vkVoidFunction) vkEnumerateInstanceExtensionProperties;
    if (!strcmp(name, "EnumerateInstanceLayerProperties"))
        return (PFN_vkVoidFunction) vkEnumerateInstanceLayerProperties;
    if (!strcmp(name, "EnumerateDeviceLayerProperties"))
        return (PFN_vkVoidFunction) vkEnumerateDeviceLayerProperties;
    if (!strcmp(name, "GetPhysicalDeviceSparseImageFormatProperties"))
        return (PFN_vkVoidFunction) vkGetPhysicalDeviceSparseImageFormatProperties;

    return NULL;
}

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #558
VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetDeviceProcAddr(VkDevice device, const char* funcName)
{
    PFN_vkVoidFunction addr;


    loader_platform_thread_once(&initOnce, initapi_dump);

    if (!strcmp("vkGetDeviceProcAddr", funcName)) {
        return (PFN_vkVoidFunction) vkGetDeviceProcAddr;
    }

    addr = layer_intercept_proc(funcName);
    if (addr)
        return addr;
    if (device == VK_NULL_HANDLE) {
        return NULL;
    }


    VkLayerDispatchTable *pDisp =  device_dispatch_table(device);
    if (deviceExtMap.size() != 0 && deviceExtMap[pDisp].wsi_enabled)
    {
        if (!strcmp("vkCreateSwapchainKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkCreateSwapchainKHR);
        if (!strcmp("vkDestroySwapchainKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkDestroySwapchainKHR);
        if (!strcmp("vkGetSwapchainImagesKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkGetSwapchainImagesKHR);
        if (!strcmp("vkAcquireNextImageKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkAcquireNextImageKHR);
        if (!strcmp("vkQueuePresentKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkQueuePresentKHR);
    }
// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #596
    {
        if (pDisp->GetDeviceProcAddr == NULL)
            return NULL;
        return pDisp->GetDeviceProcAddr(device, funcName);
    }
}

// CODEGEN : file C:/releasebuild/VulkanTools/vk-vtlayer-generate.py line #603
VK_LAYER_EXPORT VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vkGetInstanceProcAddr(VkInstance instance, const char* funcName)
{
    PFN_vkVoidFunction addr;
    if (!strcmp(funcName, "vkGetInstanceProcAddr"))
        return (PFN_vkVoidFunction) vkGetInstanceProcAddr;
    if (!strcmp(funcName, "vkCreateInstance"))
        return (PFN_vkVoidFunction) vkCreateInstance;
    if (!strcmp(funcName, "vkCreateDevice"))
        return (PFN_vkVoidFunction) vkCreateDevice;

    loader_platform_thread_once(&initOnce, initapi_dump);

    addr = layer_intercept_instance_proc(funcName);
    if (addr)
        return addr;
    if (instance == VK_NULL_HANDLE) {
        return NULL;
    }


    VkLayerInstanceDispatchTable* pTable = instance_dispatch_table(instance);

    if (instanceExtMap.size() != 0 && instanceExtMap[pTable].wsi_enabled)
    {
        if (!strcmp("vkDestroySurfaceKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkDestroySurfaceKHR);
        if (!strcmp("vkGetPhysicalDeviceSurfaceSupportKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfaceSupportKHR);
        if (!strcmp("vkGetPhysicalDeviceSurfaceCapabilitiesKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
        if (!strcmp("vkGetPhysicalDeviceSurfaceFormatsKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfaceFormatsKHR);
        if (!strcmp("vkGetPhysicalDeviceSurfacePresentModesKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceSurfacePresentModesKHR);
#ifdef VK_USE_PLATFORM_WIN32_KHR
        if (!strcmp("vkCreateWin32SurfaceKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkCreateWin32SurfaceKHR);
#endif  // VK_USE_PLATFORM_WIN32_KHR
#ifdef VK_USE_PLATFORM_WIN32_KHR
        if (!strcmp("vkGetPhysicalDeviceWin32PresentationSupportKHR", funcName))
            return reinterpret_cast<PFN_vkVoidFunction>(vkGetPhysicalDeviceWin32PresentationSupportKHR);
#endif  // VK_USE_PLATFORM_WIN32_KHR
    }

    if (pTable->GetInstanceProcAddr == NULL)
        return NULL;
    return pTable->GetInstanceProcAddr(instance, funcName);
}

