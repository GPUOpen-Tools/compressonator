// AMD DeferredTiledBasedLightingD3D12 sample code
//
// Copyright(c) 2017-2024 Advanced Micro Devices, Inc.All rights reserved.
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

#include "cmp_cameravk.h"
#include "cmp_devicevk.h"
#include "cmp_texturevk.h"

#include "cmp_commandlistringvk.h"
#include "cmp_staticbufferpoolvk.h"

#include "cmp_imguivk.h"
#include "cmp_resourceviewheapsvk.h"
#include "cmp_trianglevk.h"

// -- Qt Support
#ifdef USE_QT10
#include <QtGUI/qvulkanfunctions>
#include <QtGUI/qvulkanwindow>
#endif

// Forward declaration
class CMP_GLTFCommon;
class CMP_GltfPbrVK;
class GltfDepthPass;
class GltfBBoxPassVK;
class SwapChainVK;

static const int cNumSwapBufs = 2;

#define USE_VID_MEM true

class Vulkan_Renderer
#ifdef USE_QT10
    : public QVulkanWindowRenderer
#endif
{
public:
#ifdef USE_QT10
    Vulkan_Renderer(QVulkanWindow* w);

    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;

    void startNextFrame() override;

private:
    QVulkanWindow*          m_window;
    QVulkanDeviceFunctions* m_devFuncs;
    float                   m_green = 0;
#else
    Vulkan_Renderer(void* w);
#endif

public:
    struct State
    {
        float      time;
        CMP_Camera camera;
        CMP_Camera light;
        float      depthBias;
        float      exposure;
        float      iblFactor;
        float      spotLightIntensity;
        float      glow;
        int        toneMapper;
        bool       bDrawBoundingBoxes;
        bool       bDrawSkyDome;
        bool       bGammaTestPattern;
    };

    void OnCreate(CMP_DeviceVK* pDevice);
    void OnDestroy();

    void OnCreateWindowSizeDependentResources(SwapChainVK* pSC, std::uint32_t Width, std::uint32_t Height);
    void OnDestroyWindowSizeDependentResources();

    void LoadScene(CMP_GLTFCommon* gltfData, void* pluginManager, void* msghandler);
    void UnloadScene();
    void OnRender(State* pState, SwapChainVK* pSwapChain);

private:
    CMP_DeviceVK* m_pDevice;

    CommandListRingVK       m_CommandListRing;
    CMP_DynamicBufferRingVK m_ConstantBufferRing;
    CMP_StaticBufferPoolVK  m_StaticBufferPool;

    // Initialize helper classes
    CMP_ResourceViewHeapsVK m_Heaps;
    CMP_UploadHeapVK        m_UploadHeap;
    ImGUIVK                 m_ImGUI;
    CMP_GltfPbrVK*          m_gltfPBR;
    GltfDepthPass*          m_gltfDepth;
    GltfBBoxPassVK*         m_gltfBBox;
    TriangleVK              m_triangle;
    Texture                 m_depthBuffer;
    Texture                 m_shadowMap;

    //RenderToSwapChainPass           m_renderToSwapChainPass;

    unsigned int m_Width;
    unsigned int m_Height;

    VkRect2D   m_scissor;
    VkViewport m_viewport;

    VkRect2D   m_shadowScissor;
    VkViewport m_shadowViewport;

    VkRenderPass m_render_pass_color;
    VkRenderPass m_render_pass_color_hud;
    VkRenderPass m_render_pass_shadow;

    VkFramebuffer* m_pFrameBuffers;
    VkFramebuffer  m_pShadowMapBuffers;

    SwapChainVK* m_pSwapChain;
};
