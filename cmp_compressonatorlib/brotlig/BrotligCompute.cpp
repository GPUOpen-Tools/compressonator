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
//  File Name:   BrotligCompute.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <dxcapi.h>

#include <stdexcept>
#include <iostream>

#include "BrotligCompute.h"
#include "DataStream.h"

using namespace BrotliG;

template<int N, typename T>
static inline T align(T a)
{
    return (a + T(N) - 1) & ~(N - 1);
}

inline std::string HrToString(HRESULT hr)
{
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}

class HrException : public std::runtime_error
{
public:
    HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
    HRESULT Error() const { return m_hr; }
private:
    const HRESULT m_hr;
};

#define SAFE_RELEASE(p) if (p) (p)->Release()

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw HrException(hr);
    }
}

// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
    pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
    WCHAR fullName[50];
    if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
    {
        pObject->SetName(fullName);
    }
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// Naming helper for ComPtr<T>.
// Assigns the name of the variable as the name of the object.
// The indexed variant will include the index in the name of the object.
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

class PageStream
{
public:
    static constexpr uint32_t sMaxTiles = (1 << 10) - 1;
    static constexpr uint32_t sBrotligId = BROTLIG_STREAM_ID;

#pragma pack(push, 1)

    struct Header
    {
        uint8_t id;
        uint8_t magic;

        uint16_t startTile;

        uint32_t maxReadSizeIdx : 2;
        uint32_t tileSizeIdx : 2;
        uint32_t numTiles : 10;
        uint32_t lastTileSize : 18;

        inline bool IsValid() const
        {
            return id == (magic ^ 0xff);
        }

        inline size_t GetUncompressedSize() const
        {
            return numTiles * GetTileSize() -
                (lastTileSize == 0 ? 0 : GetTileSize() - lastTileSize);
        }

        inline size_t GetTileSize() const
        {
            return BROTLIG_MIN_PAGE_SIZE << tileSizeIdx;
        }

        inline size_t GetMaxReadSize() const
        {
            return BROTLIG_MIN_MAX_READ_SIZE << maxReadSizeIdx;
        }
    };

#pragma pack(pop)

    static_assert(sizeof(Header) == 8, "Tile stream header size overrun!");
};

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_
void BrotligCompute::GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
{
    ComPtr<IDXGIAdapter1> adapter;
    *ppAdapter = nullptr;

    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || desc.VendorId == 0x8086 /* Intel */)
        {
            // Don't select the Basic Render Driver adapter.
            // If you want a software adapter, pass in "/warp" on the command line.
            continue;
        }

        // Check to see if the adapter supports Direct3D 12, but don't create the
        // actual device yet.
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
        {
            wprintf(L"\nUsing: %s\n", desc.Description);
            break;
        }
    }

    *ppAdapter = adapter.Detach();
}

bool BrotligCompute::Setup(uint32_t maxStreamsPerLaunch, uint32_t launchSize)
{
    if (!m_device && !InitDevice())
        return false;

    maxStreamsPerLaunch = std::min<uint32_t>(maxStreamsPerLaunch, BROTLIG_GPUD_DEFAULT_MAX_STREAMS_PER_LAUNCH);

    if (!m_initialized || m_maxStreamsPerLaunch != maxStreamsPerLaunch ||
        m_launchSize != launchSize)
    {
        m_maxStreamsPerLaunch = maxStreamsPerLaunch;
        m_launchSize = launchSize;

        InitResources();
    }

    m_initialized = true;

    return true;
}

bool BrotligCompute::InitDevice()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    if (m_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_12_0,
            IID_PPV_ARGS(&m_device)
        ));

        DXGI_ADAPTER_DESC desc;
        warpAdapter->GetDesc(&desc);

        wprintf(L"\nUsing: %s\n", desc.Description);
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_12_0,
            IID_PPV_ARGS(&m_device)
        ));
    }

    D3D12_FEATURE_DATA_SHADER_MODEL model{ D3D_SHADER_MODEL_6_5 };
    ThrowIfFailed(m_device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &model, sizeof(model)));

    if (model.HighestShaderModel < D3D_SHADER_MODEL_6_0)
    {
        printf("Fatal: Device does not support Shader Model 6.0\n");
        m_device.Reset();
        return false;
    }

    return true;
}

void BrotligCompute::InitResources()
{
    // Queue
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = { D3D12_COMMAND_LIST_TYPE_COMPUTE, 0, D3D12_COMMAND_QUEUE_FLAG_NONE };
        ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
        ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&m_commandAllocator)));
        ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE,
            m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
        ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&m_fence)));

        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    }

    // Pipeline
    {
        // Compute root signature.
        {
            CD3DX12_ROOT_PARAMETER1 rootParameters[RootParametersCount];
            rootParameters[RootSRVInput].InitAsShaderResourceView(0);
            rootParameters[RootUAVControl].InitAsUnorderedAccessView(0);
            rootParameters[RootUAVOutput].InitAsUnorderedAccessView(1);

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC computeRootSignatureDesc;
            computeRootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr);

            ComPtr<ID3DBlob> signature;
            ComPtr<ID3DBlob> error;
            ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&computeRootSignatureDesc,
                D3D_ROOT_SIGNATURE_VERSION_1_1, &signature, &error));

            if (error)
                printf("Root signature creation failed with: %s\n", (LPCSTR)error->GetBufferPointer());

            ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                IID_PPV_ARGS(&m_rootSignature)));

            NAME_D3D12_OBJECT(m_rootSignature);
        }

        ComPtr<IDxcLibrary> library;
        HRESULT hr;
        ThrowIfFailed(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library)));

        ComPtr<IDxcCompiler> compiler;
        ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));

        ComPtr<IDxcIncludeHandler> includeHandler;
        library->CreateIncludeHandler(&includeHandler);

        uint32_t codePage = CP_UTF8;
        ComPtr<IDxcBlobEncoding> sourceBlob;
        hr = library->CreateBlobFromFile(p_sShaderName, &codePage, &sourceBlob);
        if (FAILED(hr))
        {
            hr = library->CreateBlobFromFile(p_sAltShaderName, &codePage, &sourceBlob);
            if (FAILED(hr))
            {
                wprintf(L"BrotligCompute.hlsl not found in the current directory. Also tried %s", p_sAltShaderName);
                exit(-1);
            }
        }

        // Check supported shader model and SIMD width and configure the decompressor kernel accordingly
        D3D12_FEATURE_DATA_SHADER_MODEL model{ D3D_SHADER_MODEL_6_5 };
        ThrowIfFailed(m_device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &model, sizeof(model)));

        D3D12_FEATURE_DATA_D3D12_OPTIONS1 options1;
        ThrowIfFailed(m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &options1, sizeof(options1)));

        D3D12_FEATURE_DATA_D3D12_OPTIONS4 options4;
        ThrowIfFailed(m_device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS4, &options4, sizeof(options4)));

        printf("SIMD width of the machine: %d\n", options1.WaveLaneCountMin);
        printf("Maximum supported shader model: %d.%d\n", model.HighestShaderModel >> 4, model.HighestShaderModel & 15);
        printf("Wave intrinsics supported: ");
        printf(options1.WaveOps ? "Yes\n" : "No\n");
        printf("16-bit types supported: ");
        printf(options4.Native16BitShaderOpsSupported ? "Yes\n" : "No\n");

        bool bUseWaveIntrinsics = options1.WaveOps;
        bool bUseWaveMatch = model.HighestShaderModel >= D3D_SHADER_MODEL_6_5;
        bool b16bitTypes = options4.Native16BitShaderOpsSupported;

        WCHAR simdWidth[32];
        swprintf_s(simdWidth, L"-DSIMD_WIDTH=%d", options1.WaveLaneCountMin);

        /*LPCWSTR args[] = {
            L"-I..\\..\\brotlig\\src\\decoder\\",
            L"-O3",
            simdWidth,
            L"",
            bUseWaveIntrinsics ? L"-DUSE_WAVE_INTRINSICS" : L"",
            bUseWaveMatch ? L"-DUSE_WAVE_MATCH" : L"",
            b16bitTypes ? L"-enable-16bit-types" : L""
        };*/

        LPCWSTR args[] = {
            L"-I..\\..\\brotlig\\src\\decoder\\"
        };

        static LPCWSTR shaderModelName[] = { L"cs_6_0", L"cs_6_1", L"cs_6_2", L"cs_6_3", L"cs_6_4", L"cs_6_5" };

        ComPtr<IDxcOperationResult> result;
        hr = compiler->Compile(sourceBlob.Get(), p_sShaderName, L"CSMain", shaderModelName[model.HighestShaderModel & 15], args, _countof(args), NULL, 0, includeHandler.Get(), &result);

        if (SUCCEEDED(hr))
            result->GetStatus(&hr);

        if (FAILED(hr))
        {
            if (result)
            {
                ComPtr<IDxcBlobEncoding> errorsBlob;
                hr = result->GetErrorBuffer(&errorsBlob);
                if (SUCCEEDED(hr) && errorsBlob)
                {
                    printf("Kernel compilation failed with: %s\n", (const char*)errorsBlob->GetBufferPointer());
                    exit(-1);
                }
            }
            // Handle compilation error...
        }

        ComPtr<IDxcBlob> computeShader;
        result->GetResult(&computeShader);

        D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineDesc = {};
        pipelineDesc.pRootSignature = m_rootSignature.Get();

        pipelineDesc.CS.BytecodeLength = computeShader->GetBufferSize();
        pipelineDesc.CS.pShaderBytecode = computeShader->GetBufferPointer();

        // ThrowIfFailed(m_device->CreateComputePipelineState(&pipelineDesc, IID_PPV_ARGS(&m_pipelineState)));
        hr = m_device->CreateComputePipelineState(&pipelineDesc, IID_PPV_ARGS(&m_pipelineState));
        if (!m_pipelineState) {
            printf("Fatal: pipeline state creation failed\n");
            exit(-1);
        }
        NAME_D3D12_OBJECT(m_pipelineState);
    }

    {
        m_controlSize = sizeof(uint32_t) + 3 * sizeof(uint32_t) * m_maxStreamsPerLaunch;

        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(BROTLIG_GPUD_DEFAULT_MAX_TEMP_BUFFER_SIZE + m_controlSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_uploadBuffer)));

        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(BROTLIG_GPUD_DEFAULT_MAX_TEMP_BUFFER_SIZE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_inBuffer)));

        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(BROTLIG_GPUD_DEFAULT_MAX_TEMP_BUFFER_SIZE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_outBuffer)));

        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(m_controlSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_controlBuffer)));

        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(BROTLIG_GPUD_DEFAULT_MAX_TEMP_BUFFER_SIZE),
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_readbackBuffer)));

        m_uploadBuffer->Map(0, nullptr, (void**)&m_pUploadPtr);
    }

    InitQueries();
}

void BrotligCompute::InitQueries()
{
    D3D12_QUERY_HEAP_DESC heapDesc{};
    heapDesc.Count = BROTLIG_GPUD_DEFAULT_MAX_QUERIES;
    heapDesc.NodeMask = 0;
    heapDesc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
    m_device->CreateQueryHeap(&heapDesc, IID_PPV_ARGS(&m_queryHeap));

    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(BROTLIG_GPUD_DEFAULT_MAX_QUERIES * sizeof(uint64_t)),
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_queryReadback)));

    ThrowIfFailed(m_queryReadback->Map(0, nullptr, reinterpret_cast<void**>(&m_gpuTimes)));
}

void BrotligCompute::KickoffCompute()
{
    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(1, ppCommandLists);

    UINT64 fenceValue = InterlockedIncrement(&m_fenceValue);
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fenceValue));
    ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
    WaitForSingleObject(m_fenceEvent, INFINITE);

    ThrowIfFailed(m_commandAllocator->Reset());
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));
}

void BrotligCompute::StartTimestamp(uint32_t idx)
{
    assert(idx * 2 < BROTLIG_GPUD_DEFAULT_MAX_QUERIES);
    m_commandList->EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, idx * 2);
}

void BrotligCompute::EndTimestamp(uint32_t idx)
{
    assert(idx * 2 < BROTLIG_GPUD_DEFAULT_MAX_QUERIES);

    m_commandList->EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, idx * 2 + 1);

    // Resolve the data
    const uint64_t dstOffset = idx * 2 * sizeof(uint64_t);
    m_commandList->ResolveQueryData(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP,
        idx * 2, 2, m_queryReadback.Get(), dstOffset);
}

double BrotligCompute::GetDeltaTime(uint32_t idx)
{
    assert(idx * 2 < BROTLIG_GPUD_DEFAULT_MAX_QUERIES);
    uint64_t frequency;
    m_commandQueue->GetTimestampFrequency(&frequency);

    return 1e6 / frequency * (m_gpuTimes[idx * 2 + 1] - m_gpuTimes[idx * 2]);
}

uint32_t BrotligCompute::AddOutputBuffer(uint8_t* output)
{
    static uint32_t ID = 1;

    m_outputList[ID] = output;

    return ID++;
}

void BrotligCompute::RemoveOutputBuffer(uint32_t outputId)
{
    m_outputList.erase(outputId);
}

bool BrotligCompute::AddInput(const uint8_t* ptr, size_t size, size_t outsize, uint32_t outputId)
{
    if (m_inputs.size() == m_maxStreamsPerLaunch ||
        m_inBytes + align<BROTLIG_DATA_ALIGNMENT>(size) > BROTLIG_GPUD_DEFAULT_MAX_TEMP_BUFFER_SIZE ||
        m_outBytes + align<BROTLIG_DATA_ALIGNMENT>(outsize) > BROTLIG_GPUD_DEFAULT_MAX_TEMP_BUFFER_SIZE)
    {
        return false;
    }

    auto* header = reinterpret_cast<const StreamHeader*>(ptr);

    if (!header->Validate())
    {
        throw("Corrupt stream.\n");
    }

    if (header->Id != PageStream::sBrotligId)
    {
        throw("Incorrect stream format: %d\n", header->Id);
    }

    const size_t outputSize = header->UncompressedSize();

    if (m_outBytes + align<BROTLIG_DATA_ALIGNMENT>(outputSize) > BROTLIG_GPUD_DEFAULT_MAX_TEMP_BUFFER_SIZE)
    {
        if (m_inputs.size() == 0) throw("Staging buffer is too small");
        return false;
    }

    if (m_outputList.find(outputId) == m_outputList.end())
    {
        throw("Output id not found");
    }

    memcpy(m_pUploadPtr + m_inBytes, ptr, size);

    CompressedStream stream;

    stream.inputPos = m_inBytes;
    stream.inputSize = size;

    stream.uncompPos = m_outBytes;
    stream.uncompSize = outputSize;

    stream.outputId = outputId;

    m_inputs.push_back(stream);

    m_inBytes += align<BROTLIG_DATA_ALIGNMENT>(size);
    m_outBytes += align<BROTLIG_DATA_ALIGNMENT>(outputSize);

    return true;
}

void BrotligCompute::ClearInputs()
{
    m_inputs.clear();
    m_inBytes = 0;
    m_outBytes = 0;
}

bool BrotligCompute::Execute()
{
    if (m_inputs.empty() || m_inBytes == 0)
        return false;

    uint32_t totalOutSize = 0;

    // Prepare and upload the control stream
    {
        uint32_t* pControl = reinterpret_cast<uint32_t*>(m_pUploadPtr + BROTLIG_GPUD_DEFAULT_MAX_TEMP_BUFFER_SIZE);

        *pControl++ = static_cast<uint32_t>(m_inputs.size());

        for (auto& stream : m_inputs)
        {
            *pControl++ = static_cast<uint32_t>(stream.inputPos);
            *pControl++ = static_cast<uint32_t>(stream.uncompPos);
            *pControl++ = 0;
        }

        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_controlBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

        m_commandList->CopyBufferRegion(m_controlBuffer.Get(), 0, m_uploadBuffer.Get(),
            BROTLIG_GPUD_DEFAULT_MAX_TEMP_BUFFER_SIZE, 4 + m_inputs.size() * 12);

        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_controlBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
    }

    // Upload input (sub)streams
    {
        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_inBuffer.Get(),
            D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

        m_commandList->CopyBufferRegion(m_inBuffer.Get(), 0, m_uploadBuffer.Get(), 0, m_inBytes);

        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_inBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON));
    }

    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_outBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

    m_commandList->SetPipelineState(m_pipelineState.Get());
    m_commandList->SetComputeRootSignature(m_rootSignature.Get());

    m_commandList->SetComputeRootShaderResourceView(RootSRVInput, m_inBuffer->GetGPUVirtualAddress());
    m_commandList->SetComputeRootUnorderedAccessView(RootUAVOutput, m_outBuffer->GetGPUVirtualAddress());
    m_commandList->SetComputeRootUnorderedAccessView(RootUAVControl, m_controlBuffer->GetGPUVirtualAddress());

    StartTimestamp(0);
    m_commandList->Dispatch(m_launchSize, 1, 1);
    EndTimestamp(0);

    // Download output
    {
        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_outBuffer.Get(),
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE));

        m_commandList->CopyBufferRegion(m_readbackBuffer.Get(), 0, m_outBuffer.Get(), 0, m_outBytes);

        m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_outBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
    }

    KickoffCompute();

    UINT8* pData = nullptr;
    m_readbackBuffer->Map(0, nullptr, (void**)&pData);

    for (const auto& stream : m_inputs)
    {
        const auto it2 = m_outputList.find(stream.outputId);
        if (it2 != m_outputList.end())
        {
            const auto& output = it2->second;
            UINT8* tptr = pData + stream.uncompPos;
            memcpy(output,
                tptr, stream.uncompSize);
        }
    }

    m_inputs.clear();

    m_inBytes = 0;
    m_outBytes = 0;

    m_readbackBuffer->Unmap(0, nullptr);

    m_totalTime += GetDeltaTime(0);

    return true;
}