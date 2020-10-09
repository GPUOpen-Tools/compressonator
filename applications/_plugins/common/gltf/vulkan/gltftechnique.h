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

#include "gltfstructures.h"

#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.h>

#include <vector>

// Forward Declaration
class DynamicBufferRingVK;
class StaticBufferPoolVK;
class GLTFCommon;

// This class takes a GltfCommon class (that holds all the non-GPU specific data) as an input and loads all the GPU specific data
//
struct Primitives {
    VkIndexType m_indexType;
    uint32_t m_NumIndices;
    VkDescriptorBufferInfo m_IBV;
    std::vector<VkDescriptorBufferInfo> m_VBV;
};

class GltfTechnique {
  public:
    virtual void OnDestroy() = 0;
    void Draw(VkCommandBuffer cmd_buf);

  protected:
    GLTFCommon *m_pGLTFData;

    DynamicBufferRingVK *m_pDynamicBufferRing;
    StaticBufferPoolVK *m_pStaticBufferPool;

    void CreateGeometry(tfAccessor indexBuffer, std::vector<tfAccessor> &vertexBuffers, Primitives *pPrimitive);
    virtual void DrawMesh(VkCommandBuffer cmd_buf, int meshIndex, const glm::mat4x4 &matrix) = 0;
};



