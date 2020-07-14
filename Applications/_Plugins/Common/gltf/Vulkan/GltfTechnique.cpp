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


#include "GltfTechnique.h"

void GltfTechnique::Draw(VkCommandBuffer cmd_buf)
{
    CMP_DWORD dwNodesVisible;
    std::vector<NodeMatrixPostTransform> nodes(m_pGLTFData->GetNodeCount());
    m_pGLTFData->TransformNodes(nodes.data(), &dwNodesVisible);

    for (std::uint32_t i = 0; i < dwNodesVisible; i++)
    {
        tfNode *pNode = nodes[i].pN;
        if (pNode != NULL && pNode->meshIndex >= 0)
        {
            DrawMesh(cmd_buf, pNode->meshIndex, nodes[i].m);
        }
    }
}

void GltfTechnique::CreateGeometry(tfAccessor indexBuffer, std::vector<tfAccessor> &vertexBuffers, Primitives *pPrimitive)
{
    pPrimitive->m_NumIndices = indexBuffer.m_count;

    pPrimitive->m_indexType = (indexBuffer.m_stride == 4) ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16;

    void *pDest;
    if (indexBuffer.m_stride == 1)
    {
        // apparently some exporters use 8bit indices
        m_pStaticBufferPool->AllocIndexBuffer(indexBuffer.m_count, 2 * indexBuffer.m_stride, &pDest, &pPrimitive->m_IBV);
        for (int i = 0; i < indexBuffer.m_count; i++)
            ((short*)pDest)[i] = ((unsigned char*)indexBuffer.m_data)[i];
    }
    else
    {
        m_pStaticBufferPool->AllocIndexBuffer(indexBuffer.m_count, indexBuffer.m_stride, &pDest, &pPrimitive->m_IBV);
        memcpy(pDest, indexBuffer.m_data, indexBuffer.m_stride*indexBuffer.m_count);
    }

    // load those buffers into the GPU
    pPrimitive->m_VBV.resize(vertexBuffers.size());
    for (unsigned int i = 0; i < vertexBuffers.size(); i++)
    {
        tfAccessor *pVertexAccessor = &vertexBuffers[i];

        void *pDest;
        m_pStaticBufferPool->AllocVertexBuffer(pVertexAccessor->m_count, pVertexAccessor->m_stride, &pDest, &pPrimitive->m_VBV[i]);
        memcpy(pDest, pVertexAccessor->m_data, pVertexAccessor->m_stride*pVertexAccessor->m_count);
    }
}
