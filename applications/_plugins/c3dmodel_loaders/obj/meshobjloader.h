//=====================================================================
// Copyright 2018-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_

#include <algorithm>
#include <vector>
#include <functional>

#include "modeldata.h"

//=================================================================================================================================
/// \brief An OBJ file loader
///  This OBJ loader supports a subset of the OBJ file format.  In particular:
///    - It supports only polygonal primitives
///    - It does not support materials
///    - It does not support texture coordinates with three channels
///
//=================================================================================================================================
class MeshObjLoader
{
public:
    /// Loads a mesh from a wavefront OBJ file
    int LoadGeometry(const char* strFileName, CMODEL_DATA& mesh, CMIPS* cmips, CMP_Feedback_Proc pFeedbackProc);

private:
    // VertexHashData
    struct VertexHashData
    {
        unsigned int vertexIndex;
        unsigned int texCoordIndex;
        unsigned int normalIndex;

        unsigned int finalIndex;
    };  // End of VertexHashData

    struct vertex_less : public std::less<VertexHashData>
    {
        bool operator()(const VertexHashData& x, const VertexHashData& y) const
        {
            if (x.vertexIndex < y.vertexIndex)
            {
                return true;
            }

            if (x.vertexIndex > y.vertexIndex)
            {
                return false;
            }

            if (x.texCoordIndex < y.texCoordIndex)
            {
                return true;
            }

            if (x.texCoordIndex > y.texCoordIndex)
            {
                return false;
            }

            if (x.normalIndex < y.normalIndex)
            {
                return true;
            }

            if (x.normalIndex > y.normalIndex)
            {
                return false;
            }

            return false;
        };  // End of operator()
    };      // End of vertex_less

    bool BuildModel(const std::vector<ObjVertex3D>& vertices,
                    const std::vector<ObjVertex3D>& normals,
                    const std::vector<ObjVertex2D>& texCoords,
                    std::vector<ObjVertexFinal>&    finalVertices,
                    std::vector<ObjFace>&           faces);

    // Returns how many vertices are specified in one line read from OBJ
    int GetNumberOfVerticesInLine(const char* szLine);

    // Read vertex indices
    void ReadVertexIndices(const char* szLine, int numVertsInLine, bool bHasTexCoords, bool bHasNormals, int* pVertIndices, int* pTexCoords, int* pNormals);

    // Buildup vertex hash map
    void MeshObjLoader::BuildFinalVertices(const std::vector<ObjVertex3D>& vertices,
                                           const std::vector<ObjVertex3D>& normals,
                                           const std::vector<ObjVertex2D>& texCoords,
                                           std::vector<ObjFace>&           faces,
                                           CMP_Mesh&                       meshData,
                                           CMIPS*                          cmips,
                                           CMP_Feedback_Proc               pFeedbackProc);

    void getFileNameExt(const char* FilePathName, char* fnameExt, int maxbuffsize);
};

#endif  // _OBJLOADER_H_
