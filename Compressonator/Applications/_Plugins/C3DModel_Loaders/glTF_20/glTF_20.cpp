//=====================================================================
// Copyright 2018 (c), Advanced Micro Devices, Inc. All rights reserved.
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
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#ifdef _WIN32
#include <windows.h>
// TODO: reference additional headers your program requires here
#include <tchar.h>
#endif

#include <assert.h>

#include "glTF_20.h"

#include <stdio.h>
#include <stdlib.h>
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"
#include "MIPS.h"
#include "Compressonator.h"

#include <iostream>
#include <fstream>

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_glTF_Loader)
SET_PLUGIN_TYPE("3DMODEL_LOADER")
SET_PLUGIN_NAME("GLTF")
#else
void *make_Plugin_glTF_Loader() { return new Plugin_glTF_Loader; }
#endif

using namespace std;
#include "stdafx.h"
#include "Misc.h"

#ifdef USE_TINYGLTF2
#include "tiny_gltf2_utils.h"
#endif

PluginManager          g_pluginManager;
bool                   g_bAbortCompression = false;
CMIPS*                 g_CMIPS = NULL;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")

Plugin_glTF_Loader::Plugin_glTF_Loader()
{

}

Plugin_glTF_Loader::~Plugin_glTF_Loader()
{
}

int Plugin_glTF_Loader::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{ 
    #ifdef _WIN32
    pPluginVersion->guid                     = g_GUID;
    #endif
    pPluginVersion->dwAPIVersionMajor        = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor        = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor     = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor     = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_glTF_Loader::TC_PluginSetSharedIO(void *Shared)
{
    if (Shared)
    {
        g_CMIPS = static_cast<CMIPS *>(Shared);
        g_CMIPS->m_infolevel = 0x01; // Turn on print Info 
        return 0;
    }
    return 1;
}

void *Plugin_glTF_Loader::GetModelData()
{
    void *data = (void *)m_gltfLoader;
    return data;
}

std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

static std::string GetFilePathExtension(const std::string &FileName) {
    if (FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".") + 1);
    return "";
}

int Plugin_glTF_Loader::LoadModelData(const char* pszFilename, const char* pszFilename2, void *pluginManager, void *msghandler, CMP_Feedback_Proc pFeedbackProc)
{
    int result = 0;
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    m_gltfLoader[0].m_filename = "";
    m_gltfLoader[1].m_filename = "";

    if (pszFilename)
    {

        std::string FilePathName(pszFilename);
        _splitpath_s(pszFilename, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
        std::string Path(dir);
        std::string File(fname);
        File.append(ext);
        double timeNow = MillisecondsNow();
        m_gltfLoader[0].m_CommonLoadTime = 0;
        result = m_gltfLoader[0].Load(Path, File, g_CMIPS);
        m_gltfLoader[0].m_CommonLoadTime = MillisecondsNow() - timeNow;
        if (result == 0)
            m_gltfLoader[0].m_filename = FilePathName;
        else
            return -2;

#ifdef USE_TINYGLTF2
        {
            tinygltf2::TinyGLTF loader;
            std::string err;
            bool ret = false;
            std::string ext = GetFilePathExtension(pszFilename);
            if (ext.compare("glb") == 0) {
                // assume binary glTF.
                ret = loader.LoadBinaryFromFile(&m_gltfLoader[0].m_model, &err, pszFilename);
            }
            else {
                // assume ascii glTF.
                ret = loader.LoadASCIIFromFile(&m_gltfLoader[0].m_model, &err, pszFilename);
            }
            if (!ret) {
                if (g_CMIPS)
                    g_CMIPS->Print("Failed to load gltf file: %s\n", pszFilename);
                // return -1;  Ignore for now until we fully utilize the replacement::: 
            }

        }
#endif


    }
    else
        return -1;

    if (strlen(pszFilename2) > 0)
    {
        std::string FilePathName(pszFilename2);
        _splitpath_s(pszFilename2, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
        std::string Path(dir);
        std::string File(fname);
        File.append(ext);
        double timeNow = MillisecondsNow();
        m_gltfLoader[1].m_CommonLoadTime = 0;
        result = m_gltfLoader[1].Load(Path, File, g_CMIPS);
        m_gltfLoader[1].m_CommonLoadTime = MillisecondsNow() - timeNow;
        if (result == 0)
            m_gltfLoader[1].m_filename = FilePathName;
        else
            return -3;

#ifdef USE_TINYGLTF2
        {
            tinygltf2::TinyGLTF loader;
            std::string err;
            bool ret = false;
            std::string ext = GetFilePathExtension(pszFilename);
            if (ext.compare("glb") == 0) {
                // assume binary glTF.
                ret = loader.LoadBinaryFromFile(&m_gltfLoader[1].m_model, &err, pszFilename);
            }
            else {
                // assume ascii glTF.
                ret = loader.LoadASCIIFromFile(&m_gltfLoader[1].m_model, &err, pszFilename);
            }
            if (!ret) 
            {
                if (g_CMIPS)
                    g_CMIPS->Print("Failed to load .gltf file: %s", pszFilename);

                // return -1;  Ignore for now until we fully utilize the replacement::: 
            }


        }
#endif

    }


    return result;
}

int Plugin_glTF_Loader::SaveModelData(const char* pdstFilename, void* meshData)
{
    GLTFCommon *gltfData  = reinterpret_cast<GLTFCommon *> (meshData);
    if (!gltfData)
    {
        if (g_CMIPS)
            g_CMIPS->Print("Failed to write .gltf file, data is empty.: %s", pdstFilename);

        return -1;
    }

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    std::string FilePathName(pdstFilename);
    _splitpath_s(pdstFilename, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
    std::string Path(dir);
    std::string File(fname);
    File.append(ext);

    int result = gltfData->Save(Path, File, g_CMIPS);

    if (result != 0)
    {
        if (g_CMIPS)
            g_CMIPS->Print("Failed to write .gltf file: %s", pdstFilename);

        return -1;
    }

    std::ofstream ofstreamdest(pdstFilename, std::ios_base::out);
    if (!ofstreamdest)
    {
        if (g_CMIPS)
            g_CMIPS->Print("Failed to write .gltf file: %s", pdstFilename);

        return -1;
    }

    ofstreamdest << gltfData->j3;
    ofstreamdest.close();

#ifdef USE_TINYGLTF2
    tinygltf2::TinyGLTF loader;

    if (!loader.WriteGltfSceneToFile(&gltfData->m_model, pdstFilename))
    {
        if (g_CMIPS)
            g_CMIPS->Print("Failed to write .gltf file: %s", pdstFilename);
    }
#endif

    return 0;
}