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
#include "cmp_PostProcPS.h"
#include "cmp_threadpool.h"

#include "cmp_error.h"


#include <d3dx12.h>
#include <d3dcompiler.h>

void PostProcPS::OnCreate(
    ID3D12Device* pDevice,
    UINT node,
    std::string path,
    std::string filename,
    DXGI_FORMAT outFormat,
    ResourceViewHeapsDX12 *pResourceViewHeaps,
    StaticBufferPoolDX12 *pStaticBufferPool,
    DWORD dwSRVTableSize,
    D3D12_BLEND_DESC *pBlendDesc,
    D3D12_DEPTH_STENCIL_DESC *pDepthStencilDesc,
    UINT sampleDescCount
) {
    m_pDevice = pDevice;

    m_pStaticBufferPool = pStaticBufferPool;
    m_pResourceViewHeaps = pResourceViewHeaps;

    //we need to cache the refrenced data so the lambda function can get a copy
    D3D12_BLEND_DESC blendDesc = pBlendDesc ? *pBlendDesc : CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    D3D12_DEPTH_STENCIL_DESC depthStencilBlankDesc = {};
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = pDepthStencilDesc ? *pDepthStencilDesc : depthStencilBlankDesc;

    GetThreadPool()->Add_Job([=]() {
        float vertices[] = {
            -1,  1,  1,  1,   0, 0,
                3,  1,  1,  1,   2, 0,
                -1, -3,  1,  1,   0, 2,
            };

        // Create the vertex shader
        static const char* vertexShader =
            "struct VERTEX_IN\
                {\
                    float4 vPosition : POSITION;\
                    float2 vTexture  : TEXCOORD;\
                };\
                struct VERTEX_OUT\
                {\
                    float4 vPosition : SV_POSITION; \
                    float2 vTexture : TEXCOORD; \
                }; \
                VERTEX_OUT mainVS(VERTEX_IN Input)\
                {\
                    VERTEX_OUT Output;\
                    Output.vPosition = Input.vPosition;\
                    Output.vTexture = Input.vTexture;\
                    return Output;\
                }";

        char *pVertices = NULL;
        m_pStaticBufferPool->AllocVertexBuffer(3, 6 * 4, (void **)&pVertices, &verticesView);
        memcpy(pVertices, vertices, 3 * (6 * 4));

        D3D12_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Compile shaders
        //
        ID3DBlob *pBlobShaderVert = NULL;
        ID3DBlob *pBlobShaderPixel = NULL;
        {
            // build macro structure
            //
            std::vector<D3D_SHADER_MACRO> macros;
            macros.push_back(D3D_SHADER_MACRO{ NULL, NULL });

            WCHAR wpath[1024];
            wsprintfW(wpath, L"%S%S", path.c_str(), filename.c_str());

            ID3DBlob *pError;
            D3DCompile(vertexShader, strlen(vertexShader), nullptr, macros.data(), nullptr, "mainVS", "vs_5_0", 0, 0, &pBlobShaderVert, &pError);
            D3DCompileFromFile(wpath, macros.data(), nullptr, "mainPS", "ps_5_0", 0, 0, &pBlobShaderPixel, &pError);
            if (pError != NULL) {
                char *msg = (char *)pError->GetBufferPointer();
                MessageBoxA(0, msg, "", 0);
                return;
            }
            if (pBlobShaderPixel == nullptr) {
                MessageBoxA(0, "unable to compile blob shader", "", 0);
                return;
            }
        }

        // Create root signature
        //
        {
            CD3DX12_DESCRIPTOR_RANGE DescRange[3];
            DescRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);               // b0 <- per frame
            DescRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, dwSRVTableSize, 0);  // t0 <- per material
            DescRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);            // s0 <- samplers

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
            // deny uneccessary access to certain pipeline stages
            descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

            ID3DBlob *pOutBlob, *pErrorBlob = NULL;
            ThrowIfFailed(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &pOutBlob, &pErrorBlob));
            ThrowIfFailed(
                pDevice->CreateRootSignature(node, pOutBlob->GetBufferPointer(), pOutBlob->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature))
            );
            m_pRootSignature->SetName(L"PostProcPS");

            pOutBlob->Release();
            if (pErrorBlob)
                pErrorBlob->Release();
        }

        {
            // Create a PSO description
            //
            D3D12_GRAPHICS_PIPELINE_STATE_DESC descPso = {};
            descPso.InputLayout = { layout, 2 };
            descPso.pRootSignature = m_pRootSignature;
            descPso.VS = { reinterpret_cast<BYTE*>(pBlobShaderVert->GetBufferPointer()), pBlobShaderVert->GetBufferSize() };
            descPso.PS = { reinterpret_cast<BYTE*>(pBlobShaderPixel->GetBufferPointer()), pBlobShaderPixel->GetBufferSize() };
            descPso.DepthStencilState = depthStencilDesc;
            BOOL istrue = true;
            if (depthStencilDesc.DepthEnable== istrue) {
                descPso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
            }
            descPso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            descPso.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
            descPso.BlendState = blendDesc;
            descPso.SampleMask = UINT_MAX;
            descPso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            descPso.NumRenderTargets = 1;
            descPso.RTVFormats[0] = outFormat;
            descPso.SampleDesc.Count = sampleDescCount;
            descPso.NodeMask = node;
            ThrowIfFailed(
                pDevice->CreateGraphicsPipelineState(&descPso, IID_PPV_ARGS(&m_pPipelineRender))
            );
        }

        // create samplers if not initialized (this should happen once)
        m_pResourceViewHeaps->AllocSamplerDescriptor(1, &m_sampler);

        //for pbr materials
        D3D12_SAMPLER_DESC SamplerDesc;
        ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.BorderColor[0] = 0.0f;
        SamplerDesc.BorderColor[1] = 0.0f;
        SamplerDesc.BorderColor[2] = 0.0f;
        SamplerDesc.BorderColor[3] = 0.0f;
        SamplerDesc.MinLOD = 0.0f;
        SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
        SamplerDesc.MipLODBias = 0;
        SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        SamplerDesc.MaxAnisotropy = 1;
        pDevice->CreateSampler(&SamplerDesc, m_sampler.GetCPU(0));

        m_ready = true;
    });
}

void PostProcPS::OnDestroy() {
    if (m_pPipelineRender)
        m_pPipelineRender->Release();
    if (m_pRootSignature)
        m_pRootSignature->Release();
}

void PostProcPS::Draw(ID3D12GraphicsCommandList* pCommandList, DWORD dwSRVTableSize, CBV_SRV_UAV *pSRVTable, D3D12_GPU_DESCRIPTOR_HANDLE constantBuffer) {
    if (m_ready == false)
        return;

    // Set state and draw
    //
    ID3D12DescriptorHeap *pDescriptorHeaps[] = { m_pResourceViewHeaps->GetCBV_SRV_UAVHeap(), m_pResourceViewHeaps->GetSamplerHeap() };
    pCommandList->SetDescriptorHeaps(2, pDescriptorHeaps);

    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pCommandList->SetPipelineState(m_pPipelineRender);
    pCommandList->SetGraphicsRootSignature(m_pRootSignature);

    pCommandList->IASetVertexBuffers(0, 1, &verticesView);

    pCommandList->SetGraphicsRootDescriptorTable(0, constantBuffer);
    pCommandList->SetGraphicsRootDescriptorTable(1, pSRVTable->GetGPU());
    pCommandList->SetGraphicsRootDescriptorTable(2, m_sampler.GetGPU());

    pCommandList->DrawInstanced(3, 1, 0, 0);
}
