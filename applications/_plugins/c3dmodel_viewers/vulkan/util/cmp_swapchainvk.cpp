// AMD AMDUtils code
//
// Copyright(c) 2017 Advanced Micro Devices, Inc.All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//  Portions
//  Copyright(C) 2015 - 2016 Valve Corporation
//  Copyright(C) 2015 - 2016 LunarG, Inc.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.


#include "cmp_devicevk.h"
#include "cmp_swapchainvk.h"


#include <cassert>

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void SwapChainVK::OnCreate(CMP_DeviceVK* pDevice, std::uint32_t numberBackBuffers, std::uint32_t dwWidth, std::uint32_t dwHeight, void* hWnd)
{
    VkResult res;

    m_pDevice = pDevice;

    m_presentQueue = pDevice->GetPresentQueue();

    VkDevice device = pDevice->GetDevice();
    VkPhysicalDevice physicaldevice = pDevice->GetPhysicalDevice();
    VkSurfaceKHR surface = pDevice->GetSurface();

    m_backBufferCount = numberBackBuffers;

    // Get the list of VkFormats that are supported:
    //
    uint32_t formatCount;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, surface, &formatCount, NULL);
    assert(res == VK_SUCCESS);
    VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicaldevice, surface, &formatCount, surfFormats);
    assert(res == VK_SUCCESS);
    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {
        m_format = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
        assert(formatCount >= 1);
        m_format = surfFormats[0].format;
    }
    free(surfFormats);

    VkSurfaceCapabilitiesKHR surfCapabilities;
    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicaldevice, surface, &surfCapabilities);
    assert(res == VK_SUCCESS);

    uint32_t presentModeCount;
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, surface, &presentModeCount, NULL);
    assert(res == VK_SUCCESS);
    VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
    assert(presentModes);
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicaldevice, surface, &presentModeCount, presentModes);
    assert(res == VK_SUCCESS);

    VkExtent2D swapchainExtent;
    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchainExtent.width = dwWidth;
        swapchainExtent.height = dwHeight;
        if (swapchainExtent.width < surfCapabilities.minImageExtent.width) {
            swapchainExtent.width = surfCapabilities.minImageExtent.width;
        } else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width) {
            swapchainExtent.width = surfCapabilities.maxImageExtent.width;
        }

        if (swapchainExtent.height < surfCapabilities.minImageExtent.height) {
            swapchainExtent.height = surfCapabilities.minImageExtent.height;
        } else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height) {
            swapchainExtent.height = surfCapabilities.maxImageExtent.height;
        }
    } else {
        // If the surface size is defined, the swap chain size must match
        swapchainExtent = surfCapabilities.currentExtent;
    }

    // The FIFO present mode is guaranteed by the spec to be supported
    // Also note that current Android driver only supports FIFO
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Determine the number of VkImage's to use in the swap chain.
    // We need to acquire only 1 presentable image at at time.
    // Asking for minImageCount images ensures that we can acquire
    // 1 presentable image as long as we present it before attempting
    // to acquire another.
    uint32_t desiredNumberOfSwapChainImages = numberBackBuffers;

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfCapabilities.currentTransform;
    }

    // Find a supported composite alpha mode - one of these is guaranteed to be set
    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) {
        if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) {
            compositeAlpha = compositeAlphaFlags[i];
            break;
        }
    }

    VkSwapchainCreateInfoKHR swapchain_ci = {};
    swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_ci.pNext = NULL;
    swapchain_ci.surface = surface;
    swapchain_ci.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    swapchain_ci.minImageCount = desiredNumberOfSwapChainImages;
    swapchain_ci.imageFormat = m_format;
    swapchain_ci.imageExtent.width = swapchainExtent.width;
    swapchain_ci.imageExtent.height = swapchainExtent.height;
    swapchain_ci.preTransform = preTransform;
    swapchain_ci.compositeAlpha = compositeAlpha;
    swapchain_ci.imageArrayLayers = 1;
    swapchain_ci.presentMode = swapchainPresentMode;
    swapchain_ci.oldSwapchain = VK_NULL_HANDLE;
#ifndef __ANDROID__
    swapchain_ci.clipped = true;
#else
    swapchain_ci.clipped = false;
#endif
    swapchain_ci.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_ci.queueFamilyIndexCount = 0;
    swapchain_ci.pQueueFamilyIndices = NULL;
    uint32_t queueFamilyIndices[2] = { pDevice->GetGraphicsQueueFamilyIndex(), pDevice->GetPresentQueueFamilyIndex() };
    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        // If the graphics and present queues are from different queue families,
        // we either have to explicitly transfer ownership of images between the
        // queues, or we have to create the swapchain with imageSharingMode
        // as VK_SHARING_MODE_CONCURRENT
        swapchain_ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_ci.queueFamilyIndexCount = 2;
        swapchain_ci.pQueueFamilyIndices = queueFamilyIndices;
    }

    res = vkCreateSwapchainKHR(device, &swapchain_ci, NULL, &m_swapChain);
    assert(res == VK_SUCCESS);

    uint32_t swapchainImageCount;
    res = vkGetSwapchainImagesKHR(device, m_swapChain, &swapchainImageCount, NULL);
    assert(res == VK_SUCCESS);

    m_Images.resize(swapchainImageCount);
    res = vkGetSwapchainImagesKHR(device, m_swapChain, &swapchainImageCount, m_Images.data());
    assert(res == VK_SUCCESS);

    m_ImageViews.resize(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        VkImageViewCreateInfo color_image_view = {};
        color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        color_image_view.pNext = NULL;
        color_image_view.format = m_format;
        color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
        color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
        color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
        color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
        color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        color_image_view.subresourceRange.baseMipLevel = 0;
        color_image_view.subresourceRange.levelCount = 1;
        color_image_view.subresourceRange.baseArrayLayer = 0;
        color_image_view.subresourceRange.layerCount = 1;
        color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
        color_image_view.flags = 0;
        color_image_view.image = m_Images[i];

        res = vkCreateImageView(device, &color_image_view, NULL, &m_ImageViews[i]);
        assert(res == VK_SUCCESS);
    }

    // create fences and semaphores
    //
    m_CmdBufExecutedFences.resize(swapchainImageCount);
    m_ImageAvailableSemaphores.resize(swapchainImageCount);
    m_RenderFinishedSemaphores.resize(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; i++) {
        VkFenceCreateInfo fence_ci = {};
        fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_ci.pNext = NULL;
        fence_ci.flags = 0;

        res = vkCreateFence(device, &fence_ci, NULL, &m_CmdBufExecutedFences[i]);

        VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo = {};
        imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        imageAcquiredSemaphoreCreateInfo.pNext = NULL;
        imageAcquiredSemaphoreCreateInfo.flags = 0;

        res = vkCreateSemaphore(device, &imageAcquiredSemaphoreCreateInfo, NULL, &m_ImageAvailableSemaphores[i]);
        assert(res == VK_SUCCESS);
        res = vkCreateSemaphore(device, &imageAcquiredSemaphoreCreateInfo, NULL, &m_RenderFinishedSemaphores[i]);
        assert(res == VK_SUCCESS);
    }

    m_index = (m_nextIndex++) % m_backBufferCount;
    m_imageIndex = 0;
    vkWaitForFences(device, 1, &m_CmdBufExecutedFences[m_index], VK_TRUE, UINT64_MAX);

    if (presentModes != NULL) {
        free(presentModes);
    }
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void SwapChainVK::OnDestroy() {
    /*
        for (int node = 0; node < m_RTVHeaps.size(); node++)
            m_RTVHeaps[node]->Release();
        m_pSwapChain->Release();
        m_pFactory->Release();
    */
}
/*
ID3D12Resource *SwapChainVK::GetCurrentBackBufferResource()
{
    UINT mIndexLastSwapBuf = m_pSwapChain->GetCurrentBackBufferIndex();

    ID3D12Resource *pBackBuffer;
    ThrowIfFailed(m_pSwapChain->GetBuffer(mIndexLastSwapBuf, IID_PPV_ARGS(&pBackBuffer)));
    pBackBuffer->Release();
    return pBackBuffer;
}
*/

VkImage SwapChainVK::GetCurrentBackBuffer() {
    return m_Images[m_imageIndex];
}

VkImageView SwapChainVK::GetCurrentBackBufferRTV() {
    return m_ImageViews[m_imageIndex];
}

uint32_t SwapChainVK::WaitForSwapChain() {
    vkAcquireNextImageKHR(m_pDevice->GetDevice(), m_swapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_index], VK_NULL_HANDLE, &m_imageIndex);

    return m_imageIndex;
}

void SwapChainVK::GetSemaphores(VkSemaphore *pImageAvailableSemaphore, VkSemaphore *pRenderFinishedSemaphores, VkFence *pCmdBufExecutedFences) {
    vkResetFences(m_pDevice->GetDevice(), 1, &m_CmdBufExecutedFences[m_index]);

    *pImageAvailableSemaphore = m_ImageAvailableSemaphores[m_index];
    *pRenderFinishedSemaphores = m_RenderFinishedSemaphores[m_index];
    *pCmdBufExecutedFences = m_CmdBufExecutedFences[m_index];
}


void SwapChainVK::Present() {
    VkPresentInfoKHR present = {};
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.pNext = NULL;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores = &(m_RenderFinishedSemaphores[m_index]);
    present.swapchainCount = 1;
    present.pSwapchains = &m_swapChain;
    present.pImageIndices = &m_imageIndex;
    present.pResults = NULL;

    VkResult res = vkQueuePresentKHR(m_presentQueue, &present);
    assert(res == VK_SUCCESS);

    m_index = (m_nextIndex++) % m_backBufferCount;

    vkWaitForFences(m_pDevice->GetDevice(), 1, &m_CmdBufExecutedFences[m_index], VK_TRUE, UINT64_MAX);
}


void SwapChainVK::SetFullScreen(bool fullscreen) {
    //ThrowIfFailed(m_pSwapChain->SetFullscreenState(fullscreen, nullptr));
}


void SwapChainVK::OnResize(std::uint32_t dwWidth, std::uint32_t dwHeight) {
    //m_currentFrame = 0;
    /*
        // Set up the swap chain to allow back buffers to live on multiple GPU nodes.
        ThrowIfFailed(
            m_pSwapChain->ResizeBuffers1(
            m_descSwapChain.BufferCount,
            dwWidth,
            dwHeight,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            m_descSwapChain.Flags,
            m_creationNodes.data(),
            m_queues.data())
        );

        //
        // create RTV's
        //
        UINT colorDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_CPUView.resize(m_descSwapChain.BufferCount);
        for (UINT i = 0; i < m_descSwapChain.BufferCount; i++)
        {
            UINT nodeIndex = i % m_nodeCount;

            m_CPUView[i] = m_RTVHeaps[nodeIndex]->GetCPUDescriptorHandleForHeapStart();
            m_CPUView[i].ptr += colorDescriptorSize * (i / m_nodeCount);

            D3D12_GPU_DESCRIPTOR_HANDLE m_GPUView = m_RTVHeaps[nodeIndex]->GetGPUDescriptorHandleForHeapStart();
            m_GPUView.ptr += colorDescriptorSize * (i / m_nodeCount);

            ID3D12Resource *pBackBuffer;
            ThrowIfFailed(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer)));

            D3D12_RESOURCE_DESC desc = pBackBuffer->GetDesc();

            D3D12_RENDER_TARGET_VIEW_DESC colorDesc = {};
            colorDesc.Format = desc.Format;
            colorDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            colorDesc.Texture2D.MipSlice = 0;
            colorDesc.Texture2D.PlaneSlice = 0;

            m_pDevice->CreateRenderTargetView(pBackBuffer, &colorDesc, m_CPUView[i]);
            pBackBuffer->Release();
        }
    */
}
