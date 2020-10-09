//=====================================================================
// Copyright (c) 2020    Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================

#ifndef _Plugin_COPENCL_H
#define _Plugin_COPENCL_H

#pragma once

#include <assert.h>
#include <tchar.h>
#include "compressonator.h"
#include "common.h"
#include "compute_base.h"
#include "Compute_OpenCL.h"
#include "plugininterface.h"

// {D88C7EB3-38D3-4B75-BE14-22ED445156FE}
static const GUID  g_GUID_GPU = { 0xd88c7eb3, 0x38d3, 0x4b75,{ 0xbe, 0x14, 0x22, 0xed, 0x44, 0x51, 0x56, 0xfe } };


#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

using namespace CMP_Compute_Base;

class Plugin_COpenCL : public PluginInterface_Pipeline {
  public:
    Plugin_COpenCL();
    virtual ~Plugin_COpenCL();
    int             TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
    int             TC_Init(void  *kernel_options);
    int             TC_PluginSetSharedIO(void* Shared);
    CMP_ERROR       TC_Compress(void  *kernel_options, MipSet  &SrcTexture, MipSet  &destTexture,CMP_Feedback_Proc pFeedback);
    void            TC_SetComputeOptions(void *options);
    char            *TC_ComputeSourceFile();
    CMP_ERROR       TC_GetPerformanceStats(void* pPerfStats);
    CMP_ERROR       TC_GetDeviceInfo(void* pDeviceInfo);
    int             TC_Close();


  private:
    ComputeBase  *m_pComputeBase;

#ifdef ENABLE_MAKE_COMPATIBLE_API
    bool IsFloatFormat(CMP_FORMAT InFormat);
    float findKneeValueHPC(float x, float y);
    CMP_ERROR CF_16BitTo8Bit(CMP_WORD* sBlock, CMP_BYTE* cBlock, CMP_DWORD dwBlockSize);
    CMP_ERROR Byte2HalfShort(CMP_HALFSHORT* hfsBlock, CMP_BYTE* cBlock, CMP_DWORD dwBlockSize);
    CMP_ERROR Float2Byte(CMP_BYTE cBlock[], CMP_FLOAT* fBlock, MipSet  &srcTexture, const CMP_CompressOptions* pOptions);
#endif

};

#endif