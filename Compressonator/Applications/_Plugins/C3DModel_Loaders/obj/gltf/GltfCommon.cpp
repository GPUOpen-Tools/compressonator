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
#include "GltfCommon.h"
#include "GltfHelpers.h"


void GetBufferDetails(json::object_t accessor, json::array_t bufferViews, std::vector<char *> buffers, tfAccessor *pAccessor)
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

    DWORD strideInBytes = 0;
    //if (bufferView.find("byteStride") != bufferView.end())
    //    strideInBytes += bufferView["byteStride"];

    DWORD dwDimensions = GetDimensions(accessor["type"].get<std::string>());
    DWORD dwFormatSize = GetFormatSize(accessor["componentType"]);

    strideInBytes += dwDimensions * dwFormatSize;

    pAccessor->m_data = &buffer[offset];
    pAccessor->m_stride = strideInBytes;
    pAccessor->m_count = accessor["count"].get<DWORD>();
    pAccessor->m_dimension = dwDimensions;
    pAccessor->m_type = accessor["componentType"];
}

int GLTFCommon::Load(std::string path, std::string filename, CMIPS *cmips)
{   
    m_path = path;
    m_filename = filename;

    std::ifstream f(path + filename);
    if (!f)
    {
        return false;
    }

    f >> j3;

    if (cmips)
    {
        cmips->SetProgress(0);
    }

    auto buffers = j3["buffers"];
    float fProgress = 0.0f;
    buffersData.resize(buffers.size());
    for (unsigned int i = 0; i < buffers.size(); i++)
    {
        std::string name = buffers[i]["uri"].get<std::string>();
        if (name.find(".bin") != string::npos)
        {
            std::ifstream ff(path + name, std::ios::in | std::ios::binary);

            if (cmips)
            {
                cmips->Print("Processing: buffers %s", name.c_str());
            }

            ff.seekg(0, ff.end);
            std::streamoff length = ff.tellg();
            ff.seekg(0, ff.beg);

            char *p = new char[(unsigned int)length];
            ff.read(p, length);
            buffersData[i] = p;
        }
        else 
        {
            if (cmips)
            {
                cmips->Print("Error loading buffers, embedded is not supported.");
            }
            return -100;
        }
    }

    // Load Meshes
    //
    auto accessors = j3["accessors"];
    auto bufferViews = j3["bufferViews"];
    auto meshes = j3["meshes"];
    m_meshes.resize(meshes.size());

    if (cmips)
    {
        cmips->Print("Processing: meshes");
    }

    float maxx = 0.0f;
    float maxy = 0.0f;

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        tfMesh *tfmesh = &m_meshes[i];
        auto primitives = meshes[i]["primitives"];
        tfmesh->m_pPrimitives.resize(primitives.size());
        for (unsigned int p = 0; p < primitives.size(); p++)
        {
            tfPrimitives *pPrimitive = &tfmesh->m_pPrimitives[p];

            auto attribute = primitives[p]["attributes"];
            auto position = attribute["POSITION"];
            auto accessor = accessors[position.get<int>()];

            XMVECTOR max = GetVector(GetElementJsonArray(accessor, "max", { 0.0, 0.0, 0.0, 0.0 }));
            XMVECTOR min = GetVector(GetElementJsonArray(accessor, "min", { 0.0, 0.0, 0.0, 0.0 }));

            pPrimitive->m_center = (min + max)*.5;
            pPrimitive->m_radius = max - pPrimitive->m_center;


            if (XMVectorGetX(max) > maxx)
                        maxx = XMVectorGetX(max);
            if (XMVectorGetY(max) > maxy)
                        maxy = XMVectorGetY(max);

        }
    }

    // This is a work around for getting
    // the views correct for various models sizes based on [min,max] attributes
    // Should use bounding box of entire model as fix.
    m_distance = max(maxx, maxy);

    if (m_distance < 0.1)
        m_distance *= 2.0f;
    else
    if (m_distance < 0.2)
        m_distance = 6.0f;
    else
    if (m_distance < 0.9)
        m_distance *= 2.0f;
    else
    if (m_distance < 1.5)
        m_distance = 4.0f;
    else
    if (m_distance < 2.5)
        m_distance = 6.0f;
    else
        m_distance *= 2.0f;
    
    // Load nodes
    //
    auto nodes = j3["nodes"];
    m_nodes.resize(nodes.size());
    if (cmips)
    {
        cmips->Print("Processing: nodes");
    }
    for (unsigned int i = 0; i < nodes.size(); i++)
    {
        tfNode *tfnode = &m_nodes[i];

        // Read node data
        //
        auto node = nodes[i];

        if (node.find("children") != node.end())
        {
            for (unsigned int c = 0; c < node["children"].size(); c++)
            {
                int nodeID = node["children"][c];
                tfnode->m_children.push_back(&m_nodes[nodeID]);
            }
        }

        if (node.find("mesh") != node.end())
            tfnode->meshIndex = nodes[i]["mesh"];

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
    if (cmips)
    {
        cmips->Print("Processing: scenes");
    }
    for (unsigned int i = 0; i < scenes.size(); i++)
    {
        auto scene = scenes[i];
        for (unsigned int n = 0; n < scene["nodes"].size(); n++)
        {
            int nodeId = scene["nodes"][n];
            m_scenes[i].m_nodes.push_back(&m_nodes[nodeId]);
        }
    }

    // Load animations
    //
    auto animations = j3["animations"];
    m_animations.resize(animations.size());
    if (cmips)
    {
        cmips->Print("Processing: animations");
    }
    for (unsigned int i = 0; i < animations.size(); i++)
    {
        auto channels = animations[i]["channels"];
        auto samplers = animations[i]["samplers"];

        tfAnimation *tfanim = &m_animations[i];
        for (unsigned int c = 0; c < channels.size(); c++)
        {
            auto channel = channels[c];
            int sampler = channel["sampler"];
            int node = GetElementInt(channel, "target/node", -1);
            std::string path = GetElementString(channel, "target/path", std::string());

            tfChannel *tfchannel;

            auto ch = tfanim->m_channels.find(node);
            if (ch == tfanim->m_channels.end())
            {
                tfchannel = &tfanim->m_channels[node];
            }
            else
            {
                tfchannel = &ch->second;
            }

            auto input = accessors[samplers[sampler]["input"].get<int>()];
            auto output = accessors[samplers[sampler]["output"].get<int>()];

            tfSampler *tfsmp = new tfSampler();

            // Get time line
            //
            GetBufferDetails(input, bufferViews, buffersData, &tfsmp->m_time);
            if (tfsmp->m_time.m_stride != 4)
            {
                tfsmp->m_time.m_stride = 4;
            }

            tfanim->m_duration = max(tfanim->m_duration, *(float*)tfsmp->m_time.Get(tfsmp->m_time.m_count - 1));

            // Get value line
            //
            GetBufferDetails(output, bufferViews, buffersData, &tfsmp->m_value);

            // Index appropriately
            // 
            if (path == "translation")
            {
                tfchannel->m_pTranslation = tfsmp;
                // Patch user settings if out of range
                if (tfsmp->m_value.m_stride != 3 * 4)
                {
                    tfsmp->m_value.m_stride = 3 * 4;
                }
                if (tfsmp->m_value.m_dimension != 3)
                {
                    tfsmp->m_value.m_dimension = 3;
                }
            }
            else if (path == "rotation")
            {
                tfchannel->m_pRotation = tfsmp;
                if (tfsmp->m_value.m_stride != 4*4)
                {
                    tfsmp->m_value.m_stride = 4 * 4;
                }
                if (tfsmp->m_value.m_dimension != 4)
                {
                    tfsmp->m_value.m_dimension = 4;
                }
            }
            else if (path == "scale")
            {
                tfchannel->m_pScale = tfsmp;
                if (tfsmp->m_value.m_stride != 3 * 4)
                {
                    tfsmp->m_value.m_stride = 3 * 4;
                }
                if (tfsmp->m_value.m_dimension != 3)
                {
                    tfsmp->m_value.m_dimension = 3;
                }
            }
        }
    }

    return 0;
}

void GLTFCommon::Unload()
{
    for (unsigned int i = 0; i < buffersData.size(); i++)
    {
        delete (buffersData[i]);
    }
    buffersData.clear();

    m_animations.clear();
    m_nodes.clear();
    m_scenes.clear();
    
    j3.clear();
}

void GLTFCommon::SetAnimationTime(int animationIndex, float time)
{
    if ((unsigned  int)animationIndex <  m_animations.size())
    {
        tfAnimation *anim = &m_animations[animationIndex];

        //loop animation
        time = fmod(time, anim->m_duration);

        for (auto it = anim->m_channels.begin(); it != anim->m_channels.end(); it++)
        {
            tfNode *pNode = &m_nodes[it->first];

            float frac, *pCurr, *pNext;

            if (it->second.m_pRotation != NULL)
            {
                it->second.m_pRotation->SampleLinear(time, &frac, &pCurr, &pNext);
                pNode->m_rotation = XMMatrixRotationQuaternion(XMQuaternionSlerp(XMVectorSet(pCurr[0], pCurr[1], pCurr[2], pCurr[3]), XMVectorSet(pNext[0], pNext[1], pNext[2], pNext[3]), frac));
            }

            if (it->second.m_pTranslation != NULL)
            {
                it->second.m_pTranslation->SampleLinear(time, &frac, &pCurr, &pNext);
                pNode->m_translation = (1.0f - frac) * XMVectorSet(pCurr[0], pCurr[1], pCurr[2], 0) + (frac)*XMVectorSet(pNext[0], pNext[1], pNext[2], 0);
            }

            if (it->second.m_pScale != NULL)
            {
                it->second.m_pScale->SampleLinear(time, &frac, &pCurr, &pNext);
                pNode->m_scale = (1.0f - frac) * XMVectorSet(pCurr[0], pCurr[1], pCurr[2], 0) + (frac)*XMVectorSet(pNext[0], pNext[1], pNext[2], 0);
            }
        }
    }
}


void GLTFCommon::TransformNodes(NodeMatrixPostTransform *pNodesOut, DWORD *pCount)
{
    DWORD cnt = 0;

    std::vector<NodeMatrixPostTransform> stack;

    tfScene *pScene = &m_scenes[0];

    for (unsigned int n = 0; n < pScene->m_nodes.size(); n++)
    {
        tfNode  *pNode = pScene->m_nodes[n];

        NodeMatrixPostTransform st = { pNode, pNode->GetWorldMat() };
        stack.push_back(st);

        //
        while (stack.size() > 0)
        {
            NodeMatrixPostTransform top = stack.back();
            stack.pop_back();

            if (top.pN->meshIndex >= 0)
            {
                pNodesOut[cnt++] = top;
            }

            for (unsigned int i = 0; i < top.pN->m_children.size(); i++)
            {
                NodeMatrixPostTransform st = { top.pN->m_children[i], top.pN->m_children[i]->GetWorldMat()*top.m };
                stack.push_back(st);
            }
        }
    }

    *pCount = cnt;
}