// AMD AMDUtils code
//
// Copyright(c) 2018-2024 Advanced Micro Devices, Inc.All rights reserved.
//
// Vulkan Samples
//
// Copyright (C) 2015-2016 Valve Corporation
// Copyright (C) 2015-2016 LunarG, Inc.
// Copyright (C) 2015-2016 Google, Inc.
//
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

#include "vulkan_renderer.h"
#include "cmp_gltfbboxpassvk.h"
#include "cmp_gltfdepthpassvk.h"
#include "cmp_gltfpbrvk.h"
#include "cmp_misc.h"
#include "cmp_swapchainvk.h"

#ifdef USE_QT10

Vulkan_Renderer::Vulkan_Renderer(QVulkanWindow* w)
    : m_window(w)
{
}

void Vulkan_Renderer::initResources()
{
    qDebug("initResources");

    m_devFuncs = m_window->vulkanInstance()->deviceFunctions(m_window->device());
}

void Vulkan_Renderer::initSwapChainResources()
{
    qDebug("initSwapChainResources");
}

void Vulkan_Renderer::releaseSwapChainResources()
{
    qDebug("releaseSwapChainResources");
}

void Vulkan_Renderer::releaseResources()
{
    qDebug("releaseResources");
}

void Vulkan_Renderer::startNextFrame()
{
    m_green += 0.005f;
    if (m_green > 1.0f)
        m_green = 0.0f;

    VkClearColorValue        clearColor = {0.0f, m_green, 0.0f, 1.0f};
    VkClearDepthStencilValue clearDS    = {1.0f, 0};
    VkClearValue             clearValues[2];
    memset(clearValues, 0, sizeof(clearValues));
    clearValues[0].color        = clearColor;
    clearValues[1].depthStencil = clearDS;

    VkRenderPassBeginInfo rpBeginInfo;
    memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
    rpBeginInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpBeginInfo.renderPass               = m_window->defaultRenderPass();
    rpBeginInfo.framebuffer              = m_window->currentFramebuffer();
    const QSize sz                       = m_window->swapChainImageSize();
    rpBeginInfo.renderArea.extent.width  = sz.width();
    rpBeginInfo.renderArea.extent.height = sz.height();
    rpBeginInfo.clearValueCount          = 2;
    rpBeginInfo.pClearValues             = clearValues;
    VkCommandBuffer cmdBuf               = m_window->currentCommandBuffer();
    m_devFuncs->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Do nothing else. We will just clear to green, changing the component on
    // every invocation. This also helps verifying the rate to which the thread
    // is throttled to. (The elapsed time between startNextFrame calls should
    // typically be around 16 ms. Note that rendering is 2 frames ahead of what
    // is displayed.)

    m_devFuncs->vkCmdEndRenderPass(cmdBuf);

    m_window->frameReady();
    m_window->requestUpdate();  // render continuously, throttled by the presentation rate
}
#else
Vulkan_Renderer::Vulkan_Renderer(void* w)
{
}
#endif

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void Vulkan_Renderer::OnCreate(CMP_DeviceVK* pDevice)
{
    VkResult res;
    m_pDevice = pDevice;

    // Initialize helpers

    // Create heaps for the different types of resources
    m_Heaps.OnCreate(pDevice, 200, 200, 10, 100);

    // Create commandlist ring for the Direct queue
    m_CommandListRing.OnCreate(pDevice, cNumSwapBufs, 8);

    // Create 'dynamic' constant buffers ring
    m_ConstantBufferRing.OnCreate(pDevice, cNumSwapBufs, 200 * 1024 + 320000 * 4);

    // Create 'static' constant buffers pool
    m_StaticBufferPool.OnCreate(pDevice, 30 * 1024 * 1024);

    //    m_renderToSwapChainPass.OnCreate(pDevice);
    /*
        m_pDirectQueue = pDirectQueue;

        // Create 'dynamic' constant buffers ring
        m_ConstantBufferRing.OnCreate(pDevice, cNumSwapBufs, 200 * 1024 + 320000 * 4, &m_Heaps, 300, node, nodemask);

        // Create 'static' constant buffers pool
        m_StaticBufferPool.OnCreate(pDevice, 30 * 1024 * 1024, USE_VID_MEM, node, nodemask);
        m_StaticConstantBufferPool.OnCreate(pDevice, 2 * 1024 * 1024, &m_Heaps, 100, USE_VID_MEM, node, nodemask);


        // This fence is to track whether the next swapchain is available for rendering
        m_FrameFence.OnCreate(pDevice);

        m_GPUTimer.OnCreate(pDevice, cNumSwapBufs, node, nodemask);

        // Quick helper to upload resources, it has it's own commandList.
        m_UploadHeap.OnCreate(pDevice, 64 * 1024 * 1024, m_pDirectQueue, node, nodemask);    // initialize an upload heap (uses suballocation for faster results)
    */

    /*
        // Create the depth buffer view
        m_Heaps.AllocDSVDescriptor(1, &m_DepthBufferDSV);

        // Shadowmap resource and view
        m_ShadowMap.InitDepthStencil(pDevice, &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, 1024, 1024, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL));
        m_ShadowMapViewPort = { 0.0f, 0.0f, (float)m_ShadowMap.GetWidth(), (float)m_ShadowMap.GetHeight(), 0.0f, 1.0f };
        m_Heaps.AllocDSVDescriptor(1, &m_ShadowMapDSV);
        m_ShadowMap.CreateDSV(0, &m_ShadowMapDSV);
        m_ShadowMap.Resource()->SetName(L"m_pShadowMap");
    */
    m_shadowMap.InitDepthStencil(m_pDevice, 1024, 1024);

    // initialize an upload heap (uses suballocation for faster results)
    m_UploadHeap.OnCreate(pDevice, 64 * 1024 * 1024);

    /*
        // Make sure upload heap has finished uploading before continuing
        m_UploadHeap.FlushAndFinish();

        // Create the depth buffer view
        m_Heaps.AllocDSVDescriptor(1, &m_DepthBufferDSV);
    */

    // Create render pass shadow
    //
    {
        m_shadowViewport.x        = 0;
        m_shadowViewport.y        = (float)1024;
        m_shadowViewport.width    = (float)1024;
        m_shadowViewport.height   = -(float)(1024);
        m_shadowViewport.minDepth = (float)0.0f;
        m_shadowViewport.maxDepth = (float)1.0f;

        m_shadowScissor.extent.width  = 1024;
        m_shadowScissor.extent.height = 1024;
        m_shadowScissor.offset.x      = 0;
        m_shadowScissor.offset.y      = 0;

        /* Need attachments for render target and depth buffer */
        VkAttachmentDescription attachments[1];

        // depth RT
        attachments[0].format         = m_shadowMap.GetFormatVK();
        attachments[0].samples        = VK_SAMPLE_COUNT_1_BIT;
        attachments[0].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[0].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[0].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[0].initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[0].finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        attachments[0].flags          = 0;

        VkAttachmentReference depth_reference = {0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

        VkSubpassDescription subpass    = {};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.flags                   = 0;
        subpass.inputAttachmentCount    = 0;
        subpass.pInputAttachments       = NULL;
        subpass.colorAttachmentCount    = 0;
        subpass.pColorAttachments       = NULL;
        subpass.pResolveAttachments     = NULL;
        subpass.pDepthStencilAttachment = &depth_reference;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments    = NULL;

        VkRenderPassCreateInfo rp_info = {};
        rp_info.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rp_info.pNext                  = NULL;
        rp_info.attachmentCount        = 1;
        rp_info.pAttachments           = attachments;
        rp_info.subpassCount           = 1;
        rp_info.pSubpasses             = &subpass;
        rp_info.dependencyCount        = 0;
        rp_info.pDependencies          = NULL;

        res = vkCreateRenderPass(m_pDevice->GetDevice(), &rp_info, NULL, &m_render_pass_shadow);
        assert(res == VK_SUCCESS);

        VkImageView shadowBufferView;
        m_shadowMap.CreateDSV(0, &shadowBufferView);

        // Create frame buffer
        //

        VkImageView attachmentViews[1] = {shadowBufferView};

        VkFramebufferCreateInfo fb_info = {};
        fb_info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_info.pNext                   = NULL;
        fb_info.renderPass              = m_render_pass_shadow;
        fb_info.attachmentCount         = 1;
        fb_info.pAttachments            = attachmentViews;
        fb_info.width                   = 1024;
        fb_info.height                  = 1024;
        fb_info.layers                  = 1;

        res = vkCreateFramebuffer(m_pDevice->GetDevice(), &fb_info, NULL, &m_pShadowMapBuffers);
        assert(res == VK_SUCCESS);
    }

    // Create render pass color with clear
    //
    {
        /* Need attachments for render target and depth buffer */
        VkAttachmentDescription attachments[2];

        // color RT
        attachments[0].format         = VK_FORMAT_B8G8R8A8_UNORM;  // pSC->GetFormat();
        attachments[0].samples        = VK_SAMPLE_COUNT_1_BIT;
        attachments[0].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[0].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[0].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[0].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[0].finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachments[0].flags          = 0;

        // depth RT
        attachments[1].format         = VK_FORMAT_D32_SFLOAT;  // m_depthBuffer.GetFormat();
        attachments[1].samples        = VK_SAMPLE_COUNT_1_BIT;
        attachments[1].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachments[1].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[1].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[1].flags          = 0;

        VkAttachmentReference color_reference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
        VkAttachmentReference depth_reference = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

        VkSubpassDescription subpass    = {};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.flags                   = 0;
        subpass.inputAttachmentCount    = 0;
        subpass.pInputAttachments       = NULL;
        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &color_reference;
        subpass.pResolveAttachments     = NULL;
        subpass.pDepthStencilAttachment = &depth_reference;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments    = NULL;

        VkRenderPassCreateInfo rp_info = {};
        rp_info.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rp_info.pNext                  = NULL;
        rp_info.attachmentCount        = 2;
        rp_info.pAttachments           = attachments;
        rp_info.subpassCount           = 1;
        rp_info.pSubpasses             = &subpass;
        rp_info.dependencyCount        = 0;
        rp_info.pDependencies          = NULL;

        res = vkCreateRenderPass(m_pDevice->GetDevice(), &rp_info, NULL, &m_render_pass_color);
        assert(res == VK_SUCCESS);
    }

    // Create render pass color for HUD
    //
    {
        /* Need attachments for render target and depth buffer */
        VkAttachmentDescription attachments[2];

        // color RT
        attachments[0].format         = VK_FORMAT_B8G8R8A8_UNORM;  // pSC->GetFormat();
        attachments[0].samples        = VK_SAMPLE_COUNT_1_BIT;
        attachments[0].loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[0].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[0].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachments[0].initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachments[0].finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        attachments[0].flags          = 0;

        // depth RT
        attachments[1].format         = VK_FORMAT_D32_SFLOAT;  // m_depthBuffer.GetFormat();
        attachments[1].samples        = VK_SAMPLE_COUNT_1_BIT;
        attachments[1].loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachments[1].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachments[1].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        attachments[1].finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments[1].flags          = 0;

        VkAttachmentReference color_reference = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
        VkAttachmentReference depth_reference = {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

        VkSubpassDescription subpass    = {};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.flags                   = 0;
        subpass.inputAttachmentCount    = 0;
        subpass.pInputAttachments       = NULL;
        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &color_reference;
        subpass.pResolveAttachments     = NULL;
        subpass.pDepthStencilAttachment = &depth_reference;
        subpass.preserveAttachmentCount = 0;
        subpass.pPreserveAttachments    = NULL;

        VkRenderPassCreateInfo rp_info = {};
        rp_info.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rp_info.pNext                  = NULL;
        rp_info.attachmentCount        = 2;
        rp_info.pAttachments           = attachments;
        rp_info.subpassCount           = 1;
        rp_info.pSubpasses             = &subpass;
        rp_info.dependencyCount        = 0;
        rp_info.pDependencies          = NULL;

        res = vkCreateRenderPass(m_pDevice->GetDevice(), &rp_info, NULL, &m_render_pass_color_hud);
        assert(res == VK_SUCCESS);
    }

    m_triangle.OnCreate(pDevice, &m_ConstantBufferRing, &m_StaticBufferPool, m_render_pass_color);

#ifdef USE_IMGUI
    m_ImGUI.OnCreate(pDevice, &m_UploadHeap, /*&m_Heaps, */ &m_ConstantBufferRing, m_render_pass);
#endif
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void Vulkan_Renderer::OnDestroy()
{
    m_triangle.OnDestroy();
#ifdef USE_IMGUI
    m_ImGUI.OnDestroy();
#endif
    m_ConstantBufferRing.OnDestroy();
    m_StaticBufferPool.OnDestroy();
    m_triangle.OnDestroy();
}

//--------------------------------------------------------------------------------------
//
// OnCreateWindowSizeDependentResources
//
//--------------------------------------------------------------------------------------
void Vulkan_Renderer::OnCreateWindowSizeDependentResources(SwapChainVK* pSC, std::uint32_t Width, std::uint32_t Height)
{
    VkResult res;

    m_Width      = Width;
    m_Height     = Height;
    m_pSwapChain = pSC;

    m_depthBuffer.InitDepthStencil(m_pDevice, Width, Height);

    m_viewport.x        = 0;
    m_viewport.y        = (float)Height;
    m_viewport.width    = (float)Width;
    m_viewport.height   = -(float)(Height);
    m_viewport.minDepth = (float)0.0f;
    m_viewport.maxDepth = (float)1.0f;

    m_scissor.extent.width  = Width;
    m_scissor.extent.height = Height;
    m_scissor.offset.x      = 0;
    m_scissor.offset.y      = 0;

    VkImageView depthBufferView;
    m_depthBuffer.CreateDSV(0, &depthBufferView);

    // Create frame buffer
    //
    std::uint32_t swapchainImageCount = (std::uint32_t)pSC->GetViews().size();
    m_pFrameBuffers                   = (VkFramebuffer*)malloc(swapchainImageCount * sizeof(VkFramebuffer));

    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
        VkImageView attachments[2] = {pSC->GetViews()[i], depthBufferView};

        VkFramebufferCreateInfo fb_info = {};
        fb_info.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_info.pNext                   = NULL;
        fb_info.renderPass              = m_render_pass_color;
        fb_info.attachmentCount         = 2;
        fb_info.pAttachments            = attachments;
        fb_info.width                   = Width;
        fb_info.height                  = Height;
        fb_info.layers                  = 1;

        res = vkCreateFramebuffer(m_pDevice->GetDevice(), &fb_info, NULL, &m_pFrameBuffers[i]);
        assert(res == VK_SUCCESS);
    }
}

//--------------------------------------------------------------------------------------
//
// OnDestroyWindowSizeDependentResources
//
//--------------------------------------------------------------------------------------
void Vulkan_Renderer::OnDestroyWindowSizeDependentResources()
{
    //m_renderToSwapChainPass.OnDestroyWindowSizeDependentResources();
    free(m_pFrameBuffers);
    m_depthBuffer.OnDestroy();
}

//--------------------------------------------------------------------------------------
//
// LoadScene
//
//--------------------------------------------------------------------------------------
void Vulkan_Renderer::LoadScene(CMP_GLTFCommon* gltfData, void* pluginManager, void* msghandler)
{
    //ID3D12GraphicsCommandList* pCmdLst = m_UploadHeap.GetCommandList();

    //create the glTF's textures, VBs, IBs, shaders and descriptors
    m_gltfDepth = new GltfDepthPass();
    m_gltfDepth->OnCreate(
        m_pDevice, m_render_pass_shadow, &m_UploadHeap, &m_Heaps, &m_ConstantBufferRing, &m_StaticBufferPool, gltfData, pluginManager, msghandler);

    m_gltfPBR = new CMP_GltfPbrVK();
    m_gltfPBR->OnCreate(m_pDevice,
                        m_render_pass_color,
                        &m_UploadHeap,
                        &m_Heaps,
                        &m_ConstantBufferRing,
                        &m_StaticBufferPool,
                        gltfData,
                        //        &m_skyDome,
                        &m_shadowMap,
                        pluginManager,
                        msghandler);

    m_gltfBBox = new GltfBBoxPassVK();
    m_gltfBBox->OnCreate(m_pDevice, m_render_pass_color, &m_UploadHeap, &m_Heaps, &m_ConstantBufferRing, &m_StaticBufferPool, gltfData);

    /*
        m_StaticBufferPool.UploadData(m_UploadHeap.GetCommandList());
        m_StaticConstantBufferPool.UploadData(m_UploadHeap.GetCommandList());

        m_UploadHeap.FlushAndFinish();

    #if (USE_VID_MEM==true)
        m_StaticBufferPool.FreeUploadHeap();
        m_StaticConstantBufferPool.FreeUploadHeap();
    #endif
    */
}

//--------------------------------------------------------------------------------------
//
// UnloadScene
//
//--------------------------------------------------------------------------------------
void Vulkan_Renderer::UnloadScene()
{
    //m_gltf->OnDestroy();
    //delete m_gltf;
}

//--------------------------------------------------------------------------------------
//
// OnRender
//
//--------------------------------------------------------------------------------------
void Vulkan_Renderer::OnRender(State* pState, SwapChainVK* pSwapChain)
{
    VkResult res = VK_RESULT_MAX_ENUM;

    // Let our resource managers do some house keeping
    //
    m_ConstantBufferRing.OnBeginFrame();
    m_CommandListRing.OnBeginFrame();

    // Shadow map -----------------------------------------------------------------------
    //

    VkCommandBuffer cmd_buf = *m_CommandListRing.GetNewCommandList();

    // command buffer calls
    //
    {
        VkCommandBufferBeginInfo cmd_buf_info = {};
        cmd_buf_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_buf_info.pNext                    = NULL;
        cmd_buf_info.flags                    = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        cmd_buf_info.pInheritanceInfo         = NULL;
        res                                   = vkBeginCommandBuffer(cmd_buf, &cmd_buf_info);
        assert(res == VK_SUCCESS);
    }

    {
        VkClearValue depth_clear_values[1];
        depth_clear_values[0].depthStencil.depth   = 1.0f;
        depth_clear_values[0].depthStencil.stencil = 0;

        VkRenderPassBeginInfo rp_begin    = {};
        rp_begin.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_begin.pNext                    = NULL;
        rp_begin.renderPass               = m_render_pass_shadow;
        rp_begin.framebuffer              = m_pShadowMapBuffers;
        rp_begin.renderArea.offset.x      = 0;
        rp_begin.renderArea.offset.y      = 0;
        rp_begin.renderArea.extent.width  = 1024;
        rp_begin.renderArea.extent.height = 1024;
        rp_begin.clearValueCount          = 1;
        rp_begin.pClearValues             = depth_clear_values;

        vkCmdBeginRenderPass(cmd_buf, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
    }

    vkCmdSetScissor(cmd_buf, 0, 1, &m_shadowScissor);
    vkCmdSetViewport(cmd_buf, 0, 1, &m_shadowViewport);

    //set per frame constant buffer values
    if (m_gltfDepth)
    {
        GltfDepthPass::per_batch* cbPerBatch = m_gltfDepth->SetPerBatchConstants();
        cbPerBatch->mViewProj                = pState->light.GetView() * pState->light.GetProjection();
        m_gltfDepth->Draw(cmd_buf);
    }

    vkCmdEndRenderPass(cmd_buf);

    res = vkEndCommandBuffer(cmd_buf);

    {
        VkPipelineStageFlags  submitWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        const VkCommandBuffer cmd_bufs[]      = {cmd_buf};
        VkSubmitInfo          submit_info     = {};
        submit_info.sType                     = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext                     = NULL;
        submit_info.waitSemaphoreCount        = 0;
        submit_info.pWaitSemaphores           = NULL;
        submit_info.pWaitDstStageMask         = &submitWaitStage;
        submit_info.commandBufferCount        = 1;
        submit_info.pCommandBuffers           = cmd_bufs;
        submit_info.signalSemaphoreCount      = 0;
        submit_info.pSignalSemaphores         = NULL;

        res = vkQueueSubmit(m_pDevice->GetGraphicsQueue(), 1, &submit_info, VK_NULL_HANDLE);
        assert(res == VK_SUCCESS);
    }

    // Render Scene to the MSAA HDR RT ------------------------------------------------
    //

    // Wait for the backbuffer used cNumSwapBufs frames ago to be available
    //
    uint32_t imageIndex = pSwapChain->WaitForSwapChain();

    cmd_buf = *m_CommandListRing.GetNewCommandList();

    {
        VkCommandBufferBeginInfo cmd_buf_info = {};
        cmd_buf_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_buf_info.pNext                    = NULL;
        cmd_buf_info.flags                    = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        cmd_buf_info.pInheritanceInfo         = NULL;
        res                                   = vkBeginCommandBuffer(cmd_buf, &cmd_buf_info);
        assert(res == VK_SUCCESS);
    }

    // prepare render pass
    {
        VkClearValue clear_values[2];
        clear_values[0].color.float32[0]     = 0.0f;
        clear_values[0].color.float32[1]     = 0.0f;
        clear_values[0].color.float32[2]     = 0.0f;
        clear_values[0].color.float32[3]     = 0.0f;
        clear_values[1].depthStencil.depth   = 1.0f;
        clear_values[1].depthStencil.stencil = 0;

        VkRenderPassBeginInfo rp_begin    = {};
        rp_begin.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_begin.pNext                    = NULL;
        rp_begin.renderPass               = m_render_pass_color;
        rp_begin.framebuffer              = m_pFrameBuffers[imageIndex];
        rp_begin.renderArea.offset.x      = 0;
        rp_begin.renderArea.offset.y      = 0;
        rp_begin.renderArea.extent.width  = m_Width;
        rp_begin.renderArea.extent.height = m_Height;
        rp_begin.clearValueCount          = 2;
        rp_begin.pClearValues             = clear_values;

        vkCmdBeginRenderPass(cmd_buf, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
    }

    {
        vkCmdSetScissor(cmd_buf, 0, 1, &m_scissor);
        vkCmdSetViewport(cmd_buf, 0, 1, &m_viewport);

        //set per frame constant buffer values
        CMP_GltfPbrVK::per_batch* cbPerBatch = m_gltfPBR->SetPerBatchConstants();
        cbPerBatch->mCameraViewProj          = pState->camera.GetView() * pState->camera.GetProjection();
        cbPerBatch->cameraPos                = pState->camera.GetPosition();
        cbPerBatch->mLightViewProj           = pState->light.GetView() * pState->light.GetProjection();
        cbPerBatch->lightDirection           = pState->light.GetDirection();
        cbPerBatch->lightColor               = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f) * pState->spotLightIntensity;
        cbPerBatch->depthBias                = pState->depthBias;
        cbPerBatch->iblFactor                = pState->iblFactor;

        m_gltfPBR->Draw(cmd_buf);
    }

    vkCmdEndRenderPass(cmd_buf);

    {
        VkImageMemoryBarrier barrier            = {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext                           = NULL;
        barrier.srcAccessMask                   = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask                   = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        barrier.oldLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.newLayout                       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;
        barrier.image                           = m_shadowMap.Resource();

        vkCmdPipelineBarrier(cmd_buf,
                             VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                             VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                             0,
                             0,
                             NULL,
                             0,
                             NULL,
                             1,
                             &barrier);
    }

    // prepare render pass
    {
        VkRenderPassBeginInfo rp_begin    = {};
        rp_begin.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rp_begin.pNext                    = NULL;
        rp_begin.renderPass               = m_render_pass_color_hud;
        rp_begin.framebuffer              = m_pFrameBuffers[imageIndex];
        rp_begin.renderArea.offset.x      = 0;
        rp_begin.renderArea.offset.y      = 0;
        rp_begin.renderArea.extent.width  = m_Width;
        rp_begin.renderArea.extent.height = m_Height;
        rp_begin.clearValueCount          = 0;
        rp_begin.pClearValues             = NULL;

        vkCmdBeginRenderPass(cmd_buf, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
    }

    // draw bounding boxes
    if (pState->bDrawBoundingBoxes)
    {
        vkCmdSetScissor(cmd_buf, 0, 1, &m_scissor);
        vkCmdSetViewport(cmd_buf, 0, 1, &m_viewport);

        glm::mat4* pCam = m_gltfBBox->SetPerBatchConstants();
        *pCam           = pState->camera.GetView() * pState->camera.GetProjection();
        m_gltfBBox->Draw(cmd_buf);
    }

    // Render HUD  ------------------------------------------------------------------------
    //

#ifdef USE_IMGUI
    m_ImGUI.Draw(cmd_buf);
#endif

    vkCmdEndRenderPass(cmd_buf);

    res = vkEndCommandBuffer(cmd_buf);

    VkSemaphore ImageAvailableSemaphore;
    VkSemaphore RenderFinishedSemaphores;
    VkFence     CmdBufExecutedFences;
    pSwapChain->GetSemaphores(&ImageAvailableSemaphore, &RenderFinishedSemaphores, &CmdBufExecutedFences);

    VkPipelineStageFlags  submitWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    const VkCommandBuffer cmd_bufs[]      = {cmd_buf};
    VkSubmitInfo          submit_info;
    submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.pNext                = NULL;
    submit_info.waitSemaphoreCount   = 1;
    submit_info.pWaitSemaphores      = &ImageAvailableSemaphore;
    submit_info.pWaitDstStageMask    = &submitWaitStage;
    submit_info.commandBufferCount   = 1;
    submit_info.pCommandBuffers      = cmd_bufs;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores    = &RenderFinishedSemaphores;

    res = vkQueueSubmit(m_pDevice->GetGraphicsQueue(), 1, &submit_info, CmdBufExecutedFences);
    assert(res == VK_SUCCESS);
}
