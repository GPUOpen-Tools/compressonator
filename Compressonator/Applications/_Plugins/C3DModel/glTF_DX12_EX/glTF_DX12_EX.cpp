#include "glTF_DX12_EX.h"

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
DECLARE_PLUGIN(Plugin_glTF_DX12_EX)
SET_PLUGIN_TYPE("3DMODEL_DX12_EX")
SET_PLUGIN_NAME("GLTF")
#else
void *make_Plugin_glTF_DX12_EX() { return new Plugin_glTF_DX12_EX; }
#endif

#include "stdafx.h"
#include "Misc.h"
#include "d3dx12\d3dx12.h"
#include "glTF_DX12DeviceEx.h"


//#define _DEBUGMEM


#ifdef _DEBUGMEM

FILE * pFile = NULL;

#define WIDTH 7

void printMemUsage(const char *slocation)
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    if (pFile)
        std::fprintf(pFile, "There is  %*ld percent of memory in use at %s\n", WIDTH, statex.dwMemoryLoad,slocation);
}



std::int32_t mallocID = 0;

// optional_ops.cpp
void * operator new(std::size_t size) throw(std::bad_alloc)
{
    size_t *p = (size_t *)std::malloc(size + sizeof(size_t) + sizeof(size_t));
    p[0] = size;           // store the size  and id in the first few bytes
    p[1] = mallocID;       // Note std::int32_t == sizeof(size_t) which is 4 bytes!
    if (pFile)
        std::fprintf(pFile,"global op new called, size[%8x] = %6zu ID = %3d\n", &p[1], size, mallocID);
    mallocID++;
    return (void*)(&p[2]); // return the memory just after the size we stored
}

void operator delete(void * ptr) throw()
{
    size_t *p = (size_t*)ptr;       // make the pointer the right type
    std::int32_t id = p[-1];        // get the data we stored at the beginning of this block
    size_t size = p[-2];            // get the data we stored at the beginning of this block
    if (pFile)
        std::fprintf(pFile, "global op del called, size[%5x] = %6d ID = %3d\n", ptr, size, id);
    void *p2 = (void*)(&p[-2]); // get a pointer to the memory we originally really allocated
    std::free(p2);
}
#endif


Plugin_glTF_DX12_EX::Plugin_glTF_DX12_EX()
{
#ifdef _DEBUGMEM
    if (pFile)
        fclose(pFile);
    pFile = fopen("memdump.txt", "w");
    printMemUsage(__func__);
#endif

    m_glTF_DX12DeviceEx = NULL;
    m_hwnd = 0L;
}

Plugin_glTF_DX12_EX::~Plugin_glTF_DX12_EX()
{
#ifdef _DEBUGMEM
    printMemUsage(__func__);
#endif

    if (m_gltfLoader[0].m_filename.length() > 0)
        m_gltfLoader[0].Unload();
    if (m_gltfLoader[1].m_filename.length() > 0)
        m_gltfLoader[1].Unload();

#ifdef _DEBUGMEM
    printMemUsage(__func__);
    if (pFile)
    {
        fclose(pFile);
        pFile = NULL;
    }
#endif
}

int Plugin_glTF_DX12_EX::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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


int Plugin_glTF_DX12_EX::TC_PluginSetSharedIO(void *Shared)
{
    if (Shared)
    {
        DX12_CMips = static_cast<CMIPS *>(Shared);
        DX12_CMips->m_infolevel = 0x01; // Turn on print Info 
        return 0;
    }
    return 1;
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


void Plugin_glTF_DX12_EX::processMSG(void *message)
{
    if (m_glTF_DX12DeviceEx && m_hwnd)
    {
        MSG *msg = static_cast<MSG*>(message);
        m_glTF_DX12DeviceEx->OnEvent(*msg);
    }
}

bool Plugin_glTF_DX12_EX::RenderView()
{
    if (m_glTF_DX12DeviceEx && m_hwnd)
    {
        m_glTF_DX12DeviceEx->OnRender();
        return true;
    }
    return false;
}

void Plugin_glTF_DX12_EX::ReSizeView(CMP_LONG w, CMP_LONG h)
{

}


void Plugin_glTF_DX12_EX::CloseView()
{
    if (m_glTF_DX12DeviceEx)
    {
        m_glTF_DX12DeviceEx->SetFullScreen(false);
        m_glTF_DX12DeviceEx->OnDestroy();
        delete m_glTF_DX12DeviceEx;
        m_glTF_DX12DeviceEx = NULL;
    }
}

// Note ToDo: Return values should be enumed values
int Plugin_glTF_DX12_EX::LoadModel(const char* pszFilename, const char* pszFilename2, CMP_Feedback_Proc pFeedbackProc)
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
        result = m_gltfLoader[0].Load(Path, File, DX12_CMips);
        if (result == 0)
            m_gltfLoader[0].m_filename = FilePathName;
    }
    else
        return -1;

    if (pszFilename2)
    {
        std::string FilePathName(pszFilename2);
        _splitpath_s(pszFilename2, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
        std::string Path(dir);
        std::string File(fname);
        File.append(ext);
        result = m_gltfLoader[1].Load(Path, File, DX12_CMips);
        if (result == 0)
            m_gltfLoader[1].m_filename = FilePathName;
    }


    return result;
}


int Plugin_glTF_DX12_EX::CreateView(const char* pszFilename, CMP_LONG Width, CMP_LONG Height, void *userHWND, void *pluginManager, void *msghandler, const char* pszFilename2 = NULL, CMP_Feedback_Proc pFeedbackProc = NULL)
{
    // device already created!
    if (m_glTF_DX12DeviceEx) return 0;

#ifdef _DEBUGMEM
    printMemUsage(__func__);
#endif

    int result = LoadModel(pszFilename, pszFilename2, pFeedbackProc);
    if (result != 0) return result;

#ifdef _DEBUGMEM
    printMemUsage(__func__);
#endif

    // Check User Canceled status for long glTF file loads!!
    if (DX12_CMips)
    {
        if (DX12_CMips->m_canceled)
        {
            DX12_CMips->m_canceled = false;
            return -1;
        }
    }

    m_hwnd = (HWND)userHWND;
    if (m_hwnd)
    {
        m_glTF_DX12DeviceEx = new glTF_DX12DeviceEx(m_gltfLoader, Width, Height, pluginManager, msghandler);
        m_glTF_DX12DeviceEx->OnCreate(m_hwnd);
    }

    // Check User Canceled status for Aborting Create Device!!
    if (DX12_CMips)
    {
        if (DX12_CMips->m_canceled)
        {
            DX12_CMips->m_canceled = false;
            return -1;
        }
    }

    if (m_glTF_DX12DeviceEx)
        m_glTF_DX12DeviceEx->OnResize(Width, Height);

#ifdef _DEBUGMEM
    printMemUsage(__func__);
#endif        
    return 0;
}
