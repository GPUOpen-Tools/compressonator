//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
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

#ifndef _PLUGIN_IMAGE_H
#define _PLUGIN_IMAGE_H

#include "PluginInterface.h"

// {3AF62198-7326-48FA-B1FB-1D12A355694D}
#ifdef _Win32
static const GUID g_GUID = { 0x3af62198, 0x7326, 0x48fa, { 0xb1, 0xfb, 0x1d, 0x12, 0xa3, 0x55, 0x69, 0x4d } };
#else
static const GUID g_GUID = {0};
#endif

#define TC_PLUGIN_VERSION_MAJOR	1
#define TC_PLUGIN_VERSION_MINOR	0


class Plugin_BoxFilter : public PluginInterface_Filters
{
	public: 
		Plugin_BoxFilter();
		virtual ~Plugin_BoxFilter();

		int TC_PluginSetSharedIO(void* Shared);
		int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
		int TC_GenerateMIPLevels(MipSet *pMipSet, int nMinSize);
};


void GenerateMipLevel(MipLevel* pCurMipLevel, MipLevel* pPrevMipLevelOne, MipLevel* pPrevMipLevelTwo = NULL);
template <typename T> void GenerateMipLevelF(MipLevel* pCurMipLevel, MipLevel* pPrevMipLevelOne, MipLevel* pPrevMipLevelTwo = NULL, T* curMipData = NULL, T* prevMip1Data = NULL, T* prevMip2Data = NULL);

extern void *make_Plugin_BoxFIlter();

#endif
