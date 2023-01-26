//=====================================================================
// Copyright (c) 2018    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file BC7.h
//
//=====================================================================

#ifndef _PLUGIN_COMPUTE_BC7_H
#define _PLUGIN_COMPUTE_BC7_H

#include "common_def.h"
#include "plugininterface.h"
#include "cmp_plugininterface.h"
#include "bc7_encode_kernel.h"
#include "hpc_compress.h"           // padline defs
#include "cmp_math_common.h"

#ifdef _WIN32
// {30B6FB31-F824-4CBD-8C03-3942F7EDDCBF}
static const GUID g_GUID = { 0x30b6fb31, 0xf824, 0x4cbd, { 0x8c, 0x3, 0x39, 0x42, 0xf7, 0xed, 0xdc, 0xbf } };
#else
static const GUID g_GUID = 0;
#endif

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

#ifdef _WIN32
#ifdef USE_ASPM_CODE
//========== SPMD Settings ============
#include "bc7_encode_kernel_spmd.h"
#endif
#endif

struct TIMERDATA {
    double start;
    double end;
    int   sequence;
    int   calls;
};

//========================================

class Plugin_BC7 : public PluginInterface_Encoder {
  public:
    Plugin_BC7();
    virtual ~Plugin_BC7();
    int     TC_PluginSetSharedIO(void* Shared);
    int     TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    void*   TC_Create();
    void    TC_Destroy(void* codec);
    int     TC_Init(void *kernel_options);
    char *  TC_ComputeSourceFile(unsigned int     Compute_type);
    void TC_Start();
    void TC_End();
  private:
    KernelOptions   *m_KernelOptions;
    CMIPS           *CMips             = nullptr;
};


extern void *make_Plugin_BC7();



class BC7_EncodeClass : public CMP_Encoder {
  public:
    BC7_EncodeClass();

    int CompressBlock(unsigned int xBlock, unsigned int yBlock, void *in, void *out);
    int CompressBlock(void *in, void *out, void *blockoptions);
    int DecompressBlock(unsigned int xBlock, unsigned int yBlock, void *in, void *out);
    int DecompressBlock(void *in, void *out);
    int CompressTexture(void *in, void *out,void *processOptions);
    int DecompressTexture(void *in, void *out,void *processOptions);
};

//============================= BC7 Codec Host Code ====================================


#include <stdio.h>      /* printf */
#include <math.h>       /* floor */
#include <algorithm>    // std::max

extern int expandbits(CGU_INT bits, CGU_INT v);

#endif