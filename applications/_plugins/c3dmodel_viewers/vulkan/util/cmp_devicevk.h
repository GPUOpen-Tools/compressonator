//=====================================================================
// Copyright 2018 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once

#include <vulkan/vulkan.h>

class CMP_DeviceVK
{
  public:
    CMP_DeviceVK();
      ~CMP_DeviceVK();
    void OnCreate(VkInstance instance, void* hWnd);
    void OnDestroy();
    VkDevice GetDevice()
    {
        return m_device;
    }
    VkQueue GetGraphicsQueue() {
        return graphics_queue;
    }
    uint32_t GetGraphicsQueueFamilyIndex() {
        return present_queue_family_index;
    }
    VkQueue GetPresentQueue() {
        return present_queue;
    }
    uint32_t GetPresentQueueFamilyIndex() {
        return graphics_queue_family_index;
    }
    VkPhysicalDevice GetPhysicalDevice() {
        return m_physicaldevice;
    }
    VkSurfaceKHR GetSurface() {
        return m_surface;
    }
    VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() {
        return m_memoryProperties;
    }
  private:
    VkDevice m_device;
    VkPhysicalDevice m_physicaldevice;
    VkPhysicalDeviceMemoryProperties m_memoryProperties;
    VkPhysicalDeviceProperties m_deviceProperties;
    VkSurfaceKHR m_surface;

    VkQueue present_queue;
    uint32_t present_queue_family_index;
    VkQueue graphics_queue;
    uint32_t graphics_queue_family_index;
};

bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties &memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
