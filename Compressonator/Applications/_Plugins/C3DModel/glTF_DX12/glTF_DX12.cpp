#include "glTF_DX12.h"

#include <stdio.h>
#include <stdlib.h>
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"
#include "MIPS.h"
#include "Compressonator.h"

#include <iostream>
#include <fstream>
using namespace std;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_glTF_DX12)
SET_PLUGIN_TYPE("3DMODEL_DX12")
SET_PLUGIN_NAME("GLTF")
#else
void *make_Plugin_glTF_DX12() { return new Plugin_glTF_DX12; }
#endif

#include "stdafx.h"
#include "glTF_DX12Renderer.h"
#include "AmdUtil\Misc.h"
#include "d3dx12\d3dx12.h"
#include "glTF_DX12Device.h"

#ifdef DEBUG
#include <DXGIDebug.h>
#endif 

Plugin_glTF_DX12::Plugin_glTF_DX12()
{
    m_glTF_DX12Device = NULL;
}

Plugin_glTF_DX12::~Plugin_glTF_DX12()
{
    if (m_glTF_DX12Device)
    {
        m_glTF_DX12Device->SetFullScreen(false);
        m_glTF_DX12Device->OnDestroy();
        delete m_glTF_DX12Device;
    }
}

int Plugin_glTF_DX12::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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


void Plugin_glTF_DX12::processMSG(void *message)
{
    if (m_glTF_DX12Device)
    {
        MSG *msg = static_cast<MSG*>(message);
        m_glTF_DX12Device->OnEvent(*msg);
    }
}

bool Plugin_glTF_DX12::RenderView()
{
    if (m_glTF_DX12Device)
    {
        m_glTF_DX12Device->OnRender();
    }
    return true;
}

void Plugin_glTF_DX12::CloseView()
{
    if (m_glTF_DX12Device)
    {
        m_glTF_DX12Device->SetFullScreen(false);
        m_glTF_DX12Device->OnDestroy();
        delete m_glTF_DX12Device;
        m_glTF_DX12Device = NULL;
    }
}


int Plugin_glTF_DX12::LoadModel(const char* pszFilename, const char* pszFilename2, CMP_Feedback_Proc pFeedbackProc)
{
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    if (pszFilename)
    {
        std::string FilePathName(pszFilename);
        m_FilePathName = FilePathName;
        _splitpath_s(pszFilename, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
        std::string Path(dir);
        std::string File(fname);
        File.append(ext);
        m_gltfLoader[0].Load(Path, File, pFeedbackProc);
    }
    else
        return -1;

    if (pszFilename2)
    {
        std::string FilePathName(pszFilename2);
        m_FilePathName = FilePathName;
        _splitpath_s(pszFilename2, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
        std::string Path(dir);
        std::string File(fname);
        File.append(ext);
        m_gltfLoader[1].Load(Path, File, pFeedbackProc);
    }
    else
        m_gltfLoader[1].m_filename = "";

    return 0;
}


int Plugin_glTF_DX12::CreateView(const char* pszFilename, CMP_LONG Width, CMP_LONG Height, void *userHWND, void *pluginManager, void *msghandler, const char* pszFilename2 = NULL, CMP_Feedback_Proc pFeedbackProc = NULL)
{
    int result = LoadModel(pszFilename, pszFilename2, pFeedbackProc);
    if (result != 0) return result;

    m_glTF_DX12Device = new glTF_DX12Device(m_gltfLoader, Width, Height,pluginManager, msghandler);
    m_glTF_DX12Device->OnCreate(*(HWND *)userHWND);
    m_glTF_DX12Device->OnResize(Width, Height);
    
    return 0;
}
