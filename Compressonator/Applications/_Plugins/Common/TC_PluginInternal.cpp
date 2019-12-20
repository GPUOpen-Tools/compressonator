//////////////////////////////////////////////////////////////////////////////
//  File Name:   TC_PluginInternal.cpp
//  Description: Texture Plugin API Internal implementation
//
//  Copyright (c) 2002-2006 ATI Technologies Inc.
//
//////////////////////////////////////////////////////////////////////////////
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


#include "TC_PluginAPI.h"
#include "Version.h"
#include <stdio.h>

#ifdef _WIN32
TC_AppPointers g_AppPointers;
HPLUGIN g_hThis = NULL;
HINSTANCE g_hInstance = NULL;
#endif

//int GetCheckedRadioButton(HWND hwndDlg, int nIDFirstButton, int nIDLastButton)
//{
////    for (int nID = nIDFirstButton; nID <= nIDLastButton; nID++)
////    {
////        if (IsDlgButtonChecked(hwndDlg, nID))
////            return nID; // id that matched
////    }
//    return 0; // invalid ID
//}


int FaceIndex(const MipSet* pMipSet, MS_CubeFace face)
{
    if(pMipSet->m_TextureType != TT_CubeMap)
        return face;

    int index=0;
    //if we're at the face they're asking for and we have it, return index
    //else error
    //if we need to move on, increment index if that face exists

    if(face == MS_CF_PositiveX)
        return (pMipSet->m_CubeFaceMask & MS_CF_PositiveX) ? index : -1;
    else
        index += (pMipSet->m_CubeFaceMask & MS_CF_PositiveX) ? 1 : 0;

    if(face == MS_CF_NegativeX)
        return (pMipSet->m_CubeFaceMask & MS_CF_NegativeX) ? index : -1;
    else
        index += (pMipSet->m_CubeFaceMask & MS_CF_NegativeX) ? 1 : 0;

    if(face == MS_CF_PositiveY)
        return (pMipSet->m_CubeFaceMask & MS_CF_PositiveY) ? index : -1;
    else
        index += (pMipSet->m_CubeFaceMask & MS_CF_PositiveY) ? 1 : 0;

    if(face == MS_CF_NegativeY)
        return (pMipSet->m_CubeFaceMask & MS_CF_NegativeY) ? index : -1;
    else
        index += (pMipSet->m_CubeFaceMask & MS_CF_NegativeY) ? 1 : 0;

    if(face == MS_CF_PositiveZ)
        return (pMipSet->m_CubeFaceMask & MS_CF_PositiveZ) ? index : -1;
    else
        index += (pMipSet->m_CubeFaceMask & MS_CF_PositiveZ) ? 1 : 0;

    if(face == MS_CF_NegativeZ)
        return (pMipSet->m_CubeFaceMask & MS_CF_NegativeZ) ? index : -1;
    else
        index += (pMipSet->m_CubeFaceMask & MS_CF_NegativeZ) ? 1 : 0;

    return -1;    //indicating error
}

CMP_DWORD MakeFourCC(const TCHAR* pszFourCC)
{
    CMP_DWORD dwFourCC = CMP_MAKEFOURCC(' ', ' ', ' ', ' ');
    if(pszFourCC)
    {
        char* pFourCC = (char*) &dwFourCC;

        int nIndex = 0;
        while(*pszFourCC && nIndex < 4)
            pFourCC[nIndex++] = (char) *pszFourCC++;
    }

    return dwFourCC;
}

#ifdef _AFXDLL
#ifdef _WIN32
HINSTANCE GetInstance()
{
    return AfxGetInstanceHandle();
}

#define MAX_FORMAT_LENGTH 160
#define MAX_ERROR_LENGTH 240


void Error(TCHAR* pszCaption, TC_ErrorLevel errorLevel, UINT nErrorString, ...)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CString strFormat, strErrorMessage;
    if(strFormat.LoadString(nErrorString))
   {
       va_list args;
       va_start(args, nErrorString);
       strErrorMessage.FormatV(strFormat, args);

       va_end(args);
   }
   else
      strErrorMessage = "Unknown Error";

    TC_AppHandleError(errorLevel, pszCaption, strErrorMessage);
}

void DebugString(TCHAR* pszString, ...)
{
    CString strFormat(pszString), strString;

    va_list args;
    va_start(args, pszString);
    strString.FormatV(strFormat, args);

    va_end(args);

    TC_AppDebugString(strString);
}

#else // !_AFXDLL

//BOOL APIENTRY DllMain(HANDLE hModule, DWORD /*ul_reason_for_call*/, LPVOID /*lpReserved*/)
//{
//    g_hInstance = static_cast<HINSTANCE>(hModule);
//    return TRUE;
//}

#define UNREFERENCED_PARAMETER(P)          (P)

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    UNREFERENCED_PARAMETER(lpReserved);

    g_hInstance = static_cast<HINSTANCE>(hModule);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


HINSTANCE GetInstance()
{
    return g_hInstance;
}

#define MAX_FORMAT_LENGTH 160
#define MAX_ERROR_LENGTH 240

void Error(TCHAR* pszCaption, TC_ErrorLevel errorLevel, UINT nErrorString, ...)
{
    TCHAR szFormat[MAX_FORMAT_LENGTH];
    TCHAR szBuffer[MAX_ERROR_LENGTH];

    LoadString(GetInstance(), nErrorString, szFormat, MAX_FORMAT_LENGTH);

    va_list args;
    va_start(args, nErrorString);

    _vsntprintf_s(szBuffer, MAX_ERROR_LENGTH, _TRUNCATE, szFormat, args);

    va_end(args);

    TC_AppHandleError(errorLevel, pszCaption, szBuffer);

    printf(szBuffer); // Movethis 
}

void DebugString(TCHAR* pszString, ...)
{
    TCHAR szBuffer[MAX_ERROR_LENGTH];

    va_list args;
    va_start(args, pszString);

    _vsntprintf_s(szBuffer, MAX_ERROR_LENGTH, _TRUNCATE, pszString, args);

    va_end(args);

    TC_AppDebugString(szBuffer);
}
#endif // !_AFXDLL

TC_PluginError _cdecl TC_PluginInitialise(const TC_AppPointers* pAppPointers, HPLUGIN hThis)
{
    assert(hThis);
    assert(pAppPointers);

    if(g_hThis) // Already initialised ?
        return PE_AlreadyLoaded;

    g_hThis = hThis;
    g_AppPointers.pAppHandleError = pAppPointers->pAppHandleError;
    g_AppPointers.pAppRegisterFileType = pAppPointers->pAppRegisterFileType;
    g_AppPointers.pAppUnregisterFileType = pAppPointers->pAppUnregisterFileType;
    g_AppPointers.pAppRegisterCodec = pAppPointers->pAppRegisterCodec;
    g_AppPointers.pAppUnregisterCodec = pAppPointers->pAppUnregisterCodec;
    g_AppPointers.pAppRegisterMipGenerator = pAppPointers->pAppRegisterMipGenerator;
    g_AppPointers.pAppUnregisterMipGenerator = pAppPointers->pAppUnregisterMipGenerator;
    g_AppPointers.pAppRegisterViewType = pAppPointers->pAppRegisterViewType;
    g_AppPointers.pAppUnregisterViewType = pAppPointers->pAppUnregisterViewType;
    g_AppPointers.pAppGetMipLevel = pAppPointers->pAppGetMipLevel;
    //Memory (de)allocation
    g_AppPointers.pAppAllocateMipSet = pAppPointers->pAppAllocateMipSet;
    g_AppPointers.pAppAllocateMipLevelData = pAppPointers->pAppAllocateMipLevelData;
    g_AppPointers.pAppAllocateCompressedMipLevelData = pAppPointers->pAppAllocateCompressedMipLevelData;
    g_AppPointers.pAppFreeMipLevelData = pAppPointers->pAppFreeMipLevelData;
    g_AppPointers.pAppCompressTextureCallback = pAppPointers->pAppCompressTextureCallback;
    g_AppPointers.pAppUpdateViewParent = pAppPointers->pAppUpdateViewParent;
    g_AppPointers.pAppEnableCommand = pAppPointers->pAppEnableCommand;
    g_AppPointers.pAppCheckCommand = pAppPointers->pAppCheckCommand;
    g_AppPointers.pAppDebugString= pAppPointers->pAppDebugString;

    return PE_OK;
}

TC_PluginError _cdecl TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{
    assert(pPluginVersion);
    if(pPluginVersion)
    {
        //pPluginVersion->guid = g_GUID;
        pPluginVersion->dwAPIVersionMajor = TC_API_VERSION_MAJOR;
        pPluginVersion->dwAPIVersionMinor = TC_API_VERSION_MINOR;
        pPluginVersion->dwPluginVersionMajor = VERSION_MAJOR_MAJOR;
        pPluginVersion->dwPluginVersionMinor = VERSION_MAJOR_MINOR;

        return PE_OK;
    }
    return PE_Unknown;
}


TC_ErrorResponse TC_AppHandleError(TC_ErrorLevel errorLevel, const TCHAR* pszCaption, const TCHAR* pszText)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppHandleError);
    if(g_AppPointers.pAppHandleError)
        return g_AppPointers.pAppHandleError(g_hThis, errorLevel, pszCaption, pszText);

    return ER_OK;
}

void TC_AppDebugString(const TCHAR* pszString)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppDebugString);
    if(g_AppPointers.pAppDebugString)
        g_AppPointers.pAppDebugString(g_hThis, pszString);
}

HFILETYPE TC_AppRegisterFileType(TCHAR* pszFileTypeDescription, TCHAR* pszFileTypeExtensions[], bool bSaveFile)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppRegisterFileType);
    if(g_AppPointers.pAppRegisterFileType)
        return g_AppPointers.pAppRegisterFileType(g_hThis, pszFileTypeDescription, pszFileTypeExtensions, bSaveFile);

    return NULL;
}

void TC_AppUnregisterFileType(HFILETYPE hFileType)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppUnregisterFileType);
    assert(hFileType);
    if(g_AppPointers.pAppUnregisterFileType)
        g_AppPointers.pAppUnregisterFileType(g_hThis, hFileType);
}

HCODEC TC_AppRegisterCodec(TCHAR* pszCodecDescription, DWORD dwFourCCs[], bool bDecompressorOnly)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppRegisterCodec);
    if(g_AppPointers.pAppRegisterCodec)
        return g_AppPointers.pAppRegisterCodec(g_hThis, pszCodecDescription, dwFourCCs, bDecompressorOnly);

    return NULL;
}

void TC_AppUnregisterCodec(HCODEC hCodec)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppUnregisterCodec);
    assert(hCodec);
    if(g_AppPointers.pAppUnregisterCodec)
        g_AppPointers.pAppUnregisterCodec(g_hThis, hCodec);
}

HMIPPER TC_AppRegisterMipGenerator(TCHAR* pszMipGeneratorDescription)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppRegisterMipGenerator);
    if(g_AppPointers.pAppRegisterMipGenerator)
        return g_AppPointers.pAppRegisterMipGenerator(g_hThis, pszMipGeneratorDescription);

    return NULL;
}

void TC_AppUnregisterMipGenerator(HMIPPER hMipper)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppUnregisterMipGenerator);
    assert(hMipper);
    if(g_AppPointers.pAppUnregisterMipGenerator)
        g_AppPointers.pAppUnregisterMipGenerator(g_hThis, hMipper);
}

HVIEWTYPE TC_AppRegisterViewType(TCHAR* pszViewDescription, bool bDefaultView)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppRegisterViewType);
    if(g_AppPointers.pAppRegisterViewType)
        return g_AppPointers.pAppRegisterViewType(g_hThis, pszViewDescription, bDefaultView);

    return NULL;
}

void TC_AppUnregisterViewType(HVIEWTYPE hViewType)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppUnregisterViewType);
    assert(hViewType);
    if(g_AppPointers.pAppUnregisterViewType)
        g_AppPointers.pAppUnregisterViewType(g_hThis, hViewType);
}

MipLevel* TC_AppGetMipLevel(const MipSet* pMipSet, int nMipLevel, int nFaceOrSlice)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppGetMipLevel);
    if(g_AppPointers.pAppGetMipLevel)
        return g_AppPointers.pAppGetMipLevel(g_hThis, pMipSet, nMipLevel, nFaceOrSlice);

    return NULL;
}

//Memory (de)allocation
bool TC_AppAllocateMipSet(MipSet* pMipSet, ChannelFormat channelFormat, TextureDataType textureDataType, TextureType textureType, int nWidth, int nHeight, int nDepth)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppAllocateMipSet);
    if(g_AppPointers.pAppAllocateMipSet)
        return g_AppPointers.pAppAllocateMipSet(g_hThis, pMipSet, channelFormat, textureDataType, textureType, nWidth, nHeight, nDepth);

    return false;
}

bool TC_AppAllocateMipLevelData(MipLevel* pMipLevel, int nWidth, int nHeight, ChannelFormat channelFormat, TextureDataType textureDataType)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppAllocateMipLevelData);
    if(g_AppPointers.pAppAllocateMipLevelData)
        return g_AppPointers.pAppAllocateMipLevelData(g_hThis, pMipLevel, nWidth, nHeight, channelFormat, textureDataType);

    return false;
}

bool TC_AppAllocateCompressedMipLevelData(MipLevel* pMipLevel, int nWidth, int nHeight, DWORD dwSize)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppAllocateCompressedMipLevelData);
    if(g_AppPointers.pAppAllocateCompressedMipLevelData)
        return g_AppPointers.pAppAllocateCompressedMipLevelData(g_hThis, pMipLevel, nWidth, nHeight, dwSize);

    return false;
}
void TC_AppFreeMipLevelData(MipLevel* pMipLevel)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppFreeMipLevelData);
    if(g_AppPointers.pAppFreeMipLevelData)
        return g_AppPointers.pAppFreeMipLevelData(g_hThis, pMipLevel);
}

bool TC_AppCompressTextureCallback(float fProgress, DWORD_PTR pUser1, DWORD_PTR pUser2)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppCompressTextureCallback);
    if(g_AppPointers.pAppCompressTextureCallback)
        return g_AppPointers.pAppCompressTextureCallback(g_hThis, fProgress, pUser1, pUser2);
    else
        return false;
}

void TC_AppUpdateViewParent(HWND hParent, UINT nWidth, UINT nHeight)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppUpdateViewParent);
    if(g_AppPointers.pAppUpdateViewParent)
        g_AppPointers.pAppUpdateViewParent(g_hThis, hParent, nWidth, nHeight);
}

void TC_AppEnableCommand(HWND hParent, UINT nCommand, BOOL bEnable)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppEnableCommand);
    if(g_AppPointers.pAppEnableCommand)
        g_AppPointers.pAppEnableCommand(g_hThis, hParent, nCommand, bEnable);
}

void TC_AppCheckCommand(HWND hParent, UINT nCommand, BOOL bCheck)
{
    assert(g_hThis);
    assert(g_AppPointers.pAppCheckCommand);
    if(g_AppPointers.pAppCheckCommand)
        g_AppPointers.pAppCheckCommand(g_hThis, hParent, nCommand, bCheck);
}
#endif

