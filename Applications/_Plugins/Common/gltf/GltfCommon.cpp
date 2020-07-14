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

#include "GltfCommon.h"

#include "glTFHelpers.h"

// Windows Header Files:
#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include <fstream>

#include <glm/gtx/quaternion.hpp>

void GetBufferDetails(json::object_t accessor, json::array_t bufferViews, std::vector<char*> buffers, tfAccessor* pAccessor)
{
    int bufferViewID = accessor["bufferView"].get<int>();
    auto bufferView = bufferViews[bufferViewID];

    int bufferID = bufferView["buffer"].get<int>();
    char* buffer = buffers[bufferID];

    CMP_DWORD offset = 0;
    if (bufferView.count("byteOffset") > 0)
        offset += bufferView["byteOffset"].get<int>();
    ;

    int byteLength = bufferView["byteLength"];

    if (accessor.count("byteOffset") > 0)
    {
        CMP_DWORD byteOffset = accessor["byteOffset"].get<int>();
        offset += byteOffset;
        byteLength -= byteOffset;
    }

    CMP_DWORD strideInBytes = 0;
    //if (bufferView.find("byteStride") != bufferView.end())
    //    strideInBytes += bufferView["byteStride"];

    CMP_DWORD dwDimensions = GetDimensions(accessor["type"].get<std::string>());
    CMP_DWORD dwFormatSize = GetFormatSize(accessor["componentType"]);

    strideInBytes += dwDimensions * dwFormatSize;

    pAccessor->m_data      = &buffer[offset];
    pAccessor->m_stride    = strideInBytes;
    pAccessor->m_count     = accessor["count"].get<CMP_DWORD>();
    pAccessor->m_dimension = dwDimensions;
    pAccessor->m_type      = accessor["componentType"];
}

// void GetBufferDetails(json::object_t* pInAccessor, json::array_t* pBufferViews, std::vector<char*>* pBuffers, tfAccessor* pAccessor)
// {
//     json::object_t bufferView = pBufferViews->at(pInAccessor->at("bufferView").get<int>());
// 
//     char* buffer = pBuffers->at(bufferView["buffer"].get<int>());
// 
//     CMP_DWORD offset = 0;
//     if (bufferView.count("byteOffset") > 0)
//         offset += bufferView["byteOffset"].get<int>();
//     ;
// 
//     int byteLength = bufferView["byteLength"];
// 
//     if (pInAccessor->count("byteOffset") > 0)
//     {
//         CMP_DWORD byteOffset = pInAccessor->at("byteOffset").get<int>();
//         offset += byteOffset;
//         byteLength -= byteOffset;
//     }
// 
//     CMP_DWORD strideInBytes = 0;
//     //if (bufferView.find("byteStride") != bufferView.end())
//     //    strideInBytes += bufferView["byteStride"];
// 
//     CMP_DWORD dwDimensions = GetDimensions(pInAccessor->at("type").get<std::string>());
//     CMP_DWORD dwFormatSize = GetFormatSize(pInAccessor->at("componentType"));
// 
//     strideInBytes += dwDimensions * dwFormatSize;
// 
//     pAccessor->m_data      = &buffer[offset];
//     pAccessor->m_stride    = strideInBytes;
//     pAccessor->m_count     = pInAccessor->at("count").get<CMP_DWORD>();
//     pAccessor->m_dimension = dwDimensions;
//     pAccessor->m_type      = pInAccessor->at("componentType");
// }

bool GLTFCommon::GetBufferData(std::string attriName, nlohmann::json::object_t accessor, nlohmann::json::array_t bufferViews, char* buffer, int index)
{
    auto bufferView = bufferViews[accessor["bufferView"].get<int>()];

    CMP_DWORD offset = 0;
    if (bufferView.count("byteOffset") > 0)
        offset += bufferView["byteOffset"].get<int>();
    ;

    int byteLength = bufferView["byteLength"];

    if (accessor.count("byteOffset") > 0)
    {
        CMP_DWORD byteOffset = accessor["byteOffset"].get<int>();
        offset += byteOffset;
        byteLength -= byteOffset;
    }

    CMP_DWORD strideInBytes = 0;

    CMP_DWORD dwDimensions = GetDimensions(accessor["type"].get<std::string>());
    CMP_DWORD dwFormatSize = GetFormatSize(accessor["componentType"]);

    strideInBytes = dwDimensions * dwFormatSize;

    int count = accessor["count"].get<CMP_DWORD>();

    if (attriName == "indices")
    {
        if (dwDimensions != 1)
            return false;
        if (dwFormatSize != 4 && dwFormatSize != 2)
            return false;
        if (dwFormatSize == 4)
        {
            unsigned int* pdata = (unsigned int*)&buffer[offset];
            for (int i = 0; i < count; i++)
            {
                m_meshBufferData.m_meshData[index].indices.push_back(*pdata);
                pdata++;
            }
        }
        else if (dwFormatSize == 2)
        {
            unsigned short* pdata = (unsigned short*)&buffer[offset];
            for (int i = 0; i < count; i++)
            {
                m_meshBufferData.m_meshData[index].indices.push_back(*pdata);
                pdata++;
            }
        }
    }
    else if (attriName == "POSITION")
    {
        if (dwDimensions != 3)
            return false;
        if (dwFormatSize != 4)
            return false;
        float* pdata = (float*)&buffer[offset];
        if (m_meshBufferData.m_meshData[index].vertices.size() == 0)
        {
            for (int i = 0; i < count; i++)
            {
                Vertex vertex;
                vertex.px = *pdata;
                pdata++;
                vertex.py = *pdata;
                //pdata += dwFormatSize;
                pdata++;
                vertex.pz = *pdata;
                pdata++;
                m_meshBufferData.m_meshData[index].vertices.push_back(vertex);
            }
        }
        else if (m_meshBufferData.m_meshData[index].vertices.size() == count)
        {
            for (int i = 0; i < count; i++)
            {
                m_meshBufferData.m_meshData[index].vertices[i].px = *pdata;
                pdata++;
                m_meshBufferData.m_meshData[index].vertices[i].py = *pdata;
                //pdata += dwFormatSize;
                pdata++;
                m_meshBufferData.m_meshData[index].vertices[i].pz = *pdata;
                pdata++;
            }
        }
    }
    else if (attriName == "NORMAL")
    {
        if (dwDimensions != 3)
            return false;
        if (dwFormatSize != 4)
            return false;
        float* pdata = (float*)&buffer[offset];
        if (m_meshBufferData.m_meshData[index].vertices.size() == 0)
        {
            for (int i = 0; i < count; i++)
            {
                Vertex temp;
                temp.nx = *pdata;
                pdata++;
                temp.ny = *pdata;
                pdata++;
                temp.nz = *pdata;
                pdata++;
                m_meshBufferData.m_meshData[index].vertices.push_back(temp);
            }
        }
        else if (m_meshBufferData.m_meshData[index].vertices.size() == count)
        {
            for (int i = 0; i < count; i++)
            {
                m_meshBufferData.m_meshData[index].vertices[i].nx = *pdata;
                pdata++;
                m_meshBufferData.m_meshData[index].vertices[i].ny = *pdata;
                pdata++;
                m_meshBufferData.m_meshData[index].vertices[i].nz = *pdata;
                pdata++;
            }
        }
    }
    else if (attriName == "TEXCOORD")
    {
        if (dwDimensions != 2)
            return false;
        if (dwFormatSize != 4)
            return false;
        float* pdata = (float*)&buffer[offset];
        if (m_meshBufferData.m_meshData[index].vertices.size() == 0)
        {
            for (int i = 0; i < count; i++)
            {
                Vertex temp;
                temp.tx = *pdata;
                pdata++;
                temp.ty = *pdata;
                pdata++;
                m_meshBufferData.m_meshData[index].vertices.push_back(temp);
            }
        }
        else if (m_meshBufferData.m_meshData[index].vertices.size() == count)
        {
            for (int i = 0; i < count; i++)
            {
                m_meshBufferData.m_meshData[index].vertices[i].tx = *pdata;
                pdata++;
                m_meshBufferData.m_meshData[index].vertices[i].ty = *pdata;
                pdata++;
            }
        }
    }

    return true;
}

bool GLTFCommon::SetBufferData(std::string attriName, nlohmann::json::object_t accessor, nlohmann::json::array_t bufferViews, char* buffer, int index)
{
    auto bufferView = bufferViews[accessor["bufferView"].get<int>()];

    CMP_DWORD offset = 0;
    if (bufferView.count("byteOffset") > 0)
        offset += bufferView["byteOffset"].get<int>();
    ;

    int byteLength = bufferView["byteLength"];

    if (accessor.count("byteOffset") > 0)
    {
        CMP_DWORD byteOffset = accessor["byteOffset"].get<int>();
        offset += byteOffset;
        byteLength -= byteOffset;
    }

    CMP_DWORD strideInBytes = 0;

    CMP_DWORD dwDimensions = GetDimensions(accessor["type"].get<std::string>());
    CMP_DWORD dwFormatSize = GetFormatSize(accessor["componentType"]);

    strideInBytes = dwDimensions * dwFormatSize;

    int count = accessor["count"].get<CMP_DWORD>();

    if (attriName == "indices")
    {
        if (dwDimensions != 1)
            return false;
        if (dwFormatSize != 4 && dwFormatSize != 2)
            return false;
        if (dwFormatSize == 4)
        {
            unsigned int* pdata = (unsigned int*)&buffer[offset];
            if (m_meshBufferData.m_meshData[index].indices.size() == count)
            {
                for (int i = 0; i < count; i++)
                {
                    *pdata = m_meshBufferData.m_meshData[index].indices[i];
                    pdata++;
                }
            }
        }
        else if (dwFormatSize == 2)
        {
            unsigned short* pdata = (unsigned short*)&buffer[offset];
            if (m_meshBufferData.m_meshData[index].indices.size() == count)
            {
                for (int i = 0; i < count; i++)
                {
                    *pdata = (unsigned short)(m_meshBufferData.m_meshData[index].indices[i]);
                    pdata++;
                }
            }
        }
    }
    else if (attriName == "POSITION")
    {
        if (dwDimensions != 3)
            return false;
        if (dwFormatSize != 4)
            return false;
        float* pdata = (float*)&buffer[offset];
        if (m_meshBufferData.m_meshData[index].vertices.size() == count)
        {
            for (int i = 0; i < count; i++)
            {
                Vertex temp = m_meshBufferData.m_meshData[index].vertices[i];
                *pdata      = temp.px;
                pdata++;
                *pdata = temp.py;
                pdata++;
                *pdata = temp.pz;
                pdata++;
            }
        }
    }
    else if (attriName == "NORMAL")
    {
        if (dwDimensions != 3)
            return false;
        if (dwFormatSize != 4)
            return false;
        float* pdata = (float*)&buffer[offset];
        if (m_meshBufferData.m_meshData[index].vertices.size() == count)
        {
            for (int i = 0; i < count; i++)
            {
                Vertex temp = m_meshBufferData.m_meshData[index].vertices[i];
                *pdata      = temp.nx;
                pdata++;
                *pdata = temp.ny;
                pdata++;
                *pdata = temp.nz;
                pdata++;
            }
        }
    }
    else if (attriName == "TEXCOORD")
    {
        if (dwDimensions != 2)
            return false;
        if (dwFormatSize != 4)
            return false;
        float* pdata = (float*)&buffer[offset];
        if (m_meshBufferData.m_meshData[index].vertices.size() == count)
        {
            for (int i = 0; i < count; i++)
            {
                Vertex temp = m_meshBufferData.m_meshData[index].vertices[i];
                *pdata      = temp.tx;
                pdata++;
                *pdata = temp.ty;
                pdata++;
            }
        }
    }

    return true;
}

// Quick check for valid local file paths
bool GLTFCommon::fileExists(const char* fileName)
{
    bool          exists = false;
    std::ifstream test(fileName);
    if (test)
    {
        exists = true;
        test.close();
    }
    return exists;
}

int length(char* array)
{
    int count = 0;
    while (*array != '\0')
    {
        count++;
        array++;
    }
    return count;
}

int GLTFCommon::Save(std::string path, std::string filename, CMIPS* cmips)
{
    nlohmann::json j3temp;
    j3temp = j3;

    // Save Meshes
    //
    auto accessors   = j3temp["accessors"];
    auto bufferViews = j3temp["bufferViews"];
    auto meshes      = j3temp["meshes"];

    if (cmips)
    {
        cmips->Print("Saving: meshes");
    }

    int primInd = 0;

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        tfMesh* tfmesh     = &m_meshes[i];
        auto    primitives = meshes[i]["primitives"];

        for (unsigned int p = 0; p < primitives.size(); p++)
        {
            tfPrimitives* pPrimitive = &tfmesh->m_pPrimitives[p];

            auto indicesAccessor = accessors[primitives[p]["indices"].get<int>()];
            bool setBuffer =
                SetBufferData("indices", indicesAccessor, bufferViews, buffersData[0], primInd);  //buffersData[0]: only support single bin file
            if (!setBuffer)
            {
                PrintInfo("Error: save indices failed. Format size is not supported yet.");
                return -1;
            }

            std::vector<std::string> semanticNames;

            auto attribute = primitives[p]["attributes"];

            for (auto it = attribute.begin(); it != attribute.end(); it++)
            {
                // the glTF attributes name may end in a number (i.e. "TEXCOORD_0"), split the attribute name from the number
                //
                CMP_DWORD   semanticIndex = 0;
                std::string semanticName;
                SplitGltfAttribute(it.key(), &semanticName, &semanticIndex);

                if (semanticName == "POSITION" || semanticName == "NORMAL" || semanticName == "TEXCOORD")
                {
                    auto accessor = accessors[it.value().get<int>()];

                    // Get VB accessors
                    //
                    setBuffer = SetBufferData(semanticName, accessor, bufferViews, buffersData[0], primInd);
                    if (!setBuffer)
                    {
                        PrintInfo("Error: Write mesh vertex buffer data failed. Format size is not supported yet.");
                        return -1;
                    }
                }
            }
            primInd++;
        }
    }

    auto buffers = j3temp["buffers"];

    if (cmips)
    {
        cmips->SetProgress(0);
    }

    for (unsigned int i = 0; i < buffers.size(); i++)
    {
        std::string name       = buffers[i]["uri"].get<std::string>();
        int         byteLength = buffers[i]["byteLength"].get<int>();
        if (name.find(".bin") != std::string::npos)
        {
            std::size_t dotPos = filename.rfind('.');
            std::string dstBinFile = filename.substr(0, dotPos) + ".bin";
            j3temp["buffers"][i]["uri"] = dstBinFile;
            std::ofstream ff(path + dstBinFile, std::ios::out | std::ios::binary);

            if (cmips)
            {
                cmips->Print("Saving: buffers %s", dstBinFile.c_str());
            }

            if (byteLength > 0)
            {
                ff.write(buffersData[0], byteLength);
            }
            ff.close();
        }
        else
        {
            if (cmips)
            {
                cmips->Print("Error saving buffers, embedded is not supported.");
            }

            return -1;
        }
    }

    j3 = j3temp;

    return 0;
}

int GLTFCommon::Load(std::string path, std::string filename, CMIPS* cmips)
{
    m_path     = path;
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

    auto  buffers   = j3["buffers"];
    float fProgress = 0.0f;
    buffersData.resize(buffers.size());
    for (unsigned int i = 0; i < buffers.size(); i++)
    {
        std::string name = buffers[i]["uri"].get<std::string>();
        if (name.find(".bin") != std::string::npos)
        {
            //if (!fileExists(name.c_str()))
            //{
            //    name = path + name;
            //    if (!fileExists(name.c_str()))
            //    {
            //        throw ("gltf buffers uri .bin file not found!");
            //    }
            //}

            std::ifstream ff(path + name, std::ios::in | std::ios::binary);

            if (cmips)
            {
                cmips->Print("Processing: buffers %s", name.c_str());
            }

            ff.seekg(0, ff.end);
            std::streamoff length = ff.tellg();

            ff.seekg(0, ff.beg);

            if (length > 0)
            {
                char* p = new char[(unsigned int)length];
                ff.read(p, length);
                buffersData[i] = p;
            }
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
    auto accessors   = j3["accessors"];
    auto bufferViews = j3["bufferViews"];
    auto meshes      = j3["meshes"];
    m_meshes.resize(meshes.size());

    if (cmips)
    {
        cmips->Print("Processing: meshes");
    }

    float maxx      = 0.0f;
    float maxy      = 0.0f;
    bool  getBuffer = false;
    int   primInd   = 0;  //primitive index
    int   meshSize  = (int)meshes.size();
    m_meshBufferData.m_meshData.resize(meshSize);  //set default size to multiple meshes with 1 attribute
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        tfMesh* tfmesh     = &m_meshes[i];
        auto    primitives = meshes[i]["primitives"];
        tfmesh->m_pPrimitives.resize(primitives.size());
        if (primitives.size() > 1)
        {  //resize the mesh buffer if there are multiple attributes
            meshSize += (int)primitives.size() - 1;
            m_meshBufferData.m_meshData.resize(meshSize);
        }
        for (unsigned int p = 0; p < primitives.size(); p++)
        {
            tfPrimitives* pPrimitive = &tfmesh->m_pPrimitives[p];

            auto indicesAccessor = accessors[primitives[p]["indices"].get<int>()];

            getBuffer =
                GetBufferData("indices", indicesAccessor, bufferViews, buffersData[0], primInd);  //buffersData[0]: only support single bin file
            if (!getBuffer)
            {
                PrintInfo("Note: Indices Buffer dimension or type not supported for mesh processing.");
            }

            std::vector<std::string> semanticNames;

            auto attribute = primitives[p]["attributes"];

            for (auto it = attribute.begin(); it != attribute.end(); it++)
            {
                // the glTF attributes name may end in a number (i.e. "TEXCOORD_0"), split the attribute name from the number
                //
                CMP_DWORD   semanticIndex = 0;
                std::string semanticName;
                SplitGltfAttribute(it.key(), &semanticName, &semanticIndex);

                if (semanticName == "POSITION" || semanticName == "NORMAL" || semanticName == "TEXCOORD")
                {
                    auto accessor = accessors[it.value().get<int>()];

                    // Get VB accessors
                    //

                    getBuffer = GetBufferData(semanticName, accessor, bufferViews, buffersData[0], primInd);
                    if (!getBuffer)
                    {
                        PrintInfo("Note: Vertices Buffer dimension or type not supported for mesh processing.");
                        return -1;
                    }
                }
            }

            auto position = attribute["POSITION"];

            auto accessor = accessors[position.get<int>()];

            glm::vec4 max = GetVector(GetElementJsonArray(accessor, "max", {0.0, 0.0, 0.0, 0.0}));
            glm::vec4 min = GetVector(GetElementJsonArray(accessor, "min", {0.0, 0.0, 0.0, 0.0}));

            pPrimitive->m_center = (min + max) * 0.5f;
            pPrimitive->m_radius = max - pPrimitive->m_center;

            if (max.x > maxx)
                maxx = max.x;
            if (max.y > maxy)
                maxy = max.y;

            primInd++;
        }
    }

    // This is a work around for getting
    // the views correct for various models sizes based on [min,max] attributes
    // Should use bounding box of entire model as fix.
    m_distance = (std::max)(maxx, maxy);

    if (m_distance < 0.1)
        m_distance *= 2.0f;
    else if (m_distance < 0.2)
        m_distance = 6.0f;
    else if (m_distance < 0.9)
        m_distance *= 2.0f;
    else if (m_distance < 1.5)
        m_distance = 4.0f;
    else if (m_distance < 2.5)
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
        tfNode* tfnode = &m_nodes[i];

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
            tfnode->m_translation = glm::vec4(0, 0, 0, 0);

        if (node.find("rotation") != node.end())
        {
            const glm::vec4 rotation = GetVector(node["rotation"]);
            tfnode->m_rotation = glm::toMat4(glm::quat(rotation.w, rotation.x, rotation.y, rotation.z));
        }
        else if (node.find("matrix") != node.end())
        {
            tfnode->m_rotation = GetMatrix(node["matrix"]);
        }
        else
        {
            tfnode->m_rotation = glm::mat4x4(1.0f);
        }

        if (node.find("scale") != node.end())
            tfnode->m_scale = GetVector(node["scale"]);
        else
            tfnode->m_scale = glm::vec4(1, 1, 1, 0);
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

        tfAnimation* tfanim = &m_animations[i];
        for (unsigned int c = 0; c < channels.size(); c++)
        {
            auto        channel = channels[c];
            int         sampler = channel["sampler"];
            int         node    = GetElementInt(channel, "target/node", -1);
            std::string path    = GetElementString(channel, "target/path", std::string());

            tfChannel* tfchannel;

            auto ch = tfanim->m_channels.find(node);
            if (ch == tfanim->m_channels.end())
            {
                tfchannel = &tfanim->m_channels[node];
            }
            else
            {
                tfchannel = &ch->second;
            }

            auto input  = accessors[samplers[sampler]["input"].get<int>()];
            auto output = accessors[samplers[sampler]["output"].get<int>()];

            tfSampler* tfsmp = new tfSampler();

            // Get time line
            //
            GetBufferDetails(input, bufferViews, buffersData, &tfsmp->m_time);
            if (tfsmp->m_time.m_stride != 4)
            {
                tfsmp->m_time.m_stride = 4;
            }

            tfanim->m_duration = (std::max)(tfanim->m_duration, *(float*)tfsmp->m_time.Get(tfsmp->m_time.m_count - 1));

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
                if (tfsmp->m_value.m_stride != 4 * 4)
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

#ifdef _WIN32
void GLTFCommon::SetAnimationTime(int animationIndex, float time)
{
    if ((unsigned int)animationIndex < m_animations.size())
    {
        tfAnimation* anim = &m_animations[animationIndex];

        //loop animation
        time = fmod(time, anim->m_duration);

        for (auto it = anim->m_channels.begin(); it != anim->m_channels.end(); it++)
        {
            tfNode* pNode = &m_nodes[it->first];

            float frac, *pCurr, *pNext;

            if (it->second.m_pRotation != NULL)
            {
                it->second.m_pRotation->SampleLinear(time, &frac, &pCurr, &pNext);
                pNode->m_rotation = glm::toMat4(glm::slerp(glm::quat(pCurr[3], pCurr[0], pCurr[1], pCurr[2]), 
                                                           glm::quat(pNext[3], pNext[0], pNext[1], pNext[2]), 
                                                           frac));
            }

            if (it->second.m_pTranslation != NULL)
            {
                it->second.m_pTranslation->SampleLinear(time, &frac, &pCurr, &pNext);
                pNode->m_translation =
                    (1.0f - frac) * glm::vec4(pCurr[0], pCurr[1], pCurr[2], 0.0f) + (frac) * glm::vec4(pNext[0], pNext[1], pNext[2], 0.0f);
            }

            if (it->second.m_pScale != NULL)
            {
                it->second.m_pScale->SampleLinear(time, &frac, &pCurr, &pNext);
                pNode->m_scale = (1.0f - frac) * glm::vec4(pCurr[0], pCurr[1], pCurr[2], 0.0f) + (frac) * glm::vec4(pNext[0], pNext[1], pNext[2], 0.0f);
            }
        }
    }
}

void GLTFCommon::TransformNodes(NodeMatrixPostTransform* pNodesOut, CMP_DWORD* pCount)
{
    DWORD cnt = 0;

    std::vector<NodeMatrixPostTransform> stack;

    tfScene* pScene = &m_scenes[0];

    for (unsigned int n = 0; n < pScene->m_nodes.size(); n++)
    {
        tfNode* pNode = pScene->m_nodes[n];

        NodeMatrixPostTransform st = {pNode, pNode->GetWorldMat()};
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
                NodeMatrixPostTransform st = {top.pN->m_children[i], top.pN->m_children[i]->GetWorldMat() * top.m};
                stack.push_back(st);
            }
        }
    }

    *pCount = cnt;
}
#endif
