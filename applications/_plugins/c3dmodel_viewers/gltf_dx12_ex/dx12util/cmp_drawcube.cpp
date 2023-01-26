//=====================================================================
// Copyright 2018 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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

#include "cmp_drawcube.h"

#include "cmp_error.h"

#include <d3dx12.h>
#include <d3dcompiler.h>


DrawCube::DrawCube() {
}


DrawCube::~DrawCube() {
}

void DrawCube::OnCreate(ID3D12Device* pDevice, UINT node, UploadHeapDX12* pUploadHeap, ResourceViewHeapsDX12 *pHeaps, DynamicBufferRingDX12 *pDynamicBufferRing, StaticBufferPoolDX12 *pStaticBufferPool) {
    m_pResourceViewHeaps = pHeaps;
    m_pDynamicBufferRing = pDynamicBufferRing;

    m_NumIndices = 32;

    void *pDest;
    pStaticBufferPool->AllocIndexBuffer(m_NumIndices, sizeof(DWORD), &pDest, &m_IBV);
    DWORD indices[] = {
        0,1, 1,2, 2,3, 3,0,
        4,5, 5,6, 6,7, 7,4,
        0,4,
        1,5,
        2,6,
        3,7
    };
    memcpy(pDest, indices, m_IBV.SizeInBytes);

    pStaticBufferPool->AllocVertexBuffer(8, 4 * sizeof(float), &pDest, &m_VBV);
    float vertices[] = {
        -1,  -1,  1,  1,
            1,  -1,  1,  1,
            1,   1,  1,  1,
            -1,   1,  1,  1,
            -1,  -1, -1,  1,
            1,  -1, -1,  1,
            1,   1, -1,  1,
            -1,   1, -1,  1,
        };
    memcpy(pDest, vertices, m_VBV.SizeInBytes);

    D3D12_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };


    // the vertex shader
    static const char* vertexShader = "\
        cbuffer cbPerObject: register(b0)\
        {\
            matrix        u_mWorld;\
            float4        u_Center;\
            float4        u_Radius;\
            float4        u_Color;\
        }\
        struct VERTEX_IN\
        {\
            float4 vPosition : POSITION;\
        };\
        struct VERTEX_OUT\
        {\
            float4 vPosition : SV_POSITION; \
            float4 vColor : COLOR; \
        }; \
        VERTEX_OUT mainVS(VERTEX_IN Input)\
        {\
            VERTEX_OUT Output;\
            Output.vPosition = mul(u_mWorld, (u_Center + Input.vPosition * float4(u_Radius.xyz,1)));\
            Output.vColor = u_Color;\
            return Output;\
        }";

    // the pixel shader
    static const char* pixelShader = "\
        struct VERTEX_IN\
        {\
            float4 vPosition : SV_POSITION; \
            float4 vColor : COLOR; \
        }; \
        float4 mainPS(VERTEX_IN Input)  : SV_Target\
        {\
            return Input.vColor;\
        }";

    // Compile shaders
    //
    ID3DBlob *pBlobShaderVert = NULL;
    ID3DBlob *pBlobShaderPixel = NULL;
    {
        ID3DBlob *pError;
        D3DCompile(vertexShader, strlen(vertexShader), nullptr, nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &pBlobShaderVert, &pError);
        D3DCompile(pixelShader, strlen(pixelShader), nullptr, nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pBlobShaderPixel, &pError);
        if (pError != NULL) {
            char *msg = (char *)pError->GetBufferPointer();
            MessageBoxA(0, msg, "", 0);
        }
    }

    // Create root signature
    //
    {
        CD3DX12_DESCRIPTOR_RANGE DescRange[1];
        CD3DX12_ROOT_PARAMETER RTSlot[1];
        CD3DX12_ROOT_SIGNATURE_DESC descRootSignature = CD3DX12_ROOT_SIGNATURE_DESC();

        DescRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);        // b0 <- per object color

        RTSlot[0].InitAsDescriptorTable(1, &DescRange[0], D3D12_SHADER_VISIBILITY_VERTEX);

        // the root signature contains 3 slots to be used
        descRootSignature.NumParameters = 1;
        descRootSignature.pParameters = RTSlot;
        descRootSignature.NumStaticSamplers = 0;
        descRootSignature.pStaticSamplers = NULL;

        // deny uneccessary access to certain pipeline stages
        descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE
                                  | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
                                  | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
                                  | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
                                  | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

        ID3DBlob *pOutBlob, *pErrorBlob = NULL;
        D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &pOutBlob, &pErrorBlob);
        if (pErrorBlob != NULL) {
            char *msg = (char *)pErrorBlob->GetBufferPointer();
            MessageBoxA(0, msg, "", 0);
        }

        ThrowIfFailed(
            pDevice->CreateRootSignature(
                node,
                pOutBlob->GetBufferPointer(),
                pOutBlob->GetBufferSize(),
                IID_PPV_ARGS(&m_RootSignature))
        );
        m_RootSignature->SetName(L"OnCreatePrimitiveDepthPass");

        pOutBlob->Release();
        if (pErrorBlob)
            pErrorBlob->Release();
    }

    // Create a PSO description
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC descPso = {};
    descPso.InputLayout = { layout, 1 };
    descPso.pRootSignature = m_RootSignature.Get();
    descPso.VS = { reinterpret_cast<BYTE*>(pBlobShaderVert->GetBufferPointer()), pBlobShaderVert->GetBufferSize() };
    descPso.PS = { reinterpret_cast<BYTE*>(pBlobShaderPixel->GetBufferPointer()), pBlobShaderPixel->GetBufferSize() };
    descPso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    descPso.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    descPso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    descPso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    descPso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    descPso.SampleMask = UINT_MAX;
    descPso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    descPso.NumRenderTargets = 1;
    descPso.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_UNORM;
    descPso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    descPso.SampleDesc.Count = 4;
    descPso.NodeMask = node;
    ThrowIfFailed(
        pDevice->CreateGraphicsPipelineState(&descPso, IID_PPV_ARGS(&m_PipelineRender))
    );
}


void DrawCube::Draw(ID3D12GraphicsCommandList* pCommandList, DWORD dwCount, per_object *pMatrixList) {

    ID3D12DescriptorHeap *pDescriptorHeaps[] = { m_pResourceViewHeaps->GetCBV_SRV_UAVHeap() };

    pCommandList->IASetIndexBuffer(&m_IBV);
    pCommandList->IASetVertexBuffers(0, 1, &m_VBV);
    pCommandList->SetDescriptorHeaps(1, pDescriptorHeaps);
    pCommandList->SetPipelineState(m_PipelineRender.Get());
    pCommandList->SetGraphicsRootSignature(m_RootSignature.Get());
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);


    // Set per Object constants
    //
    for (DWORD i = 0; i < dwCount; i++) {
        per_object *cbPerObject;
        D3D12_GPU_DESCRIPTOR_HANDLE perObjectDesc;
        m_pDynamicBufferRing->AllocConstantBuffer(sizeof(per_object), (void **)&cbPerObject, &perObjectDesc);
        *cbPerObject = pMatrixList[i];

        pCommandList->SetGraphicsRootDescriptorTable(0, perObjectDesc);

        pCommandList->DrawIndexedInstanced(m_NumIndices, 1, 0, 0, 0);
    }
}