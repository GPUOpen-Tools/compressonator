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
#include "meshdata.h"
#include <qmath.h>

MeshData::MeshData()
{
    m_vertexCount = 0;
    m_indexCount  = 0;
}

void MeshData::LoadData(const CMP_Mesh& l_mesh)
{
    // Clean old data
    m_vertexData.clear();
    m_indexData.clear();
    m_vertexCount = 0;
    m_indexCount  = 0;
    m_hasNormals  = false;

    // assign new mesh
    m_basemesh = l_mesh;

    int numVertices = (int)l_mesh.vertices.size();

    for (int i = 0; i < numVertices; i++)
    {
        if (l_mesh.vertices[i].nx > 0 || l_mesh.vertices[i].ny > 0 || l_mesh.vertices[i].nz > 0)
            m_hasNormals = true;
    }

    // Set index data
    int numIndices = (int)l_mesh.indices.size();
    m_indexData.resize(numIndices);  // Face Index data 3 ints
    for (int i = 0; i < numIndices; i++)
    {
        m_indexData[i] = (GLint)l_mesh.indices[i];
    }
    m_indexCount = numIndices;

    // Calc Normals if we dont have any in original
    if (!m_hasNormals)
        calcNormals(m_basemesh);

    // Set vertex buffer data
    m_vertexData.resize(numVertices * 6);  // vertices 3 floats + normals 3 floats = 6
    for (int i = 0; i < numVertices; i++)
    {
        addVertices(QVector3D((GLfloat)m_basemesh.vertices[i].px, (GLfloat)m_basemesh.vertices[i].py, (GLfloat)m_basemesh.vertices[i].pz),
                    QVector3D((GLfloat)m_basemesh.vertices[i].nx, (GLfloat)m_basemesh.vertices[i].ny, (GLfloat)m_basemesh.vertices[i].nz));
    }
}

void MeshData::LoadIndex(const CMP_Mesh& l_mesh)
{
    // Clean old data
    m_indexData.clear();
    m_indexCount = 0;

    int numIndices = (int)l_mesh.indices.size();
    m_indexData.resize(numIndices);  // Face Index data 3 ints
    for (int i = 0; i < numIndices; i++)
    {
        m_indexData[i] = (GLint)l_mesh.indices[i];
    }
    m_indexCount = numIndices;
}

void MeshData::addVertices(const QVector3D& v, const QVector3D& n)
{
    GLfloat* p = m_vertexData.data() + m_vertexCount;
    *p++       = v.x();
    *p++       = v.y();
    *p++       = v.z();

    *p++ = n.x();
    *p++ = n.y();
    *p++ = n.z();
    m_vertexCount += 6;
}

void MeshData::calc_bounds(int width, int height, const CMP_Mesh& l_mesh)
{
    m_centerx = 0;
    m_centery = 0;
    m_centerz = 0;

    m_minx = FLT_MAX;
    m_miny = FLT_MAX;
    m_minz = FLT_MAX;
    m_maxx = 0;
    m_maxy = 0;
    m_maxz = 0;

    for (size_t i = 0; i < l_mesh.vertices.size(); ++i)
    {
        const Vertex& v = l_mesh.vertices[i];

        if (v.px > m_maxx)
            m_maxx = v.px;

        if (v.px < m_minx)
            m_minx = v.px;

        if (v.py > m_maxy)
            m_maxy = v.py;

        if (v.py < m_miny)
            m_miny = v.py;

        if (v.pz > m_maxz)
            m_maxz = v.pz;

        if (v.pz < m_minz)
            m_minz = v.pz;
    }

    m_centerx = float((m_maxx - m_minx) / 2) + m_minx;
    m_centery = float((m_maxy - m_miny) / 2) + m_miny;
    m_centerz = float((m_maxz - m_minz) / 2) + m_minz;

    m_extentx = (std::max)(fabsf(m_maxx - m_minx), fabsf(m_maxx));
    m_extenty = (std::max)(fabsf(m_maxy - m_miny), fabsf(m_maxy));
    m_extentz = (std::max)(fabsf(m_maxz - m_minz), fabsf(m_maxz));

    m_scaleh = width > height ? float(width) / float(height) : float(height) / float(width);
}

CMP_Mesh MeshData::optimize(const CMP_Mesh& mesh, int lod)
{
    float  threshold          = powf(0.7f, float(lod));
    size_t target_index_count = size_t(mesh.indices.size() * threshold);

    CMP_Mesh result = mesh;
    result.indices.resize(meshopt_simplify(
        &result.indices[0], &result.indices[0], mesh.indices.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex), target_index_count));
    return result;
}

#include "jml.h"
using namespace JML;

void MeshData::calcNormals(CMP_Mesh& mesh)
{
    //generate flat shading normals, this should have more obvious view different compare to smooth shading
    for (int i = 0; i < m_indexCount; i += 3)
    {
        // get the three vertices that make the faces
        Vec3f p0 = Vec3f(mesh.vertices[m_indexData[i]].px, mesh.vertices[m_indexData[i]].py, mesh.vertices[m_indexData[i]].pz);
        Vec3f p1 = Vec3f(mesh.vertices[m_indexData[i + 1]].px, mesh.vertices[m_indexData[i + 1]].py, mesh.vertices[m_indexData[i + 1]].pz);
        Vec3f p2 = Vec3f(mesh.vertices[m_indexData[i + 2]].px, mesh.vertices[m_indexData[i + 2]].py, mesh.vertices[m_indexData[i + 2]].pz);

        Vec3f v1     = p1 - p0;
        Vec3f v2     = p2 - p0;
        Vec3f normal = Cross(v1, v2);

        normal = Normalize(normal);

        mesh.vertices[m_indexData[i]].nx = normal.x;
        mesh.vertices[m_indexData[i]].ny = normal.y;
        mesh.vertices[m_indexData[i]].nz = normal.z;

        mesh.vertices[m_indexData[i + 1]].nx = normal.x;
        mesh.vertices[m_indexData[i + 1]].ny = normal.y;
        mesh.vertices[m_indexData[i + 1]].nz = normal.z;

        mesh.vertices[m_indexData[i + 2]].nx = normal.x;
        mesh.vertices[m_indexData[i + 2]].ny = normal.y;
        mesh.vertices[m_indexData[i + 2]].nz = normal.z;
    }
}

// Average Normals
/******
// Now loop through each vertex vector, and avarage out all the normals stored.
for (int i = 0; i < num_vertices; ++i)
{
    for (int j = 0; j < normal_buffer[i].size(); ++j)
        vertices[i].normal += normal_buffer[i][j];

    vertices[i].normal /= normal_buffer[i].size();
}
************************/
