//=============================================================================
// Copyright (c) 2021-2024  Advanced Micro Devices, Inc. All rights reserved.
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
#include "prototype.h"

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
DECLARE_PLUGIN(Plugin_Prototype)
SET_PLUGIN_TYPE("PLUGIN")
SET_PLUGIN_NAME("PROTOTYPE")
#else
void* make_Plugin_Prototype()
{
    return new Plugin_Prototype;
}
#endif

CMIPS* CMipsPrototype = NULL;

Plugin_Prototype::Plugin_Prototype()
{
}

Plugin_Prototype::~Plugin_Prototype()
{
    CMipsPrototype = NULL;
}

// Not used return error!
int Plugin_Prototype::TC_PluginSetSharedIO(void* SharedCMips)
{
    // check if already initialized
    if (CMipsPrototype != NULL)
        return CMP_OK;

    if (SharedCMips && CMipsPrototype == NULL)
    {
        CMipsPrototype = reinterpret_cast<CMIPS*>(SharedCMips);
        return CMP_OK;
    }
    else
        CMipsPrototype = NULL;

    return CMP_ERR_GENERIC;
}

int Plugin_Prototype::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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
