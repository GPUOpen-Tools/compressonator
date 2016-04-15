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

#ifndef _PLUGIN_IMAGE_EXR_H
#define _PLUGIN_IMAGE_EXR_H

#include "PluginInterface.h"

#define IDS_STRING1                     1
#define IDS_ERROR_FILE_OPEN             1
#define IDS_ERROR_REGISTER_FILETYPE     2
#define IDS_ERROR_NOT_EXR           	3
#define IDS_ERROR_UNSUPPORTED_TYPE      4

//#define EXR_SAVE_32F 
#define TILE_WIDTH	64
#define TILE_HEIGHT 64

// {2438C059-CD7B-49AD-A626-FBAE37A059F5}
static const GUID g_GUID = { 0x2438c059, 0xcd7b, 0x49ad, { 0xa6, 0x26, 0xfb, 0xae, 0x37, 0xa0, 0x59, 0xf5 } };


#define TC_PLUGIN_VERSION_MAJOR	1
#define TC_PLUGIN_VERSION_MINOR	0

static const USHORT BMP_HEADER = ((USHORT)(BYTE)('B') | ((USHORT)(BYTE)('M') << 8));

class Plugin_EXR : public PluginInterface_Image
{
	public: 
		Plugin_EXR();
		virtual ~Plugin_EXR();

		int TC_PluginSetSharedIO(void* Shared);
		int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
		int TC_PluginFileLoadTexture(const TCHAR* pszFilename, MipSet* pMipSet);
		int TC_PluginFileSaveTexture(const TCHAR* pszFilename, MipSet* pMipSet);
		int TC_PluginFileLoadTexture(const TCHAR* pszFilename, CMP_Texture *srcTexture);
		int TC_PluginFileSaveTexture(const TCHAR* pszFilename, CMP_Texture *srcTexture);

};

extern void *make_Plugin_EXR();

#endif