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

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "obj_10.h"

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"

#include "compressonator.h"
#include "utilfuncs.h"

#include <iostream>
#include <fstream>

#ifdef USE_MESHOPTIMIZER
#include "meshoptimizer.h"
#include "objparser.h"
#endif

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_obj_Loader)
SET_PLUGIN_TYPE("3DMODEL_LOADER")
SET_PLUGIN_NAME("OBJ")
#else
void* make_Plugin_obj_Loader()
{
    return new Plugin_obj_Loader;
}
#endif

#include "misc.h"

#ifdef BUILD_AS_PLUGIN_DLL
bool   g_bAbortCompression = false;
CMIPS* g_CMIPS             = nullptr;
#else
extern bool   g_bAbortCompression;
extern CMIPS* g_CMIPS;
#endif

//using namespace ML_obj;

Plugin_obj_Loader::Plugin_obj_Loader()
{
}

Plugin_obj_Loader::~Plugin_obj_Loader()
{
}

int Plugin_obj_Loader::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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

int Plugin_obj_Loader::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        g_CMIPS              = static_cast<CMIPS*>(Shared);
        g_CMIPS->m_infolevel = 0x01;  // Turn on print Info
        return 0;
    }
    return 1;
}

void* Plugin_obj_Loader::GetModelData()
{
    void* data = (void*)m_ModelData;
    return data;
}

#ifdef USE_MESHOPTIMIZER
static CMP_Mesh parseObj(const char* path, CMIPS* cmips, CMP_Feedback_Proc pFeedbackProc)
{
    ObjFile file;

    if (cmips)
    {
        cmips->SetProgress(0);
        cmips->Print("OBJ: ParseFile ...");
    }

    if (!objParseFile(file, path) || !objValidate(file))
    {
        if (cmips)
            cmips->Print("Error loading %s\n", path);
        return CMP_Mesh();
    }

    if (cmips)
        cmips->Print("OBJ: Triangulate");
    objTriangulate(file);

    //Assign data to Mesh
    CMP_Mesh result;

    size_t total_indices = file.f.size() / 3;
    size_t fsize         = file.f.size();
    int    perf          = (int)fsize / 100;

    for (size_t i = 0; i < fsize; i += 3)
    {
        int vi  = file.f[i + 0];  // Indices 0 to (total_indices -1) used to index Vertices
        int vti = file.f[i + 1];
        int vni = file.f[i + 2];

        result.indices.push_back(vi + 1);  // Save indices starting value starts from 1 to total_indices

        Vertex V;

        if (vi >= 0)
        {
            V.px = file.v[vi * 3 + 0];
            V.py = file.v[vi * 3 + 1];
            V.pz = file.v[vi * 3 + 2];
        }
        else
        {
            // This should never happen we always have vertices!
            V.px = 0;
            V.py = 0;
            V.pz = 0;
        }

        if (vni >= 0)
        {
            V.nx = file.vn[vni * 3 + 0];
            V.ny = file.vn[vni * 3 + 1];
            V.nz = file.vn[vni * 3 + 2];
        }
        else
        {
            V.nx = 0;
            V.ny = 0;
            V.nz = 0;
        }

        // We dont support texture UV for now
        if (vti >= 0)
        {
            V.tx = file.vt[vti * 3 + 0];
            V.ty = file.vt[vti * 3 + 1];
        }
        else
        {
            V.tx = 0;
            V.ty = 0;
        }

        result.vertices.push_back(V);

        if (pFeedbackProc && ((i % perf) == 0))
        {
            float fProgress = 100.f * ((float)(i) / fsize);
            if (pFeedbackProc(fProgress, NULL, NULL))
            {
                break;
            }
        }
    }

    /*
        std::vector<Vertex> vertices;
        vertices.reserve(total_indices);

        size_t  fsize = file.f.size();
        int perf = fsize / 100;

        if (cmips)
            cmips->Print("OBJ: Processing...");
        for (size_t i = 0; i < fsize; i += 3)
        {
            int vi = file.f[i + 0];
            int vti = file.f[i + 1];
            int vni = file.f[i + 2];

            Vertex v =
            {
                file.v[vi * 3 + 0],
                file.v[vi * 3 + 1],
                file.v[vi * 3 + 2],

                vni >= 0 ? file.vn[vni * 3 + 0] : 0,
                vni >= 0 ? file.vn[vni * 3 + 1] : 0,
                vni >= 0 ? file.vn[vni * 3 + 2] : 0,

                vti >= 0 ? file.vt[vti * 3 + 0] : 0,
                vti >= 0 ? file.vt[vti * 3 + 1] : 0,
            };

            vertices.push_back(v);

            if (pFeedbackProc && ((i % perf)==0))
            {
                float fProgress = 100.f * ((float)(i) / fsize);
                if (pFeedbackProc(fProgress, NULL, NULL))
                {
                    break;
                }
            }

        }

        Mesh result;

        if (cmips)
            cmips->Print("OBJ: Generate Vertex Remap");
        std::vector<unsigned int> remap(total_indices);
        size_t total_vertices = meshopt_generateVertexRemap(&remap[0], NULL, total_indices, &vertices[0], total_indices, sizeof(Vertex));

        if (cmips)
            cmips->Print("OBJ: Remap Index Buffer");
        result.indices.resize(total_indices);
        meshopt_remapIndexBuffer(&result.indices[0], NULL, total_indices, &remap[0]);

        if (cmips)
            cmips->Print("OBJ: Remap Vertex Buffer");
        result.vertices.resize(total_vertices);
        meshopt_remapVertexBuffer(&result.vertices[0], &vertices[0], total_indices, sizeof(Vertex), &remap[0]);
    */

    if (cmips)
        cmips->Print("OBJ: Load Done ...");

    return result;
}
#endif

int Plugin_obj_Loader::LoadModelData(const char* pszFilename, const char* pszFilename2, void* pluginManager, void* msghandler, CMP_Feedback_Proc pFeedbackProc)
{
    int result = 0;

    if (pszFilename)
    {
        clock_t start               = clock();
        m_ModelData[0].m_model_name = pszFilename;
        result                      = m_loader.LoadGeometry(pszFilename, m_ModelData[0], g_CMIPS, pFeedbackProc);
        clock_t       elapsed       = clock() - start;
        unsigned long loadTime      = elapsed / (CLOCKS_PER_SEC / 1000);
        m_ModelData[0].m_LoadTime   = loadTime;
    }
    else
        return -1;

    return result;
}

int Plugin_obj_Loader::SaveModelData(const char* pdstFilename, void* meshData)
{
    if (!pdstFilename)
    {
        PrintInfo("Invalid Filename.");
        return -1;
    }
    if (!meshData)
    {
        PrintInfo("Invalid Mesh Data.");
        return -1;
    }

    if (g_CMIPS)
    {
        std::string sfullfilename = pdstFilename;
        std::string sfilename     = sfullfilename.substr(sfullfilename.find_last_of(".") + 1);

        g_CMIPS->Print("Saving Output File %s...", sfilename.c_str());
    }

    std::ofstream outputFileStream(pdstFilename);

    if (!outputFileStream)
    {
        PrintInfo("Cannot open file to write.");
        return -1;
    }

    CMP_Mesh* modelData = (CMP_Mesh*)meshData;
    int       nVCount   = (int)modelData->vertices.size();
    int       nICount   = (int)modelData->indices.size();

    bool hasVN = false;  //has vertex normal
    bool hasVT = false;  // has texcoordinate

    int nTCount = 0;  //texture coordinate count

    if (g_CMIPS)
    {
        g_CMIPS->Print("Writing vertices...");
    }
    for (int iv = 0; iv < nVCount; iv++)
    {
        outputFileStream << "v ";
        outputFileStream << modelData->vertices[iv].px << " " << modelData->vertices[iv].py << " " << modelData->vertices[iv].pz << std::endl;

        if (modelData->vertices[iv].nx > 0 || modelData->vertices[iv].ny > 0 || modelData->vertices[iv].nz > 0)
        {
            hasVN = true;
        }
        if (modelData->vertices[iv].tx > 0 || modelData->vertices[iv].ty > 0)
        {
            hasVT = true;
            nTCount++;
        }
    }

    if (hasVN)
    {
        if (g_CMIPS)
        {
            g_CMIPS->Print("Writing vertices normals...");
        }
        for (int iv = 0; iv < nVCount; iv++)
        {
            outputFileStream << "vn ";
            outputFileStream << modelData->vertices[iv].nx << " " << modelData->vertices[iv].ny << " " << modelData->vertices[iv].nz << std::endl;
        }
    }

    if (hasVT)
    {
        if (g_CMIPS)
        {
            g_CMIPS->Print("Writing vertices texture...");
        }
        for (int iv = 0; iv < nVCount; iv++)
        {
            outputFileStream << "vt ";
            outputFileStream << modelData->vertices[iv].tx << " " << modelData->vertices[iv].ty << std::endl;
        }
    }

    if (g_CMIPS)
    {
        g_CMIPS->Print("Writing faces...");
    }
    for (int k = 0; k < nICount; k += 3)
    {
        outputFileStream << "f ";
        for (int j = 0; j < 3; j++)
        {
            if (hasVN)
            {
                outputFileStream << modelData->indices[k + j] + 1 << "//" << modelData->indices[k + j] + 1 << " ";
            }
            else
                outputFileStream << modelData->indices[k + j] + 1 << " ";
        }
        outputFileStream << std::endl;
    }

    outputFileStream.close();

    if (g_CMIPS)
    {
        g_CMIPS->Print("Saving done successfully...");
    }
    return 0;
}