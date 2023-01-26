//===============================================================================
// Copyright (c) 2022  Advanced Micro Devices, Inc. All rights reserved.
//===============================================================================
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//
//  File Name:   BrotligCompute.h
//
//////////////////////////////////////////////////////////////////////////////

#include <dxgi1_4.h>
#include <wrl.h>
#include <cstdint>
#include <unordered_map>

#include <d3dx12/d3dx12.h>

using Microsoft::WRL::ComPtr;

class BrotligCompute
{
    static constexpr const wchar_t* p_sShaderName = L"BrotliGCompute.hlsl";
    static constexpr const wchar_t* p_sAltShaderName = L"plugins/compute/BrotliGCompute.hlsl";

    struct CompressedStream
    {
        size_t inputPos;    // input position
        size_t inputSize;   // input size

        size_t uncompPos;   // uncompressed position
        size_t uncompSize;  // uncompressed size

        uint32_t outputId;  // output resource id
        size_t outputPos;   // output resource position
    };

public:
    ~BrotligCompute()
    {
        if (m_fenceEvent)
            CloseHandle(m_fenceEvent);
    }

    bool Setup(uint32_t maxStreamsPerLaunch, uint32_t launchSize);

    uint32_t AddOutputBuffer(uint8_t* output);
    void RemoveOutputBuffer(uint32_t outputId);

    bool AddInput(const uint8_t* ptr, size_t size, size_t outsize, uint32_t outputId);
    bool Execute();
    void ClearInputs();

    double GetTime() const
    {
        return m_totalTime;
    }

private:

    bool InitDevice();
    void InitQueries();
    void InitResources();
    void KickoffCompute();
    void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter);
    void StartTimestamp(uint32_t idx);
    void EndTimestamp(uint32_t idx);
    double GetDeltaTime(uint32_t idx);

    ComPtr<ID3D12Device> m_device;
    bool m_useWarpDevice = false;

    std::unordered_map<uint32_t, uint8_t*> m_outputList;
    std::list<CompressedStream> m_inputs;

    uint8_t* m_pUploadPtr = nullptr;

    size_t m_inBytes = 0;
    size_t m_outBytes = 0;

    enum RootParameters : uint32_t
    {
        RootSRVInput = 0,
        RootUAVControl,
        RootUAVOutput,
        RootParametersCount
    };

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12Resource> m_uploadBuffer;
    ComPtr<ID3D12Resource> m_inBuffer;
    ComPtr<ID3D12Resource> m_outBuffer;
    ComPtr<ID3D12Resource> m_controlBuffer;
    ComPtr<ID3D12Resource> m_readbackBuffer;

    size_t m_controlSize;

    ComPtr<ID3D12Resource> m_cryptoCtxBuffer;
    bool m_cryptoInitialized = false;
    bool m_cryptoEnabled = false;

    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    ComPtr<ID3D12Fence> m_fence;
    HANDLE m_fenceEvent = nullptr;
    uint64_t m_fenceValue;

    ComPtr<ID3D12QueryHeap> m_queryHeap;
    ComPtr<ID3D12Resource> m_queryReadback;
    uint64_t* m_gpuTimes;
    double m_totalTime = 0;

    uint32_t m_launchSize = 1;
    uint32_t m_maxStreamsPerLaunch = 128;

    bool m_initialized = false;
};