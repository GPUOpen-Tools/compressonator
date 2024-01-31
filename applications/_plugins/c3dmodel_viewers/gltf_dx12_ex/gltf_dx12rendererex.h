// AMD SampleDX12 sample code
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

#include <d3d12.h>

#include "cmp_uploadheapdx12.h"
#include "cmp_texturedx12.h"
#include "cmp_dynamicbufferringdx12.h"
#include "cmp_staticbufferpooldx12.h"
#include "cmp_staticconstantbufferpooldx12.h"
#include "cmp_commandlistringdx12.h"
#include "cmp_fencedx12.h"
#include "cmp_resourceviewheapsdx12.h"
#include "cmp_camera.h"
#include "cmp_gputimestampsdx12.h"

#include "dx12util/cmp_imguidx12.h"
#include "imgui.h"
#include <qtimgui/imgui_dx12renderer.h>

#include "cmp_gltfpbr.h"
#include "cmp_gltfdepthpass.h"
#include "cmp_gltfbboxpass.h"
#include "cmp_blurps.h"
#include "cmp_tonemapping.h"
#include "cmp_bloom.h"
#include "cmp_skydome.h"
#include "userinterface.h"

static const int cNumSwapBufs = 2;

#define USE_VID_MEM true

//
// This class deals with the GPU side of the sample and there is one instance per GPU.
//

class glTF_DX12RendererEx
{
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

    PluginManager* m_pluginManager;
    void*          m_msghandler;
    DWORD          m_TimeToLoadScene;

    void OnCreate(ID3D12Device*              pDevice,
                  ID3D12CommandQueue*        pDirectQueue,
                  UINT                       node,
                  UINT                       nodemask,
                  void*                      pluginManager,
                  void*                      msghandler,
                  ImGuiRenderer_DX12*        ImGuiRenderer,
                  QImGUI_WindowWrapper_DX12* window);
    void OnDestroy();

    void OnCreateWindowSizeDependentResources(ID3D12Device* pDevice, DWORD Width, DWORD Height, UINT node, UINT nodemask);
    void OnDestroyWindowSizeDependentResources();

    void LoadScene(CMP_GLTFCommon* gltfData, void* pluginManager, void* msghandler);
    void UnloadScene();

    std::vector<TimeStamp>& GetTimingValues()
    {
        return m_TimeStamps;
    }

    void OnRender(State*                       pState,
                  ID3D12Resource*              pRenderTarget,
                  D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetSRV,
                  ImGuiRenderer_DX12*          ImGuiRenderer,
                  UserInterface*               UI);

private:
    ID3D12Device*       m_pDevice;
    ID3D12CommandQueue* m_pDirectQueue;

    TextureDX12 m_pDepthBuffer;
    DSV         m_DepthBufferDSV;

    // MSAA RT
    TextureDX12 m_pHDRMSAA;
    CBV_SRV_UAV m_HDRSRVMSAA;
    RTV         m_HDRRTVMSAA;

    // Resolved RT
    TextureDX12 m_HDR;
    CBV_SRV_UAV m_HDRSRV;
    RTV         m_HDRRTV;

    SkyDome m_skyDome;

    ToneMapping m_toneMapping;

#ifdef USE_BLOOM
    CMP_Bloom m_bloom;
#endif

#ifdef USE_SHADOWMAPS
    DSV            m_ShadowMapDSV;
    Texture        m_ShadowMap;
    D3D12_VIEWPORT m_ShadowMapViewPort;
#endif

    // Initialize helper classes
    ResourceViewHeapsDX12        m_Heaps;
    UploadHeapDX12               m_UploadHeap;
    FenceDX12                    m_FrameFence;
    DynamicBufferRingDX12        m_ConstantBufferRing;
    StaticBufferPoolDX12         m_StaticBufferPool;
    StaticConstantBufferPoolDX12 m_StaticConstantBufferPool;
    CommandListRingDX12          m_CommandListRing;
    GPUTimestampsDX12            m_GPUTimer;

    CMP_GltfPbr*   m_gltfPBR;
    GltfDepthPass* m_gltfDepth;
    GltfBBoxPass*  m_gltfBBox;

    DWORD m_Width;
    DWORD m_Height;

    D3D12_VIEWPORT m_ViewPort;
    D3D12_RECT     m_RectScissor;

    std::vector<TimeStamp> m_TimeStamps;
};
