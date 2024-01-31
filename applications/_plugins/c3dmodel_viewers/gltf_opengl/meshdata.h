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
#ifndef MESHDATA_H
#define MESHDATA_H

#include <qopengl.h>
#include <QVector>
#include <QVector3D>

#include "modeldata.h"

class MeshData
{
public:
    MeshData();
    void LoadData(const CMP_Mesh& l_mesh);

    bool MeshLoaded;

    float m_centerx = 0;
    float m_centery = 0;
    float m_centerz = 0;

    float m_scaleh = 1;

    float m_extentx = 0;
    float m_extenty = 0;
    float m_extentz = 0;

    float m_minx = 0;
    float m_miny = 0;
    float m_minz = 0;

    float m_maxx = 0;
    float m_maxy = 0;
    float m_maxz = 0;

    const GLfloat* constVertexData() const
    {
        return m_vertexData.constData();
    }
    int count() const
    {
        return m_vertexCount;
    }
    int vertexCount() const
    {
        return m_vertexCount / 6;
    }

    const GLint* constIndexData() const
    {
        return m_indexData.constData();
    }
    int indexCount() const
    {
        return m_indexCount;
    }

    void calc_bounds(int width, int height, const CMP_Mesh& mesh);

    // Optimizer LOD
    CMP_Mesh         optimize(const CMP_Mesh& mesh, int lod);
    void             LoadIndex(const CMP_Mesh& l_mesh);
    QVector<GLfloat> m_vertexData;
    QVector<GLint>   m_indexData;

    CMP_Mesh m_basemesh;

private:
    void addVertices(const QVector3D& v, const QVector3D& n);
    int  m_vertexCount;
    int  m_indexCount;
    bool m_hasNormals;

    void calcNormals(CMP_Mesh& l_mesh);
};

#endif  // LOGO_H
