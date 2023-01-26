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

#pragma once
#include "cmp_devicevk.h"

#include <vector>
#include <cstdint>

class SwapChainVK {
  public:
    void OnCreate(CMP_DeviceVK* pDevice, std::uint32_t numberBackBuffers, std::uint32_t dwWidth, std::uint32_t dwHeight, void* hWnd);
    void OnDestroy();

    void SetFullScreen(bool fullscreen);
    void OnResize(std::uint32_t dwWidth, std::uint32_t dwHeight);

    /*
    ID3D12Resource *GetCurrentBackBufferResource();
    */
    VkFormat GetFormat() {
        return m_format;
    };
    std::vector<VkImageView> GetViews() {
        return m_ImageViews;
    }
    VkImageView GetCurrentBackBufferRTV();
    VkImage GetCurrentBackBuffer();
    VkSwapchainKHR GetSwapChain() {
        return m_swapChain;
    }
    uint32_t WaitForSwapChain();
    void GetSemaphores(VkSemaphore *pImageAvailableSemaphore, VkSemaphore *pRenderFinishedSemaphores, VkFence *pCmdBufExecutedFences);
    void Present();
  private:
    CMP_DeviceVK* m_pDevice;

    VkSwapchainKHR m_swapChain;
    VkFormat m_format;

    VkQueue m_presentQueue;

    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;

    std::vector<VkFence>       m_CmdBufExecutedFences;
    std::vector<VkSemaphore>   m_ImageAvailableSemaphores;
    std::vector<VkSemaphore>   m_RenderFinishedSemaphores;

    uint32_t m_index = 0;
    uint32_t m_nextIndex = 0;
    uint32_t m_imageIndex = 0;

    uint32_t m_backBufferCount = 0;
};

