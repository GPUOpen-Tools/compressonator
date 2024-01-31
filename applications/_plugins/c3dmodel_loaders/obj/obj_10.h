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
#ifndef _PLUGIN_IMAGE_OBJ10_H
#define _PLUGIN_IMAGE_OBJ10_H

#include "plugininterface.h"
#include "meshobjloader.h"
#include "modeldata.h"

#ifdef _WIN32
// {73376578-3ABB-4F03-A595-95C8552DDFB1}
static const GUID g_GUID = {0x73376578, 0x3abb, 0x4f03, {0xa5, 0x95, 0x95, 0xc8, 0x55, 0x2d, 0xdf, 0xb1}};

#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR 1
#define TC_PLUGIN_VERSION_MINOR 0

namespace cmp_ml_obj
{
CMIPS* DX12_CMips = NULL;
}

using namespace cmp_ml_obj;

class Plugin_obj_Loader : public PluginInterface_3DModel_Loader
{
public:
    Plugin_obj_Loader();
    virtual ~Plugin_obj_Loader();
    int   TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int   TC_PluginSetSharedIO(void* Shared);
    void* GetModelData();
    int   LoadModelData(const char* pszFilename, const char* pszFilename2, void* pluginManager, void* msghandler, CMP_Feedback_Proc pFeedbackProc);
    int   SaveModelData(const char* pdstFilename, void* meshData);

private:
    CMODEL_DATA m_ModelData[2];

    std::vector<ObjVertexFinal> m_objVertices;
    std::vector<ObjFace>        m_objFaces;
    std::vector<ObjVertex3D>    m_viewpoints;
    std::vector<unsigned int>   m_faceCluster;

    const char*  pViewpointName = NULL;
    const float* pViewpoints    = NULL;
    unsigned int nViewpoints;

    MeshObjLoader m_loader;
};

extern void* make_Plugin_obj_Loader();

#endif