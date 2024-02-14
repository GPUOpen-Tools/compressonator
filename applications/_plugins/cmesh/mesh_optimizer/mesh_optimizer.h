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
#ifndef _PLUGIN_MESH_OPTIMIZER_H
#define _PLUGIN_MESH_OPTIMIZER_H

#include <cassert>

#include "plugininterface.h"
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "compressonator.h"
#include "meshoptimizer.h"
#include "modeldata.h"

#ifdef _WIN32
// {7702EE85-0957-4EDC-9A65-58B5C5E8C4C8}
static const GUID g_GUID = {0x7702ee85, 0x957, 0x4edc, {0x9a, 0x65, 0x58, 0xb5, 0xc5, 0xe8, 0xc4, 0xc8}};
#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR 1
#define TC_PLUGIN_VERSION_MINOR 0

//=================================================================================================================================
/// A simple structure to store the settings for this mesh process
//=================================================================================================================================
struct MeshSettings
{
    const char* pMeshName;
    const char* pDestMeshName;

    unsigned int nCacheSize;
    unsigned int nlevelofDetails;  //LOD for mesh simplify (edge collapse)
    float        nOverdrawACMRthreshold;

    bool bSimplifyMesh;  //edge collapse according to user input LOD, higher->lesser triangles
    bool bOptimizeVFetch;
    bool bOptimizeOverdraw;
    bool bOptimizeVCacheFifo;  //optimize based on which go into cache first (using timestamp)
    bool bOptimizeVCache;      //optimize based on table of const score
    bool bRandomizeMesh;
};

static MeshSettings m_settings;

class Plugin_Mesh_Optimizer : public PluginInterface_Mesh
{
public:
    Plugin_Mesh_Optimizer();
    virtual ~Plugin_Mesh_Optimizer();
    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginSetSharedIO(void*);

    int Init();
    int CleanUp();

    void* ProcessMesh(void* data, void* setting, void* statsOut, CMP_Feedback_Proc pFeedbackProc);

private:
    bool                  m_InitOK;
    std::vector<CMP_Mesh> m_copy;
};

extern void* make_Plugin_Mesh_Optimizer();

#endif