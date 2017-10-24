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

#include "stdafx.h"
#include "glTF.h"
#include <map>

/*
#include "..\\libs\\jsmn\\jsmn.h"

#ifdef DEBUG
#pragma comment(lib, "..\\libs\\jsmn\\Bin\\x64\\Debug\\jsmn.lib")
#include <DXGIDebug.h>
#else
#pragma comment(lib, "..\\libs\\jsmn\\Bin\\x64\\Release\\jsmn.lib")
#endif 
*/

#include <iostream>
#include <fstream>

using json = nlohmann::json;

static int formatSize(std::string str, int id)
{
    int dim = 0;
    if (str== "SCALAR")
        dim = 1;
    else if (str=="VEC2")
        dim = 2;
    else if (str=="VEC3")
        dim = 3;
    else if (str=="VEC4")
        dim = 4;
    else 
        dim = -1;

    switch (id)
    {
        case 5120: return 1 * dim; //(BYTE)
        case 5121: return 1 * dim; //(UNSIGNED_BYTE)1
        case 5122: return 2 * dim; //(SHORT)2
        case 5123: return 2 * dim; //(UNSIGNED_SHORT)2
        case 5124: return 4 * dim; //(SIGNED_INT)4
        case 5125: return 4 * dim; //(UNSIGNED_INT)4
        case 5126: return 4 * dim; //(FLOAT)
    }

    return -1;
}

static DXGI_FORMAT format(std::string str, int id)
{
    if (str == "SCALAR")
    {
        switch (id)
        {
            case 5120: return DXGI_FORMAT_R8_SINT; //(BYTE)
            case 5121: return DXGI_FORMAT_R8_UINT; //(UNSIGNED_BYTE)1
            case 5122: return DXGI_FORMAT_R16_SINT; //(SHORT)2
            case 5123: return DXGI_FORMAT_R16_UINT; //(UNSIGNED_SHORT)2
            case 5124: return DXGI_FORMAT_R32_SINT; //(SIGNED_INT)4
            case 5125: return DXGI_FORMAT_R32_UINT; //(UNSIGNED_INT)4
            case 5126: return DXGI_FORMAT_R32_FLOAT; //(FLOAT)
        }
    }
    else if (str == "VEC2")
    {
        switch (id)
        {
        case 5120: return DXGI_FORMAT_R8G8_SINT; //(BYTE)
        case 5121: return DXGI_FORMAT_R8G8_UINT; //(UNSIGNED_BYTE)1
        case 5122: return DXGI_FORMAT_R16G16_SINT; //(SHORT)2
        case 5123: return DXGI_FORMAT_R16G16_UINT; //(UNSIGNED_SHORT)2
        case 5124: return DXGI_FORMAT_R32G32_SINT; //(SIGNED_INT)4
        case 5125: return DXGI_FORMAT_R32G32_UINT; //(UNSIGNED_INT)4
        case 5126: return DXGI_FORMAT_R32G32_FLOAT; //(FLOAT)
        }
    }
    else if (str == "VEC3")
    {
        switch (id)
        {
        case 5120: return DXGI_FORMAT_UNKNOWN; //(BYTE)
        case 5121: return DXGI_FORMAT_UNKNOWN; //(UNSIGNED_BYTE)1
        case 5122: return DXGI_FORMAT_UNKNOWN; //(SHORT)2
        case 5123: return DXGI_FORMAT_UNKNOWN; //(UNSIGNED_SHORT)2
        case 5124: return DXGI_FORMAT_R32G32B32_SINT; //(SIGNED_INT)4
        case 5125: return DXGI_FORMAT_R32G32B32_UINT; //(UNSIGNED_INT)4
        case 5126: return DXGI_FORMAT_R32G32B32_FLOAT; //(FLOAT)
        }
    }
    else if (str == "VEC4")
    {
        switch (id)
        {
        case 5120: return DXGI_FORMAT_R8G8B8A8_SINT; //(BYTE)
        case 5121: return DXGI_FORMAT_R8G8B8A8_UINT; //(UNSIGNED_BYTE)1
        case 5122: return DXGI_FORMAT_R16G16B16A16_SINT; //(SHORT)2
        case 5123: return DXGI_FORMAT_R16G16B16A16_UINT; //(UNSIGNED_SHORT)2
        case 5124: return DXGI_FORMAT_R32G32B32A32_SINT; //(SIGNED_INT)4
        case 5125: return DXGI_FORMAT_R32G32B32A32_UINT; //(UNSIGNED_INT)4
        case 5126: return DXGI_FORMAT_R32G32B32A32_FLOAT; //(FLOAT)
        }
    }

    return DXGI_FORMAT_UNKNOWN;
}

template <class type>
type GetElement(json::object_t root, char *path, type pDefault)
{
    char *p = path;
    char token[128];
    while (true)
    {
        for (; *p != '/' && *p != 0 && *p != '['; p++);
        memcpy(token, path, p - path);
        token[p - path] = 0;

        //printf(" %s\n", token);
        json::object_t::iterator it = root.find(token);
        if (it == root.end())
            return pDefault;
        
        if (*p == '[')
        {
            p++;
            int i = atoi(p);
            for (; *p != 0 && *p != ']'; p++);
            root = it->second.at(i).get<json::object_t>();
            p++;
        }
        else
        {
            if (it->second.is_object())
                root = it->second.get<json::object_t>();
            else
            {
                return it->second.get<type>();// ..get<type>();
            }
        }
        p++;
        path = p;
    }

    return pDefault;
}

XMVECTOR GetVector(json::array_t accessor)
{
    return XMVectorSet(accessor[0], accessor[1], accessor[2], (accessor.size() == 4) ? accessor[3] : 0);
}

XMMATRIX GetMatrix(json::array_t accessor)
{
    return XMMatrixSet(accessor[ 0], accessor[ 1], accessor[ 2], accessor[ 3],
                       accessor[ 4], accessor[ 5], accessor[ 6], accessor[ 7],
                       accessor[ 8], accessor[ 9], accessor[10], accessor[11],
                       accessor[12], accessor[13], accessor[14], accessor[15]);
}


void CompileMacros(const std::map<std::string, std::string> *pMacros, std::vector<D3D_SHADER_MACRO> *pOut)
{
    for (auto it = pMacros->begin(); it != pMacros->end(); it++)
    {
        D3D_SHADER_MACRO macro;
        macro.Name = it->first.c_str();
        macro.Definition = it->second.c_str();
        pOut->push_back(macro);
    }
}

bool GLTFLoader::Load(std::string path, std::string filename, CMP_Feedback_Proc pFeedbackProc)
{   
    m_path = path;
    m_filename = filename;

    std::ifstream f(path + filename);
    if (!f)
    {
        return false;
    }

    f >> j3;

    auto buffers = j3["buffers"];
    float fProgress = 0.0f;
    buffersData.resize(buffers.size());
    for (int i = 0; i < buffers.size(); i++)
    {
        std::string name = buffers[i]["uri"].get<std::string>();       
        std::ifstream ff(path + name, std::ios::in | std::ios::binary);

        ff.seekg(0, ff.end);
        std::streamoff length = ff.tellg();
        ff.seekg(0, ff.beg);

        char *p = new char[length];
        ff.read(p, length);
        buffersData[i] = p;
        if (pFeedbackProc) 
        {
            fProgress = 100.f * ((i+1)/ buffers.size());
            if (pFeedbackProc(fProgress, NULL, NULL))
            {
                printf("Loading model difference canceled!\n");
                return false; //abort
            }
        }
    }

    return true;
}

void GLTFLoader::Unload()
{
    for (int i = 0; i < buffersData.size(); i++)
    {
        delete (buffersData[i]);
    }

    buffersData.clear();
    j3.clear();
}

void GetBufferDetails(json::object_t accessor, json::array_t bufferViews, std::vector<char *> buffers , void **pSrc, DWORD *dwStride, DWORD *dwCount)
{
    auto bufferView = bufferViews[accessor["bufferView"].get<int>()];

    char *buffer = buffers[bufferView["buffer"].get<int>()];

    DWORD offset = 0;
    if (bufferView.count("byteOffset")>0)
        offset += bufferView["byteOffset"].get<int>();;

    int byteLength = bufferView["byteLength"];

    if (accessor.count("byteOffset") > 0)
    {
        DWORD byteOffset = accessor["byteOffset"].get<int>();
        offset += byteOffset;
        byteLength -= byteOffset;
    }

    DWORD strideInBytes;
    if (bufferView.find("byteStride") != bufferView.end())
        strideInBytes = bufferView["byteStride"];
    else
        strideInBytes = formatSize(accessor["type"], accessor["componentType"]);
    
    *pSrc = &buffer[offset];
    *dwStride = strideInBytes;
    *dwCount = accessor["count"].get<DWORD>();
}

void GlTF::AddTextureIfExists(json::object_t material, json::array_t textures, std::map<std::string, Texture *> &map, char *texturePath, char *textureName)
{
    int id = GetElement<int>(material, texturePath, -1);
    if (id >= 0)
    {
        int tex = textures[id]["source"];
        map[textureName] = m_textures[tex];
    }
}

void GlTF::OnCreate(
        ID3D12Device* pDevice,
        UploadHeapDX12* pUploadHeap,
        ResourceViewHeapsDX12 *pHeaps,
        DynamicBufferRingDX12 *pDynamicBufferRing,
        StaticConstantBufferPoolDX12 *pStaticConstantBufferPool,
        StaticBufferPoolDX12 *pStaticBufferPool,
        GLTFLoader *pGLTFData, void *pluginManager, void *msghandler)
{
    m_pDynamicBufferRing = pDynamicBufferRing;
    m_pResourceViewHeaps = pHeaps;

    // Load cubemaps maps for IBL
    m_CubeDiffuseTexture.InitFromFile(pDevice, pUploadHeap, pluginManager, msghandler, L"./plugins/media/envmap/diffuse.dds");
    m_CubeSpecularTexture.InitFromFile(pDevice, pUploadHeap, pluginManager, msghandler, L"./plugins/media/envmap/specular.dds");
    m_BrdfTexture.InitFromFile(pDevice, pUploadHeap, pluginManager, msghandler, L"./plugins/media/envmap/brdf.dds");
    pUploadHeap->FlushAndFinish();


    nlohmann::json &j3 = pGLTFData->j3;

    // Load Textures
    //    
    auto images = j3["images"];
    for (int i=0; i<images.size(); i++)
    {
        std::string filename = images[i]["uri"];
#ifndef AMDLOAD
        filename[filename.size() - 3] = 'd';
        filename[filename.size() - 2] = 'd';
        filename[filename.size() - 1] = 's';
#endif
        WCHAR wcstrPath[MAX_PATH];
        MultiByteToWideChar(CP_UTF8, 0, (pGLTFData->m_path+ filename).c_str(), -1, wcstrPath, MAX_PATH);

        Texture *pTexture = new Texture();
        INT32 result = pTexture->InitFromFile(pDevice, pUploadHeap, pluginManager, msghandler, wcstrPath);
        pUploadHeap->FlushAndFinish();
        m_textures.push_back(pTexture);
    }

    // Load Materials
    //
    std::vector<tfMaterial *> materialsData;
    auto materials = j3["materials"];
    auto textures = j3["textures"];
    for (int i = 0; i < materials.size(); i++)
    {
        json::object_t material = materials[i];

        tfMaterial *tfmat = new tfMaterial();
        materialsData.push_back(tfmat);

        std::map<std::string, Texture *> texturesBase;

        // load glTF 2.0 materials (if present)
        if (textures.size() > 0)
        {
            AddTextureIfExists(material, textures, texturesBase, "emissiveTexture/index", "emissiveTexture");
            AddTextureIfExists(material, textures, texturesBase, "normalTexture/index", "normalTexture");
            AddTextureIfExists(material, textures, texturesBase, "occlusionTexture/index", "occlusionTexture");
            AddTextureIfExists(material, textures, texturesBase, "pbrMetallicRoughness/baseColorTexture/index", "baseColorTexture");
            AddTextureIfExists(material, textures, texturesBase, "pbrMetallicRoughness/metallicRoughnessTexture/index", "metallicRoughnessTexture");
        }

        tfmat->m_textureCount = (int)texturesBase.size();

        //we are using 2 cubemaps + 1 brdf lookup texture, add that to the total count of textures used
        tfmat->m_textureCount += 3;

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
            tfmat->m_defines["ID_diffuseCube"] = std::to_string(cnt);
            m_CubeDiffuseTexture.CreateCubeSRV(cnt++, tfmat->m_pTexturesTable);
            tfmat->m_defines["ID_specularCube"] = std::to_string(cnt);
            m_CubeSpecularTexture.CreateCubeSRV(cnt++, tfmat->m_pTexturesTable);
            tfmat->m_defines["ID_brdfTexture"] = std::to_string(cnt);
            m_BrdfTexture.CreateSRV(cnt++, tfmat->m_pTexturesTable);
            tfmat->m_defines["USE_IBL"] = "1";
            
        }

        // Load material constants
        //
        json::array_t ones = { 1.0, 1.0, 1.0, 1.0 };
        json::array_t zeroes = { 0.0, 0.0, 0.0, 0.0 };
        tfmat->emissiveFactor = GetVector(GetElement<json::array_t>(material, "emissiveFactor", zeroes));
        tfmat->baseColorFactor = GetVector(GetElement<json::array_t>(material, "pbrMetallicRoughness/baseColorFactor", ones));
        tfmat->metallicFactor = GetElement<float>(material, "pbrMetallicRoughness/metallicFactor", 1.0);
        tfmat->roughnessFactor = GetElement<float>(material, "pbrMetallicRoughness/roughnessFactor", 1.0);

        if (material.find("alphaMode") != material.end())
            tfmat->m_defines["alphaMode"] = material["alphaMode"].get<std::string>();
    }

    // Load Meshes
    //
    auto accessors = j3["accessors"];
    auto bufferViews = j3["bufferViews"];
    auto meshes = j3["meshes"];
    m_meshes.resize(meshes.size());
    for (int i = 0; i < meshes.size(); i++)
    {
        tfMesh *tfmesh = new tfMesh();
        m_meshes[i] = tfmesh;        

        auto primitives = meshes[i]["primitives"];
        tfmesh->m_pPrimitives.resize(primitives.size());
        for (int p = 0; p < primitives.size(); p++)
        {
            tfPrimitives *pPrimitive = &tfmesh->m_pPrimitives[p];

            // Set Material
            //
            pPrimitive->m_pMaterial = materialsData[primitives[p]["material"]];

            // Set Index buffer
            //
            {
                json::object_t indicesAccessor = accessors[primitives[p]["indices"].get<int>()];
                pPrimitive->m_NumIndices = indicesAccessor["count"];

                void *pData, *pDest;
                DWORD dwStride, dwCount;
                GetBufferDetails(indicesAccessor, bufferViews, pGLTFData->buffersData, &pData, &dwStride, &dwCount);
                pStaticBufferPool->AllocIndexBuffer(dwCount, dwStride, &pDest, &pPrimitive->m_IBV);
                memcpy(pDest, pData, dwStride*dwCount);
            }

            std::vector<D3D12_INPUT_ELEMENT_DESC> layout;
            std::vector<std::string> key;

            auto attribute = primitives[p]["attributes"];
            key.reserve(attribute.size());
            layout.reserve(attribute.size());
            pPrimitive->m_VBV.resize(attribute.size());

            for (auto it = attribute.begin(); it != attribute.end(); it++)
            {
                //fix semantic name so it doesn't end with a number or an underscore
                //
                int SemanticIndex = 0;
                std::string SemanticName = it.key();
                if (isdigit(SemanticName.back()))
                {
                    SemanticIndex = SemanticName.back() - '0';
                    SemanticName.pop_back();
                    if (SemanticName.back()=='_')
                        SemanticName.pop_back();
                }
                key.push_back(SemanticName);

                pPrimitive->m_defines[std::string("HAS_") + SemanticName] = "1";
                
                auto accessor = accessors[it.value().get<int>()];

                // load VB
                //
                void *pData, *pDest;
                DWORD dwStride, dwCount;
                GetBufferDetails(accessor, bufferViews, pGLTFData->buffersData, &pData, &dwStride, &dwCount);
                pStaticBufferPool->AllocVertexBuffer(dwCount, dwStride, &pDest, &pPrimitive->m_VBV[layout.size()]);
                memcpy(pDest, pData, dwStride*dwCount);                

                // Create Input Layout
                //
                D3D12_INPUT_ELEMENT_DESC l;
                l.SemanticName = key[layout.size()].c_str();
                l.SemanticIndex = SemanticIndex;
                l.Format = format(accessor["type"], accessor["componentType"]);
                l.InputSlot = (UINT)layout.size();
                l.InputSlotClass= D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
                l.InstanceDataStepRate = 0;
                l.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

                layout.push_back(l);
            }            
            
            OnCreatePrimitiveColorPass(pDevice, layout, pUploadHeap->GetNodeMask(), pPrimitive);
        }
    }

    // Load nodes
    //
    auto nodes = j3["nodes"];
    m_nodes.resize(nodes.size());
    for (int i = 0; i < nodes.size(); i++)
    {
        tfNode *tfnode = &m_nodes[i];

        // Read node data
        //
        auto node = nodes[i];

        if (node.find("children") != node.end())
        {
            for (int c = 0; c < node["children"].size(); c++)
            {
                int nodeID = node["children"][c];
                tfnode->m_children.push_back(&m_nodes[nodeID]);
            }
        }
        
        if (node.find("mesh") != node.end())
            tfnode->m_pMesh = m_meshes[nodes[i]["mesh"]];

        if (node.find("translation") != node.end())
            tfnode->m_translation = GetVector(node["translation"]);
        else
            tfnode->m_translation = XMVectorSet(0, 0, 0, 0);

        if (node.find("rotation") != node.end())
            tfnode->m_rotation = XMMatrixRotationQuaternion(GetVector(node["rotation"]));
        else if (node.find("matrix") != node.end())
                tfnode->m_rotation = GetMatrix(node["matrix"]);
        else
            tfnode->m_rotation = XMMatrixIdentity();

        if (node.find("scale") != node.end())
            tfnode->m_scale = GetVector(node["scale"]);
        else
            tfnode->m_scale = XMVectorSet(1, 1, 1, 0);            
    }

    // Load scenes
    //
    auto scenes = j3["scenes"];
    m_scenes.resize(scenes.size());
    for (int i = 0; i < scenes.size(); i++)
    {
        m_scenes[i] = new tfScene();
        auto scene = scenes[i];
        for (int n = 0; n < scene["nodes"].size(); n++)
        {
            int nodeId = scene["nodes"][n];
            m_scenes[i]->m_nodes.push_back(&m_nodes[nodeId]);
        }
    }

    // Load animations
    //
    auto animations = j3["animations"];
    m_animations.resize(animations.size());
    for (int i = 0; i < animations.size(); i++)
    {
        auto channels = animations[i]["channels"];
        auto samplers = animations[i]["samplers"];

        tfAnimation *tfanim = &m_animations[i];
        for (int c = 0; c < channels.size(); c++)
        {
            auto channel = channels[c];
            int sampler = channel["sampler"];
            int node = GetElement<int>(channel, "target/node", -1);
            std::string path = GetElement<std::string>(channel, "target/path", std::string());

            tfChannel *tfchannel;

            auto ch = tfanim->m_channels.find(node);
            if (ch == tfanim->m_channels.end())
            {
                tfchannel = new tfChannel();
                tfanim->m_channels[node] = tfchannel;
            }
            else
            {
                tfchannel = ch->second;
            }

            tfSampler *tfsampler = new tfSampler();

            auto input = accessors[samplers[sampler]["input"].get<int>()];
            auto output = accessors[samplers[sampler]["output"].get<int>()];

            tfSampler *tfsmp = new tfSampler();

            //we are not doing anything with the animation yet.

            if (path == "translation")
            {
                tfchannel->m_pTranslation = tfsmp;
            }
            else if (path == "rotation")
            {
                tfchannel->m_pRotation = tfsmp;
            }
            else if (path == "scale")
            {
                tfchannel->m_pScale = tfsmp;
            }
        }
    }
}

void GlTF::OnDestroy()
{
    for (int i = 0; i < m_textures.size(); i++)
    {
        m_textures[i]->OnDestroy();
    }

    for (int i = 0; i < m_meshes.size(); i++)
    {
        delete m_meshes[i];
    }

    for (int i = 0; i < m_scenes.size(); i++)
    {        
        delete m_scenes[i];
    }

    m_CubeDiffuseTexture.OnDestroy();
    m_CubeSpecularTexture.OnDestroy();
    m_BrdfTexture.OnDestroy();

}

void GlTF::OnCreatePrimitiveColorPass(ID3D12Device* pDevice, std::vector<D3D12_INPUT_ELEMENT_DESC> layout, UINT node, tfPrimitives *pPrimitive)
{
    // Compile shaders
    //
    ID3DBlob *pBlobShaderVert, *pBlobShaderPixel;
    {
        // build macro structure
        //
        std::vector<D3D_SHADER_MACRO> macros;
        CompileMacros(&pPrimitive->m_defines, &macros);
        CompileMacros(&pPrimitive->m_pMaterial->m_defines, &macros);
        macros.push_back(D3D_SHADER_MACRO{ NULL, NULL });

        ID3DBlob *pError;
        D3DCompileFromFile(L"./plugins/shaders/glTF20.hlsl", macros.data(), nullptr, "mainVS", "vs_5_0", 0, 0, &pBlobShaderVert, &pError);
        D3DCompileFromFile(L"./plugins/shaders/glTF20.hlsl", macros.data(), nullptr, "mainPS", "ps_5_0", 0, 0, &pBlobShaderPixel, &pError);
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
        DescRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);		// b1 <- per material
        DescRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 4, 0);	// s0 <- samplers

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
        pPrimitive->m_RootSignature->SetName(L"StaticHeapDX12");

        pOutBlob->Release();
        if (pErrorBlob)
            pErrorBlob->Release();
    }

    // Create a PSO description
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC descPso;
    ZeroMemory(&descPso, sizeof(descPso));
    descPso.InputLayout = { layout.data(), (UINT)layout.size() };
    descPso.pRootSignature = pPrimitive->m_RootSignature;
    descPso.VS = { reinterpret_cast<BYTE*>(pBlobShaderVert->GetBufferPointer()), pBlobShaderVert->GetBufferSize() };
    descPso.PS = { reinterpret_cast<BYTE*>(pBlobShaderPixel->GetBufferPointer()), pBlobShaderPixel->GetBufferSize() };
    descPso.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    descPso.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    descPso.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    descPso.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    descPso.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    descPso.SampleMask = UINT_MAX;
    descPso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    descPso.NumRenderTargets = 1;
    descPso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    descPso.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    descPso.SampleDesc.Count = 1;
    descPso.NodeMask = node;
    ThrowIfFailed(
        pDevice->CreateGraphicsPipelineState(&descPso, IID_PPV_ARGS(&pPrimitive->m_PipelineRender))
    );

    // create samplers
    {
        m_pResourceViewHeaps->AllocSamplerDescriptor(4, &pPrimitive->m_sampler);

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
        pDevice->CreateSampler(&SamplerDesc, pPrimitive->m_sampler.GetCPU(0));


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
        pDevice->CreateSampler(&SamplerDesc, pPrimitive->m_sampler.GetCPU(1));

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
        pDevice->CreateSampler(&SamplerDesc, pPrimitive->m_sampler.GetCPU(2));

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
        pDevice->CreateSampler(&SamplerDesc, pPrimitive->m_sampler.GetCPU(3));
    }
}

void GlTF::Draw(ID3D12GraphicsCommandList* pCommandList, Camera *pCam)
{
    struct per_batch
    {
        XMMATRIX mViewProj;
        XMVECTOR Camera;
        XMVECTOR LightDirection;
        XMVECTOR LightColor;
    } ;

    struct per_object
    {
        XMMATRIX mWorld;
        XMVECTOR u_emissiveFactor;
        XMVECTOR u_baseColorFactor;
        XMVECTOR u_metallicRoughnessValues;
    };

    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    // Set per Batch constants
    //
    per_batch *cbPerBatch;
    D3D12_GPU_DESCRIPTOR_HANDLE perBatchDesc;
    m_pDynamicBufferRing->AllocConstantBuffer(sizeof(per_batch), (void **)&cbPerBatch, &perBatchDesc);
    cbPerBatch->mViewProj      = pCam->GetView() * pCam->GetProjection();
    cbPerBatch->Camera         = pCam->GetPosition(); 
    cbPerBatch->LightDirection = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
    cbPerBatch->LightColor     = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f) * 10.0f;
   
    struct StackData { tfNode *pN; XMMATRIX m; };
    std::vector<StackData> stack;

    tfScene *pScene = m_scenes[0];

    for (int n = 0; n < pScene->m_nodes.size(); n++)
    {
        tfNode  *pNode = pScene->m_nodes[n];

        StackData st = { pNode, pNode->GetWorldMat() };
        stack.push_back(st);

        //
        while (stack.size() > 0)
        {
            StackData top = stack.back();
            stack.pop_back();

            tfMesh *pMesh = top.pN->m_pMesh;
            if (pMesh != NULL)
            {
                for (int p = 0; p < pMesh->m_pPrimitives.size(); p++)
                {
                    tfPrimitives *pPrimitive = &pMesh->m_pPrimitives[p];

                    // Set per Object constants
                    //
                    per_object *cbPerObject;
                    D3D12_GPU_DESCRIPTOR_HANDLE perObjectDesc;
                    m_pDynamicBufferRing->AllocConstantBuffer(sizeof(per_batch), (void **)&cbPerObject, &perObjectDesc);
                    cbPerObject->mWorld = top.m;
                    cbPerObject->u_emissiveFactor = pPrimitive->m_pMaterial->emissiveFactor;
                    cbPerObject->u_baseColorFactor = pPrimitive->m_pMaterial->baseColorFactor;
                    cbPerObject->u_metallicRoughnessValues = XMVectorSet(pPrimitive->m_pMaterial->metallicFactor, pPrimitive->m_pMaterial->roughnessFactor, 0, 0);

                    // Set state and draw
                    //
                    ID3D12DescriptorHeap *pDescriptorHeaps[] = { m_pResourceViewHeaps->GetCBV_SRV_UAVHeap(), m_pResourceViewHeaps->GetSamplerHeap() };
                    pCommandList->SetDescriptorHeaps(2, pDescriptorHeaps);

                    pCommandList->SetPipelineState(pPrimitive->m_PipelineRender);
                    pCommandList->SetGraphicsRootSignature(pPrimitive->m_RootSignature);

                    pCommandList->IASetVertexBuffers(0, (UINT)pPrimitive->m_VBV.size(), pPrimitive->m_VBV.data());
                    pCommandList->IASetIndexBuffer(&pPrimitive->m_IBV);

                    pCommandList->SetGraphicsRootDescriptorTable(0, perBatchDesc);
                    pCommandList->SetGraphicsRootDescriptorTable(1, pPrimitive->m_pMaterial->m_pTexturesTable->GetGPU());
                    pCommandList->SetGraphicsRootDescriptorTable(2, perObjectDesc);
                    pCommandList->SetGraphicsRootDescriptorTable(3, pPrimitive->m_sampler.GetGPU());

                    pCommandList->DrawIndexedInstanced(pPrimitive->m_NumIndices, 1, 0, 0, 0);
                }
            }
                                  
            for (int i = 0; i < top.pN->m_children.size(); i++)
            {
                StackData st = { top.pN->m_children[i], top.pN->m_children[i]->GetWorldMat()*top.m };
                stack.push_back(st);
            }
        }
    }
}

