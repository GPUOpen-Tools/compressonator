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
#ifndef GLTFCOMMOM_H
#define GLTFCOMMOM_H

#include "Compressonator.h"
#include "Common.h"
#include "GltfFeatures.h"
#include "GltfStructures.h"
#include "ModelData.h"

#include <json/json.h>

#ifdef USE_TINYGLTF2
#include "tiny_gltf2.h"
#include "tiny_gltf2_utils.h"
#endif

#include <glm/matrix.hpp>
#include <glm/vec4.hpp>

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
    glm::vec4 m_center;
    glm::vec4 m_radius;
};

struct tfMesh
{
    std::vector<tfPrimitives> m_pPrimitives;
};

struct NodeMatrixPostTransform
{ 
    tfNode *pN; glm::mat4x4 m; 
};

class GLTFCommon
{
public:
    nlohmann::json j3;
    std::string m_path = "";
    std::string m_filename = "";         // *1*
    std::vector<tfScene> m_scenes;
    std::vector<char *> buffersData;
    std::vector<tfMesh> m_meshes;

    CMODEL_DATA m_meshBufferData;  // vertices and indices buffers for mesh process 

    float m_distance = 4.0f;       // calc from mesh atrribes what the max value in ether x or y direction is going to be
    double m_CommonLoadTime = 0.0; // Total time spent in GLTF Common to load data from files
    bool  isBinFile = false;

    int   Load(std::string path, std::string filename, CMIPS *cmips = NULL);
    int   Save(std::string path, std::string filename, CMIPS *cmips = NULL);
    void  Unload();
#ifdef _WIN32
    void  SetAnimationTime(int animationIndex, float time);
    void  TransformNodes(NodeMatrixPostTransform *pNodesOut, CMP_DWORD *pCount);
#endif
    CMP_DWORD GetNodeCount() { return (CMP_DWORD)m_nodes.size(); }
    bool  fileExists(const char* fileName);
    bool  GetBufferData(std::string attriName, nlohmann::json::object_t accessor, nlohmann::json::array_t bufferViews, char * buffer, int index);
    bool  SetBufferData(std::string attriName, nlohmann::json::object_t accessor, nlohmann::json::array_t bufferViews, char * buffer, int index);
    

#ifdef USE_TINYGLTF2
    tinygltf2::Model   m_model;
#endif

private:
    std::vector<tfNode>         m_nodes;
    std::vector<tfAnimation>    m_animations;
};

void GetBufferDetails(nlohmann::json::object_t  accessor   , nlohmann::json::array_t bufferViews  , std::vector<char *> buffers,   tfAccessor *pAccessor);
//void GetBufferDetails(nlohmann::json::object_t *pInAccessor, nlohmann::json::array_t *pBufferViews, std::vector<char *> *pBuffers, tfAccessor *pAccessor);

#endif
