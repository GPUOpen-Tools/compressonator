//=====================================================================
// Copyright 2016-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "compressonator.h"
#include "copengl.h"

CMIPS* COpenGL_CMips;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_COpenGL)
SET_PLUGIN_TYPE("GPUDECODE")
SET_PLUGIN_NAME("OPENGL")
#else
void* make_Plugin_GPUDecode_OpenGL()
{
    return new Plugin_COpenGL;
}
#endif

#pragma comment(lib, "advapi32.lib")  // for RegCloseKey and other Reg calls ...

Plugin_COpenGL::Plugin_COpenGL()
{
    m_pGPUDecode = NULL;
}

Plugin_COpenGL::~Plugin_COpenGL()
{
    if (m_pGPUDecode)
        delete m_pGPUDecode;
}

int Plugin_COpenGL::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{
    pPluginVersion->guid                 = g_GUID_OPENGL;
    pPluginVersion->dwAPIVersionMajor    = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor    = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_COpenGL::TC_Init(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback)
{
    m_pGPUDecode = (TextureControl*)new GPU_OpenGL(Width, Height, callback);
    if (m_pGPUDecode == NULL)
        return -1;
    return 0;
}

CMP_ERROR Plugin_COpenGL::TC_Decompress(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture)
{
    CMP_ERROR result = CMP_OK;
    if (m_pGPUDecode)
        result = m_pGPUDecode->Decompress(pSourceTexture, pDestTexture);
    return result;
}

int Plugin_COpenGL::TC_Close()
{
    if (m_pGPUDecode)
    {
        delete m_pGPUDecode;
        m_pGPUDecode = NULL;
    }
    return 0;
}
