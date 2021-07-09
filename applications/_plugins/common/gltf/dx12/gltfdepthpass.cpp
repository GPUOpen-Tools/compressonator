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

#include "gltfdepthpass.h"
#include "gltfhelpers.h"
#include "threadpool.h"


#include "gltfgetformat_dx12.h"
#include "gltfhelpers_dx12.h"

#include <error.h>

#include <d3dcompiler.h>

using namespace DirectX;

bool GltfDepthPass::OnCreate(
    ID3D12Device* pDevice,
    UploadHeapDX12* pUploadHeap,
    ResourceViewHeapsDX12 *pHeaps,
    DynamicBufferRingDX12 *pDynamicBufferRing,
    StaticBufferPoolDX12 *pStaticBufferPool,
    GLTFCommon *pGLTFData, void *pluginManager, void *msghandler) {
    m_pGLTFData = pGLTFData;
    m_pDynamicBufferRing = pDynamicBufferRing;
    m_pResourceViewHeaps = pHeaps;
    m_pStaticBufferPool = pStaticBufferPool;

    json &j3 = pGLTFData->j3;

    // Load Textures (A cache should take care of deduplication)
    //
    auto images = j3["images"];
    if (images.size() > 0) {
        m_textures.resize(images.size());
        for (unsigned int i = 0; i < images.size(); i++) {
            std::string filename = images[i]["uri"];
            WCHAR wcstrPath[MAX_PATH];
            MultiByteToWideChar(CP_UTF8, 0, (pGLTFData->m_path + filename).c_str(), -1, wcstrPath, MAX_PATH);

            INT32 result = m_textures[i].InitFromFile(pDevice, pUploadHeap, wcstrPath, pluginManager, msghandler);
        }
    }

    pUploadHeap->FlushAndFinish();

    // Create materials (in a depth pass materials are still needed to handle non opaque textures
    //
    std::vector<DepthMaterial *> materialsData;
    auto materials = j3["materials"];
    auto textures = j3["textures"];
    if (materials.size() > 0) {
        for (unsigned int i = 0; i < materials.size(); i++) {
            json::object_t material = materials[i];

            DepthMaterial *tfmat = new DepthMaterial();
            materialsData.push_back(tfmat);

            // Load material constants. This is a depth pass and we are only interested in the txtures that are transparent
            //
            std::string opaque = GetElementString(material, "alphaMode", "OPAQUE");
            if (opaque != "OPAQUE") {
                tfmat->m_defines["DEF_alphaMode_" + GetElementString(material, "alphaMode", "OPAQUE")] = 1;
                tfmat->m_defines["DEF_alphaCutoff"] = std::to_string(GetElementFloat(material, "alphaCutoff", 1.0));

                // If transparent create glTF 2.0 baseColorTexture SRV
                //
                if (textures.size() > 0) {
                    int id = GetElementInt(material, "pbrMetallicRoughness/baseColorTexture/index", -1);
                    if (id > 0) {
                        //allocate descriptor table for the textures
                        tfmat->m_pTransparency = new CBV_SRV_UAV();
                        pHeaps->AllocCBV_SRV_UAVDescriptor(1, tfmat->m_pTransparency);

                        int source = textures[id]["source"];
                        m_textures[source].CreateSRV(0, tfmat->m_pTransparency);

                        tfmat->m_defines["ID_baseColorTexture"] = "0";
                    }
                }
            }
        }
    }

    // Load Meshes
    //
    auto accessors = j3["accessors"];
    auto bufferViews = j3["bufferViews"];
    auto meshes = j3["meshes"];
    m_meshes.resize(meshes.size());
    for (unsigned int i = 0; i < meshes.size(); i++) {
        DepthMesh *tfmesh = &m_meshes[i];

        auto primitives = meshes[i]["primitives"];
        tfmesh->m_pPrimitives.resize(primitives.size());
        for (unsigned int p = 0; p < primitives.size(); p++) {
            DepthPrimitives *pPrimitive = &tfmesh->m_pPrimitives[p];

            // Set Material
            //
            pPrimitive->m_pMaterial = materialsData[primitives[p]["material"]];

            // Set Index buffer
            //
            tfAccessor indexBuffer;
            {
                json::object_t indicesAccessor = accessors[primitives[p]["indices"].get<int>()];
                GetBufferDetails(indicesAccessor, bufferViews, pGLTFData->buffersData, &indexBuffer);
            }

            // Get input layout from glTF attributes
            //
            std::vector<tfAccessor> vertexBuffers;
            std::vector<std::string> semanticNames;
            std::vector<D3D12_INPUT_ELEMENT_DESC> layout;

            auto attribute = primitives[p]["attributes"];
            layout.reserve(attribute.size());
            semanticNames.reserve(attribute.size());
            vertexBuffers.resize(attribute.size());
            for (auto it = attribute.begin(); it != attribute.end(); it++) {
                // the glTF attributes name may end in a number, DX12 doest like this and if this is the case we need to split the attribute name from the number
                //
                CMP_DWORD semanticIndex = 0;
                std::string semanticName;
                SplitGltfAttribute(it.key(), &semanticName, &semanticIndex);

                // We are only interested in the position or the texcoords (that is if the geoemtry uses a transparent material)
                //
                if (semanticName != "Position") {
                    if (pPrimitive->m_pMaterial->m_defines.find("DEF_alphaMode_OPAQUE")!= pPrimitive->m_pMaterial->m_defines.end())
                        if (semanticName != "TEXCOORD")
                            continue;
                }

                semanticNames.push_back(semanticName);

                auto accessor = accessors[it.value().get<int>()];

                // Get VB accessors
                //
                GetBufferDetails(accessor, bufferViews, pGLTFData->buffersData, &vertexBuffers[layout.size()]);

                // Create Input Layout
                //
                D3D12_INPUT_ELEMENT_DESC l;
                l.SemanticName = NULL; // we need to set it in the pipeline function (because of multithreading)
                l.SemanticIndex = semanticIndex;
                l.Format = GetFormatDX12(accessor["type"], accessor["componentType"]);
                l.InputSlot = (UINT)layout.size();
                l.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                l.InstanceDataStepRate = 0;
                l.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

                layout.push_back(l);
            }

            if (!CreateGeometry(indexBuffer, vertexBuffers, pPrimitive)) return false;
            GetThreadPool()->Add_Job([=]() {
                CreatePipeline(pDevice, pUploadHeap->GetNodeMask(), semanticNames, layout, pPrimitive);
            });
        }
    }

    return true;
}

void GltfDepthPass::OnDestroy() {
    for (unsigned int i = 0; i < m_textures.size(); i++) {
        m_textures[i].OnDestroy();
    }
}

bool GltfDepthPass::CreateGeometry(tfAccessor indexBuffer, std::vector<tfAccessor> vertexBuffers, DepthPrimitives *pPrimitive) {
    pPrimitive->m_NumIndices = indexBuffer.m_count;

    void *pDest;
    if (!m_pStaticBufferPool->AllocIndexBuffer(indexBuffer.m_count, indexBuffer.m_stride, &pDest, &pPrimitive->m_IBV)) return false;
    memcpy(pDest, indexBuffer.m_data, indexBuffer.m_stride*indexBuffer.m_count);

    // load those buffers onto the GPU
    pPrimitive->m_VBV.resize(vertexBuffers.size());
    for (unsigned int i = 0; i < vertexBuffers.size(); i++) {
        tfAccessor *pVertexAccessor = &vertexBuffers[i];

        void *pDest;
        m_pStaticBufferPool->AllocVertexBuffer(pVertexAccessor->m_count, pVertexAccessor->m_stride, &pDest, &pPrimitive->m_VBV[i]);
        memcpy(pDest, pVertexAccessor->m_data, pVertexAccessor->m_stride*pVertexAccessor->m_count);
    }

    return true;
}

void GltfDepthPass::CreatePipeline(ID3D12Device* pDevice, UINT node, std::vector<std::string> semanticNames, std::vector<D3D12_INPUT_ELEMENT_DESC> layout, DepthPrimitives *pPrimitive) {
    // let vertex shader know what buffers are present
    std::map<std::string, std::string> attributeDefines;
    for (unsigned int i = 0; i < layout.size(); i++) {
        layout[i].SemanticName = semanticNames[i].c_str();
        attributeDefines[std::string("HAS_") + layout[i].SemanticName] = "1";
    }


    // Compile shaders
    //
    ID3DBlob *pBlobShaderVert = NULL;
    ID3DBlob *pBlobShaderPixel = NULL;
    {
        // build macro structure
        //
        std::vector<D3D_SHADER_MACRO> macros;
        CompileMacros(&attributeDefines, &macros);
        CompileMacros(&pPrimitive->m_pMaterial->m_defines, &macros);
        macros.push_back(D3D_SHADER_MACRO{ NULL, NULL });

        ID3DBlob *pError;
        D3DCompileFromFile(L"./plugins/shaders/Shadows.hlsl", macros.data(), nullptr, "mainVS", "vs_5_0", 0, 0, &pBlobShaderVert, &pError);
        D3DCompileFromFile(L"./plugins/shaders/Shadows.hlsl", macros.data(), nullptr, "mainPS", "ps_5_0", 0, 0, &pBlobShaderPixel, &pError);
        if (pError != NULL) {
            char *msg = (char *)pError->GetBufferPointer();
            MessageBoxA(0, msg, "", 0);
        }
    }

    bool bUsingTransparency = (pPrimitive->m_pMaterial->m_pTransparency != NULL);

    // Create root signature
    //
    {
        CD3DX12_DESCRIPTOR_RANGE DescRange[4];
        CD3DX12_ROOT_PARAMETER RTSlot[4];
        CD3DX12_ROOT_SIGNATURE_DESC descRootSignature = CD3DX12_ROOT_SIGNATURE_DESC();

        if (bUsingTransparency) {
            DescRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);        // b0 <- per frame
            DescRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);        // t0 <- per material
            DescRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);        // b1 <- per material parameters
            DescRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 4, 0);    // s0 <- samplers

            RTSlot[0].InitAsDescriptorTable(1, &DescRange[0], D3D12_SHADER_VISIBILITY_ALL);
            RTSlot[1].InitAsDescriptorTable(1, &DescRange[1], D3D12_SHADER_VISIBILITY_PIXEL);
            RTSlot[2].InitAsDescriptorTable(1, &DescRange[2], D3D12_SHADER_VISIBILITY_ALL);
            RTSlot[3].InitAsDescriptorTable(1, &DescRange[3], D3D12_SHADER_VISIBILITY_PIXEL);

            // the root signature contains 3 slots to be used
            descRootSignature.NumParameters = 4;
            descRootSignature.pParameters = RTSlot;
            descRootSignature.NumStaticSamplers = 0;
            descRootSignature.pStaticSamplers = NULL;
        } else {
            DescRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);        // b0 <- per frame
            DescRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);        // b1 <- per material parameters

            RTSlot[0].InitAsDescriptorTable(1, &DescRange[0], D3D12_SHADER_VISIBILITY_ALL);
            RTSlot[1].InitAsDescriptorTable(1, &DescRange[1], D3D12_SHADER_VISIBILITY_ALL);

            // the root signature contains 3 slots to be used
            descRootSignature.NumParameters = 2;
            descRootSignature.pParameters = RTSlot;
            descRootSignature.NumStaticSamplers = 0;
            descRootSignature.pStaticSamplers = NULL;
        }


        // deny uneccessary access to certain pipeline stages
        descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE
                                  | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
                                  //| D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS
                                  | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
                                  | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
                                  | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
        //| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

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
                IID_PPV_ARGS(&pPrimitive->m_RootSignature))
        );
        pPrimitive->m_RootSignature->SetName(L"OnCreatePrimitiveDepthPass");

        pOutBlob->Release();
        if (pErrorBlob)
            pErrorBlob->Release();
    }

    // Create a PSO description
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC descPso = {};
    descPso.InputLayout = { layout.data(), (UINT)layout.size() };
    descPso.pRootSignature = pPrimitive->m_RootSignature.Get();
    descPso.VS = { reinterpret_cast<BYTE*>(pBlobShaderVert->GetBufferPointer()), pBlobShaderVert->GetBufferSize() };
    if (bUsingTransparency)
        descPso.PS = { reinterpret_cast<BYTE*>(pBlobShaderPixel->GetBufferPointer()), pBlobShaderPixel->GetBufferSize() };
    descPso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    descPso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    descPso.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    descPso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    descPso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    descPso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    descPso.SampleMask = UINT_MAX;
    descPso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    descPso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    descPso.SampleDesc.Count = 1;
    descPso.NodeMask = node;
    ThrowIfFailed(
        pDevice->CreateGraphicsPipelineState(&descPso, IID_PPV_ARGS(&pPrimitive->m_PipelineRender))
    );

    // create samplers if not initialized (this should happen once)
    if (m_sampler.GetSize() == 0) {
        m_pResourceViewHeaps->AllocSamplerDescriptor(1, &m_sampler);

        //for pbr materials
        D3D12_SAMPLER_DESC SamplerDesc;
        ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
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

    }

    pPrimitive->m_sampler = &m_sampler;
}

GltfDepthPass::per_batch *GltfDepthPass::SetPerBatchConstants() {
    GltfDepthPass::per_batch *cbPerBatch;
    m_pDynamicBufferRing->AllocConstantBuffer(sizeof(GltfDepthPass::per_batch), (void **)&cbPerBatch, &m_perBatchDesc);

    return cbPerBatch;
}

void GltfDepthPass::DrawMesh(ID3D12GraphicsCommandList* pCommandList, int meshIndex, XMMATRIX worldMatrix) {
    struct per_object {
        XMMATRIX mWorld;
    };

    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ID3D12DescriptorHeap *pDescriptorHeaps[] = { m_pResourceViewHeaps->GetCBV_SRV_UAVHeap(), m_pResourceViewHeaps->GetSamplerHeap() };
    pCommandList->SetDescriptorHeaps(2, pDescriptorHeaps);

    DepthMesh *pMesh = &m_meshes[meshIndex];
    for (unsigned int p = 0; p < pMesh->m_pPrimitives.size(); p++) {
        DepthPrimitives *pPrimitive = &pMesh->m_pPrimitives[p];

        // Set per Object constants
        //
        per_object *cbPerObject;
        D3D12_GPU_DESCRIPTOR_HANDLE perObjectDesc;
        m_pDynamicBufferRing->AllocConstantBuffer(sizeof(per_object), (void **)&cbPerObject, &perObjectDesc);
        cbPerObject->mWorld = worldMatrix;

        // Set state and draw
        //
        pCommandList->SetPipelineState(pPrimitive->m_PipelineRender.Get());
        pCommandList->SetGraphicsRootSignature(pPrimitive->m_RootSignature.Get());

        pCommandList->IASetIndexBuffer(&pPrimitive->m_IBV);
        pCommandList->IASetVertexBuffers(0, (UINT)pPrimitive->m_VBV.size(), pPrimitive->m_VBV.data());

        if (pPrimitive->m_pMaterial->m_pTransparency == NULL) {
            pCommandList->SetGraphicsRootDescriptorTable(0, m_perBatchDesc);
            pCommandList->SetGraphicsRootDescriptorTable(1, perObjectDesc);
        } else {
            pCommandList->SetGraphicsRootDescriptorTable(0, m_perBatchDesc);
            pCommandList->SetGraphicsRootDescriptorTable(1, pPrimitive->m_pMaterial->m_pTransparency->GetGPU());
            pCommandList->SetGraphicsRootDescriptorTable(2, perObjectDesc);
            pCommandList->SetGraphicsRootDescriptorTable(3, m_sampler.GetGPU());
        }

        pCommandList->DrawIndexedInstanced(pPrimitive->m_NumIndices, 1, 0, 0, 0);
    }
}

