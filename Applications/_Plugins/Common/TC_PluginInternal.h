//////////////////////////////////////////////////////////////////////////////
//
//  File Name:   TC_PluginInternal.h
//  Description: Texture Plugin API Internal definitions
//
//  Copyright (c) 2007,2008 Advanced Micro Devices, Inc.
//  Copyright (c) 2002-2006 ATI Technologies Inc.
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
//
//  Version:    1.4
//
//////////////////////////////////////////////////////////////////////////////

/// \file
/// TC_PluginInternal.h declares the internal workings of the Plugin API.

#if !defined(_AMD_TA_INTERNAL_H_INCLUDED_)
#define _AMD_TA_INTERNAL_H_INCLUDED_

// Windows Header Files:
#ifdef _WIN32
#include <windows.h>
#endif

#include "Compressonator.h"
#include "Common.h"
#include "TC_PluginAPI.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #define TC_API_VERSION_MAJOR 1
// #define TC_API_VERSION_MINOR 4

#ifndef assert
#define assert ASSERT
#endif

#ifndef ASSERT
#define ASSERT assert
#endif

#ifdef __cplusplus
extern "C" {
#endif

void Error(TCHAR* pszCaption, TC_ErrorLevel errorLevel, unsigned int nErrorString, ...);
void DebugString(TCHAR* pszString, ...);
#ifdef _WIN32
HINSTANCE GetInstance();
typedef UINT_PTR (CALLBACK *LPTCHOOKPROC) (HWND, UINT, WPARAM, LPARAM);
// int MaxFacesOrSlices(const MipSet* pMipSet, int nMipLevel);    //Returns maximum number of faces or slices present at a mip level, 0 if error
#else
typedef CMP_DWORD LPTCHOOKPROC;
typedef unsigned int UINT;
#endif

int GetCheckedRadioButton(HWND hwndDlg, int nIDFirstButton, int nIDLastButton);
int FaceIndex(const MipSet* pMipSet, MS_CubeFace face);    //Returns what nFaceOrSlice you should ask for from TC_AppGetMipLevel, negative return means error
CMP_DWORD MakeFourCC(const TCHAR* pszFourCC);

typedef CMP_DWORD_PTR HPLUGIN;
#define PLUGIN_INTERNAL ((HPLUGIN) -1)


#define PAD_BYTE(width, bpp) (((bpp) * (width) + 7) / 8)
#define PAD_WORD(width, bpp) ((((bpp) * (width) + 15) / 16) * 2)
#define PAD_WORD_SIZE(size) ((size + 1) & (~0x1))
#define PAD_DWORD(width, bpp) ((((bpp) * (width) + 31) / 32) * 4)
#define PAD_DWORD_SIZE(size) ((size + 3) & (~0x3))

/// Function pointer for File Save dialog procedures.
/// This function is a message handler for your plugin's file save options dialog, and can have any name. You should find the function in MyCodec.cpp and copy it into your plugin, changing the class/structure names as appropriate. There is no need for you to rewrite the function.
/// \param[in] hwndDlg Handle to the host window to subclass for your dialog..
/// \param[in] uMsg One of the following, indicating why this function was called: WM_PLUGIN_INIT_DLG, WM_PLUGIN_END_DLG, WM_PLUGIN_SET_PARAMS, WM_PLUGIN_GET_PARAMS.
/// \param[in] wParam In the case of END_DLG, SET_PARAMS, and GET_PARAMS, the identifier to your dialog returned when this function was called with INIT_DLG earlier. Not used in INIT_DLG. See Remarks for more details.
/// \param[in] lParam In the case of SET_PARAMS and GET_PARAMS, a pointer to the TC_FileSaveParams structure that you returned through a previous call to TC_PluginFileGetFileSaveParametersDialog. If SET_PARAMS you will be transferring information from the structure into your dialog, if GET_PARAMS, vice versa.
/// \return In the case of INIT_DLG, an identifier to your dialog window. This can be a HWND, a pointer to a CDialog or any other identifier you choose, this value is passed into CompressDialogProc to identify your dialog. Return FALSE in all other cases.
/// \remarks See the MyCodec template plugin for an example of the CDialog subclass.
/// \remarks The Compressonator will call whatever function's address is passed out of TC_PluginFileGetFileSaveParametersDialog using ppfnDialogProc. The first line of the function must be AFX_MANAGE_STATE(AfxGetStaticModuleState());
/// \remarks INIT_DLG is where you subclass the host window passed to you and return an identifier to your dialog.
/// \remarks END_DLG is for un-subclassing the window subclassed in INIT_DLG and releasing any resources or memory allocated in INIT_DLG.
/// \remarks SET_PARAMS is for you to update your dialog using the TC_FileSaveParams pointer passed in lParam. Afterwards return FALSE.
/// \remarks GET_PARAMS is for setting information in the TC_FileSaveParams structure whose pointer was passed in lParam from your dialog as identified in wParam. Afterwards return FALSE.
/// \sa FilePlugins, TC_PluginFileGetFileSaveParametersDialog()
typedef LPTCHOOKPROC FileSaveDialogProc;

/// Function pointer for Compress dialog procedures.
/// This function is a message handler for your plugin's file save options dialog, and can have any name. You should find the function in MyCodec.cpp and copy it into your plugin, changing the class/structure names as appropriate. There is no need for you to rewrite the function.
/// \param[in] hwndDlg Handle to the host window to subclass for your dialog..
/// \param[in] uMsg One of the following, indicating why this function was called: WM_PLUGIN_INIT_DLG, WM_PLUGIN_END_DLG, WM_PLUGIN_SET_PARAMS, WM_PLUGIN_GET_PARAMS.
/// \param[in] wParam In the case of END_DLG, SET_PARAMS, and GET_PARAMS, the identifier to your dialog returned when this function was called with INIT_DLG earlier. Not used in INIT_DLG. See Remarks for more details.
/// \param[in] lParam In the case of SET_PARAMS and GET_PARAMS, a pointer to the TC_CompressParams structure that you returned through a previous call to TC_PluginCodecGetCompressParametersDialog. If SET_PARAMS you will be transferring information from the structure into your dialog, if GET_PARAMS, vice versa.
/// \return In the case of INIT_DLG, an identifier to your dialog window. This can be a HWND, a pointer to a CDialog or any other identifier you choose, this value is passed into CompressDialogProc to identify your dialog. Return FALSE in all other cases.
/// \remarks See the MyCodec template plugin for an example of the CDialog subclass.
/// \remarks The Compressonator will call whatever function's address is passed out of TC_PluginFileGetCompressParametersDialog using ppfnDialogProc. The first line of the function must be AFX_MANAGE_STATE(AfxGetStaticModuleState());
/// \remarks INIT_DLG is where you subclass the host window passed to you and return an identifier to your dialog.
/// \remarks END_DLG is for un-subclassing the window subclassed in INIT_DLG and releasing any resources or memory allocated in INIT_DLG.
/// \remarks SET_PARAMS is for you to update your dialog using the TC_CompressParams pointer passed in lParam. Afterwards return FALSE.
/// \remarks GET_PARAMS is for setting information in the TC_CompressParams structure whose pointer was passed in lParam from your dialog as identified in wParam. Afterwards return FALSE.
/// \sa CodecPlugins, TC_PluginFileGetCompressParametersDialog()
typedef LPTCHOOKPROC CompressDialogProc;

/// Function pointer for Mip Generator dialog procedures.
/// This function is a message handler for your plugin's mip generation options dialog, and can have any name. You should find the function in MyCodec.cpp and copy it into your plugin, changing the class/structure names as appropriate. There is no need for you to rewrite the function.
/// \param[in] hwndDlg Handle to the host window to subclass for your dialog..
/// \param[in] uMsg One of the following, indicating why this function was called: WM_PLUGIN_INIT_DLG, WM_PLUGIN_END_DLG, WM_PLUGIN_SET_PARAMS, WM_PLUGIN_GET_PARAMS.
/// \param[in] wParam In the case of END_DLG, SET_PARAMS, and GET_PARAMS, the identifier to your dialog returned when this function was called with INIT_DLG earlier. Not used in INIT_DLG. See Remarks for more details.
/// \param[in] lParam In the case of SET_PARAMS and GET_PARAMS, a pointer to the TC_CompressParams structure that you returned through a previous call to TC_PluginCodecGetCompressParametersDialog. If SET_PARAMS you will be transferring information from the structure into your dialog, if GET_PARAMS, vice versa.
/// \return In the case of INIT_DLG, an identifier to your dialog window. This can be a HWND, a pointer to a CDialog or any other identifier you choose, this value is passed into CompressDialogProc to identify your dialog. Return FALSE in all other cases.
/// \remarks See the MyCodec template plugin for an example of the CDialog subclass.
/// \remarks The Compressonator will call whatever function's address is passed out of TC_PluginCodecGetCompressParametersDialog using ppfnDialogProc. The first line of the function must be AFX_MANAGE_STATE(AfxGetStaticModuleState());
/// \remarks INIT_DLG is where you subclass the host window passed to you and return an identifier to your dialog.
/// \remarks END_DLG is for un-subclassing the window subclassed in INIT_DLG and releasing any resources or memory allocated in INIT_DLG.
/// \remarks SET_PARAMS is for you to update your dialog using the TC_CompressParams pointer passed in lParam. Afterwards return FALSE.
/// \remarks GET_PARAMS is for setting information in the TC_CompressParams structure whose pointer was passed in lParam from your dialog as identified in wParam. Afterwards return FALSE.
/// \sa MipperPlugins, TC_PluginCodecGetCompressParametersDialog()
typedef LPTCHOOKPROC GenerateMipsDialogProc;

//typedef struct _TC_PluginVersion
//{
//   GUID  guid;
//   DWORD dwAPIVersionMajor;    // App will not load plugin with greater API major version than app
//   DWORD dwAPIVersionMinor;
//   DWORD dwPluginVersionMajor;
//   DWORD dwPluginVersionMinor;
//} TC_PluginVersion;

typedef CMP_DWORD (* TC_GetAppAPIVersionPROC) (HPLUGIN hPlugin);

typedef TC_ErrorResponse ( * TC_AppHandleErrorPROC) (HPLUGIN hPlugin, TC_ErrorLevel errorLevel, const TCHAR* pszCaption, const TCHAR* pszText);
typedef void (* TC_AppDebugStringPROC) (HPLUGIN hPlugin, const TCHAR* pszString);

typedef HFILETYPE (* TC_AppRegisterFileTypePROC) (HPLUGIN hPlugin, TCHAR* pszFileTypeDescription, TCHAR* pszFileTypeExtensions[], bool bSaveFile);
typedef void ( * TC_AppUnregisterFileTypePROC) (HPLUGIN hPlugin, HFILETYPE hFileType);

typedef HCODEC ( * TC_AppRegisterCodecPROC) (HPLUGIN hPlugin, TCHAR* pszCodecDescription, CMP_DWORD dwFourCCs[], bool bDecompressorOnly);
typedef void ( * TC_AppUnregisterCodecPROC) (HPLUGIN hPlugin, HCODEC hCodec);

typedef HMIPPER ( * TC_AppRegisterMipGeneratorPROC) (HPLUGIN hPlugin, TCHAR* pszMipGeneratorDescription);
typedef void ( * TC_AppUnregisterMipGeneratorPROC) (HPLUGIN hPlugin, HMIPPER hMipper);

typedef HVIEWTYPE ( * TC_AppRegisterViewTypePROC) (HPLUGIN hPlugin, TCHAR* pszViewDescription, bool bDefaultView);
typedef void ( * TC_AppUnregisterViewTypePROC) (HPLUGIN hPlugin, HVIEWTYPE hViewType);

typedef MipLevel* ( * TC_AppGetMipLevelPROC) (HPLUGIN hPlugin, const MipSet* pMipSet, int nMipLevel, int nFaceOrSlice);

//Memory (de)allocation
typedef bool ( * TC_AppAllocateMipSetPROC) (HPLUGIN hPlugin, MipSet* pMipSet, ChannelFormat channelFormat, TextureDataType textureDataType, TextureType textureType, int nWidth, int nHeight, int nDepth);
typedef bool ( * TC_AppAllocateMipLevelDataPROC) (HPLUGIN hPlugin, MipLevel* pMipLevel, int nWidth, int nHeight, ChannelFormat channelFormat, TextureDataType textureDataType);
typedef bool ( * TC_AppAllocateCompressedMipLevelDataPROC) (HPLUGIN hPlugin, MipLevel* pMipLevel, int nWidth, int nHeight, CMP_DWORD dwSize);
typedef void ( * TC_AppFreeMipLevelDataPROC) (HPLUGIN hPlugin, MipLevel* pMipLevel);

typedef bool ( * TC_AppCompressTextureCallbackPROC) (HPLUGIN hPlugin, float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2);

typedef void ( * TC_AppUpdateViewParentPROC) (HPLUGIN hPlugin, HWND hParent, UINT nWidth, UINT nHeight);
typedef void ( * TC_AppEnableCommandPROC) (HPLUGIN hPlugin, HWND hParent, UINT nCommand, bool bEnable);
typedef void (* TC_AppCheckCommandPROC) (HPLUGIN hPlugin, HWND hParent, UINT nCommand, bool bCheck);

typedef struct _TC_AppPointers
{
   TC_GetAppAPIVersionPROC                    pAppGetAppAPIVersionMajor;
   TC_GetAppAPIVersionPROC                    pAppGetAppAPIVersionMinor;
   TC_AppHandleErrorPROC                    pAppHandleError;
   TC_AppRegisterFileTypePROC                pAppRegisterFileType;
   TC_AppUnregisterFileTypePROC                pAppUnregisterFileType;
   TC_AppRegisterCodecPROC                    pAppRegisterCodec;
   TC_AppUnregisterCodecPROC                pAppUnregisterCodec;
   TC_AppRegisterMipGeneratorPROC            pAppRegisterMipGenerator;
   TC_AppUnregisterMipGeneratorPROC            pAppUnregisterMipGenerator;
   TC_AppRegisterViewTypePROC                pAppRegisterViewType;
   TC_AppUnregisterViewTypePROC                pAppUnregisterViewType;
   TC_AppGetMipLevelPROC                    pAppGetMipLevel;
   //Memory (de)allocation
   TC_AppAllocateMipSetPROC                    pAppAllocateMipSet;
   TC_AppAllocateMipLevelDataPROC            pAppAllocateMipLevelData;
   TC_AppAllocateCompressedMipLevelDataPROC    pAppAllocateCompressedMipLevelData;
   TC_AppFreeMipLevelDataPROC                pAppFreeMipLevelData;
   TC_AppCompressTextureCallbackPROC        pAppCompressTextureCallback;
   TC_AppUpdateViewParentPROC                pAppUpdateViewParent;
   TC_AppEnableCommandPROC                    pAppEnableCommand;
   TC_AppCheckCommandPROC                    pAppCheckCommand;
   TC_AppDebugStringPROC                    pAppDebugString;
} TC_AppPointers;

TC_PluginError TC_PluginInitialise(const TC_AppPointers* pAppPointers, HPLUGIN hThis);

typedef TC_PluginError ( * TC_PluginInitialisePROC) (const TC_AppPointers* pAppPointers, HPLUGIN hThis);
typedef TC_PluginError (* TC_PluginGetVersionPROC) (TC_PluginVersion* pPluginVersion);
//typedef TC_PluginError ( * TC_PluginLoadPROC) ();
typedef TC_PluginError ( * TC_PluginUnloadPROC) ();

typedef const TCHAR* (* TC_PluginGetAboutInfoPROC) ();
typedef const TCHAR* (* TC_PluginGetHelpInfoPROC) ();

typedef TC_PluginError ( * TC_PluginGetDWORDPropertyPROC) (const TCHAR* pszProperty, const MipSet* pMipSet, CMP_DWORD* pdwValue);
typedef TC_PluginError (* TC_PluginGetFloatPropertyPROC) (const TCHAR* pszFilename, const MipSet* pMipSet, float* pfValue);

typedef TC_PluginError ( * TC_PluginFileLoadTexturePROC) (const HFILETYPE hFileType, const TCHAR* pszFilename, MipSet* pMipSet);
typedef TC_PluginError ( * TC_PluginFileSaveTexturePROC) (const HFILETYPE hFileType, const TCHAR* pszFilename, const MipSet* pMipSet, const TC_FileSaveParams* pFileSaveParams);
typedef bool ( * TC_PluginFileSupportsFormatPROC) (const HFILETYPE hFileType, const MipSet* pMipSet);
typedef bool ( * TC_PluginFileSupportsMipLevelsPROC) (const HFILETYPE hFileType, const int nMipLevels);
typedef bool ( * TC_PluginFileGetFileSaveParametersDialogPROC) (const HFILETYPE hFileType, LPTSTR* ppTemplate, FileSaveDialogProc* ppfnDialogProc, TC_FileSaveParams* pFileSaveParams);

typedef TC_PluginError ( * TC_PluginCodecDecompressTexturePROC) (const HCODEC hCodec, const MipSet* pMipSetIn, MipSet* pMipSetOut);
typedef TC_PluginError ( * TC_PluginCodecCompressTexturePROC) (const HCODEC hCodec, const MipSet* pMipSetIn, MipSet* pMipSetOut, TC_CompressParams* pCompressParams);
typedef bool ( * TC_PluginCodecSupportsFormatPROC) (const HCODEC hCodec, const MipSet* pMipSet);
typedef bool ( * TC_PluginCodecGetCompressParametersDialogPROC) (const HCODEC hCodec, LPTSTR* ppTemplate, CompressDialogProc* ppfnDialogProc, TC_CompressParams* pCompressParams);

typedef TC_PluginError (* TC_PluginMipGenerateMipLevelsPROC) (const HMIPPER hMipper, MipSet* pMipSet, TC_GenerateMipsParams* pGenerateMipsParams);
typedef bool (* TC_PluginMipGetGenerateMipLevelsParametersDialogPROC) (const HMIPPER hMipper, LPTSTR* ppTemplate, GenerateMipsDialogProc* ppfnDialogProc, TC_GenerateMipsParams* pGenerateMipsParams);
typedef bool ( * TC_PluginMipSupportsFormatPROC) (const HMIPPER hMipper, const MipSet* pMipSet);

typedef TC_PluginError ( * TC_PluginViewCreateViewPROC) (const HVIEWTYPE hViewType, HWND hParent, HWND* phWnd, HVIEW* phView, TC_ViewMode viewMode);
typedef TC_PluginError (* TC_PluginViewUpdateViewPROC) (const HVIEW hView, const MipSet* pMipSetSourceRGBA, const MipSet* pMipSetCompressedRGBA, const MipSet* pMipSetCompressedData, const MipSet* pMipSetDiff);
typedef TC_PluginError (* TC_PluginViewUpdateViewTitlesPROC) (const HVIEW hView, const TCHAR* pszTitle, const TCHAR* pszSourceTitle, const TCHAR* pszCompressedTitle, const TCHAR* pszDiffTitle);
typedef TC_PluginError ( * TC_PluginViewReleaseViewPROC) (HVIEW hView);

// Legacy plugin support - DO NOT USE
typedef TC_PluginError ( * TC_PluginFileLoadTextureV1_0PROC) (const TCHAR* pszFilename, MipSet* pMipSet);
typedef TC_PluginError ( * TC_PluginFileSaveTextureV1_0PROC) (const TCHAR* pszFilename, const MipSet* pMipSet, const TC_FileSaveParams* pFileSaveParams);
typedef bool (* TC_PluginFileSupportsFormatV1_0PROC) (const MipSet* pMipSet);
typedef bool ( * TC_PluginFileSupportsMipLevelsV1_0PROC) (const int nMipLevels);

typedef TC_PluginError (* TC_PluginCodecDecompressTextureV1_0PROC) (const MipSet* pMipSetIn, MipSet* pMipSetOut);
typedef bool ( * TC_PluginCodecSupportsFormatV1_0PROC) (const MipSet* pMipSet);

typedef bool ( * TC_PluginMipSupportsFormatV1_0PROC) (const MipSet* pMipSet);

#ifdef __cplusplus
};
#endif

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(_AMD_TEXTURE_API_PLUGININTERNAL_H_INCLUDED_)

