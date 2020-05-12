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

#pragma once

#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers
#include <assert.h>
#include <tchar.h>
#include "Compressonator.h"
#include "Common.h"
#include "Compute_Base.h"
#include "Compute_DirectX.h"
#include "PluginInterface.h"


// {21DE462B-D6C5-4F2A-99A6-2514F087946E}
static const GUID g_GUID_DIREXTX = { 0x21de462b, 0xd6c5, 0x4f2a,{ 0x99, 0xa6, 0x25, 0x14, 0xf0, 0x87, 0x94, 0x6e } };


#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

using namespace CMP_Compute_Base;

class Plugin_CDirectX : public PluginInterface_Pipeline
{
public: 
        Plugin_CDirectX();
        virtual ~Plugin_CDirectX();
        int             TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
        int             TC_Init(void  *kernel_options);
        int             TC_PluginSetSharedIO(void* Shared);
        CMP_ERROR       TC_Compress(void  *kernel_options, MipSet  &SrcTexture, MipSet  &destTexture, CMP_Feedback_Proc pFeedback);
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