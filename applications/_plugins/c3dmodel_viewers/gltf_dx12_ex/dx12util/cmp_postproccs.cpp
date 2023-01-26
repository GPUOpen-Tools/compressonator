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

#include "cmp_dynamicbufferringdx12.h"
#include "cmp_staticbufferpooldx12.h"
#include "cmp_uploadheapdx12.h"
#include "cmp_texturedx12.h"
#include "cmp_postproccs.h"

#include "cmp_error.h"

#include <d3dcompiler.h>

PostProcCS::PostProcCS() {
}

PostProcCS::~PostProcCS() {
}

void PostProcCS::OnCreate(
    ID3D12Device* pDevice,
    UINT node,
    ResourceViewHeapsDX12 *pResourceViewHeaps,
    DynamicBufferRingDX12 *pDynamicBufferRing,
    DWORD UAVTableSize,
    CBV_SRV_UAV *pUAVTable,
    DWORD dwTileX, DWORD dwTileY, DWORD dwTileZ,
    DWORD dwWidth, DWORD dwHeigth, DWORD dwDepth
) {
    m_pDevice = pDevice;
    m_pUAVTable = pUAVTable;
    m_dwThreadGroupCountX = (DWORD)ceil((float)dwWidth  / dwTileX);
    m_dwThreadGroupCountY = (DWORD)ceil((float)dwHeigth / dwTileY);
    m_dwThreadGroupCountZ = (DWORD)ceil((float)dwDepth  / dwTileZ);

    m_pResourceViewHeaps = pResourceViewHeaps;

    // Compile shaders
    //
    ID3DBlob *pBlobShader;
    {
        // build macro structure
        //
        std::vector<D3D_SHADER_MACRO> macros;
        macros.push_back(D3D_SHADER_MACRO{"TILE_X",std::to_string(dwTileX).c_str() });
        macros.push_back(D3D_SHADER_MACRO{"TILE_Y",std::to_string(dwTileY).c_str() });
        macros.push_back(D3D_SHADER_MACRO{"TILE_Z",std::to_string(dwTileZ).c_str() });
        macros.push_back(D3D_SHADER_MACRO{"WIDTH",std::to_string(dwWidth).c_str() });
        macros.push_back(D3D_SHADER_MACRO{"HEIGHT",std::to_string(dwHeigth).c_str() });
        macros.push_back(D3D_SHADER_MACRO{"DEPTH",std::to_string(dwDepth).c_str() });
        macros.push_back(D3D_SHADER_MACRO{ NULL, NULL });

        ID3DBlob *pError;
        D3DCompileFromFile(L"./plugins/shaders/Blur.hlsl", macros.data(), nullptr, "mainCS", "vs_5_0", 0, 0, &pBlobShader, &pError);
        if (pError != NULL) {
            char *msg = (char *)pError->GetBufferPointer();
            MessageBoxA(0, msg, "", 0);
        }
    }

    // Create root signature
    //
    {
        CD3DX12_DESCRIPTOR_RANGE DescRange[3];
        DescRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);             // b0 <- per frame
        DescRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UAVTableSize, 0);  // u0 <- per material
        DescRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);          // s0 <- samplers

        CD3DX12_ROOT_PARAMETER RTSlot[3];
        RTSlot[0].InitAsDescriptorTable(1, &DescRange[0], D3D12_SHADER_VISIBILITY_ALL);
        RTSlot[1].InitAsDescriptorTable(1, &DescRange[1], D3D12_SHADER_VISIBILITY_ALL);
        RTSlot[2].InitAsDescriptorTable(1, &DescRange[2], D3D12_SHADER_VISIBILITY_ALL);

        // the root signature contains 3 slots to be used
        CD3DX12_ROOT_SIGNATURE_DESC descRootSignature = CD3DX12_ROOT_SIGNATURE_DESC();
        descRootSignature.NumParameters = 3;
        descRootSignature.pParameters = RTSlot;
        descRootSignature.NumStaticSamplers = 0;
        descRootSignature.pStaticSamplers = NULL;

        // deny uneccessary access to certain pipeline stages
        descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

        ID3DBlob *pOutBlob, *pErrorBlob = NULL;
        ThrowIfFailed(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &pOutBlob, &pErrorBlob));
        ThrowIfFailed(
            pDevice->CreateRootSignature(node,pOutBlob->GetBufferPointer(),pOutBlob->GetBufferSize(),IID_PPV_ARGS(&m_pRootSignature))
        );
        m_pRootSignature->SetName(L"PostProcCS");

        pOutBlob->Release();
        if (pErrorBlob)
            pErrorBlob->Release();
    }

    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC descPso = {};
        descPso.CS = { reinterpret_cast<BYTE*>(pBlobShader->GetBufferPointer()), pBlobShader->GetBufferSize() };
        descPso.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        descPso.pRootSignature = m_pRootSignature;
        descPso.NodeMask = node;
    }
}

void PostProcCS::OnDestroy() {
    m_pRootSignature->Release();
}

void PostProcCS::Draw(ID3D12GraphicsCommandList* pCommandList, D3D12_GPU_DESCRIPTOR_HANDLE constantBuffer) {
    ID3D12DescriptorHeap *pDescriptorHeaps[] = { m_pResourceViewHeaps->GetCBV_SRV_UAVHeap(), m_pResourceViewHeaps->GetSamplerHeap() };
    pCommandList->SetDescriptorHeaps(2, pDescriptorHeaps);

    pCommandList->SetComputeRootSignature(m_pRootSignature);
    pCommandList->SetGraphicsRootDescriptorTable(0, constantBuffer);
    pCommandList->SetComputeRootDescriptorTable(1, m_pUAVTable->GetGPU());
    pCommandList->Dispatch(m_dwThreadGroupCountX, m_dwThreadGroupCountY, m_dwThreadGroupCountZ);
}
