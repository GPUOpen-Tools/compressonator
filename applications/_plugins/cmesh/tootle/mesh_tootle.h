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
#ifndef _PLUGIN_MESH_TOOTLE_H
#define _PLUGIN_MESH_TOOTLE_H

#include <cassert>

#include "tootlelib.h"
#ifndef USE_ASSIMP
#include "objloader.h"  // This should be removed and shared with Compressonator Common!
#else
#include "modeldata.h"
#endif

#ifdef _WIN32
// {80EF74BA-CB47-4D21-9F98-000FE2C7CFB0}
static const GUID g_GUID = {0x80ef74ba, 0xcb47, 0x4d21, {0x9f, 0x98, 0x0, 0xf, 0xe2, 0xc7, 0xcf, 0xb0}};
#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR 1
#define TC_PLUGIN_VERSION_MINOR 0

//=================================================================================================================================
/// Enumeration for the choice of test cases for tootle.
//=================================================================================================================================
enum TootleAlgorithm
{
    NA_TOOTLE_ALGORITHM,                 // Default invalid choice.
    TOOTLE_VCACHE_ONLY,                  // Only perform vertex cache optimization.
    TOOTLE_CLUSTER_VCACHE_OVERDRAW,      // Call the clustering, optimize vertex cache and overdraw individually.
    TOOTLE_FAST_VCACHECLUSTER_OVERDRAW,  // Call the functions to optimize vertex cache and overdraw individually.  This is using
    //  the algorithm from SIGGRAPH 2007.
    TOOTLE_OPTIMIZE,      // Call a single function to optimize vertex cache, cluster and overdraw.
    TOOTLE_FAST_OPTIMIZE  // Call a single function to optimize vertex cache, cluster and overdraw using
    //  a fast algorithm from SIGGRAPH 2007.
};

//=================================================================================================================================
/// A simple structure to hold Tootle statistics
//=================================================================================================================================
struct TootleStats
{
    unsigned int nClusters;
    float        fVCacheIn;
    float        fVCacheOut;
    float        fOverdrawIn;
    float        fOverdrawOut;
    float        fMaxOverdrawIn;
    float        fMaxOverdrawOut;
    double       fOptimizeVCacheTime;
    double       fClusterMeshTime;
    double       fOptimizeOverdrawTime;
    double       fVCacheClustersTime;
    double       fOptimizeVCacheAndClusterMeshTime;
    double       fTootleOptimizeTime;
    double       fTootleFastOptimizeTime;
    double       fMeasureOverdrawTime;
    double       fOptimizeVertexMemoryTime;
};

const float INVALID_TIME = -1;

//=================================================================================================================================
/// A simple structure to store the settings for this sample app
//=================================================================================================================================
struct TootleSettings
{
    const char*           pMeshName;
    const char*           pDestMeshName;
    const char*           pViewpointName;
    unsigned int          nClustering;
    unsigned int          nCacheSize;
    TootleFaceWinding     eWinding;
    TootleAlgorithm       algorithmChoice;   // five different types of algorithm to test Tootle
    TootleVCacheOptimizer eVCacheOptimizer;  // the choice for vertex cache optimization algorithm, it can be either
    //  TOOTLE_VCACHE_AUTO, TOOTLE_VCACHE_LSTRIPS, TOOTLE_VCACHE_DIRECT3D or
    //  TOOTLE_VCACHE_TIPSY.
    bool bOptimizeVertexMemory;  // true if you want to optimize vertex memory location, false to skip
    bool bMeasureOverdraw;       // true if you want to measure overdraw, false to skip
};

class Plugin_Mesh_Tootle : public PluginInterface_Mesh
{
public:
    Plugin_Mesh_Tootle();
    virtual ~Plugin_Mesh_Tootle();
    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginSetSharedIO(void*);

    int Init();
    int CleanUp();

    void* ProcessMesh(void* data, void* setting, void* statsOut, CMP_Feedback_Proc pFeedbackProc);

private:
    bool           m_InitOK;
    TootleSettings m_settings;

    // read the mesh from the OBJ file
    std::vector<ObjVertexFinal> m_objVertices;
    std::vector<ObjFace>        m_objFaces;

    // build buffers containing only the vertex positions and indices, since this is what Tootle requires
    std::vector<ObjVertex3D> m_vertices;

    // read viewpoints if needed
    std::vector<ObjVertex3D>  m_viewpoints;
    std::vector<unsigned int> m_indices;

    // allocate an array to hold the cluster ID for each face
    std::vector<unsigned int> m_faceCluster;
    unsigned int              m_nNumClusters;

    std::vector<unsigned int> m_pnVertexRemapping;
    unsigned int              m_nReferencedVertices = 0;  // The actual total number of vertices referenced by the indices
};

extern void* make_Plugin_Mesh_Tootle();

#endif