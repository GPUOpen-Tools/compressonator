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

#pragma once
#include "../json/json.h"

#include "GltfFeatures.h"
#include "MIPS.h"
#include "GltfStructures.h"

// The GlTF file is loaded in 2 steps
// 1) loading the GPU agnostic data (This is done in the GLTFCommon class you can see here below)
//     - nodes
//     - scenes
//     - animations
//     - binary buffers
//     - GPU specific data
//
//2) Loading the GPU specific data and rendering it with a specific technique, this is done in the GltfPbr class (see GltfPbr.h)
//     This split allows us to have different techniques to render a same model. 
//     In the future we'll have different techniques to render the scene:
//         - depth only
//         - shadow maps
//         - motion vectors
//         - transparent pass
//         - ...

struct tfPrimitives
{
    XMVECTOR m_center;
    XMVECTOR m_radius;
};

struct tfMesh
{
    std::vector<tfPrimitives> m_pPrimitives;
};

struct NodeMatrixPostTransform
{ 
    tfNode *pN; XMMATRIX m; 
};

class GLTFCommon
{
public:
    nlohmann::json j3;
    std::string m_path = "";
    std::string m_filename = "";         // *1*
    std::vector<tfScene> m_scenes;
    std::vector<char *> buffersData;

    int Load(std::string path, std::string filename, CMIPS *cmips = NULL);
    void Unload();
    void SetAnimationTime(int animationIndex, float time);
    void TransformNodes(NodeMatrixPostTransform *pNodesOut, DWORD *pCount);
    DWORD GetNodeCount() { return (DWORD)m_nodes.size(); }

    std::vector<tfMesh> m_meshes;

    float m_distance   = 4.0f;       // calc from mesh atrribes what the max value in ether x or y direction is going to be

private:
    std::vector<tfNode> m_nodes;
    std::vector<tfAnimation> m_animations;
};

void GetBufferDetails(nlohmann::json::object_t accessor, nlohmann::json::array_t bufferViews, std::vector<char *> buffers, tfAccessor *pAccessor);