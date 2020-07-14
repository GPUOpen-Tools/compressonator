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

#include "GltfPbr.h"

#include "GltfFeatures.h"
#include "gltfGetFormat_DX12.h"
#include "GltfHelpers_DX12.h"
#include "GltfHelpers.h"
#include "ThreadPool.h"

#include <Error.h>

#include <d3dcompiler.h>
#include <d3dx12.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")

using namespace DirectX;

void GltfPbr::AddTextureIfExists(json::object_t material, json::array_t textures, std::map<std::string, TextureDX12 *> &map, char *texturePath, char *textureName)
{
    int id = GetElementInt(material, texturePath, -1);
    if (id >= 0)
    {
        unsigned int tex = textures[id]["source"];
        if (m_textures.size() > tex)
            map[textureName] = &m_textures[tex];
    }
}

bool GltfPbr::OnCreate(
    ID3D12Device* pDevice,
    UploadHeapDX12* pUploadHeap,
    ResourceViewHeapsDX12 *pHeaps,
    DynamicBufferRingDX12 *pDynamicBufferRing,
    StaticBufferPoolDX12 *pStaticBufferPool,
    GLTFCommon *pGLTFData,
    SkyDome *pSkyDome,
#ifdef USE_SHADOWMAPS
    Texture *pShadowMap,
#endif
    void *pluginManager, void *msghandler)
{
    m_pGLTFData = pGLTFData;
    m_pDynamicBufferRing = pDynamicBufferRing;
    m_pResourceViewHeaps = pHeaps;
    m_pStaticBufferPool = pStaticBufferPool;

    // Load cubemaps maps for IBL
    m_pCubeDiffuseTexture = pSkyDome->GetDiffuseCubeMap();
    m_pCubeSpecularTexture = pSkyDome->GetSpecularCubeMap();
    if (m_BrdfTexture.InitFromFile(pDevice, pUploadHeap, L"./plugins/media/envmap/brdf.dds", pluginManager, msghandler) != 0)
    {
        return false;
    }
    pUploadHeap->FlushAndFinish();

    json &j3 = pGLTFData->j3;

    // Load Textures for gltf file

    if (!pGLTFData->isBinFile)
    {
        auto images = j3["images"];
        m_textures.resize(images.size());
        for (unsigned int i = 0; i < images.size(); i++)
        {
            std::string filename = images[i]["uri"];
            WCHAR wcstrPath[MAX_PATH];
            MultiByteToWideChar(CP_UTF8, 0, (pGLTFData->m_path + filename).c_str(), -1, wcstrPath, MAX_PATH);

            INT32 result = m_textures[i].InitFromFile(pDevice, pUploadHeap, wcstrPath, pluginManager, msghandler);
        }
        pUploadHeap->FlushAndFinish();
    }

    // Load PBR 2.0 Materials
    //
    if (DX12_CMips)
    {
        DX12_CMips->Print("Load PBR 2.0 Materials");
    }

    std::vector<PBRMaterial *> materialsData;
    auto materials = j3["materials"];
    auto textures = j3["textures"];
    for (unsigned int i = 0; i < materials.size(); i++)
    {
        json::object_t material = materials[i];

        PBRMaterial *tfmat = new PBRMaterial();
        materialsData.push_back(tfmat);

        // Load material constants
        //
        json::array_t ones = { 1.0, 1.0, 1.0, 1.0 };
        json::array_t zeroes = { 0.0, 0.0, 0.0, 0.0 };
        tfmat->emissiveFactor  = (XMVECTOR) GetXVector(GetElementJsonArray(material, "emissiveFactor", zeroes));
        tfmat->baseColorFactor = (XMVECTOR) GetXVector(GetElementJsonArray(material, "pbrMetallicRoughness/baseColorFactor", ones));
        try {
            tfmat->metallicFactor = GetElementFloat(material, "pbrMetallicRoughness/metallicFactor", 1.0);
        }
        catch (json::exception& e)
        {
            UNREFERENCED_PARAMETER(e);
            tfmat->metallicFactor = (GetElementJsonArray(material, "pbrMetallicRoughness/metallicFactor", ones))[0];
        }
        try {
            tfmat->roughnessFactor = GetElementFloat(material, "pbrMetallicRoughness/roughnessFactor", 1.0);
        }
        catch (json::exception& e)
        {
            UNREFERENCED_PARAMETER(e);
            tfmat->roughnessFactor = (GetElementJsonArray(material, "pbrMetallicRoughness/roughnessFactor", ones))[0];
        }

        tfmat->m_defines["DEF_alphaMode_" + GetElementString(material, "alphaMode", "OPAQUE")] = 1;

        float alphaCutOff = 0.0f;
        try {
            alphaCutOff = GetElementFloat(material, "alphaCutoff", 1.0);
        }
        catch (json::exception& e)
        {
            UNREFERENCED_PARAMETER(e);
            alphaCutOff = (GetElementJsonArray(material, "alphaCutoff", ones))[0];
        }
        tfmat->m_defines["DEF_alphaCutoff"] = std::to_string(alphaCutOff);

        // load glTF 2.0 material's textures (if present) and create descriptor set
        //
        std::map<std::string, TextureDX12 *> texturesBase;
        if (textures.size() > 0)
        {
            AddTextureIfExists(material, textures, texturesBase, "pbrMetallicRoughness/baseColorTexture/index", "baseColorTexture");
            AddTextureIfExists(material, textures, texturesBase, "pbrMetallicRoughness/metallicRoughnessTexture/index", "metallicRoughnessTexture");
            AddTextureIfExists(material, textures, texturesBase, "emissiveTexture/index", "emissiveTexture");
            AddTextureIfExists(material, textures, texturesBase, "normalTexture/index", "normalTexture");
            AddTextureIfExists(material, textures, texturesBase, "occlusionTexture/index", "occlusionTexture");
        }

        tfmat->m_textureCount = (int)texturesBase.size();

        if (m_pCubeDiffuseTexture)
            tfmat->m_textureCount += 1;

        if (m_pCubeSpecularTexture)
            tfmat->m_textureCount += 1;

        //+ 1 brdf lookup texture, add that to the total count of textures used
        tfmat->m_textureCount += 1;

#ifdef USE_SHADOWMAPS
        // plus shadows
        if (pShadowMap != NULL)
            tfmat->m_textureCount += 1;
#endif

        if (tfmat->m_textureCount >= 0)
        {
            //allocate descriptor table for the textures            
            tfmat->m_pTexturesTable = new CBV_SRV_UAV[tfmat->m_textureCount];
            pHeaps->AllocCBV_SRV_UAVDescriptor(tfmat->m_textureCount, tfmat->m_pTexturesTable);

            int cnt = 0;

            //create SRVs and #defines so the shader compiler knows what the index of each texture is           
            for (auto it = texturesBase.begin(); it != texturesBase.end(); it++)
            {
                tfmat->m_defines[std::string("ID_") + it->first] = std::to_string(cnt);
                it->second->CreateSRV(cnt++, tfmat->m_pTexturesTable);
            }

            //create SRVs and #defines for the IBL resources
            if (m_pCubeDiffuseTexture)
            {
                tfmat->m_defines["ID_diffuseCube"] = std::to_string(cnt);
                m_pCubeDiffuseTexture->CreateCubeSRV(cnt++, tfmat->m_pTexturesTable);
                tfmat->m_defines["USE_IBL"] = "1";
            }

            if (m_pCubeSpecularTexture)
            {
                tfmat->m_defines["ID_specularCube"] = std::to_string(cnt);
                m_pCubeSpecularTexture->CreateCubeSRV(cnt++, tfmat->m_pTexturesTable);
                tfmat->m_defines["USE_IBL"] = "1";
            }

            tfmat->m_defines["ID_brdfTexture"] = std::to_string(cnt);
            m_BrdfTexture.CreateSRV(cnt++, tfmat->m_pTexturesTable);

#ifdef USE_SHADOWMAPS
            // add SRV for the shadowmap
            if (pShadowMap!=NULL)
            {
                tfmat->m_defines["ID_shadowMap"] = std::to_string(cnt);
                pShadowMap->CreateSRV(cnt++, tfmat->m_pTexturesTable);
            }
#endif

        }
    }

    // Load Meshes
    //
    if (DX12_CMips)
    {
        DX12_CMips->Print("Load Meshes");
    }

    auto accessors = j3["accessors"];
    auto bufferViews = j3["bufferViews"];
    auto meshes = j3["meshes"];
    m_meshes.resize(meshes.size());

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        PBRMesh *tfmesh = &m_meshes[i];

        auto primitives = meshes[i]["primitives"];
        tfmesh->m_pPrimitives.resize(primitives.size());
        for (unsigned int p = 0; p < primitives.size(); p++)
        {
            PBRPrimitives *pPrimitive = &tfmesh->m_pPrimitives[p];

            // Set Material
            //
            pPrimitive->m_pMaterial = materialsData[primitives[p]["material"]];

            // Set Index buffer
            //
            tfAccessor indexBuffer;
            {
                int indicesID = primitives[p]["indices"].get<int>();
                json::object_t indicesAccessor = accessors[indicesID];
                GetBufferDetails(indicesAccessor, bufferViews, pGLTFData->buffersData, &indexBuffer);
            }

            // Get input layout
            //
            std::vector<tfAccessor> vertexBuffers;
            std::vector<std::string> semanticNames;
            std::vector<D3D12_INPUT_ELEMENT_DESC> layout;

            auto attribute = primitives[p]["attributes"];
            layout.reserve(attribute.size());
            semanticNames.reserve(attribute.size());
            vertexBuffers.resize(attribute.size());
            for (auto it = attribute.begin(); it != attribute.end(); it++)
            {
                // glTF attributes may end in a number, DX12 doest like this and if this is the case we need to split the attribute name from the number
                //
                CMP_DWORD semanticIndex = 0;
                std::string semanticName;
                SplitGltfAttribute(it.key(), &semanticName, &semanticIndex);

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
            GetThreadPool()->Add_Job([=]()
            {               
                CreatePipeline(pDevice, pUploadHeap->GetNodeMask(), semanticNames, layout, pPrimitive);
            });          
        }
    }

    return true;
}

void GltfPbr::OnDestroy()
{
    for (unsigned int i = 0; i < m_textures.size(); i++)
    {
        m_textures[i].OnDestroy();
    }

    m_BrdfTexture.OnDestroy();
}

bool GltfPbr::CreateGeometry(tfAccessor indexBuffer, std::vector<tfAccessor> vertexBuffers, PBRPrimitives *pPrimitive)
{
    pPrimitive->m_NumIndices = indexBuffer.m_count;

    void *pDest;
    if (!m_pStaticBufferPool->AllocIndexBuffer(indexBuffer.m_count, indexBuffer.m_stride, &pDest, &pPrimitive->m_IBV)) return false;
    memcpy(pDest, indexBuffer.m_data, indexBuffer.m_stride*indexBuffer.m_count);

    // load those buffers into the GPU
    pPrimitive->m_VBV.resize(vertexBuffers.size());
    for (unsigned int i = 0; i < vertexBuffers.size(); i++)
    {
        tfAccessor *pVertexAccessor = &vertexBuffers[i];

        void *pDest;
        if (!m_pStaticBufferPool->AllocVertexBuffer(pVertexAccessor->m_count, pVertexAccessor->m_stride, &pDest, &pPrimitive->m_VBV[i])) return false;
        memcpy(pDest, pVertexAccessor->m_data, pVertexAccessor->m_stride*pVertexAccessor->m_count);
    }

    return true;
}

void GltfPbr::CreatePipeline(ID3D12Device* pDevice, UINT node, std::vector<std::string> semanticNames, std::vector<D3D12_INPUT_ELEMENT_DESC> layout, PBRPrimitives *pPrimitive)
{
    //=================================================================================================
    // let vertex shader know what buffers are present
    // The Shader Code glTF20_EX.hlsl has if defs that are enable using these attributes when compiled
    // and ref by the D3D12_GRAPHICS_PIPELINE_STATE_DESC 
    //=================================================================================================
    bool Has_Normals = false;

    std::map<std::string, std::string> attributeDefines;
    for (unsigned int i = 0; i < layout.size(); i++)
    {
        layout[i].SemanticName = semanticNames[i].c_str();
        attributeDefines[std::string("HAS_") + layout[i].SemanticName] = "1";

        if (semanticNames[i].compare("NORMAL") == 0) Has_Normals = true;

    }

    // Compile shaders
    //
    ID3DBlob *pBlobShaderVert, *pBlobShaderPixel;
    {
        // build macro structure
        //
        std::vector<D3D_SHADER_MACRO> macros;
        CompileMacros(&attributeDefines, &macros);
        CompileMacros(&pPrimitive->m_pMaterial->m_defines, &macros);
        macros.push_back(D3D_SHADER_MACRO{ NULL, NULL });

        ID3DBlob *pError;
        D3DCompileFromFile(L"./plugins/shaders/glTF20_EX.hlsl", macros.data(), nullptr, "mainVS", "vs_5_0", 0, 0, &pBlobShaderVert, &pError);
        D3DCompileFromFile(L"./plugins/shaders/glTF20_EX.hlsl", macros.data(), nullptr, "mainPS", "ps_5_0", 0, 0, &pBlobShaderPixel, &pError);
        if (pError != NULL)
        {
            char *msg = (char *)pError->GetBufferPointer();
            MessageBoxA(0, msg, "", 0);
        }
    }

    // Create root signature
    //
    {
        CD3DX12_DESCRIPTOR_RANGE DescRange[4];
        DescRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);		// b0 <- per frame
        DescRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, pPrimitive->m_pMaterial->m_textureCount, 0);		// t0 <- per material
        DescRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);		// b1 <- per material parameters
        DescRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 5, 0);	// s0 <- samplers

        CD3DX12_ROOT_PARAMETER RTSlot[4];
        RTSlot[0].InitAsDescriptorTable(1, &DescRange[0], D3D12_SHADER_VISIBILITY_ALL);
        RTSlot[1].InitAsDescriptorTable(1, &DescRange[1], D3D12_SHADER_VISIBILITY_PIXEL);
        RTSlot[2].InitAsDescriptorTable(1, &DescRange[2], D3D12_SHADER_VISIBILITY_ALL);
        RTSlot[3].InitAsDescriptorTable(1, &DescRange[3], D3D12_SHADER_VISIBILITY_PIXEL);

        // the root signature contains 3 slots to be used
        CD3DX12_ROOT_SIGNATURE_DESC descRootSignature = CD3DX12_ROOT_SIGNATURE_DESC();
        descRootSignature.NumParameters = 4;
        descRootSignature.pParameters = RTSlot;
        descRootSignature.NumStaticSamplers = 0;
        descRootSignature.pStaticSamplers = NULL;

        // deny uneccessary access to certain pipeline stages   
        descRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE
            | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
            //| D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS
            | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
            | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
            | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
            //| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

        ID3DBlob *pOutBlob, *pErrorBlob = NULL;
        ThrowIfFailed(D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &pOutBlob, &pErrorBlob));
        ThrowIfFailed(
            pDevice->CreateRootSignature(
                node,
                pOutBlob->GetBufferPointer(),
                pOutBlob->GetBufferSize(),
                IID_PPV_ARGS(&pPrimitive->m_RootSignature))
        );
        pPrimitive->m_RootSignature->SetName(L"OnCreatePrimitiveColorPass");

        pOutBlob->Release();
        if (pErrorBlob)
            pErrorBlob->Release();
    }

    D3D12_RENDER_TARGET_BLEND_DESC blendingOpaque = D3D12_RENDER_TARGET_BLEND_DESC
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };

    D3D12_RENDER_TARGET_BLEND_DESC blendingBlend = D3D12_RENDER_TARGET_BLEND_DESC
    {
        TRUE,FALSE,
        D3D12_BLEND_SRC_ALPHA, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };

    // Create a PSO description
    //
    if (!pBlobShaderVert || !pBlobShaderPixel)
    {
        throw 1;
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC descPso = {};
    
    descPso.InputLayout = { layout.data(), (UINT)layout.size() };
    descPso.pRootSignature = pPrimitive->m_RootSignature.Get();
    descPso.VS = { reinterpret_cast<BYTE*>(pBlobShaderVert->GetBufferPointer()), pBlobShaderVert->GetBufferSize() };
    descPso.PS = { reinterpret_cast<BYTE*>(pBlobShaderPixel->GetBufferPointer()), pBlobShaderPixel->GetBufferSize() };
    descPso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    descPso.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;

    // Decide on default view based on attributes available
    if (m_pGLTFData)
    {
        if(m_pGLTFData->isBinFile)
            descPso.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
        else {
            if (Has_Normals)
            {
                descPso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
            }
            else
                descPso.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
        }
    }
    else 
    {
        if (Has_Normals)
        {
            descPso.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        }
        else
            descPso.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    }

    descPso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    descPso.BlendState.IndependentBlendEnable = TRUE;
    descPso.BlendState.RenderTarget[0] = blendingBlend;
    descPso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    descPso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    descPso.SampleMask = UINT_MAX;
    descPso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    descPso.NumRenderTargets = 1;
    descPso.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_UNORM;
    descPso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    descPso.SampleDesc.Count = 4;
    descPso.NodeMask = node;
    descPso.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    ThrowIfFailed(
        pDevice->CreateGraphicsPipelineState(&descPso, IID_PPV_ARGS(&pPrimitive->m_PipelineRender))
    );

    // create samplers if not initialized (this should happen once)
    if (m_sampler.GetSize()==0)
    {
        m_pResourceViewHeaps->AllocSamplerDescriptor(5, &m_sampler);

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


        // diffuse env map sampler
        ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
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
        pDevice->CreateSampler(&SamplerDesc, m_sampler.GetCPU(1));

        // specular env map sampler
        ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
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
        pDevice->CreateSampler(&SamplerDesc, m_sampler.GetCPU(2));

        // specular BRDF lut sampler
        ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
        SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
        SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
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
        pDevice->CreateSampler(&SamplerDesc, m_sampler.GetCPU(3));

        // specular BRDF lut sampler
        D3D12_SAMPLER_DESC samplerShadow = {
            D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            0.0f,
            1u,
            D3D12_COMPARISON_FUNC_LESS_EQUAL,
            { 0.0f, 0.0f, 0.0f, 0.0f },
            0.0f,
            D3D12_FLOAT32_MAX
        };

        pDevice->CreateSampler(&samplerShadow, m_sampler.GetCPU(4));
    }

    pPrimitive->m_sampler = &m_sampler;
}

GltfPbr::per_batch *GltfPbr::SetPerBatchConstants()
{
    per_batch *cbPerBatch;    
    m_pDynamicBufferRing->AllocConstantBuffer(sizeof(per_batch), (void **)&cbPerBatch, &m_perBatchDesc);

    return cbPerBatch;
}

void GltfPbr::DrawMesh(ID3D12GraphicsCommandList* pCommandList, int meshIndex, DirectX::XMMATRIX worldMatrix)
{
    struct per_object
    {
        XMMATRIX mWorld;
        XMVECTOR u_emissiveFactor;
        XMVECTOR u_baseColorFactor;
        XMVECTOR u_metallicRoughnessValues;
    };

    // common state
    pCommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ID3D12DescriptorHeap *pDescriptorHeaps[] = { m_pResourceViewHeaps->GetCBV_SRV_UAVHeap(), m_pResourceViewHeaps->GetSamplerHeap() };
    pCommandList->SetDescriptorHeaps(2, pDescriptorHeaps);

    m_TotalNumIndices = 0;
    PBRMesh *pMesh = &m_meshes[meshIndex];
    for (unsigned int p = 0; p < pMesh->m_pPrimitives.size(); p++)
    {
        PBRPrimitives *pPrimitive = &pMesh->m_pPrimitives[p];

        if (pPrimitive->m_sampler == NULL)
            continue;

        m_TotalNumIndices += pPrimitive->m_NumIndices;

        // Set per Object constants
        //
        per_object *cbPerObject;
        D3D12_GPU_DESCRIPTOR_HANDLE perObjectDesc;
        m_pDynamicBufferRing->AllocConstantBuffer(sizeof(per_object), (void **)&cbPerObject, &perObjectDesc);
        cbPerObject->mWorld = worldMatrix;
        cbPerObject->u_emissiveFactor = pPrimitive->m_pMaterial->emissiveFactor;
        cbPerObject->u_baseColorFactor = pPrimitive->m_pMaterial->baseColorFactor;
        cbPerObject->u_metallicRoughnessValues = XMVectorSet(pPrimitive->m_pMaterial->metallicFactor, pPrimitive->m_pMaterial->roughnessFactor, 0, 0);

        // Set state and draw
        //
        pCommandList->SetPipelineState(pPrimitive->m_PipelineRender.Get());
        pCommandList->SetGraphicsRootSignature(pPrimitive->m_RootSignature.Get());

        pCommandList->IASetIndexBuffer(&pPrimitive->m_IBV);
        pCommandList->IASetVertexBuffers(0, (UINT)pPrimitive->m_VBV.size(), pPrimitive->m_VBV.data());

        pCommandList->SetGraphicsRootDescriptorTable(0, m_perBatchDesc);
        pCommandList->SetGraphicsRootDescriptorTable(1, pPrimitive->m_pMaterial->m_pTexturesTable->GetGPU());
        pCommandList->SetGraphicsRootDescriptorTable(2, perObjectDesc);
        pCommandList->SetGraphicsRootDescriptorTable(3, m_sampler.GetGPU());

        pCommandList->DrawIndexedInstanced(pPrimitive->m_NumIndices, 1, 0, 0, 0);
    }
}

