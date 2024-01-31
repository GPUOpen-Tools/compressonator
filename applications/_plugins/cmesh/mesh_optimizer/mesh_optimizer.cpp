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
#include "mesh_optimizer.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <random>

#define min(a, b) (((a) < (b)) ? (a) : (b))

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_Mesh_Optimizer)
SET_PLUGIN_TYPE("MESH_OPTIMIZER")
SET_PLUGIN_NAME("TOOTLE_MESH")
#else
void* make_Plugin_Mesh_Optimizer()
{
    return new Plugin_Mesh_Optimizer;
}
#endif

namespace cmesh_mesh_opt
{
CMIPS* g_CMIPS = nullptr;
}

using namespace cmesh_mesh_opt;

Plugin_Mesh_Optimizer::Plugin_Mesh_Optimizer()
{
    m_InitOK = false;

    m_settings.pMeshName              = NULL;
    m_settings.bOptimizeOverdraw      = true;
    m_settings.bOptimizeVCache        = true;
    m_settings.bOptimizeVCacheFifo    = false;
    m_settings.bOptimizeVFetch        = true;
    m_settings.bRandomizeMesh         = false;
    m_settings.bSimplifyMesh          = false;
    m_settings.nCacheSize             = 16;
    m_settings.nlevelofDetails        = 5;
    m_settings.nOverdrawACMRthreshold = 1.05f;
}

Plugin_Mesh_Optimizer::~Plugin_Mesh_Optimizer()
{
    CleanUp();
}

int Plugin_Mesh_Optimizer::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{
#ifdef _WIN32
    pPluginVersion->guid = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor    = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor    = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_Mesh_Optimizer::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        g_CMIPS              = static_cast<CMIPS*>(Shared);
        g_CMIPS->m_infolevel = 0x01;  // Turn on print Info
        return 0;
    }
    return 1;
}

int Plugin_Mesh_Optimizer::Init()
{
    if (m_InitOK)
        return 0;

    //TootleResult result;
    //
    //// initialize Tootle
    //result = TootleInit();
    //if (result != TOOTLE_OK)
    //{
    //    return -1;
    //}

    m_InitOK = true;

    return 0;
}

int Plugin_Mesh_Optimizer::CleanUp()
{
    if (m_InitOK)
    {
        // clean up mesh optimizer
        //TootleCleanup();
        m_InitOK = false;
    }
    return 0;
}

struct Triangle
{
    Vertex v[3];

    bool operator<(const Triangle& other) const
    {
        return memcmp(v, other.v, sizeof(Triangle)) < 0;
    }
};

CMP_Mesh generatePlane(unsigned int N)
{
    CMP_Mesh result;

    result.vertices.reserve((N + 1) * (N + 1));
    result.indices.reserve(N * N * 6);

    for (unsigned int y = 0; y <= N; ++y)
        for (unsigned int x = 0; x <= N; ++x)
        {
            Vertex v = {float(x), float(y), 0, 0, 0, 1, float(x) / float(N), float(y) / float(N)};

            result.vertices.push_back(v);
        }

    for (unsigned int y = 0; y < N; ++y)
        for (unsigned int x = 0; x < N; ++x)
        {
            result.indices.push_back((y + 0) * (N + 1) + (x + 0));
            result.indices.push_back((y + 0) * (N + 1) + (x + 1));
            result.indices.push_back((y + 1) * (N + 1) + (x + 0));

            result.indices.push_back((y + 1) * (N + 1) + (x + 0));
            result.indices.push_back((y + 0) * (N + 1) + (x + 1));
            result.indices.push_back((y + 1) * (N + 1) + (x + 1));
        }

    return result;
}

bool isMeshValid(const CMP_Mesh& mesh)
{
    if (mesh.indices.size() % 3 != 0)
        return false;

    for (size_t i = 0; i < mesh.indices.size(); ++i)
        if (mesh.indices[i] >= mesh.vertices.size())
            return false;

    return true;
}

bool rotateTriangle(Triangle& t)
{
    int c01 = memcmp(&t.v[0], &t.v[1], sizeof(Vertex));
    int c02 = memcmp(&t.v[0], &t.v[2], sizeof(Vertex));
    int c12 = memcmp(&t.v[1], &t.v[2], sizeof(Vertex));

    if (c12 < 0 && c01 > 0)
    {
        // 1 is minimum, rotate 012 => 120
        Vertex tv = t.v[0];
        t.v[0] = t.v[1], t.v[1] = t.v[2], t.v[2] = tv;
    }
    else if (c02 > 0 && c12 > 0)
    {
        // 2 is minimum, rotate 012 => 201
        Vertex tv = t.v[2];
        t.v[2] = t.v[1], t.v[1] = t.v[0], t.v[0] = tv;
    }

    return c01 != 0 && c02 != 0 && c12 != 0;
}

void deindexMesh(std::vector<Triangle>& dest, const CMP_Mesh& mesh)
{
    size_t triangles = mesh.indices.size() / 3;

    dest.reserve(triangles);

    for (size_t i = 0; i < triangles; ++i)
    {
        Triangle t;

        for (int k = 0; k < 3; ++k)
            t.v[k] = mesh.vertices[mesh.indices[i * 3 + k]];

        // skip degenerate triangles since some algorithms don't preserve them
        if (rotateTriangle(t))
            dest.push_back(t);
    }
}

bool areMeshesEqual(const CMP_Mesh& lhs, const CMP_Mesh& rhs)
{
    std::vector<Triangle> lt, rt;
    deindexMesh(lt, lhs);
    deindexMesh(rt, rhs);

    std::sort(lt.begin(), lt.end());
    std::sort(rt.begin(), rt.end());

    return lt.size() == rt.size() && memcmp(&lt[0], &rt[0], lt.size() * sizeof(Triangle)) == 0;
}

void optNone(CMP_Mesh& mesh)
{
    (void)mesh;
}

void optRandomShuffle(CMP_Mesh& mesh)
{
    std::vector<unsigned int> faces(mesh.indices.size() / 3);

    for (size_t i = 0; i < faces.size(); ++i)
        faces[i] = static_cast<unsigned int>(i);

    std::random_device rd;
    std::mt19937       g(rd());
    std::shuffle(faces.begin(), faces.end(), g);

    std::vector<unsigned int> result(mesh.indices.size());

    for (size_t i = 0; i < faces.size(); ++i)
    {
        result[i * 3 + 0] = mesh.indices[faces[i] * 3 + 0];
        result[i * 3 + 1] = mesh.indices[faces[i] * 3 + 1];
        result[i * 3 + 2] = mesh.indices[faces[i] * 3 + 2];
    }

    mesh.indices.swap(result);
}

void optCache(CMP_Mesh& mesh)
{
    meshopt_optimizeVertexCache(&mesh.indices[0], &mesh.indices[0], mesh.indices.size(), mesh.vertices.size(), 16);
}

void optCacheFifo(CMP_Mesh& mesh)
{
    meshopt_optimizeVertexCacheFifo(&mesh.indices[0], &mesh.indices[0], mesh.indices.size(), mesh.vertices.size(), m_settings.nCacheSize);
}

void optOverdraw(CMP_Mesh& mesh)
{
    // use worst-case ACMR threshold so that overdraw optimizer can sort *all* triangles
    // warning: this significantly deteriorates the vertex cache efficiency so it is not advised; look at optComplete for the recommended method
    //const float kThreshold = 3.f;
    meshopt_optimizeOverdraw(
        &mesh.indices[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex), m_settings.nOverdrawACMRthreshold);
}

void optFetch(CMP_Mesh& mesh)
{
    meshopt_optimizeVertexFetch(&mesh.vertices[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0], mesh.vertices.size(), sizeof(Vertex));
}

void optComplete(CMP_Mesh& mesh)
{
    // vertex cache optimization should go first as it provides data for overdraw
    meshopt_optimizeVertexCache(&mesh.indices[0], &mesh.indices[0], mesh.indices.size(), mesh.vertices.size(), 16);

    // reorder indices for overdraw, balancing overdraw and vertex cache efficiency
    const float kThreshold = 1.05f;  // allow up to 5% worse ACMR to get more reordering opportunities for overdraw
    meshopt_optimizeOverdraw(&mesh.indices[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex), kThreshold);

    // vertex fetch optimization should go last as it depends on the final index order
    meshopt_optimizeVertexFetch(&mesh.vertices[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0], mesh.vertices.size(), sizeof(Vertex));
}

void optSimplify(CMP_Mesh& mesh)
{
    const size_t lod                = m_settings.nlevelofDetails;
    float        threshold          = powf(0.7f, float(lod));
    size_t       target_index_count = size_t(mesh.indices.size() * threshold);

    CMP_Mesh result = mesh;
    result.indices.resize(meshopt_simplify(
        &result.indices[0], &result.indices[0], mesh.indices.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex), target_index_count));
    mesh.indices.swap(result.indices);
}

void optCompleteSimplify(CMP_Mesh& mesh)
{
    const size_t lod_count = m_settings.nlevelofDetails;

    // generate 4 LOD levels (1-4), with each subsequent LOD using 70% triangles
    // note that each LOD uses the same (shared) vertex buffer
    // vertex cache optimization should go first as it provides data for overdraw

    //std::vector<unsigned int> lods[lod_count];
    std::vector<std::vector<unsigned int>> lods;
    lods.resize(lod_count);

    lods[0] = mesh.indices;

    for (size_t i = 1; i < lod_count; ++i)
    {
        std::vector<unsigned int>& lod = lods[i];

        float  threshold          = powf(0.7f, float(i));
        size_t target_index_count = size_t(mesh.indices.size() * threshold) / 3 * 3;

        // we can simplify all the way from base level or from the last result
        // simplifying from the base level sometimes produces better results, but simplifying from last level is faster
        const std::vector<unsigned int>& source = lods[i - 1];

        lod.resize(source.size());
        lod.resize(meshopt_simplify(
            &lod[0], &source[0], source.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex), min(source.size(), target_index_count)));
    }

    // optimize each individual LOD for vertex cache & overdraw
    for (size_t i = 0; i < lod_count; ++i)
    {
        std::vector<unsigned int>& lod = lods[i];

        meshopt_optimizeVertexCache(&lod[0], &lod[0], lod.size(), mesh.vertices.size(), 16);
        meshopt_optimizeOverdraw(&lod[0], &lod[0], lod.size(), &mesh.vertices[0].px, mesh.vertices.size(), sizeof(Vertex), 1.0f);
    }

    // concatenate all LODs into one IB
    // note: the order of concatenation is important - since we optimize the entire IB for vertex fetch,
    // putting coarse LODs first makes sure that the vertex range referenced by them is as small as possible
    // some GPUs process the entire range referenced by the index buffer region so doing this optimizes the vertex transform
    // cost for coarse LODs
    std::vector<size_t> lod_index_offsets;
    lod_index_offsets.resize(lod_count);
    std::vector<size_t> lod_index_counts;
    lod_index_counts.resize(lod_count);

    size_t total_index_count = 0;

    for (size_t i = lod_count - 1; i >= 0; --i)
    {
        lod_index_offsets[i] = total_index_count;
        lod_index_counts[i]  = lods[i].size();

        total_index_count += lods[i].size();
    }

    mesh.indices.resize(total_index_count);

    for (size_t i = 0; i < lod_count; ++i)
    {
        memcpy(&mesh.indices[lod_index_offsets[i]], lods[i].data(), lods[i].size() * sizeof(lods[i][0]));
    }

    // vertex fetch optimization should go last as it depends on the final index order
    // note that the order of LODs above affects vertex fetch results
    meshopt_optimizeVertexFetch(&mesh.vertices[0], &mesh.indices[0], mesh.indices.size(), &mesh.vertices[0], mesh.vertices.size(), sizeof(Vertex));

    printf("%-9s:", "Simplify");

    for (size_t i = 0; i < sizeof(lods) / sizeof(lods[0]); ++i)
    {
        printf(" LOD%d %d", int(i), int(lods[i].size()) / 3);
    }

    printf("\n");

    // for using LOD data at runtime, in addition to VB and IB you have to save lod_index_offsets/lod_index_counts.
    (void)lod_index_offsets;
    (void)lod_index_counts;
}

void optimize(CMP_Mesh& mesh, const char* name, void (*optf)(CMP_Mesh& mesh), bool compare = true)
{
    CMP_Mesh copy = mesh;

    clock_t start = clock();
    optf(mesh);
    clock_t end = clock();

    assert(isMeshValid(copy));
    assert(!compare || areMeshesEqual(mesh, copy));
    (void)compare;

    PrintInfo("%-9s: Process in %.2f msec\n", name, double(end - start) / CLOCKS_PER_SEC * 1000);
}

void encodeIndex(const CMP_Mesh& mesh)
{
    clock_t start = clock();

    std::vector<unsigned char> buffer(meshopt_encodeIndexBufferBound(mesh.indices.size(), mesh.vertices.size()));
    buffer.resize(meshopt_encodeIndexBuffer(&buffer[0], buffer.size(), &mesh.indices[0], mesh.indices.size()));

    clock_t middle = clock();

    std::vector<unsigned int> result(mesh.indices.size());
    int                       res = meshopt_decodeIndexBuffer(&result[0], mesh.indices.size(), &buffer[0], buffer.size());
    assert(res == 0);
    (void)res;

    clock_t end = clock();

    for (size_t i = 0; i < mesh.indices.size(); i += 3)
    {
        assert((result[i + 0] == mesh.indices[i + 0] && result[i + 1] == mesh.indices[i + 1] && result[i + 2] == mesh.indices[i + 2]) ||
               (result[i + 1] == mesh.indices[i + 0] && result[i + 2] == mesh.indices[i + 1] && result[i + 0] == mesh.indices[i + 2]) ||
               (result[i + 2] == mesh.indices[i + 0] && result[i + 0] == mesh.indices[i + 1] && result[i + 1] == mesh.indices[i + 2]));
    }

    printf("Index encode: %.1f bits/triangle; encode %.2f msec, decode %.2f msec (%.2f Mtri/s)\n",
           double(buffer.size() * 8) / double(mesh.indices.size() / 3),
           double(middle - start) / CLOCKS_PER_SEC * 1000,
           double(end - middle) / CLOCKS_PER_SEC * 1000,
           (double(result.size() / 3) / 1e6) / (double(end - middle) / CLOCKS_PER_SEC));
}

void stripify(const CMP_Mesh& mesh)
{
    clock_t                   start = clock();
    std::vector<unsigned int> strip(mesh.indices.size() / 3 * 4);
    strip.resize(meshopt_stripify(&strip[0], &mesh.indices[0], mesh.indices.size(), mesh.vertices.size()));
    clock_t end = clock();

    CMP_Mesh copy = mesh;
    copy.indices.resize(meshopt_unstripify(&copy.indices[0], &strip[0], strip.size()));

    assert(isMeshValid(copy));
    assert(areMeshesEqual(mesh, copy));
}

void* Plugin_Mesh_Optimizer::ProcessMesh(void* data, void* setting, void* statsOut, CMP_Feedback_Proc pFeedbackProc)
{
    CMP_Mesh* mesh = NULL;

    if (!data)
    {
        PrintInfo("Model data mesh buffer is null.\n");
        return nullptr;
    }

    if (!setting)
    {
        PrintInfo("Setting for mesh optimize is null.\n");
        return nullptr;
    }

    m_settings = (*((MeshSettings*)setting));

    CMODEL_DATA* meshdata = (CMODEL_DATA*)data;

    for (int i = 0; i < meshdata->m_meshData.size(); i++)
    {
        mesh = &(meshdata->m_meshData[i]);
        if (mesh)
        {
            if (mesh->vertices.empty())
            {
                PrintInfo("Model data mesh buffer is empty.\n");
                return nullptr;
            }
        }
        else
            return nullptr;

        if (m_settings.bRandomizeMesh)
        {
            if (g_CMIPS)
            {
                g_CMIPS->Print("Randomize mesh indices buffer for #%d mesh...", i + 1);
            }
            optimize(*mesh, "Random", optRandomShuffle);
        }
        if (m_settings.bOptimizeVCache)
        {
            if (g_CMIPS)
            {
                g_CMIPS->Print("Optimizing Vertex Cache for #%d mesh....", i + 1);
            }
            optimize(*mesh, "Cache", optCache);
        }
        if (m_settings.bOptimizeVCacheFifo)
        {
            if (g_CMIPS)
            {
                g_CMIPS->Print("Optimizing Vertex Cache FIFO for #%d mesh....", i + 1);
            }
            optimize(*mesh, "CacheFifo", optCacheFifo);
        }
        if (m_settings.bOptimizeOverdraw)
        {
            if (g_CMIPS)
            {
                g_CMIPS->Print("Optimizing Overdraw for #%d mesh....", i + 1);
            }
            optimize(*mesh, "Overdraw", optOverdraw);
        }
        if (m_settings.bOptimizeVFetch)
        {
            if (g_CMIPS)
            {
                g_CMIPS->Print("Optimizing Vertex Fetch for #%d mesh....", i + 1);
            }
            optimize(*mesh, "Fetch", optFetch);
        }

        // note: the ATVR/overdraw output from this pass is not necessarily correct since we analyze all LODs at once
        if (m_settings.bSimplifyMesh)
        {
            if (g_CMIPS)
            {
                g_CMIPS->Print("Simplifying Mesh with Edge Collapse for #%d mesh....", i + 1);
            }
            optimize(*mesh, "Simplify", optSimplify, /* compare= */ false);
        }

        m_copy.push_back(*mesh);
    }

    return (void*)&m_copy;
}
