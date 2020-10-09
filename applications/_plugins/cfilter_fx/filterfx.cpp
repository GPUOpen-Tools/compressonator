//=============================================================================
// Copyright (c) 2020  Advanced Micro Devices, Inc. All rights reserved.
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
//==============================================================================
#include "filterfx.h"

#define BUILD_AS_PLUGIN_DLL

// Windows Header Files:
#ifdef _WIN32
#include <windows.h>
#endif

#include <tc_pluginapi.h>
#include <tc_plugininternal.h>
#include <compressonator.h>
#include <texture.h>

#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
#include "textureio.h"
#endif
#include <algorithm>

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_CFilterFx)
SET_PLUGIN_TYPE("FILTERS")
SET_PLUGIN_NAME("EFFECTS")
#else
void* make_Plugin_CFilterFx()
{
    return new Plugin_CFilterFx;
}
#endif

CMIPS       *CFilterMipsFx = NULL;

Plugin_CFilterFx::Plugin_CFilterFx()
{
}

Plugin_CFilterFx::~Plugin_CFilterFx()
{
    CFilterMipsFx = NULL;
}

// Not used return error!
int Plugin_CFilterFx::TC_PluginSetSharedIO(void* SharedCMips)
{
    // check if already initialized
    if (CFilterMipsFx != NULL)
        return CMP_OK;

    if (SharedCMips && CFilterMipsFx == NULL)
    {
        CFilterMipsFx = reinterpret_cast<CMIPS*>(SharedCMips);
        return CMP_OK;
    } else
        CFilterMipsFx = NULL;

    return CMP_ERR_GENERIC;
}

int Plugin_CFilterFx::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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

//nMinSize : The size in pixels used to determine how many mip levels to generate. Once all dimensions are less than or equal to nMinSize your mipper should generate no more mip levels.
int Plugin_CFilterFx::TC_CFilter(MipSet* pMipSetSrc, CMP_MipSet* pMipSetDst, CMP_CFilterParams* pCFilterParams)
{
    (pMipSetDst);
    assert(pMipSetSrc);
    assert(pMipSetSrc->m_nMipLevels);
    int result = CMP_OK;
    if (!CFilterMipsFx)
        return CMP_ERR_PLUGIN_SHAREDIO_NOT_SET;
     return result;
}

//--------------------------------------------------------------------------------------------
// DirectX Filter
//--------------------------------------------------------------------------------------------
void Plugin_CFilterFx::Error(TCHAR* pszCaption, TC_ErrorLevel errorLevel, UINT nErrorString)
{
    // Add code to print message to caller
}

