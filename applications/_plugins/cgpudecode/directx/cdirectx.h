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

#ifndef _Plugin_CDIRECTX_H
#define _Plugin_CDIRECTX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "gpu_directx.h"
#include "cmp_plugininterface.h"

#include <assert.h>
#include <tchar.h>

// {91551830-C6D3-49A3-B75D-116A0AD5901B}
static const GUID  g_GUID_DIRECTX = { 0x91551830, 0xc6d3, 0x49a3,{ 0xb7, 0x5d, 0x11, 0x6a, 0xa, 0xd5, 0x90, 0x1b } };


#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

// Forward Declaration
namespace GPU_Decode {
class TextureControl;
}

class Plugin_CDirectX : public PluginInterface_GPUDecode {
  public:
    Plugin_CDirectX();
    virtual ~Plugin_CDirectX();
    int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int TC_Init(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback);
    CMP_ERROR TC_Decompress(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture);
    int TC_Close();
  private:
    GPU_Decode::TextureControl  *m_pGPUDecode;
};

#endif