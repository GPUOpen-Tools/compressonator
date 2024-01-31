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
#ifndef _GLTF_OPENGL_H
#define _GLTF_OPENGL_H

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "plugininterface.h"
#include "gltf_ogldevice.h"

#ifdef _WIN32
// {79436B32-C8E1-45C9-99E3-AAA56B076A05}
static const GUID g_GUID = {0x79436b32, 0xc8e1, 0x45c9, {0x99, 0xe3, 0xaa, 0xa5, 0x6b, 0x7, 0x6a, 0x5}};
#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR 1
#define TC_PLUGIN_VERSION_MINOR 0

CMIPS* OPENGL_CMips = NULL;

class Plugin_glTF_OpenGL : public PluginInterface_3DModel
{
public:
    Plugin_glTF_OpenGL();
    virtual ~Plugin_glTF_OpenGL();
    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_PluginSetSharedIO(void* Shared);

    void* CreateView(void* ModelData, CMP_LONG Width, CMP_LONG Height, void* userHWND, void* pluginManager, void* msghandler, CMP_Feedback_Proc pFeedbackProc);
    void* ShowView(void* data);
    void  CloseView();
    void  processMSG(void* message);

    bool OnRenderView();
    void OnReSizeView(CMP_LONG w, CMP_LONG h);

    bool LoadModel(const char* pszFilename, const char* pszFilename2, CMP_Feedback_Proc pFeedbackProc = NULL);

private:
    glTF_OGLDevice* m_glTF_OGLDevice;
    std::string     m_FilePathName;
    CMODEL_DATA*    m_model;
    QWidget*        m_parent;
};

extern void* make_Plugin_glTF_OpenGL();

#endif