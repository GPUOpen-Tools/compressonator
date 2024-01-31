//=============================================================================
/// Copyright (c) 2017-2024 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
//=============================================================================

#ifndef PLUGIN_FFC9DDE2_1913_4305_A633_B8E9B9CAA895_H
#define PLUGIN_FFC9DDE2_1913_4305_A633_B8E9B9CAA895_H

#include <cinttypes>
#include <cstdlib>
#include <fstream>

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4018)
#pragma warning(disable : 4005)
#include "draco/compression/encode.h"
#include "draco/compression/decode.h"
#include "draco/io/mesh_io.h"
#include "draco/io/point_cloud_io.h"
#include "draco/io/obj_encoder.h"
#include "draco/core/cycle_timer.h"
#pragma warning(pop)

#include "compressonator.h"
#include "plugininterface.h"

#include "modeldata.h"

#define TC_PLUGIN_VERSION_MAJOR 1
#define TC_PLUGIN_VERSION_MINOR 0

#ifdef _WIN32
// {FFC9DDE2-1913-4305-A633-B8E9B9CAA895}
static const GUID g_GUID = {0xffc9dde2, 0x1913, 0x4305, {0xa6, 0x33, 0xb8, 0xe9, 0xb9, 0xca, 0xa8, 0x95}};
#else
static const GUID g_GUID = {0};
#endif

class Plugin_ModelLoader_drc : public PluginInterface_3DModel_Loader
{
public:
    Plugin_ModelLoader_drc();
    virtual ~Plugin_ModelLoader_drc();
    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginSetSharedIO(void* Shared);

    void* GetModelData();
    int   LoadModelData(const char* pszFilename, const char* pszFilename2, void* pluginManager, void* msghandler, CMP_Feedback_Proc pFeedbackProc);
    int   SaveModelData(const char* pdstFilename, void* meshData);

private:
    draco::Mesh*                       m_mesh;
    CMODEL_DATA*                       m_ModelData;
    std::unique_ptr<draco::PointCloud> m_pc;
    bool                               m_loadedMesh;
    bool                               m_loadModel;
};

#ifndef BUILD_AS_PLUGIN_DLL
extern void* make_Plugin_ModelLoader_drc();
#endif

#endif