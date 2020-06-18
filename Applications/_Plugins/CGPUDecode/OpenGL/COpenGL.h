//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef _Plugin_COPENGL_H
#define _Plugin_COPENGL_H

#pragma once

#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers
#include <assert.h>
#include <tchar.h>
#include "GPU_OpenGL.h"
#include "PluginInterface.h"

// {D88C7EB3-38D3-4B75-BE14-22ED445156FE}
static const GUID  g_GUID_OPENGL = { 0xd88c7eb3, 0x38d3, 0x4b75,{ 0xbe, 0x14, 0x22, 0xed, 0x44, 0x51, 0x56, 0xfe } };


#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

using namespace GPU_Decode;

class Plugin_COpenGL : public PluginInterface_GPUDecode
{
public: 
    Plugin_COpenGL();
        virtual ~Plugin_COpenGL();
        int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
        int TC_Init(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback);
        CMP_ERROR TC_Decompress(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture);
        int TC_Close();
private:
        TextureControl  *m_pGPUDecode;
};

#endif