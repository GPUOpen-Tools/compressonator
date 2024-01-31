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
#ifndef PLUGIN_25E831EF_9BC0_4D7C_AFD8_16BE6E0A40CB_H
#define PLUGIN_25E831EF_9BC0_4D7C_AFD8_16BE6E0A40CB_H

#include <cassert>

#include <fstream>

// #pragma warning( push )
// #pragma warning( disable : 4244)
// #pragma warning( disable : 4267)
// #pragma warning( disable : 4018)
// #pragma warning( disable : 4005)
// #include "draco/compression/encode.h"
// #include "draco/compression/decode.h"
// #include "draco/io/mesh_io.h"
// #include "draco/io/point_cloud_io.h"
// #pragma warning( pop )

#include "plugininterface.h"
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "compressonator.h"
#include "modeldata.h"

// using namespace draco;

#ifdef _WIN32
// {25E831EF-9BC0-4D7C-AFD8-16BE6E0A40CB}
static const GUID g_GUID = {0x25e831ef, 0x9bc0, 0x4d7c, {0xaf, 0xd8, 0x16, 0xbe, 0x6e, 0xa, 0x40, 0xcb}};

#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR 1
#define TC_PLUGIN_VERSION_MINOR 0

class Plugin_Mesh_Compressor : public PluginInterface_Mesh
{
public:
    Plugin_Mesh_Compressor();
    virtual ~Plugin_Mesh_Compressor();
    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginSetSharedIO(void*);

    int Init();
    int CleanUp();

    void* ProcessMesh(void* data, void* setting, void* statsOut, CMP_Feedback_Proc pFeedbackProc);

private:
    bool m_InitOK;
};

#ifndef BUILD_AS_PLUGIN_DLL
extern void* make_Plugin_Mesh_Compressor();
#endif

#endif