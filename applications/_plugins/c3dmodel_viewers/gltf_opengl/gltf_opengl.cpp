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
#include "gltf_opengl.h"

#include <stdio.h>
#include <stdlib.h>
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "compressonator.h"

#include <iostream>
#include <fstream>

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_glTF_OpenGL)
SET_PLUGIN_TYPE("3DMODEL_VIEWER")
SET_PLUGIN_NAME("OPENGL")
#else
void* make_Plugin_glTF_OpenGL()
{
    return new Plugin_glTF_OpenGL();
}
#endif

#include "gltf_ogldevice.h"

#ifdef BUILD_AS_PLUGIN_DLL
bool   g_bAbortCompression = false;
CMIPS* g_CMIPS             = nullptr;
#else
extern bool   g_bAbortCompression;
extern CMIPS* g_CMIPS;
#endif

//using namespace MV_gltf_opengl;

Plugin_glTF_OpenGL::Plugin_glTF_OpenGL()
{
    m_glTF_OGLDevice = NULL;
}

Plugin_glTF_OpenGL::~Plugin_glTF_OpenGL()
{
    if (m_glTF_OGLDevice)
    {
        m_glTF_OGLDevice->DeviceReady = false;
        m_glTF_OGLDevice->SetFullScreen(false);
        m_glTF_OGLDevice->OnDestroy();
        delete m_glTF_OGLDevice;
    }
}

int Plugin_glTF_OpenGL::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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

int Plugin_glTF_OpenGL::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        OPENGL_CMips              = static_cast<CMIPS*>(Shared);
        OPENGL_CMips->m_infolevel = 0x01;  // Turn on print Info
        return 0;
    }
    return 1;
}

void Plugin_glTF_OpenGL::OnReSizeView(CMP_LONG w, CMP_LONG h)
{
    if (m_glTF_OGLDevice)
    {
        m_glTF_OGLDevice->OnResize(w, h);
    }
}

void Plugin_glTF_OpenGL::processMSG(void* message)
{
    if (m_glTF_OGLDevice)
    {
        MSG* msg = static_cast<MSG*>(message);
        m_glTF_OGLDevice->OnEvent(*msg);
    }
}

bool Plugin_glTF_OpenGL::OnRenderView()
{
    if (m_glTF_OGLDevice)
    {
        if (m_glTF_OGLDevice->DeviceReady)
        {
            m_glTF_OGLDevice->OnRender();
            return true;
        }
    }
    return false;
}

void Plugin_glTF_OpenGL::CloseView()
{
    if (m_glTF_OGLDevice)
    {
        if (m_glTF_OGLDevice->DeviceReady)
        {
            m_glTF_OGLDevice->DeviceReady = false;
            m_glTF_OGLDevice->SetFullScreen(false);
            m_glTF_OGLDevice->OnDestroy();
        }
        delete m_glTF_OGLDevice;
        m_glTF_OGLDevice = NULL;
    }
}

void* Plugin_glTF_OpenGL::ShowView(void* data)
{
    return NULL;
}

static std::string GetFilePathExtension(const std::string& FileName)
{
    if (FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".") + 1);
    return "";
}

// bool Plugin_glTF_OpenGL::LoadModel(const char* pszFilename1, const char* pszFilename2, CMP_Feedback_Proc pFeedbackProc)
// {
//     if (!pszFilename1) return false;
//
//     std::string FilePathName1(pszFilename1);
//     int ret = false;
//     std::string err;
//
//     if (FilePathName1.length() > 0)
//     {
//         std::string ext = GetFilePathExtension(FilePathName1);
//         if (ext.compare("glb") == 0) {
//             ret = m_loader.LoadBinaryFromFile(&m_model, &err, FilePathName1.c_str());
//         }
//         else {
//             ret = m_loader.LoadASCIIFromFile(&m_model, &err, FilePathName1.c_str());
//         }
//     }
//
//     return ret;
// }

void* Plugin_glTF_OpenGL::CreateView(void*             ModelData,
                                     CMP_LONG          Width,
                                     CMP_LONG          Height,
                                     void*             userWidget,
                                     void*             pluginManager,
                                     void*             msghandler,
                                     CMP_Feedback_Proc pFeedbackProc = NULL)
{
    m_model  = (CMODEL_DATA*)ModelData;
    m_parent = (QWidget*)userWidget;

    //   m_glTF_OGLDevice = new glTF_OGLDevice();
    //   if (m_glTF_OGLDevice)
    //   {
    //       m_glTF_OGLDevice->init(m_model, Width, Height, pluginManager, msghandler, m_parent);
    //       m_glTF_OGLDevice->OnCreate();
    //       return m_glTF_OGLDevice;
    //   }
    //
    //   return nullptr;

    m_glTF_OGLDevice = new glTF_OGLDevice(m_model, Width, Height, pluginManager, msghandler, m_parent);
    if (m_glTF_OGLDevice)
    {
        m_glTF_OGLDevice->OnCreate();
    }
    else
        return nullptr;

    return m_glTF_OGLDevice;
}
