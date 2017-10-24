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

#include "stdafx.h"
#include "glTF_DX12Renderer.h"
#include "AmdUtil\Misc.h"
#include "d3dx12\d3dx12.h"

#ifdef DEBUG
#include <DXGIDebug.h>
#endif 

void glTF_DX12Renderer::ShowOptions(bool doshow)
{
    m_UseImGUI = doshow;
}

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void glTF_DX12Renderer::OnCreate(ID3D12Device* pDevice, ID3D12CommandQueue* pDirectQueue, UINT node, UINT nodemask)
{
    m_pDevice = pDevice;
    m_pDirectQueue = pDirectQueue;

    // Initialize helpers

    // Create heaps for the different types of resources
    m_Heaps.OnCreate(pDevice, 200, 200, 10, 2, 10, 100, nodemask);

    // Create 'dynamic' constant buffers ring
    m_ConstantBufferRing.OnCreate(pDevice, cNumSwapBufs, 200 * 1024 + 320000 * 4, &m_Heaps, 300, node, nodemask);

    // Create 'static' constant buffers pool
    m_StaticBufferPool.OnCreate(pDevice, 30 * 1024 * 1024, USE_VID_MEM, node, nodemask);
    m_StaticConstantBufferPool.OnCreate(pDevice, 2 * 1024 * 1024, &m_Heaps, 100, USE_VID_MEM, node, nodemask);

    // Create commandlist ring for the Direct queue
    m_CommandListRing.OnCreate(pDevice, m_pDirectQueue, cNumSwapBufs, 8, nodemask);

    // This fence is to track whether the next swapchain is available for rendering
    m_FrameFence.OnCreate(pDevice);

    m_GPUTimer.OnCreate(pDevice, cNumSwapBufs, node, nodemask);

    // Quick helper to upload resources, it has it's own commandList.
    m_UploadHeap.OnCreate(pDevice, 64 * 1024 * 1024, m_pDirectQueue, node, nodemask);    // initialize an upload heap (uses suballocation for faster results)

    if (m_UseImGUI)
        m_ImGUI.OnCreate(pDevice, &m_UploadHeap, &m_Heaps, &m_ConstantBufferRing, node, nodemask);

    // Make sure upload heap has finished uploading before continuing
    m_UploadHeap.FlushAndFinish();

    // Create the depth buffer view
    m_Heaps.AllocDSVDescriptor(1, &m_DepthBufferDSV);
}


//--------------------------------------------------------------------------------------
//
// OnDestroy 
//
//--------------------------------------------------------------------------------------
void glTF_DX12Renderer::OnDestroy()
{
    // Destroy ui

    if (m_UseImGUI)
        m_ImGUI.OnDestroy();

    // Destroy helpers
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
void glTF_DX12Renderer::OnCreateWindowSizeDependentResources(ID3D12Device* pDevice, DWORD Width, DWORD Height, UINT node, UINT nodemask)
{
    ID3D12GraphicsCommandList* pCmdLst = m_UploadHeap.GetCommandList();

    m_Width = Width;
    m_Height = Height;

    // Create GBuffer render targets
    m_pDepthBuffer.InitDepthStencil(pDevice, Width, Height, node, nodemask);
    m_pDepthBuffer.CreateDSV(0, &m_DepthBufferDSV);

    m_pDepthBuffer.Resource()->SetName(L"GBugger::DepthBuffer");


    // set the viewport
    mViewPort =
    {
        0.0f,
        0.0f,
        static_cast<float>(Width),
        static_cast<float>(Height),
        0.0f,
        1.0f
    };

    // create scissor rectangle
    mRectScissor = { 0, 0, (LONG)Width, (LONG)Height };

    m_UploadHeap.FlushAndFinish();
}

//--------------------------------------------------------------------------------------
//
// OnDestroyWindowSizeDependentResources
//
//--------------------------------------------------------------------------------------
void glTF_DX12Renderer::OnDestroyWindowSizeDependentResources()
{
    m_pDepthBuffer.OnDestroy();
}


//--------------------------------------------------------------------------------------
//
// LoadScene
//
//--------------------------------------------------------------------------------------
void glTF_DX12Renderer::LoadScene(GLTFLoader *gltfData, void *pluginManager, void *msghandler)
{
    m_gltf = new GlTF();

    m_gltf->OnCreate(
        m_pDevice,
        &m_UploadHeap,
        &m_Heaps,
        &m_ConstantBufferRing,
        &m_StaticConstantBufferPool,
        &m_StaticBufferPool,
        gltfData,
        pluginManager,
        msghandler
    );

    m_StaticBufferPool.UploadData(m_UploadHeap.GetCommandList());
    m_StaticConstantBufferPool.UploadData(m_UploadHeap.GetCommandList());

    m_UploadHeap.FlushAndFinish();

#if (USE_VID_MEM==true)
    m_StaticBufferPool.FreeUploadHeap();
    m_StaticConstantBufferPool.FreeUploadHeap();
#endif    
}

//--------------------------------------------------------------------------------------
//
// UnloadScene
//
//--------------------------------------------------------------------------------------
void glTF_DX12Renderer::UnloadScene()
{
    m_gltf->OnDestroy();
    delete m_gltf;
}

//--------------------------------------------------------------------------------------
//
// OnRender
//
//--------------------------------------------------------------------------------------
void glTF_DX12Renderer::OnRender(int mIndexLastSwapBuf, double elapsedTime, ID3D12Resource* pRenderTarget, D3D12_CPU_DESCRIPTOR_HANDLE *pRenderTargetSRV, Camera *pCam)
{
    // Timing values

    UINT64 timings[256];
    DWORD timingCounts = 256;

    UINT64 gpuFrequency;
    m_pDirectQueue->GetTimestampFrequency(&gpuFrequency);
    double millisecondsPerTick = 1000.0 / (double)gpuFrequency;

    double t0 = MillisecondsNow();

    // Wait for the backbuffer used cNumSwapBufs frames ago to be available
    //
    m_FrameFence.WaitForFence(cNumSwapBufs);

    // Let our resource managers do some house keeping 
    //
    m_ConstantBufferRing.OnBeginFrame();
    m_CommandListRing.OnBeginFrame();
    m_GPUTimer.OnBeginFrame(timings, &timingCounts);

    double t1 = MillisecondsNow();
    double gpuWaitingOnSwapchainTime = (t1 - t0);

    ID3D12GraphicsCommandList* pCmdLst = m_CommandListRing.GetNewCommandList();

    m_GPUTimer.GetTimeStamp(pCmdLst);

    // Clear GBuffer and depth stencil
    //
    pCmdLst->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pRenderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    pCmdLst->ClearRenderTargetView(*pRenderTargetSRV, clearColor, 0, nullptr);
    pCmdLst->ClearDepthStencilView(m_DepthBufferDSV.GetCPU(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    // Render Scene -----------------------------------------------------------------------
    //

    pCmdLst->OMSetRenderTargets(1, pRenderTargetSRV, true, &m_DepthBufferDSV.GetCPU());
    pCmdLst->RSSetViewports(1, &mViewPort);
    pCmdLst->RSSetScissorRects(1, &mRectScissor);

    m_GPUTimer.GetTimeStamp(pCmdLst);

    m_gltf->Draw(pCmdLst, pCam);

    m_GPUTimer.GetTimeStamp(pCmdLst);

    // HUD  -------------------------------------------------------------------------------
    //

    pCmdLst->OMSetRenderTargets(1, pRenderTargetSRV, true, NULL);
    pCmdLst->RSSetViewports(1, &mViewPort);
    pCmdLst->RSSetScissorRects(1, &mRectScissor);

    static double ClearingTime;
    static double RenderingTime;
    static double UITime;
    static double TotalGPUTime;

    if (m_UseImGUI)
    m_ImGUI.BeginRender();
    if (m_UseImGUI)
        {
            bool opened = true;
            ImGui::Begin("Stats", &opened, ImGuiWindowFlags_ShowBorders);
            ImGui::Text("Resolution       : %ix%i", m_Width, m_Height);
            ImGui::Text("GPU node         : %i", m_UploadHeap.GetNode());
            ImGui::Text("Clear Color/Depth: %f ms", ClearingTime);
            ImGui::Text("RenderingTime    : %f ms", RenderingTime);
            ImGui::Text("UI Time          : %f ms", UITime);
        
            //scrolling data and average computing
            static float values[128];
            values[127] = (float)TotalGPUTime;
            float avg = values[0];
            for (int i = 0; i < 128 - 1; i++) { values[i] = values[i + 1]; avg += values[i]; }
            avg /= 128;
        
            ImGui::Text("TotalGPUTime     : %f ms", avg);
            ImGui::PlotLines("", values, 128, 0, "", 0.0f, 3.0f, ImVec2(0, 80));
            ImGui::End();
        }
    if (m_UseImGUI)
        m_ImGUI.EndRender(pCmdLst);


    // Transition swapchain into present mode

    pCmdLst->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(pRenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    m_GPUTimer.GetTimeStamp(pCmdLst);

    m_GPUTimer.OnEndFrame();

    m_GPUTimer.CollectTimings(pCmdLst);

    // Close & Submit the command list ---------------------------------------------------
    //

    ThrowIfFailed(pCmdLst->Close());

    ID3D12CommandList* CmdListList[] = { pCmdLst };
    m_pDirectQueue->ExecuteCommandLists(1, CmdListList);

    // issue a fence so we can tell when this frame ended
    m_FrameFence.IssueFence(m_pDirectQueue);

    // Compute timings --------------------------------------------------------------------
    //

    ClearingTime = (timings[1] - timings[0])*millisecondsPerTick;
    RenderingTime = (timings[2] - timings[1])*millisecondsPerTick;
    UITime = (timings[3] - timings[2])*millisecondsPerTick;
    TotalGPUTime = (timings[3] - timings[0])*millisecondsPerTick;
}

