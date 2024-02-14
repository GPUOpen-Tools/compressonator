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

#include "mesh_tootle.h"

#include "timer.h"
#include "modeldata.h"
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "compressonator.h"
#include "plugininterface.h"
#include "cmp_fileio.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <fstream>

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_Mesh_Tootle)
SET_PLUGIN_TYPE("MESH_OPTIMIZER")
SET_PLUGIN_NAME("TOOTLE")
#else
void* make_Plugin_Mesh_Tootle()
{
    return new Plugin_Mesh_Tootle;
}
#endif

#ifdef BUILD_AS_PLUGIN_DLL
bool   g_bAbortCompression = false;
CMIPS* g_CMIPS             = nullptr;
#else
extern bool   g_bAbortCompression;
extern CMIPS* g_CMIPS;
#endif

//using namespace CMP_Mesh_Tootle;

Plugin_Mesh_Tootle::Plugin_Mesh_Tootle()
{
    m_InitOK = false;

    m_settings.pMeshName             = NULL;
    m_settings.pViewpointName        = NULL;
    m_settings.nClustering           = 0;
    m_settings.nCacheSize            = TOOTLE_DEFAULT_VCACHE_SIZE;
    m_settings.eWinding              = TOOTLE_CW;
    m_settings.algorithmChoice       = TOOTLE_OPTIMIZE;
    m_settings.eVCacheOptimizer      = TOOTLE_VCACHE_AUTO;  // the auto selection as the default to optimize vertex cache
    m_settings.bOptimizeVertexMemory = true;                // default value is to optimize the vertex memory
    m_settings.bMeasureOverdraw      = false;               // default is to skip measure overdraw
}

Plugin_Mesh_Tootle::~Plugin_Mesh_Tootle()
{
    CleanUp();
}

int Plugin_Mesh_Tootle::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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

int Plugin_Mesh_Tootle::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        g_CMIPS              = static_cast<CMIPS*>(Shared);
        g_CMIPS->m_infolevel = 0x01;  // Turn on print Info
        return 0;
    }
    return 1;
}

int Plugin_Mesh_Tootle::Init()
{
    if (m_InitOK)
        return 0;

    TootleResult result;

    // initialize Tootle
    result = TootleInit();
    if (result != TOOTLE_OK)
    {
        return -1;
    }

    m_InitOK = true;

    return 0;
}

int Plugin_Mesh_Tootle::CleanUp()
{
    if (m_InitOK)
    {
        // clean up tootle
        TootleCleanup();
        m_InitOK = false;
    }
    return 0;
}

void getFileName(const char* FilePathName, char* fnameExt, int maxbuffsize)
{
    std::string FileName = CMP_GetJustFileName(FilePathName);
    std::string FileExt  = CMP_GetJustFileExt(FilePathName);
    snprintf(fnameExt, maxbuffsize, "%s%s", FileName.c_str(), FileExt.c_str());
}

//=================================================================================================================================
/// Reads a list of camera positions from a viewpoint file.
///
/// \param pFileName   The name of the file to read
/// \param rVertices   A vector which will be filled with vertex positions
/// \param rIndices    A vector which will will be filled with face indices
//=================================================================================================================================
bool LoadViewpoints(const char* pFileName, std::vector<ObjVertex3D>& rViewPoints)
{
    assert(pFileName);

    FILE* pFile = fopen(pFileName, "r");

    if (!pFile)
    {
        return false;
    }

    int iSize;

    if (fscanf(pFile, "%i\n", &iSize) != 1)
    {
        return false;
    }

    for (int i = 0; i < iSize; i++)
    {
        float x, y, z;

        if (fscanf(pFile, "%f %f %f\n", &x, &y, &z) != 3)
        {
            return false;
        }

        ObjVertex3D vert;
        vert.x = x;
        vert.y = y;
        vert.z = z;
        rViewPoints.push_back(vert);
    }

    fclose(pFile);

    return true;
}

//=================================================================================================================================
/// This function reads an obj file and re-emits its vertices and faces in the specified order
/// \param rInput      Input stream from which to read the obj
/// \param rOutput     Output stream on which to emit
/// \param rVertices   A vector containing the vertices that were referenced in the OBJ file
/// \param rIndices    A vector containing the sorted index buffer
/// \param vertexRemap A vector containing the remapped ID of the vertices.  Element i in the array will contain the new output
///                     location of the vertex i.  This is the result of TootleOptimizeVertexMemory().  May be NULL.
/// \param nVertices   The total number of vertices referenced in vertexRemap.
///
/// \return True if successful.  False otherwise
//=================================================================================================================================
bool EmitModifiedObj(std::istream&                      rInput,
                     std::ostream&                      rOutput,
                     const std::vector<ObjVertexFinal>& rVertices,
                     const std::vector<unsigned int>&   rIndices,
                     const unsigned int*                vertexRemap,
                     unsigned int                       nVertices)
{
    // store the original copy of vertices into an array
    // we need to do this because rVertices contains the reordered vertex by ObjLoader using a hash map.
    std::vector<ObjVertex3D> inputVertices;
    inputVertices.reserve(rVertices.size());  // reserve at least this size, but the total input vertices might be larger.

    unsigned int nCount = 0;

    while (!rInput.eof())
    {
        std::string currentLine;
        std::getline(rInput, currentLine, '\n');
        const char* pLineText = currentLine.c_str();

        //if there is an "f " before the first space, then this is a face line
        if (strstr(pLineText, "f ") == strstr(pLineText, " ") - 1)
        {
            // face line
        }
        else if (strstr(pLineText, "v ") == strstr(pLineText, " ") - 1)
        {  // vertex line
            ObjVertex3D vert;

            // vertex line
            sscanf(pLineText, "v %f %f %f", &vert.x, &vert.y, &vert.z);

            inputVertices.push_back(vert);

            nCount++;
        }
        else
        {
            // not a face line, pass it through
            rOutput << currentLine << std::endl;
        }
    }

    // Create a local copy of the vertex remapping array.
    //  Note that because the input vertices in the OBJ file might be larger than the vertex buffer
    //  in rVertices, vertexRemapping buffer might be larger than vertexRemap.
    std::vector<unsigned int> vertexRemapping;

    // if there is no vertex remapping, create a default one
    if (vertexRemap == NULL)
    {
        vertexRemapping.resize(nCount);

        for (unsigned int i = 0; i < nCount; i++)
        {
            vertexRemapping[i] = i;
        }
    }
    else
    {
        vertexRemapping.reserve(nCount);
        if (nVertices >= nCount)
        {
            vertexRemapping.assign(vertexRemap, vertexRemap + nCount);
        }
        else
        {
            vertexRemapping.assign(vertexRemap, vertexRemap + rVertices.size());
        }

        // It is possible for the input list of vertices to be larger than the one in rVertices, thus,
        //  they are not mapped by TootleOptimizeVertexMemory().
        //  In that case, we will reassign the unmapped vertices to the end of the vertex buffer.

        for (unsigned int i = nVertices; i < nCount; i++)
        {
            vertexRemapping.push_back(i);
        }

        // print out the vertex mapping indexes in the output obj
        const unsigned int NUM_ITEMS_PER_LINE = 50;
        rOutput << "#vertexRemap = ";
        for (unsigned int i = 0; i < nCount; ++i)
        {
            rOutput << vertexRemapping[i] << " ";
            if ((i + 1) % NUM_ITEMS_PER_LINE == 0)
            {
                rOutput << std::endl << "#vertexRemap = ";
            }
        }
        rOutput << std::endl;
    }

    // compute the inverse vertex mapping to output the remapped vertices
    std::vector<unsigned int> inverseVertexRemapping(nCount);

    unsigned int nVID;

    for (unsigned int i = 0; i < nCount; i++)
    {
        nVID                         = vertexRemapping[i];
        inverseVertexRemapping[nVID] = i;
    }

    // make sure that vertexRemapping and inverseVertexRemapping is consistent
    for (unsigned int i = 0; i < nCount; i++)
    {
        if (inverseVertexRemapping[vertexRemapping[i]] != i)
        {
            std::cerr << "EmitModifiedObj: inverseVertexRemapping and vertexRemapping is not consistent.\n";

            return false;
        }
    }

    // output the vertices
    for (unsigned int i = 0; i < nCount; i++)
    {
        rOutput << "v ";

        // output the remapped vertices (require an inverse mapping).
        nVID = inverseVertexRemapping[i];

        rOutput << inputVertices[nVID].x << " " << inputVertices[nVID].y << " " << inputVertices[nVID].z << std::endl;
    }

    // generate a new set of faces, using re-ordered index buffer
    for (unsigned int i = 0; i < rIndices.size(); i += 3)
    {
        rOutput << "f ";

        for (int j = 0; j < 3; j++)
        {
            const ObjVertexFinal& rVertex = rVertices[rIndices[i + j]];
            rOutput << 1 + vertexRemapping[rVertex.nVertexIndex - 1];

            if (rVertex.nNormalIndex > 0 && rVertex.nTexcoordIndex)
            {
                rOutput << "/" << rVertex.nTexcoordIndex << "/" << rVertex.nNormalIndex;
            }
            else if (rVertex.nNormalIndex > 0)
            {
                rOutput << "//" << rVertex.nNormalIndex;
            }
            else if (rVertex.nTexcoordIndex > 0)
            {
                rOutput << "/" << rVertex.nTexcoordIndex;
            }

            rOutput << " ";
        }

        rOutput << std::endl;
    }

    return true;
}

//@param data contain the mesh data from CMODEL_DATA struct
//@param setting is user input setting from TootleSettings struct
//@param pFeedbackProc used to show progress
//@ return 0 if error in process, 1 when success
void* Plugin_Mesh_Tootle::ProcessMesh(void* data, void* setting, void* statOut, CMP_Feedback_Proc pFeedbackProc)
{
    if (data == NULL)
        return 0;
    if (setting == NULL)
        return 0;

    CMODEL_DATA* mesh = (CMODEL_DATA*)data;
#ifndef USE_ASSIMP
    m_objVertices = mesh->m_objVertices;
    m_vertices.resize(m_objVertices.size());

    if (g_CMIPS)
    {
        //g_CMIPS->SetProgress(0);
        g_CMIPS->Print("Processing Mesh. This may takes some time....");
        g_CMIPS->Print("Reading vertices, indices ...");
    }

    for (unsigned int i = 0; i < m_vertices.size(); i++)
    {
        m_vertices[i] = m_objVertices[i].pos;
    }

    m_objFaces = mesh->m_objFaces;
    m_indices.resize(m_objFaces.size() * 3);
    for (unsigned int i = 0; i < m_indices.size(); i++)
    {
        m_indices[i] = m_objFaces[i / 3].finalVertexIndices[i % 3];
    }
#else
    if (mesh->m_Scene)
    {
        if (mesh->m_Scene->HasMeshes())
        {
            for (int m = 0; m < mesh->m_Scene->mNumMeshes; m++)
            {
                m_objVertices.resize(mesh->m_Scene->mMeshes[m]->mNumVertices);
                m_vertices.resize(m_objVertices.size());
                for (unsigned int i = 0; i < m_objVertices.size(); i++)
                {
                    m_objVertices[i].pos.x = mesh->m_Scene->mMeshes[m]->mVertices[i].x;
                    m_objVertices[i].pos.y = mesh->m_Scene->mMeshes[m]->mVertices[i].y;
                    m_objVertices[i].pos.z = mesh->m_Scene->mMeshes[m]->mVertices[i].z;
                    m_vertices[i]          = m_objVertices[i].pos;
                }

                m_objFaces.resize(mesh->m_Scene->mMeshes[m]->mNumFaces);
                for (int n = 0; n < mesh->m_Scene->mMeshes[m]->mNumFaces; n++)
                {
                    for (int d = 0; d < mesh->m_Scene->mMeshes[m]->mFaces[n].mNumIndices; d++)
                    {
                        m_objFaces[n].finalVertexIndices[d] = mesh->m_Scene->mMeshes[m]->mFaces[n].mIndices[d];
                        m_indices.push_back(m_objFaces[n].finalVertexIndices[d]);
                    }
                }
            }
        }
        else
            return 0;
    }
    else
        return 0;

#endif

    m_settings = (*((TootleSettings*)setting));

    // ******************************************
    //    Load viewpoints if necessary
    // ******************************************

    if (strcmp(m_settings.pViewpointName, "") != 0)
    {
        if (g_CMIPS)
        {
            g_CMIPS->Print("Loading Viewpoints from file %s ...", m_settings.pViewpointName);
        }
        if (!LoadViewpoints(m_settings.pViewpointName, m_viewpoints))
        {
            std::cerr << "Unable to load viewpoints from file: " << m_settings.pViewpointName;
            return 0;
        }
    }

    // if we didn't get any viewpoints, then use a NULL array
    const float* pViewpoints = NULL;
    unsigned int nViewpoints = (unsigned int)m_viewpoints.size();

    if (m_viewpoints.size() > 0)
    {
        pViewpoints = (const float*)&m_viewpoints[0];
    }

    // *****************************************************************
    //   Prepare the mesh and initialize stats variables
    // *****************************************************************

    unsigned int  nFaces    = (unsigned int)m_indices.size() / 3;
    unsigned int  nVertices = (unsigned int)m_vertices.size();
    float*        pfVB      = (float*)&m_vertices[0];
    unsigned int* pnIB      = (unsigned int*)&m_indices[0];
    unsigned int  nStride   = 3 * sizeof(float);

    // todo: tootle statistics tie to cpprojectdata
    TootleStats* stats = nullptr;
    if (statOut != NULL)
    {
        stats = ((TootleStats*)(statOut));
    }
    else
    {
        stats = new TootleStats;
    }

    // initialize the timing variables
    stats->fOptimizeVCacheTime               = INVALID_TIME;
    stats->fClusterMeshTime                  = INVALID_TIME;
    stats->fVCacheClustersTime               = INVALID_TIME;
    stats->fOptimizeVCacheAndClusterMeshTime = INVALID_TIME;
    stats->fOptimizeOverdrawTime             = INVALID_TIME;
    stats->fTootleOptimizeTime               = INVALID_TIME;
    stats->fTootleFastOptimizeTime           = INVALID_TIME;
    stats->fMeasureOverdrawTime              = INVALID_TIME;
    stats->fOptimizeVertexMemoryTime         = INVALID_TIME;

    TootleResult result;

    // measure input VCache efficiency
    if (g_CMIPS)
    {
        g_CMIPS->Print("Measuring Input Cache Efficiency ...");
    }
    result = TootleMeasureCacheEfficiency(pnIB, nFaces, m_settings.nCacheSize, &stats->fVCacheIn);

    if (result != TOOTLE_OK)
    {
        return 0;
    }

    if (m_settings.bMeasureOverdraw)
    {
        // measure input overdraw.  Note that we assume counter-clockwise vertex winding.
        if (g_CMIPS)
        {
            g_CMIPS->Print("Measuring Input Overdraw ...");
        }
        result = TootleMeasureOverdraw(
            pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints, m_settings.eWinding, &stats->fOverdrawIn, &stats->fMaxOverdrawIn);

        if (result != TOOTLE_OK)
        {
            return 0;
        }
    }

    // allocate an array to hold the cluster ID for each face
    m_faceCluster.resize(nFaces + 1);

    Timer timer;
    timer.Reset();
    // **********************************************************************************************************************
    //   Optimize the mesh:
    //
    // The following cases show five examples for developers on how to use the library functions in Tootle.
    // 1. If you are interested in optimizing vertex cache only, see the TOOTLE_VCACHE_ONLY case.
    // 2. If you are interested to optimize vertex cache and overdraw, see either TOOTLE_CLUSTER_VCACHE_OVERDRAW
    //     or TOOTLE_OPTIMIZE cases.  The former uses three separate function calls while the latter uses a single
    //     utility function.
    // 3. To use the algorithm from SIGGRAPH 2007 (v2.0), see TOOTLE_FAST_VCACHECLUSTER_OVERDRAW or TOOTLE_FAST_OPTIMIZE
    //     cases.  The former uses two separate function calls while the latter uses a single utility function.
    //
    // Note the algorithm from SIGGRAPH 2007 (v2.0) is very fast but produces less quality results especially for the
    //  overdraw optimization.  During our experiments with some medium size models, we saw an improvement of 1000x in
    //  running time (from 20+ minutes to less than 1 second) for using v2.0 calls vs v1.2 calls.  The resulting vertex
    //  cache optimization is very similar while the overdraw optimization drops from 3.8x better to 2.5x improvement over
    //  the input mesh.
    //  Developers should always run the overdraw optimization using the fast algorithm from SIGGRAPH initially.
    //  If they require a better result, then re-run the overdraw optimization using the old v1.2 path (TOOTLE_OVERDRAW_AUTO).
    //  Passing TOOTLE_OVERDRAW_AUTO to the algorithm will let the algorithm choose between Direct3D or raytracing path
    //  depending on the total number of clusters (less than 200 clusters, it will use Direct3D path otherwise it will
    //  use raytracing path since the raytracing path will be faster than the Direct3D path at that point).
    //
    // Tips: use v2.0 for fast optimization, and v1.2 to further improve the result by mix-matching the calls.
    // **********************************************************************************************************************

    switch (m_settings.algorithmChoice)
    {
    case TOOTLE_VCACHE_ONLY:
        // *******************************************************************************************************************
        // Perform Vertex Cache Optimization ONLY
        // *******************************************************************************************************************

        // Optimize vertex cache
        if (g_CMIPS)
        {
            g_CMIPS->Print("Optimizing using VCache only ...");
        }
        result = TootleOptimizeVCache(pnIB, nFaces, nVertices, m_settings.nCacheSize, pnIB, NULL, m_settings.eVCacheOptimizer);

        if (result != TOOTLE_OK)
        {
            PrintInfo("Error: %d", result);
            return 0;
        }

        break;

    case TOOTLE_CLUSTER_VCACHE_OVERDRAW:
        // *******************************************************************************************************************
        // An example of calling clustermesh, vcacheclusters and optimize overdraw individually.
        // This case demonstrate mix-matching v1.2 clustering with v2.0 overdraw optimization.
        // *******************************************************************************************************************

        // Cluster the mesh, and sort faces by cluster.
        if (g_CMIPS)
        {
            g_CMIPS->Print("Clustering mesh and sorting faces by cluster ...");
        }
        result = TootleClusterMesh(pfVB, pnIB, nVertices, nFaces, nStride, m_settings.nClustering, pnIB, &m_faceCluster[0], NULL);

        if (result != TOOTLE_OK)
        {
            PrintInfo("Error: %d", result);
            return 0;
        }

        // Perform vertex cache optimization on the clustered mesh.
        if (g_CMIPS)
        {
            g_CMIPS->Print("Perform vertex cache optimization on the clustered mesh ...");
        }
        result = TootleVCacheClusters(pnIB, nFaces, nVertices, m_settings.nCacheSize, &m_faceCluster[0], pnIB, NULL, m_settings.eVCacheOptimizer);

        if (result != TOOTLE_OK)
        {
            PrintInfo("Error: %d", result);
            return 0;
        }

        // Optimize the draw order (using v1.2 path: TOOTLE_OVERDRAW_AUTO, the default path is from v2.0--SIGGRAPH version).
        if (g_CMIPS)
        {
            g_CMIPS->Print("Optimizing Overdraw ...");
        }
        result = TootleOptimizeOverdraw(
            pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints, m_settings.eWinding, &m_faceCluster[0], pnIB, NULL, TOOTLE_OVERDRAW_AUTO);

        if (result != TOOTLE_OK)
        {
            PrintInfo("Error: %d", result);
            return 0;
        }

    case TOOTLE_FAST_VCACHECLUSTER_OVERDRAW:
        // *******************************************************************************************************************
        // An example of calling v2.0 optimize vertex cache and clustering mesh with v1.2 overdraw optimization.
        // *******************************************************************************************************************

        // Optimize vertex cache and create cluster
        // The algorithm from SIGGRAPH combine the vertex cache optimization and clustering mesh into a single step
        if (g_CMIPS)
        {
            g_CMIPS->Print("Optimizing vertex cache and cluster mesh ...");
        }
        result = TootleFastOptimizeVCacheAndClusterMesh(
            pnIB, nFaces, nVertices, m_settings.nCacheSize, pnIB, &m_faceCluster[0], &m_nNumClusters, TOOTLE_DEFAULT_ALPHA);

        if (result != TOOTLE_OK)
        {
            // an error detected
            PrintInfo("Error: %d", result);
            return 0;
        }

        // In this example, we use TOOTLE_OVERDRAW_AUTO to show that we can mix-match the clustering and
        //  vcache computation from the new library with the overdraw optimization from the old library.
        //  TOOTLE_OVERDRAW_AUTO will choose between using Direct3D or CPU raytracing path.  This path is
        //  much slower than TOOTLE_OVERDRAW_FAST but usually produce 2x better results.
        if (g_CMIPS)
        {
            g_CMIPS->Print("Optimizing overdraw ...");
        }
        result =
            TootleOptimizeOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, NULL, 0, m_settings.eWinding, &m_faceCluster[0], pnIB, NULL, TOOTLE_OVERDRAW_AUTO);

        if (result != TOOTLE_OK)
        {
            // an error detected
            PrintInfo("Error: %d", result);
            return 0;
        }

        break;

    case TOOTLE_OPTIMIZE:
        // *******************************************************************************************************************
        // An example of using a single utility function to perform v1.2 optimizations.
        // *******************************************************************************************************************

        // This function will compute the entire optimization (cluster mesh, vcache per cluster, and optimize overdraw).
        // It will use TOOTLE_OVERDRAW_FAST as the default overdraw optimization
        if (g_CMIPS)
        {
            g_CMIPS->Print("Optimizing: Clustering Mesh, optimize vcache per cluster and optimize overdraw...");
        }
        result = TootleOptimize(pfVB,
                                pnIB,
                                nVertices,
                                nFaces,
                                nStride,
                                m_settings.nCacheSize,
                                pViewpoints,
                                nViewpoints,
                                m_settings.eWinding,
                                pnIB,
                                &m_nNumClusters,
                                m_settings.eVCacheOptimizer);

        if (result != TOOTLE_OK)
        {
            PrintInfo("Error: %d", result);
            return 0;
        }

        break;

    case TOOTLE_FAST_OPTIMIZE:
        // *******************************************************************************************************************
        // An example of using a single utility function to perform v2.0 optimizations.
        // *******************************************************************************************************************

        // This function will compute the entire optimization (optimize vertex cache, cluster mesh, and optimize overdraw).
        // It will use TOOTLE_OVERDRAW_FAST as the default overdraw optimization
        if (g_CMIPS)
        {
            g_CMIPS->Print("(Fast Optimizing: Clustering Mesh, optimize vcache per cluster and optimize overdraw ...");
        }
        result =
            TootleFastOptimize(pfVB, pnIB, nVertices, nFaces, nStride, m_settings.nCacheSize, m_settings.eWinding, pnIB, &m_nNumClusters, TOOTLE_DEFAULT_ALPHA);

        if (result != TOOTLE_OK)
        {
            PrintInfo("Error: %d", result);
            return 0;
        }

        break;

    default:
        PrintInfo("Error: wrong algorithm choice!");
        return 0;
    }

    // measure output VCache efficiency
    if (g_CMIPS)
    {
        g_CMIPS->Print("Measuring Vcache Efficiency after process...");
    }
    result = TootleMeasureCacheEfficiency(pnIB, nFaces, m_settings.nCacheSize, &stats->fVCacheOut);
    if (result != TOOTLE_OK)
    {
        PrintInfo("Error: %d", result);
        return 0;
    }

    if (m_settings.bMeasureOverdraw)
    {
        // measure output overdraw
        timer.Reset();
        if (g_CMIPS)
        {
            g_CMIPS->Print("Measuring Overdraw after process...");
        }
        result = TootleMeasureOverdraw(
            pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints, m_settings.eWinding, &stats->fOverdrawOut, &stats->fMaxOverdrawOut);
        stats->fMeasureOverdrawTime = timer.GetElapsed();

        if (result != TOOTLE_OK)
        {
            PrintInfo("Error: %d", result);
            return 0;
        }
    }

    //-----------------------------------------------------------------------------------------------------
    // PERFORM VERTEX MEMORY OPTIMIZATION (rearrange memory layout for vertices based on the final indices
    //  to exploit vertex cache prefetch).
    //  We want to optimize the vertex memory locations based on the final optimized index buffer that will
    //  be in the output file.
    //  Thus, in this sample code, we recompute a copy of the indices that point to the original vertices
    //  (pnIBTmp) to be passed into the function TootleOptimizeVertexMemory.  If we use the array pnIB, we
    //  will optimize for the wrong result since the array pnIB is based on the rehashed vertex location created
    //  by the function ObjLoader.
    //-----------------------------------------------------------------------------------------------------
    timer.Reset();

    m_nReferencedVertices = 0;  // The actual total number of vertices referenced by the indices
    if (m_settings.bOptimizeVertexMemory)
    {
        std::vector<unsigned int> pnIBTmp;
        pnIBTmp.resize(nFaces * 3);

        // compute the indices to be optimized for (the original pointed by the obj file).
        for (unsigned int i = 0; i < m_indices.size(); i += 3)
        {
            for (int j = 0; j < 3; j++)
            {
                const ObjVertexFinal& rVertex = m_objVertices[pnIB[i + j]];
                pnIBTmp[i + j]                = rVertex.nVertexIndex - 1;  // index is off by 1

                // compute the max vertices
                if (rVertex.nVertexIndex > m_nReferencedVertices)
                {
                    m_nReferencedVertices = rVertex.nVertexIndex;
                }
            }
        }

        m_pnVertexRemapping.resize(m_nReferencedVertices);

        // For this sample code, we are just going to use vertexRemapping array result.  This is to support general obj
        //  file input and output.
        //  In fact, we are sending the wrong vertex buffer here (it should be based on the original file instead of the
        //  rehashed vertices).  But, it is ok because we do not request the reordered vertex buffer as an output.
        if (g_CMIPS)
        {
            g_CMIPS->Print("Perform vertex memory optimization...");
        }
        result = TootleOptimizeVertexMemory(pfVB, &pnIBTmp[0], m_nReferencedVertices, nFaces, nStride, NULL, NULL, &m_pnVertexRemapping[0]);

        if (result != TOOTLE_OK)
        {
            PrintInfo("Error: %d", result);
            return 0;
        }
    }

    // todo : print tootle statistics to stdout and stderr
    // display the current test case
    //PrintAlgorithm(stderr, m_settings.eVCacheOptimizer, m_settings.algorithmChoice, m_settings.nCacheSize, m_stats.nClusters);
    //PrintAlgorithm(stdout, m_settings.eVCacheOptimizer, m_settings.algorithmChoice, m_settings.nCacheSize, m_stats.nClusters);
    //
    //PrintStats(stdout, &stats);
    //PrintStats(stderr, &stats);
    bool bResult = true;
#ifndef USE_ASSIMP
    // emit a modified .OBJ file (save obj file)
    std::ifstream inputStream(m_settings.pMeshName);
    std::ofstream outputFileStream(m_settings.pDestMeshName);

    if (g_CMIPS)
    {
#ifdef _WIN32
        char fname[_MAX_FNAME];
        getFileName(m_settings.pDestMeshName, fname, _MAX_FNAME);
#else
        char fname[256];
        getFileName(m_settings.pDestMeshName, fname, 256);
#endif

        g_CMIPS->Print("Saving Output File %s...", fname);
    }
    if (m_settings.bOptimizeVertexMemory)
    {
        bResult = EmitModifiedObj(inputStream, outputFileStream, m_objVertices, m_indices, &m_pnVertexRemapping[0], m_nReferencedVertices);
    }
    else
    {
        bResult = EmitModifiedObj(inputStream, outputFileStream, m_objVertices, m_indices, NULL, 0);
    }
    return &bResult;
#else

    CMODEL_DATA* tempOut = (CMODEL_DATA*)mesh;  //by default output initialized with input
    for (int i = 0; i < m_objVertices.size(); i++)
    {
        tempOut->m_Scene->mMeshes[0]->mVertices[i].x = m_objVertices[i].pos.x;
        tempOut->m_Scene->mMeshes[0]->mVertices[i].y = m_objVertices[i].pos.y;
        tempOut->m_Scene->mMeshes[0]->mVertices[i].z = m_objVertices[i].pos.z;

        tempOut->m_Scene->mMeshes[0]->mNormals[i].x = m_objVertices[i].normal.x;
        tempOut->m_Scene->mMeshes[0]->mNormals[i].y = m_objVertices[i].normal.y;
        tempOut->m_Scene->mMeshes[0]->mNormals[i].z = m_objVertices[i].normal.z;
    }

    int ind = 0;
    for (int n = 0; n < m_objFaces.size(); n++)
    {
        tempOut->m_Scene->mMeshes[0]->mFaces[n].mIndices[0] = m_indices.at(ind);
        ind++;
        tempOut->m_Scene->mMeshes[0]->mFaces[n].mIndices[1] = m_indices.at(ind);
        ind++;
        tempOut->m_Scene->mMeshes[0]->mFaces[n].mIndices[2] = m_indices.at(ind);
        ind++;
    }
    //dataOut = tempOut;
    return tempOut;
#endif
}
