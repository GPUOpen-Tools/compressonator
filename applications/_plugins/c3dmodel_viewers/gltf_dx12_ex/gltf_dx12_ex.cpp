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
#include "gltf_dx12_ex.h"

#include <stdio.h>
#include <stdlib.h>
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "compressonator.h"

#include <iostream>
#include <fstream>

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_glTF_DX12_EX)
SET_PLUGIN_TYPE("3DMODEL_VIEWER")
SET_PLUGIN_NAME("DX12_EX")
#else
void* make_Plugin_glTF_DX12_EX()
{
    return new Plugin_glTF_DX12_EX;
}
#endif

#include "misc.h"
#include "d3dx12/d3dx12.h"
#include "gltf_dx12deviceex.h"

#ifdef BUILD_AS_PLUGIN_DLL
bool   g_bAbortCompression = false;
CMIPS* g_CMIPS             = nullptr;
#else
extern bool   g_bAbortCompression;
extern CMIPS* g_CMIPS;
#endif

//using namespace ML_gltf_dx12_ex;

//#define _DEBUGMEM
#ifdef _DEBUGMEM

FILE* pFile = NULL;

#define WIDTH 7

void printMemUsage(const char* slocation)
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    if (pFile)
        std::fprintf(pFile, "There is  %*ld percent of memory in use at %s\n", WIDTH, statex.dwMemoryLoad, slocation);
}

std::int32_t mallocID = 0;

// optional_ops.cpp
void* operator new(std::size_t size) throw(std::bad_alloc)
{
    size_t* p = (size_t*)std::malloc(size + sizeof(size_t) + sizeof(size_t));
    p[0]      = size;      // store the size  and id in the first few bytes
    p[1]      = mallocID;  // Note std::int32_t == sizeof(size_t) which is 4 bytes!
    if (pFile)
        std::fprintf(pFile, "global op new called, size[%8x] = %6zu ID = %3d\n", &p[1], size, mallocID);
    mallocID++;
    return (void*)(&p[2]);  // return the memory just after the size we stored
}

void operator delete(void* ptr) throw()
{
    size_t*      p    = (size_t*)ptr;  // make the pointer the right type
    std::int32_t id   = p[-1];         // get the data we stored at the beginning of this block
    size_t       size = p[-2];         // get the data we stored at the beginning of this block
    if (pFile)
        std::fprintf(pFile, "global op del called, size[%5x] = %6d ID = %3d\n", ptr, size, id);
    void* p2 = (void*)(&p[-2]);  // get a pointer to the memory we originally really allocated
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
    m_gltfLoader        = NULL;
    m_glTF_DX12DeviceEx = NULL;
    m_hwnd              = 0L;
}

Plugin_glTF_DX12_EX::~Plugin_glTF_DX12_EX()
{
#ifdef _DEBUGMEM
    printMemUsage(__func__);
#endif
    if (m_gltfLoader)
    {
        if (m_gltfLoader[0].m_filename.length() > 0)
            m_gltfLoader[0].Unload();
        if (m_gltfLoader[1].m_filename.length() > 0)
            m_gltfLoader[1].Unload();
    }

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
    pPluginVersion->guid = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor    = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor    = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_glTF_DX12_EX::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        DX12_CMips              = static_cast<CMIPS*>(Shared);
        DX12_CMips->m_infolevel = 0x01;  // Turn on print Info
        return 0;
    }
    return 1;
}

void Plugin_glTF_DX12_EX::processMSG(void* message)
{
    if (m_glTF_DX12DeviceEx && m_hwnd)
    {
        MSG* msg = static_cast<MSG*>(message);
        m_glTF_DX12DeviceEx->OnEvent(*msg);
    }
}

bool Plugin_glTF_DX12_EX::OnRenderView()
{
    if (m_glTF_DX12DeviceEx && m_hwnd)
    {
        m_glTF_DX12DeviceEx->OnRender();
        return true;
    }
    return false;
}

void Plugin_glTF_DX12_EX::OnReSizeView(CMP_LONG w, CMP_LONG h)
{
    if (m_glTF_DX12DeviceEx && m_hwnd)
    {
        m_glTF_DX12DeviceEx->OnResize(w, h);
    }
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

void* Plugin_glTF_DX12_EX::ShowView(void* data)
{
    return NULL;
}

void* Plugin_glTF_DX12_EX::CreateView(void*             ModelData,
                                      CMP_LONG          Width,
                                      CMP_LONG          Height,
                                      void*             userHWND,
                                      void*             pluginManager,
                                      void*             msghandler,
                                      CMP_Feedback_Proc pFeedbackProc = NULL)
{
    // device already created!
    if (m_glTF_DX12DeviceEx)
        return 0;
    if (!ModelData)
        return 0;

#ifdef _DEBUGMEM
    printMemUsage(__func__);
#endif

    m_gltfLoader = (CMP_GLTFCommon*)ModelData;

    // Check User Canceled status for long glTF file loads!!
    if (DX12_CMips)
    {
        if (DX12_CMips->m_canceled)
        {
            DX12_CMips->m_canceled = false;
            return nullptr;
        }
    }

    m_parent = (QWidget*)userHWND;
    // m_hwnd = (HWND)userHWND

    m_hwnd = reinterpret_cast<HWND>(m_parent->winId());

    if (m_hwnd)
    {
        m_glTF_DX12DeviceEx = new glTF_DX12DeviceEx(m_gltfLoader, Width, Height, pluginManager, msghandler, m_parent);
        m_glTF_DX12DeviceEx->OnCreate(m_hwnd);
    }

    // Check User Canceled status for Aborting Create Device!!
    if (DX12_CMips)
    {
        if (DX12_CMips->m_canceled)
        {
            DX12_CMips->m_canceled = false;
            return nullptr;
        }
    }

    if (m_glTF_DX12DeviceEx)
        m_glTF_DX12DeviceEx->OnResize(Width, Height);

#ifdef _DEBUGMEM
    printMemUsage(__func__);
#endif
    return m_glTF_DX12DeviceEx;
}
