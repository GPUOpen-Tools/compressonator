//=====================================================================
// Copyright (c) 2016    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file BC3.h
//
//=====================================================================

#ifndef _PLUGIN_COMPUTE_BC3_H
#define _PLUGIN_COMPUTE_BC3_H

#include "common_def.h"
#include "plugininterface.h"
#include "cmp_plugininterface.h"
#include "bc3_encode_kernel.h"
#include "hpc_compress.h"           // padline defs
#include "cmp_math_common.h"

#ifdef _WIN32
// {98EEA256-F2E6-465D-92A0-533537FEF266}
static const GUID g_GUID = { 0x98eea256, 0xf2e6, 0x465d, { 0x92, 0xa0, 0x53, 0x35, 0x37, 0xfe, 0xf2, 0x66 } };
#else
static const GUID g_GUID = 0;
#endif

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

class Plugin_BC3 : public PluginInterface_Encoder {
  public:
    Plugin_BC3();
    virtual ~Plugin_BC3();
    int     TC_PluginSetSharedIO(void* Shared);
    int     TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    void*   TC_Create();
    void    TC_Destroy(void* codec);
    int     TC_Init(void   *kernel_options);
    char *  TC_ComputeSourceFile(CGU_UINT32     Compute_type);
    void TC_Start();
    void TC_End();
  private:
    KernelOptions *m_KernelOptions;
    CMIPS *CMips = nullptr;
};

class BC3_EncodeClass : public CMP_Encoder {
    int CompressBlock(CGU_UINT32 xBlock, CGU_UINT32 yBlock, void *in, void *out);
    int CompressBlock(void *in, void *out, void *blockoptions);
    int DecompressBlock(CGU_UINT32 xBlock, CGU_UINT32 yBlock, void *in, void *out);
    int DecompressBlock(void *in, void *out);
    int CompressTexture(void *in, void *out,void *processOptions);
    int DecompressTexture(void *in, void *out,void *processOptions);
};

extern void *make_Plugin_BC3();


#endif