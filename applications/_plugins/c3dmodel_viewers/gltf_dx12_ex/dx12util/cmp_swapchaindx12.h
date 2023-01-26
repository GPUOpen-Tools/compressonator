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
#include <dxgi1_4.h>
#include <vector>
#include <d3dx12.h>
#include <windows.h>


class SwapChainDX12 {
  public:
    void OnCreate(ID3D12Device *pDevice, DWORD numberBackBuffers, std::vector<ID3D12CommandQueue *> *pQueue, DWORD dwWidth, DWORD dwHeight, HWND hWnd);
    void OnDestroy();

    void OnResize(DWORD dwWidth, DWORD dwHeight);
    void SetFullScreen(bool fullscreen);

    void Present();
    UINT GetCurrentNode() {
        return m_currentFrame % m_nodeCount;
    }
    ID3D12Resource *GetCurrentBackBufferResource();
    D3D12_CPU_DESCRIPTOR_HANDLE *GetCurrentBackBufferRTV();

  private:
    UINT m_nodeCount = 0;
    UINT m_currentFrame = 0;
    DWORD m_BackBufferCount = 0;
    DWORD m_BackBuffersPerNodeCount = 0;

    ID3D12Device *m_pDevice = NULL;
    IDXGIFactory4 *m_pFactory = NULL;
    IDXGISwapChain3 *m_pSwapChain = NULL;

    std::vector<UINT> m_creationNodes;
    std::vector<IUnknown*> m_queues;

    std::vector<ID3D12DescriptorHeap *> m_RTVHeaps;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_CPUView;

    DXGI_SWAP_CHAIN_DESC1 m_descSwapChain = {};
};

