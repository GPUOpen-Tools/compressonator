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

#include <cstdio>

void GltfTechnique::Draw(ID3D12GraphicsCommandList* pCommandList)
{
    uint32_t dwNodesVisible;

    std::vector<NodeMatrixPostTransform> nodes(m_pGLTFData->GetNodeCount());
    if (nodes.size() == 0) 
        return;
    m_pGLTFData->TransformNodes(nodes.data(), &dwNodesVisible);

    for (uint32_t i = 0; i < dwNodesVisible; i++)
    {
        tfNode *pNode = nodes[i].pN;
        if (pNode != NULL && pNode->meshIndex >= 0)
        {
            const DirectX::XMMATRIX mat = { nodes[i].m[0][0], nodes[i].m[0][1], nodes[i].m[0][2], nodes[i].m[0][3],
                                            nodes[i].m[1][0], nodes[i].m[1][1], nodes[i].m[1][2], nodes[i].m[1][3],
                                            nodes[i].m[2][0], nodes[i].m[2][1], nodes[i].m[2][2], nodes[i].m[2][3],
                                            nodes[i].m[3][0], nodes[i].m[3][1], nodes[i].m[3][2], nodes[i].m[3][3] };
            DrawMesh(pCommandList, pNode->meshIndex, mat);
        }
    }
}

