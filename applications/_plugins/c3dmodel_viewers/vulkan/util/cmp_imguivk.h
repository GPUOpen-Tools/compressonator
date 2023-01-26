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

#include "cmp_dynamicbufferringvk.h"
#include "cmp_commandlistringvk.h"
#include "cmp_uploadheapvk.h"
#include "imgui.h"

// This is the rendering backend for the excellent ImGUI library.

class ImGUIVK {
  public:
    void OnCreate(CMP_DeviceVK* pDevice, CMP_UploadHeapVK* pUploadHeap, CMP_DynamicBufferRingVK* pConstantBufferRing, VkRenderPass renderPass);
    void OnDestroy();

    void Draw(VkCommandBuffer cmd_buf);

  private:
    CMP_DeviceVK*        m_pDevice;
      CMP_DynamicBufferRingVK* m_pConstBuf;

    VkImage                     m_pTexture2D;
    VkDeviceMemory              m_deviceMemory;
    VkDescriptorBufferInfo      m_geometry;
    VkPipelineLayout            m_pipelineLayout;
    VkPipelineCache             m_pipelineCache;
    VkDescriptorPool            m_descriptorPool;
    VkPipeline                  m_pipeline;
    VkDescriptorSet             m_descriptorSet;
    VkSampler                   m_sampler;
    VkImageView                 m_pTextureSRV;
};


bool ImGUIVK_Init(void* hwnd);
void ImGUIVK_Shutdown();
void ImGUIVK_UpdateIO();
LRESULT ImGUIVK_WndProcHandler(MSG msg);