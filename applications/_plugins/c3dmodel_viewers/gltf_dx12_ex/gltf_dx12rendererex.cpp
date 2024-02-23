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

#include "cmp_misc.h"
#include "cmp_gltfpbr.h"
#include "cmp_bloom.h"
#include "cmp_threadpool.h"
#include "gltf_dx12rendererex.h"
#include "cmp_error.h"

#include <directxmath.h>

using namespace DirectX;

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void glTF_DX12RendererEx::OnCreate(ID3D12Device*              pDevice,
                                   ID3D12CommandQueue*        pDirectQueue,
                                   UINT                       node,
                                   UINT                       nodemask,
                                   void*                      pluginManager,
                                   void*                      msghandler,
                                   ImGuiRenderer_DX12*        ImGuiRenderer,
                                   QImGUI_WindowWrapper_DX12* window)
{
    m_pDevice      = pDevice;
    m_pDirectQueue = pDirectQueue;

    m_gltfDepth = NULL;
    m_gltfPBR   = NULL;
    m_gltfBBox  = NULL;

    // Initialize helpers

    // Create all the heaps for the resources views
    m_Heaps.OnCreate(pDevice, 2000, 3000, 10, 3, 40, 1000, nodemask);

    // Create a 'dynamic' constant buffers ring
    m_ConstantBufferRing.OnCreate(pDevice, cNumSwapBufs, 1 * 1024 * 1024, &m_Heaps, 1500, node, nodemask);

    // Create a 'static' constant buffers pool
    m_StaticBufferPool.OnCreate(pDevice, 30 * 1024 * 1024, USE_VID_MEM, node, nodemask);
    m_StaticConstantBufferPool.OnCreate(pDevice, 2 * 1024 * 1024, &m_Heaps, 100, USE_VID_MEM, node, nodemask);

    // Create a commandlist ring for the Direct queue
    m_CommandListRing.OnCreate(pDevice, m_pDirectQueue, cNumSwapBufs, 8, nodemask);

    // This fence is for the swap chain synchronization
    m_FrameFence.OnCreate(pDevice);

    // initialize the GPU time stamps module
    m_GPUTimer.OnCreate(pDevice, cNumSwapBufs, node, nodemask);

    // Quick helper to upload resources, it has it's own commandList.
    m_UploadHeap.OnCreate(pDevice, 64 * 1024 * 1024, m_pDirectQueue, node, nodemask);  // initialize an upload heap (uses suballocation for faster results)

    if (window)
        ImGuiRenderer->initialize(window, pDevice, &m_UploadHeap, &m_Heaps, &m_ConstantBufferRing, node, nodemask);

    // Create the depth buffer view
    m_Heaps.AllocDSVDescriptor(1, &m_DepthBufferDSV);

    // Shadowmap resource and view
#ifdef USE_SHADOWMAPS
    m_ShadowMap.InitDepthStencil(
        pDevice, &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, 1024, 1024, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL), node, nodemask);
    m_ShadowMapViewPort = {0.0f, 0.0f, (float)m_ShadowMap.GetWidth(), (float)m_ShadowMap.GetHeight(), 0.0f, 1.0f};
    m_Heaps.AllocDSVDescriptor(1, &m_ShadowMapDSV);
    m_ShadowMap.CreateDSV(0, &m_ShadowMapDSV);
    m_ShadowMap.Resource()->SetName(L"m_pShadowMap");
#endif

#ifdef USE_BLOOM
    m_bloom.OnCreate(pDevice, node, &m_Heaps, &m_StaticBufferPool, &m_ConstantBufferRing);
#endif

    m_skyDome.OnCreate(
        pDevice, &m_UploadHeap, DXGI_FORMAT_R16G16B16A16_UNORM, &m_Heaps, &m_StaticBufferPool, &m_ConstantBufferRing, 4, pluginManager, msghandler);

    m_Heaps.AllocRTVDescriptor(1, &m_HDRRTV);
    m_Heaps.AllocRTVDescriptor(1, &m_HDRRTVMSAA);

    m_Heaps.AllocCBV_SRV_UAVDescriptor(1, &m_HDRSRVMSAA);
    m_Heaps.AllocCBV_SRV_UAVDescriptor(1, &m_HDRSRV);

    // Create tonemapping pass
    m_toneMapping.OnCreate(pDevice, node, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, &m_Heaps, &m_StaticBufferPool, &m_ConstantBufferRing);

    // Make sure upload heap has finished uploading before continuing
    m_UploadHeap.FlushAndFinish();
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void glTF_DX12RendererEx::OnDestroy()
{
    m_skyDome.OnDestroy();

#ifdef USE_BLOOM
    m_bloom.OnDestroy();
#endif

    m_toneMapping.OnDestroy();

#ifdef USE_SHADOWMAPS
    m_ShadowMap.OnDestroy();
#endif

    m_UploadHeap.OnDestroy();
    m_GPUTimer.OnDestroy();
    m_FrameFence.OnDestroy();
    m_CommandListRing.OnDestroy();
    m_StaticConstantBufferPool.OnDestroy();
    m_StaticBufferPool.OnDestroy();
    m_ConstantBufferRing.OnDestroy();
    m_Heaps.OnDestroy();
}

//--------------------------------------------------------------------------------------
//
// OnCreateWindowSizeDependentResources
//
//--------------------------------------------------------------------------------------
void glTF_DX12RendererEx::OnCreateWindowSizeDependentResources(ID3D12Device* pDevice, DWORD Width, DWORD Height, UINT node, UINT nodemask)
{
    m_Width  = Width;
    m_Height = Height;

#ifdef USE_BLOOM
    //create bloom temporal and effect
    m_bloom.OnCreateWindowSizeDependentResources(pDevice, Width, Height, node, nodemask);
#endif

    // Create depth buffer
    m_pDepthBuffer.InitDepthStencil(
        pDevice, &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_TYPELESS, Width, Height, 1, 1, 4, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL), node, nodemask);
    m_pDepthBuffer.CreateDSV(0, &m_DepthBufferDSV);
    m_pDepthBuffer.Resource()->SetName(L"DepthBuffer");

    // Create Texture + RTV with x4 MSAA
    CD3DX12_RESOURCE_DESC RDescMSAA =
        CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R16G16B16A16_UNORM, Width, Height, 1, 1, 4, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    m_pHDRMSAA.InitRendertarget(pDevice, &RDescMSAA, node, nodemask);
    m_pHDRMSAA.CreateSRV(0, &m_HDRSRVMSAA);
    m_pHDRMSAA.CreateRTV(0, &m_HDRRTVMSAA);
    m_pHDRMSAA.Resource()->SetName(L"HDRMSAA");

    // Create Texture + RTV, to hold trhe resolved scene
    CD3DX12_RESOURCE_DESC RDesc =
        CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R16G16B16A16_UNORM, Width, Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    m_HDR.InitRendertarget(pDevice, &RDesc, node, nodemask);
    m_HDR.CreateSRV(0, &m_HDRSRV);
    m_HDR.CreateRTV(0, &m_HDRRTV);
    m_HDR.Resource()->SetName(L"HDR");

    // Set the viewport
    m_ViewPort = {0.0f, 0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, 1.0f};

    // Create scissor rectangle
    m_RectScissor = {0, 0, (LONG)Width, (LONG)Height};
}

//--------------------------------------------------------------------------------------
//
// OnDestroyWindowSizeDependentResources
//
//--------------------------------------------------------------------------------------
void glTF_DX12RendererEx::OnDestroyWindowSizeDependentResources()
{
    m_HDR.OnDestroy();
    m_pHDRMSAA.OnDestroy();
    m_pDepthBuffer.OnDestroy();

#ifdef USE_BLOOM
    m_bloom.OnDestroyWindowSizeDependentResources();
#endif
}

//--------------------------------------------------------------------------------------
// LoadScene
//--------------------------------------------------------------------------------------

void glTF_DX12RendererEx::LoadScene(CMP_GLTFCommon* gltfData, void* pluginManager, void* msghandler)
{
    ID3D12GraphicsCommandList* pCmdLst = m_UploadHeap.GetCommandList();

    DWORD ini = GetTickCount();

    //create the glTF's textures, VBs, IBs, shaders and descriptors

#ifdef USE_SHADOWMAPS
    if (m_gltfDepth = new GltfDepthPass())
    {
        m_gltfDepth->OnCreate(m_pDevice, &m_UploadHeap, &m_Heaps, &m_ConstantBufferRing, &m_StaticBufferPool, gltfData, pluginManager, msghandler);
    }
#endif

    if (m_gltfPBR = new CMP_GltfPbr())
    {
        m_gltfPBR->OnCreate(m_pDevice,
                            &m_UploadHeap,
                            &m_Heaps,
                            &m_ConstantBufferRing,
                            &m_StaticBufferPool,
                            gltfData,
                            &m_skyDome,
#ifdef USE_SHADOWMAPS
                            &m_ShadowMap,
#endif
                            pluginManager,
                            msghandler);
    }

    if (m_gltfBBox = new GltfBBoxPass())
    {
        m_gltfBBox->OnCreate(m_pDevice, &m_UploadHeap, &m_Heaps, &m_ConstantBufferRing, &m_StaticBufferPool, gltfData, pluginManager, msghandler);
    }

    m_TimeToLoadScene = (GetTickCount() - ini) + gltfData->m_CommonLoadTime;

    // we are borrowing the upload heap command list for uploading to the GPU the IBs and VBs
    m_StaticBufferPool.UploadData(m_UploadHeap.GetCommandList());
    m_UploadHeap.FlushAndFinish();

#if (USE_VID_MEM == true)
    //once everything is uploaded we dont need he upload heaps anymore
    m_StaticBufferPool.FreeUploadHeap();
    m_StaticConstantBufferPool.FreeUploadHeap();
#endif
}

//--------------------------------------------------------------------------------------
//
// UnloadScene
//
//--------------------------------------------------------------------------------------
void glTF_DX12RendererEx::UnloadScene()
{
    if (m_gltfBBox)
    {
        m_gltfBBox->OnDestroy();
        delete m_gltfBBox;
    }

    if (m_gltfPBR)
    {
        m_gltfPBR->OnDestroy();
        delete m_gltfPBR;
    }

    if (m_gltfDepth)
    {
        m_gltfDepth->OnDestroy();
        delete m_gltfDepth;
    }
}

//--------------------------------------------------------------------------------------
//
// OnRender
//
//--------------------------------------------------------------------------------------
void glTF_DX12RendererEx::OnRender(State*                       pState,
                                   ID3D12Resource*              pRenderTarget,
                                   D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetRTV,
                                   ImGuiRenderer_DX12*          ImGuiRenderer,
                                   UserInterface*               UI)
{
    // Timing values
    //
    UINT64 gpuFrequency;
    m_pDirectQueue->GetTimestampFrequency(&gpuFrequency);

    // Wait for the backbuffer used cNumSwapBufs frames ago to be available
    //
    m_FrameFence.WaitForFence(cNumSwapBufs);

    // Let our resource managers do some house keeping
    //
    m_ConstantBufferRing.OnBeginFrame();
    m_CommandListRing.OnBeginFrame();
    m_GPUTimer.OnBeginFrame(gpuFrequency, &m_TimeStamps);

    ID3D12GraphicsCommandList* pCmdLst = m_CommandListRing.GetNewCommandList();

    m_GPUTimer.GetTimeStamp(pCmdLst, "Begin Frame");

    // Clear GBuffer and depth stencil
    //
    pCmdLst->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pRenderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Clears -----------------------------------------------------------------------
    //
#ifdef USE_SHADOWMAPS
    pCmdLst->ClearDepthStencilView(m_ShadowMapDSV.GetCPU(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    m_GPUTimer.GetTimeStamp(pCmdLst, "Clear Shadow Map");
#endif

    float clearColor[] = {0.0f, 0.0f, 0.0f, 1.0f};
    pCmdLst->ClearRenderTargetView(m_HDRRTVMSAA.GetCPU(), clearColor, 0, nullptr);
    m_GPUTimer.GetTimeStamp(pCmdLst, "Clear HDR");

    pCmdLst->ClearDepthStencilView(m_DepthBufferDSV.GetCPU(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    m_GPUTimer.GetTimeStamp(pCmdLst, "Clear Depth");

    // Shadow map -----------------------------------------------------------------------
    //
#ifdef USE_SHADOWMAPS
    pCmdLst->RSSetScissorRects(1, &m_RectScissor);
    pCmdLst->RSSetViewports(1, &m_ShadowMapViewPort);
    pCmdLst->OMSetRenderTargets(0, NULL, true, &m_ShadowMapDSV.GetCPU());

    if (m_gltfDepth)
    {
        GltfDepthPass::per_batch* cbDepthPerBatch = m_gltfDepth->SetPerBatchConstants();
        cbDepthPerBatch->mViewProj                = pState->light.GetView() * pState->light.GetProjection();
        m_gltfDepth->Draw(pCmdLst);
    }

    m_GPUTimer.GetTimeStamp(pCmdLst, "Shadow maps");

    // Render Scene to the MSAA HDR RT ------------------------------------------------
    //
    pCmdLst->ResourceBarrier(
        1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMap.Resource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
#endif

    pCmdLst->RSSetViewports(1, &m_ViewPort);
    pCmdLst->RSSetScissorRects(1, &m_RectScissor);
    pCmdLst->OMSetRenderTargets(1, &m_HDRRTVMSAA.GetCPU(), true, &m_DepthBufferDSV.GetCPU());

    // Render skydome
    //
    if (pState->bDrawSkyDome)
    {
        XMMATRIX clipToView = XMMatrixInverse(NULL, pState->camera.GetView() * pState->camera.GetProjection());
        m_skyDome.Draw(pCmdLst, clipToView);
    }

    // Render scene
    //
    if (m_gltfPBR)
    {
        //set per frame constant buffer values
        CMP_GltfPbr::per_batch* cbPerBatch = m_gltfPBR->SetPerBatchConstants();
        cbPerBatch->mCameraViewProj        = pState->camera.GetView() * pState->camera.GetProjection();
        cbPerBatch->cameraPos              = pState->camera.GetPosition();
        cbPerBatch->mLightViewProj         = pState->light.GetView() * pState->light.GetProjection();
        cbPerBatch->lightDirection         = pState->light.GetDirection();
        cbPerBatch->lightColor             = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f) * pState->spotLightIntensity;
        cbPerBatch->depthBias              = pState->depthBias;
        cbPerBatch->iblFactor              = pState->iblFactor;
        m_gltfPBR->Draw(pCmdLst);
        UI->m_TotalNumIndices = m_gltfPBR->m_TotalNumIndices;
    }

    // draw bounding boxes
    if (m_gltfBBox)
    {
        if (pState->bDrawBoundingBoxes)
        {
            XMMATRIX* pCam = m_gltfBBox->SetPerBatchConstants();
            *pCam          = pState->camera.GetView() * pState->camera.GetProjection();
            m_gltfBBox->Draw(pCmdLst);
        }
    }

#ifdef USE_SHADOWMAPS
    pCmdLst->ResourceBarrier(
        1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMap.Resource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE));
#endif

    m_GPUTimer.GetTimeStamp(pCmdLst, "Rendering Scene");

    // Resolve ------------------------------------------------------------------------
    //
    pCmdLst->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_HDR.Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_DEST));
    pCmdLst->ResourceBarrier(
        1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pHDRMSAA.Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE));

    pCmdLst->ResolveSubresource(m_HDR.Resource(), 0, m_pHDRMSAA.Resource(), 0, DXGI_FORMAT_R16G16B16A16_UNORM);

    pCmdLst->ResourceBarrier(
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(
            m_HDR.Resource(), D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));  // the bloom needs to read from this
    pCmdLst->ResourceBarrier(
        1, &CD3DX12_RESOURCE_BARRIER::Transition(m_pHDRMSAA.Resource(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

    m_GPUTimer.GetTimeStamp(pCmdLst, "Resolve");

#ifdef USE_BLOOM
    // Blom -------------------------------------------------------------------------------
    //
    m_bloom.Draw(pCmdLst, &m_HDR, pState->glow);
    m_GPUTimer.GetTimeStamp(pCmdLst, "Bloom");
#endif

    // Tonemapping ------------------------------------------------------------------------
    //
    pCmdLst->RSSetViewports(1, &m_ViewPort);
    pCmdLst->RSSetScissorRects(1, &m_RectScissor);
    pCmdLst->OMSetRenderTargets(1, pRenderTargetRTV, true, NULL);

    m_toneMapping.Draw(pCmdLst, &m_HDRSRV, pState->exposure, pState->toneMapper, pState->bGammaTestPattern);
    m_GPUTimer.GetTimeStamp(pCmdLst, "Tone mapping");

    // Render HUD  ------------------------------------------------------------------------
    //
    pCmdLst->RSSetViewports(1, &m_ViewPort);
    pCmdLst->RSSetScissorRects(1, &m_RectScissor);
    pCmdLst->OMSetRenderTargets(1, pRenderTargetRTV, true, NULL);

    if (UI->m_showimgui && ImGuiRenderer)
    {
        ImGuiRenderer->Draw(pCmdLst);
    }

    // Transition swapchain into present mode

    pCmdLst->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    m_GPUTimer.GetTimeStamp(pCmdLst, "ImGUI Rendering");

    m_GPUTimer.OnEndFrame();

    m_GPUTimer.CollectTimings(pCmdLst);

    // Close & Submit the command list ----------------------------------------------------
    //
    ThrowIfFailed(pCmdLst->Close());

    ID3D12CommandList* CmdListList[] = {pCmdLst};
    m_pDirectQueue->ExecuteCommandLists(1, CmdListList);

    // issue a fence so we can tell when this frame ended
    m_FrameFence.IssueFence(m_pDirectQueue);
}
