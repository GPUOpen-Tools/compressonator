//=====================================================================
// Copyright (c) 2016-2022    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file codec_brlg.h
//
//=====================================================================

#ifndef _PLUGIN_COMPUTE_BRLG_H
#define _PLUGIN_COMPUTE_BRLG_H

#include "common_def.h"
#include "plugininterface.h"
#include "cmp_plugininterface.h"
#include "brlg_encode_kernel.h"
#include "hpc_compress.h"           // padline defs
#include "cmp_math_common.h"

#ifdef _WIN32
// {909FF6DC-0F23-4A3F-937C-C3EFAA9C07EE}
static const GUID g_GUID = {0x909ff6dc, 0xf23, 0x4a3f, {0x93, 0x7c, 0xc3, 0xef, 0xaa, 0x9c, 0x7, 0xee}};

#else
static const GUID g_GUID = 0;
#endif

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

class Codec_Plugin_BRLG : public PluginInterface_Encoder
{
public: 
        Codec_Plugin_BRLG();
        virtual ~Codec_Plugin_BRLG();
        int     TC_PluginSetSharedIO(void* Shared);
        int     TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
        void*   TC_Create();
        void    TC_Destroy(void* codec);
        int     TC_Init(void   *kernel_options);
        char *  TC_ComputeSourceFile(CGU_UINT32     Compute_type);
        void TC_Start();
        void TC_End();
private:
        KernelOptions   *m_KernelOptions;
        CMIPS *CMips = nullptr;
};


class BRLG_EncodeClass : public CMP_Encoder 
{
    CGU_INT CompressBlock(CGU_UINT32 xBlock, CGU_UINT32 yBlock, void *in, void *out);
    CGU_INT CompressBlock(void *in, void *out, void *blockoptions);
    CGU_INT DecompressBlock(CGU_UINT32 xBlock, CGU_UINT32 yBlock, void *in, void *out);
    CGU_INT DecompressBlock(void *in, void *out);
    CGU_INT CompressTexture(void *in, void *out,void *processOptions);
    CGU_INT DecompressTexture(void *in, void *out,void *processOptions);
};

extern void *make_Codec_Plugin_BRLG();

#endif