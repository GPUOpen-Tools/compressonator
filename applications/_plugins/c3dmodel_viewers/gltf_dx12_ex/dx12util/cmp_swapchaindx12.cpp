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

#include "cmp_swapchaindx12.h"

#include "cmp_error.h"


#pragma comment(lib, "dxgi.lib")

//--------------------------------------------------------------------------------------
//
// OnCreate
//
//--------------------------------------------------------------------------------------
void SwapChainDX12::OnCreate(ID3D12Device *pDevice, DWORD numberBackBuffers, std::vector<ID3D12CommandQueue *> *pQueue, DWORD dwWidth, DWORD dwHeight, HWND hWnd) {
    CreateDXGIFactory1(IID_PPV_ARGS(&m_pFactory));

    m_nodeCount = (int)pQueue->size();
    m_BackBuffersPerNodeCount = numberBackBuffers;
    m_BackBufferCount = m_BackBuffersPerNodeCount * m_nodeCount;

    m_pDevice = pDevice;
    // Describe the swap chain.
    m_descSwapChain = {};
    m_descSwapChain.BufferCount = m_BackBufferCount;
    m_descSwapChain.Width = dwWidth;
    m_descSwapChain.Height = dwHeight;
    m_descSwapChain.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    m_descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_descSwapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    m_descSwapChain.SampleDesc.Count = 1;
    m_descSwapChain.Flags = 0;// DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    // It is recommended to always use the tearing flag when it is available.
    //descSwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    m_queues.resize(m_descSwapChain.BufferCount);
    m_creationNodes.resize(m_descSwapChain.BufferCount);
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    for (UINT n = 0; n < m_descSwapChain.BufferCount; n++) {
        UINT nodeIndex = n % m_nodeCount;
        m_creationNodes[n] = 1 << nodeIndex;
        queueDesc.NodeMask = m_creationNodes[n];
        m_queues[n] = pQueue->at(nodeIndex);
    }

    IDXGISwapChain1 *pSwapChain;
    ThrowIfFailed(m_pFactory->CreateSwapChainForHwnd(
                      m_queues[0],        // Swap chain needs the queue so that it can force a flush on it.
                      hWnd,
                      &m_descSwapChain,
                      nullptr,
                      nullptr,
                      &pSwapChain
                  ));

    ThrowIfFailed(m_pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(pSwapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_pSwapChain));
    pSwapChain->Release();

    //
    // create RTV heaps
    //
    m_RTVHeaps.resize(m_nodeCount);
    for (unsigned int node = 0; node < m_RTVHeaps.size(); node++) {
        D3D12_DESCRIPTOR_HEAP_DESC descHeapRtv;
        descHeapRtv.NumDescriptors = m_descSwapChain.BufferCount;
        descHeapRtv.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        descHeapRtv.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        descHeapRtv.NodeMask = 1 << node;
        ThrowIfFailed(m_pDevice->CreateDescriptorHeap(&descHeapRtv, IID_PPV_ARGS(&m_RTVHeaps[node])));
    }
}

//--------------------------------------------------------------------------------------
//
// OnDestroy
//
//--------------------------------------------------------------------------------------
void SwapChainDX12::OnDestroy() {
    for (unsigned int node = 0; node < m_RTVHeaps.size(); node++)
        m_RTVHeaps[node]->Release();
    m_pSwapChain->Release();
    m_pFactory->Release();
}

ID3D12Resource *SwapChainDX12::GetCurrentBackBufferResource() {
    UINT mIndexLastSwapBuf = m_pSwapChain->GetCurrentBackBufferIndex();

    ID3D12Resource *pBackBuffer;
    ThrowIfFailed(m_pSwapChain->GetBuffer(mIndexLastSwapBuf, IID_PPV_ARGS(&pBackBuffer)));
    pBackBuffer->Release();
    return pBackBuffer;
}

D3D12_CPU_DESCRIPTOR_HANDLE *SwapChainDX12::GetCurrentBackBufferRTV() {
    UINT mIndexLastSwapBuf = m_pSwapChain->GetCurrentBackBufferIndex();

    return &m_CPUView[mIndexLastSwapBuf % (m_nodeCount* m_BackBuffersPerNodeCount)];
}


void SwapChainDX12::Present() {
    m_pSwapChain->Present(0, 0);
    m_currentFrame++;
}


void SwapChainDX12::SetFullScreen(bool fullscreen) {
    ThrowIfFailed(m_pSwapChain->SetFullscreenState(fullscreen, nullptr));
}


void SwapChainDX12::OnResize(DWORD dwWidth, DWORD dwHeight) {
    m_currentFrame = 0;

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
    for (UINT i = 0; i < m_descSwapChain.BufferCount; i++) {
        UINT nodeIndex = i % m_nodeCount;

        m_CPUView[i] = m_RTVHeaps[nodeIndex]->GetCPUDescriptorHandleForHeapStart();
        m_CPUView[i].ptr += colorDescriptorSize * (i / m_nodeCount);

        D3D12_GPU_DESCRIPTOR_HANDLE m_GPUView = m_RTVHeaps[nodeIndex]->GetGPUDescriptorHandleForHeapStart();
        m_GPUView.ptr += colorDescriptorSize * (i / m_nodeCount);

        ID3D12Resource *pBackBuffer;
        ThrowIfFailed(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer)));
        pBackBuffer->SetName(L"SwapChain");

        D3D12_RESOURCE_DESC desc = pBackBuffer->GetDesc();

        D3D12_RENDER_TARGET_VIEW_DESC colorDesc = {};
        colorDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        //colorDesc.Format = desc.Format;
        colorDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        colorDesc.Texture2D.MipSlice = 0;
        colorDesc.Texture2D.PlaneSlice = 0;

        m_pDevice->CreateRenderTargetView(pBackBuffer, &colorDesc, m_CPUView[i]);
        pBackBuffer->Release();
    }
}
