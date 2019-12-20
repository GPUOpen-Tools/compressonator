//////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2007,2008 Advanced Micro Devices, Inc.
//  Copyright (c) 2002-2006 ATI Technologies Inc.
//
//  File Name:   TC_PluginAPI.h
//  Description: Definition of Texture Plugin API
//
//  Version:    1.4
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

/// \file
/// TC_PluginAPI.h declares both the plugin functions exported by The Compressonator & those that your plugin can implement to extend 
/// The Compressonator.

#if !defined(_TC_PLUGINAPI_INCLUDED_)
#define _TC_PLUGINAPI_INCLUDED_

#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#endif

#include <assert.h>

#include "PluginInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define WM_PLUGIN_BASE        WM_USER+100
//#define WM_PLUGIN_INIT_DLG    WM_PLUGIN_BASE    ///< Windows Message passed to a options dialog routine to initialize the dialog.
//#define WM_PLUGIN_END_DLG     WM_PLUGIN_BASE+1  ///< Windows Message passed to a options dialog routine to close the dialog.
//#define WM_PLUGIN_SET_PARAMS  WM_PLUGIN_BASE+2  ///< Windows Message passed to a options dialog routine to set the dialog parameters.
//#define WM_PLUGIN_GET_PARAMS  WM_PLUGIN_BASE+3  ///< Windows Message passed to a options dialog routine to get the dialog parameters.
//
//#define ID_BASE                        40000       ///< \internal
//#define ID_VIEW_HIGHER_MIPLEVEL        ID_BASE     ///< \internal
//#define ID_VIEW_LOWER_MIPLEVEL         ID_BASE+1   ///< \internal
//#define ID_VIEW_RGB                    ID_BASE+2   ///< \internal
//#define ID_VIEW_ALPHA                  ID_BASE+3   ///< \internal
//#define ID_VIEW_ZOOM_IN                ID_BASE+4   ///< \internal
//#define ID_VIEW_ZOOM_OUT               ID_BASE+5   ///< \internal
//#define ID_VIEW_BRIGHTNESS_UP          ID_BASE+6   ///< \internal
//#define ID_VIEW_BRIGHTNESS_DOWN        ID_BASE+7   ///< \internal
//#define ID_VIEW_SHOW_CROSSHAIR         ID_BASE+8   ///< \internal
//#define ID_VIEW_NEXT_FACEORSLICE       ID_BASE+9   ///< \internal
//#define ID_VIEW_PREVIOUS_FACEORSLICE   ID_BASE+10  ///< \internal
//#define ID_VIEW_POSITIVE_X             ID_BASE+11  ///< \internal
//#define ID_VIEW_NEGATIVE_X             ID_BASE+12  ///< \internal
//#define ID_VIEW_POSITIVE_Y             ID_BASE+13  ///< \internal
//#define ID_VIEW_NEGATIVE_Y             ID_BASE+14  ///< \internal
//#define ID_VIEW_POSITIVE_Z             ID_BASE+15  ///< \internal
//#define ID_VIEW_NEGATIVE_Z             ID_BASE+16  ///< \internal
//#define ID_VIEW_DIFF_MODE              ID_BASE+17  ///< \internal
//#define ID_VIEW_FLIP_MODE              ID_BASE+18  ///< \internal
//#define ID_VIEW_FLIP                   ID_BASE+19  ///< \internal
//#define ID_VIEW_RGBA                   ID_BASE+20  ///< \internal
//
typedef CMP_DWORD_PTR TC_HANDLE;  ///< Generic Texture API handle
typedef TC_HANDLE HFILETYPE;  ///< Handle to a FileType.
typedef TC_HANDLE HCODEC;     ///< Handle to a Codec.
typedef TC_HANDLE HMIPPER;    ///< Handle to a Mipper.
typedef TC_HANDLE HVIEWTYPE;  ///< \internal Handle to a ViewType.
typedef TC_HANDLE HVIEW;      ///< \internal Handle to a View.
#ifndef _WIN32
typedef wchar_t TCHAR;
typedef char* LPTSTR;
typedef int HWND;
typedef CMP_DWORD FileSaveDialogProc;
typedef CMP_DWORD CompressDialogProc;
typedef CMP_DWORD GenerateMipsDialogProc;
typedef int* GUID;
typedef CMP_DWORD WNDPROC;
#endif
///\def MAKEFOURCC(ch0, ch1, ch2, ch3)
/// Utility macro for defining a FourCC code.
#ifndef C_MAKEFOURCC
#define C_MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
   ((CMP_DWORD)(CMP_BYTE)(ch0) | ((CMP_DWORD)(CMP_BYTE)(ch1) << 8) |   \
   ((CMP_DWORD)(CMP_BYTE)(ch2) << 16) | ((CMP_DWORD)(CMP_BYTE)(ch3) << 24 ))
#endif

/// A structure for storing compression parameters specific to each codec.
typedef struct
{
    void*          pPluginSpecific;  ///< Usually used as a pointer to a user-defined structure that can contain all the necessary codec options.
    const TCHAR*   pszTextParams;    ///< A string containing the command line options for the codec when the app is called from the command line.
} TC_CompressParams;

/// A structure for storing compression parameters specific to each codec.
typedef struct
{
    void*          pPluginSpecific;  ///< Usually used as a pointer to a user-defined structure that can contain all the necessary file save options.
    const TCHAR*   pszTextParams;    ///< A string containing the command line options for the file handler when the app is called from the command line.
} TC_FileSaveParams;

/// A structure for storing mip generation parameters specific to each mipper.
typedef struct
{
    int            nMinSize;         ///< The size in pixels used to determine how many mip levels to generate. Once all dimensions are less than or equal to nMinSize your mipper should generate no more mip levels.
    void*          pPluginSpecific;  ///< Usually used as a pointer to a user-defined structure that can contain all the necessary mipper options.
    const TCHAR*   pszTextParams;    ///< A string containing the command line options for the mipper when the app is called from the command line.
} TC_GenerateMipsParams;

/// Error codes returned by application & plugin functions.
/// We should obviously return more meaningful error codes than we currently like.
//typedef enum
//{
//    PE_OK,            ///< No error - success.
//    PE_AlreadyLoaded, ///< The plugin is already loaded. 
//    PE_Unknown,       ///< An error occured.
//} TC_PluginError;

/// Indicates the error level of an error message.
//typedef enum
//{
//    EL_Error,      ///< The error message is for an error.
//    EL_Warning,    ///< The error message is for a warning.
//} TC_ErrorLevel;

/// \internal
typedef enum
{
    ER_OK,
    ER_Cancel,
    ER_Ignore,
} TC_ErrorResponse;

/// \internal
typedef enum
{
    TC_VM_Normal,
    TC_VM_Diff,
} TC_ViewMode;

#include "Texture.h"
#ifdef _WIN32
#include "TC_PluginInternal.h"
#endif
//static const TCHAR TC_Property_BitsPerPixel[] = _T("BitsPerPixel");

/// Plugin Functions

/// This function is called after the Compressonator has loaded the plugin DLL and initialized it. See Loading Process for more details. This is where you place any initialization code that you need.
/// \return PE_OK if there are no errors. PE_Unknown otherwise.
/// \remarks Your function must do some type of registration with The Compressonator in this function. File plugins call TC_AppRegisterFileType. Codec plugins call TC_AppRegisterCodec. Mipper plugins call TC_AppRegisterMipGenerator. Any other initialization that should happen only once can be put in this function.
/// \sa \link TC_PluginUnload() TC_PluginUnload\endlink
TC_PluginError TC_PluginLoad();

/// Called by The Compressonator when a plugin is being unloaded from memory. Do any unregistering from here, unallocation of extra memory, and miscellaneous other cleanup.
/// \return PE_OK if successful, PE_Unknown otherwise.
/// \remarks It is vital that you call AppUnregister functions for any corresponding AppRegister functions that you may have called earlier, passing in to Unregister the value that was returned from the corresponding Register function.
/// \par Example:
/// \code
/// assert(g_FileType != NULL);
/// TC_AppUnregisterFileType(g_FileType);
/// g_FileType = NULL;
/// return PE_OK;
/// \endcode
/// \sa \link TC_PluginLoad() TC_PluginLoad\endlink
TC_PluginError TC_PluginUnload();

/// It is recommended that every plugin implements this function. What this function returns is displayed in The Compressonator's about box when it is displayed.
/// \return A string describing what the plugin does, who made it, version information, copyright, etc..
/// \remarks Please don't make it too long (3-4 lines is good), and be sure to include version information for debugging purposes.
/// \par Example:
/// Declare g_szAboutInfo a global char *, put something like this in your TC_PluginLoad:
/// \code
/// _sntprintf_s(g_szAboutInfo, MAX_ABOUT_SIZE, _TRUNCATE, "GIF texture file plugin.\r\nLoads & Saves GIF texture files.\r\nVersion %i.%i.%i\r\nCopyright (C) 2004 Advanced Micro Devices, Inc.\r\n"
/// "The Graphics Interchange Format(c) is the Copyright property of CompuServe\r\n"
/// "Incorporated. GIF(sm) is a Service Mark property of CompuServe Incorporated.",
/// VERSION_MAJOR_MAJOR, VERSION_MAJOR_MINOR, VERSION_MINOR_MAJOR);
/// \endcode
/// Then:
/// \code
/// const TCHAR* TC_PluginGetAboutInfo()
/// {
///    return g_szAboutInfo;
/// }
/// \endcode
/// \sa \link TC_PluginGetHelpInfo() TC_PluginGetHelpInfo\endlink
const TCHAR* TC_PluginGetAboutInfo();

/// If your codec or mipper plugin has options beyond choosing a fourCC with +FourCC, your plugin should implement this instruction. The string returned by this function is displayed on the command line when The Compressonator is called with -plugins.
/// \return A string describing the options this codec/mipper accepts in the form of "+attribute value".
/// \remarks For a more uniform format, please make your help string look like the one shown in the example code. That is, put the attribute followed by <> enclosing the valid range of values, and follow with a short description if appropriate. Separate similar attributes with commas then follow with the \<range\>. After the description, put a semicolon. Don't prefix your help string with "Options:" (or a something similar) or postfix with newlines, The Compressonator does that for you.
/// \par Example:
/// Declare g_szHelpInfo a global char *, put something like this in your TC_PluginLoad:
/// \code
/// g_szHelpInfo = "+FourCC <fourCC>;+red,+green,+blue <0.0-1.0> sets relative weighting;+alpha_threshold <0-255> for DXTC1";
/// \endcode
/// Then:
/// \code
/// const TCHAR* TC_PluginGetHelpInfo()
/// {
///    return g_szHelpInfo;
/// }
/// \endcode
/// \sa \link TC_PluginGetAboutInfo() TC_PluginGetAboutInfo\endlink
const TCHAR* TC_PluginGetHelpInfo();

/// This function is used to query plugins in a flexible manner.
/// \param[in] pszProperty A string used by the plugin to determine what to return.
/// \param[in] pMipSet A pointer to a MipSet, it is also used by the plugin to determine what to return.
/// \param[in, out] pdwValue Pointer to a DWORD that is used by the plugin to return information.
/// \return PE_OK if the query is recognized, PE_Unknown if it is not.
/// \remarks Supporting this function is optional for plugins.
/// \sa \link TC_PluginGetFloatProperty() TC_PluginGetFloatProperty\endlink
TC_PluginError TC_PluginGetDWORDProperty(const TCHAR* pszProperty, const MipSet* pMipSet, CMP_DWORD* pdwValue);

/// This function is used to query plugins in a flexible manner.
/// \param[in] pszProperty A string used by the plugin to determine what to return.
/// \param[in] pMipSet A pointer to a MipSet, it is also used by the plugin to determine what to return.
/// \param[in, out] pfValue Pointer to a float that is used by the plugin to return information.
/// \return PE_OK if the query is recognized, PE_Unknown if it is not.
/// \remarks Supporting this function is optional for plugins.
/// \sa \link TC_PluginGetDWORDProperty() TC_PluginGetDWORDProperty\endlink
TC_PluginError TC_PluginGetFloatProperty(const TCHAR* pszProperty, const MipSet* pMipSet, float* pfValue);

//File plugin functions

/// This function is called by The Compressonator when a file type that this plugin has registered is opened. It takes a texture file on disk and returns a texture.
/// \param[in] hFileType A HFILETYPE as returned to this plugin when by TC_AppRegisterFileType. Can be used to determine between
/// multiple file types registered by the same plugin.
/// \param[in] pszFilename Full path and filename of texture to load.
/// \param[in, out] pMipSet Pointer to the MipSet that plugin will put texture information in.
/// \return PE_OK if no errors are encountered. PE_Unknown otherwise.
/// \remarks Remember to set the appropriate flags in the MipSet such as ChannelFormat, TextureDataType, TextureType, MS_Flags, and m_dwFourCC.
/// \remarks While the MipSet pointed to by pMipSet has been allocated already, pMipSet->m_MipLevel[0].m_pbData will be NULL (and all subsequent MipLevels' m_pbData as well). You must use TC_AppAllocateCompressedMipLevelData to allocate memory for the compressed texture that your plugin will place in pMipSet (and if this plugin outputs an uncompressed texture, use TC_AppAllocateMipLevelData instead). Allocating with malloc or new or any other allocator will cause problems when that memory is eventually deallocated.
/// \par Example:
/// Your code could go something like this. 
/// \code
/// TC_PluginError TC_PluginFileLoadTexture(const HFILETYPE hFileType, const TCHAR* pszFilename, MipSet* pMipSet)
/// {
///    OutputDebugString("TextureAPI SomeFile Plugin : TC_PluginFileLoadTexture\n");
///    assert(pszFileName);
///    assert(pMipSet);
/// 
///    //open the file and read from it
/// 
///    if(!TC_AppAllocateMipSet(pMipSet, CF_Compressed, TDT_XRGB, TT_2D, imageDescriptor.wWidth, imageDescriptor.wHeight, 1))
///       return PE_Unknown;
/// 
///    DWORD dwSize = imageDescriptor.wWidth * imageDescriptor.wHeight;
///    MipLevel* pMipLevelZero = TC_AppGetMipLevel(pMipSet, 0);
/// 
///    if(!TC_AppAllocateCompressedMipLevelData(pMipLevelZero , imageDescriptor.wWidth, imageDescriptor.wHeight, dwSize))
///       return PE_Unknown;
/// 
///    //take the data from the file, put it into pMipSet
/// 
///    return PE_OK;
/// }
/// \endcode
/// \sa \link FilePlugins \endlink, \link TC_PluginFileSaveTexture() TC_PluginFileSaveTexture\endlink
TC_PluginError TC_PluginFileLoadTexture(const HFILETYPE hFileType, const TCHAR* pszFilename, MipSet* pMipSet);

/// This function is called when a texture is saved with a file type that this plugin has registered. It takes a (compressed) texture and saves to disk a texture file.
/// \param[in] hFileType A HFILETYPE as returned to this plugin when by TC_AppRegisterFileType. Can be used to determine between
/// multiple file types registered by the same plugin.
/// \param[in] pszFilename Full path and filename of where the plugin will save texture file.
/// \param[in] pMipSet Pointer to a MipSet that is the (compressed) texture input, used to write the texture to disk.
/// \param[in] pFileSaveParams A pointer to a file save parameters. Can be NULL.
/// \return PE_OK if no errors are encountered. PE_Unknown otherwise.
/// \remarks You are free to write the file to disk however you want to. You can use the Standard C Library functions (_tfopen, fwrite etc.) or the Windows MFC CFile class, or any other method. You can assume that the directory of pszFilename has already been created. 
/// \sa \link FilePlugins \endlink, \link TC_PluginFileLoadTexture() TC_PluginFileLoadTexture\endlink
TC_PluginError TC_PluginFileSaveTexture(const HFILETYPE hFileType, const TCHAR* pszFilename, const MipSet* pMipSet, const TC_FileSaveParams* pFileSaveParams);

/// Called by The Compressonator to determine if this plugin can save the given format.
/// \param[in] hFileType A HFILETYPE as returned to this plugin when by TC_AppRegisterFileType. Can be used to determine between
/// multiple file types registered by the same plugin.
/// \param[in] pMipSet Pointer to the MipSet that contains the format information.
/// \return TRUE if this plugin can save the given format, FALSE otherwise.
/// \remarks pMipSet is not guaranteed to contain texture data as sometimes it won't. Such is the case for batch compression: a MipSet is created that only has format information, not any texture data, and it is used to determine which file plugins should be listed as able to save to their format.
/// \par Example:
/// If the plugin supported only P8:
/// \code
/// BOOL TC_PluginFileSupportsFormat(const HFILETYPE hFileType, const MipSet* pMipSet)
/// {
///    assert(pMipSet);
///    if(pMipSet == NULL)
///       return FALSE;
/// 
///    switch(pMipSet->m_dwFourCC)
///    {
///    case CMP_MAKEFOURCC('P', '8', ' ', ' '):
///       return TRUE;
///    }
/// 
///    return FALSE;
/// }
/// \endcode
/// \sa \link FilePlugins \endlink, \link TC_PluginFileSaveTexture() TC_PluginFileSaveTexture\endlink, \link TC_PluginFileSupportsMipLevels() TC_PluginFileSupportsMipLevels\endlink
bool TC_PluginFileSupportsFormat(const HFILETYPE hFileType, const MipSet* pMipSet);

/// Called by The Compressonator to determine if this plugin can save the given number of Miplevels.
/// \param[in] hFileType A HFILETYPE as returned to this plugin when by TC_AppRegisterFileType. Can be used to determine between
/// multiple file types registered by the same plugin.
/// \param[in] nMipLevels The number of Miplevels in question.
/// \return TRUE if this plugin can save the given number of Miplevels, FALSE otherwise.
/// \par Example:
/// If the plugin supported only a single Miplevel:
/// \code
/// BOOL TC_PluginFileSupportsMipLevels(const HFILETYPE hFileType, const int nMipLevels)
/// {
///    if(nMipLevels == 1)
///       return TRUE;
///    else
///       return FALSE;
/// }
/// \endcode
/// \sa \link FilePlugins \endlink, \link TC_PluginFileSaveTexture() TC_PluginFileSaveTexture\endlink, \link TC_PluginFileSupportsFormat() TC_PluginFileSupportsFormat\endlink
bool TC_PluginFileSupportsMipLevels(const HFILETYPE hFileType, const int nMipLevels);

/// This function is called by The Compressonator before enabling the compression options dialog specific to this plugin. Its purpose is to do the setup before the compression options dialog is called.
/// \param[in] hFileType Should be the same HFILETYPE that was returned to this plugin when this plugin called TC_AppRegisterFileType. Return PE_Unknown if it's not.
/// \param[in, out] ppTemplate A pointer to a LPTSTR that this plugin will assign a dialog resource to.
/// \param[in, out] ppfnDialogProc A pointer to a dialog proc. This plugin will assign the message handler for this plugin's compression dialog to ppfnDialogProc.
/// \param[in, out] pFileSaveParams A pointer to a set of TC_FileSaveParams. This plugin will assign the address of a static TC_FileSaveParams variable to pFileSaveParams.
/// \return TRUE if successful, FALSE otherwise.
/// \remarks If this function returns false, The Compressonator will not enable the compression options dialog specific to this plugin. The static TC_FileSaveParams variable that is assigned to pFileSaveParams should contain an already allocated pPluginSpecific object, whatever that object is (you decide).
/// \par Example:
/// g_FileType is where the return value from the previous call to TC_AppRegisterFileType was assigned. g_FileSaveParams is a static object that can be anything. The plugin will use it to store information from the compression options dialog.
/// \code
/// BOOL TC_PluginFileGetFileSaveParametersDialog(const HFILETYPE hFileType, LPTSTR* ppTemplate, FileSaveDialogProc* ppfnDialogProc, TC_FileSaveParams* pFileSaveParams)
/// {
///    assert(hFileType == g_FileType);
///    assert(ppTemplate);
///    assert(ppfnDialogProc);
///    assert(pFileSaveParams);
///    if(hFileType != g_FileType)
///       return FALSE;
///    *ppTemplate = MAKEINTRESOURCE(IDD_COMPRESS_PARAMETERS);
///    *ppfnDialogProc = &FileSaveParametersDialogProc;
///    if(pFileSaveParams)
///    {
///       pFileSaveParams->pPluginSpecific = &g_FileSaveParams;
///       assert(pFileSaveParams->pPluginSpecific);
///    }
///    return TRUE;
/// }
/// \endcode
/// \sa \link FilePlugins \endlink, \link FileSaveDialogProc \endlink, FileSaveParams
//bool TC_PluginFileGetFileSaveParametersDialog(const HFILETYPE hFileType, LPTSTR* ppTemplate, FileSaveDialogProc* ppfnDialogProc, TC_FileSaveParams* pFileSaveParams);

//Codec plugin functions
/// This function is called by The Compressonator after a compressed texture is loaded and after compression (for the purpose of comparing the original and compressed versions).
/// \param[in] hCodec A HCODEC as returned to this plugin when by TC_AppRegisterCodec. Can be used to determine between multiple codecs registered by the same plugin.
/// \param[in] pMipSetIn Pointer to the compressed MipSet.
/// \param[in, out] pMipSetOut Pointer to the MipSet where this plugin will place its decompressed texture output.
/// \return PE_OK if successful, PE_Unknown otherwise.
/// \remarks For each MipLevel of pMipSetOut that you are writing to, you must use TC_AppAllocateMipLevelData to allocate memory for the decompressed texture that your plugin will place place there. Allocating with malloc or new or any other allocator will cause problems when that memory is eventually deallocated.
/// \par Example:
/// This is taken from the Palletized plugin.
/// \code
/// TC_PluginError TC_PluginCodecDecompressTexture(const HCODEC hCodecconst MipSet* pMipSetIn, MipSet* pMipSetOut)
/// {
///     assert(pMipSetIn);
///     assert(pMipSetIn->m_nMipLevels);
///     assert(pMipSetIn->m_MipLevel[0].m_pbData);
///     assert(pMipSetOut);
///     assert(!pMipSetOut->m_nMipLevels);
/// 
///     switch(pMipSetIn->m_dwFourCC)
///     {
///         case CMP_FOURCC_P1:
///             return Decompress_P1(pMipSetIn, pMipSetOut);
///             break;
/// 
///         case CMP_FOURCC_P2:
///             return Decompress_P2(pMipSetIn, pMipSetOut);
///             break;
/// 
///         case CMP_FOURCC_P4:
///             return Decompress_P4(pMipSetIn, pMipSetOut);
///             break;
/// 
///         case CMP_FOURCC_P8:
///             return Decompress_P8(pMipSetIn, pMipSetOut);
///             break;
/// 
///         default:
///             ASSERT(0);
///             return PE_Unknown;
///             break;
///     }
/// }
/// \endcode
/// \sa \link CodecPlugins \endlink, \link TC_PluginCodecCompressTexture() TC_PluginCodecCompressTexture\endlink
TC_PluginError TC_PluginCodecDecompressTexture(const HCODEC hCodec, const MipSet* pMipSetIn, MipSet* pMipSetOut);

/// This function is called by The Compressonator to compress a texture using this plugin.
/// \param[in] hCodec A HCODEC as returned to this plugin when by TC_AppRegisterCodec. Can be used to
/// determine between multiple codecs registered by the same plugin.
/// \param[in] pMipSetIn A pointer to the MipSet that needs compressing.
/// \param[in, out] pMipSetOut A pointer to the MipSet where this plugin will place its compressed texture output.
/// \param[in] pCompressParams A pointer to a set of TC_CompressParams that inform this plugin of the specific compression options that were chosen.
/// \return PE_OK if successful, PE_Unknown otherwise.
/// \remarks For each MipLevel of pMipSetOut that you are writing to, you must use TC_AppAllocateCompressedMipLevelData to allocate memory for the compressed texture that your plugin will place place there. Allocating with malloc or new or any other allocator will cause problems when that memory is eventually unallocated. You can call TC_AppCompressTextureCallback from this function to let users know compression progress.
/// \par Example:
/// See the codec plugin example included with the SDK.
/// \sa \link CodecPlugins \endlink, 
/// \link TC_AppAllocateCompressedMipLevelData() TC_AppAllocateCompressedMipLevelData\endlink
/// \link TC_AppCompressTextureCallback() TC_AppCompressTextureCallback\endlink
/// \link TC_PluginCodecDecompressTexture() TC_PluginCodecDecompressTexture\endlink, TC_CompressParams
TC_PluginError TC_PluginCodecCompressTexture(const HCODEC hCodec, const MipSet* pMipSetIn, MipSet* pMipSetOut, TC_CompressParams* pCompressParams);

/// Called by The Compressonator to determine if this plugin can compress the given format.
/// \param[in] hCodec A HCODEC as returned to this plugin when by TC_AppRegisterCodec. Can be used to determine between multiple codecs registered by the same plugin.
/// \param[in] pMipSet Pointer to the MipSet that contains the format information.
/// \return TRUE if this plugin can compress the given format, FALSE otherwise.
/// \remarks pMipSet is not guaranteed to contain texture data as sometimes it won't. Such is the case for batch compression: a MipSet is created that only has format information, not any texture data, and it is used to determine which file plugins should be listed as able to save to their format.
/// \par Example:
/// If the plugin supported only 8-bit ARGB:
/// \code
/// BOOL TC_PluginCodecSupportsFormat(const HCODEC hCodec, const MipSet* pMipSet)
/// {
///    assert(pMipSet);
///    if(pMipSet == NULL)
///       return FALSE;
/// 
///    if(pMipSet->m_ChannelFormat == CF_8bit && pMipSet->m_TextureDataType == TDT_ARGB)
///    {
///       return TRUE;
///    }
/// 
///    return FALSE;
/// }
/// \endcode
/// \sa \link CodecPlugins \endlink, \link TC_PluginCodecCompressTexture() TC_PluginCodecCompressTexture\endlink 
bool TC_PluginCodecSupportsFormat(const HCODEC hCodec, const MipSet* pMipSet);

/// This function is called by The Compressonator before enabling the compression options dialog specific to this plugin. Its purpose is to do the setup before the compression options dialog is called.
/// \param[in] hCodec A HCODEC as returned to this plugin when by TC_AppRegisterCodec. Can be used to determine between multiple codecs registered by the same plugin.
/// \param[in, out] ppTemplate A pointer to a LPTSTR that this plugin will assign a dialog resource to.
/// \param[in, out] ppfnDialogProc A pointer to a dialog proc. This plugin will assign the message handler for this plugin's compression dialog to ppfnDialogProc.
/// \param[in, out] pCompressParams A pointer to a set of TC_CompressParams. This plugin will assign the address of a static TC_CompressParams variable to pCompressParams.
/// \return TRUE if successful, FALSE otherwise.
/// \remarks If this function returns false, The Compressonator will not enable the compression options dialog specific to this plugin. The static TC_CompressParams variable that is assigned to pCompressParams should contain an already allocated pPluginSpecific object, whatever that object is (you decide).
/// \par Example:
/// g_Codec is where the return value from the previous call to TC_AppRegisterCodec was assigned. g_CompressParams is a static object that can be anything. The plugin will use it to store information from the compression options dialog.
/// \code
/// BOOL TC_PluginCodecGetCompressParametersDialog(const HCODEC hCodec, LPTSTR* ppTemplate, CompressDialogProc* ppfnDialogProc, TC_CompressParams* pCompressParams)
/// {
///    assert(hCodec == g_Codec);
///    assert(ppTemplate);
///    assert(ppfnDialogProc);
///    assert(pCompressParams);
///    if(hCodec != g_Codec)
///       return FALSE;
///    *ppTemplate = MAKEINTRESOURCE(IDD_COMPRESS_PARAMETERS);
///    *ppfnDialogProc = &CompressParametersDialogProc;
///    if(pCompressParams)
///    {
///       pCompressParams->pPluginSpecific = &g_CompressParams;
///       assert(pCompressParams->pPluginSpecific);
///    }
///    return TRUE;
/// }
/// \endcode
/// \sa \link CodecPlugins \endlink, CompressDialogProc, TC_CompressParams
//bool TC_PluginCodecGetCompressParametersDialog(const HCODEC hCodec, LPTSTR* ppTemplate, CompressDialogProc* ppfnDialogProc, TC_CompressParams* pCompressParams);

//Mip plugin functions

/// This function is called by The Compressonator to generate mipmaps with this plugin.
/// \param[in] hMipper A HMIPPER as returned to this plugin when by TC_AppRegisterMipGenerator. Can be used to determine between multiple mippers registered by the same plugin.
/// \param[in, out] pMipSet A pointer to the MipSet that this plugin is generating mipmaps for.
/// \param[in] pGenerateMipsParams A pointer to a set of TC_GenerateMipsParams that inform this plugin of the specific mipmapping options that were chosen.
/// \return PE_OK if successful, PE_Unknown otherwise.
/// \remarks Place the Miplevels that you generate into pMipSet. Generate mip levels as long as any of your dimensions (width, height or depth) is greater than pGenerateMipsParams->nMinSize.
/// \sa \link MipperPlugins \endlink, TC_GenerateMipsParams
TC_PluginError TC_PluginMipGenerateMipLevels(const HMIPPER hMipper, MipSet* pMipSet, TC_GenerateMipsParams* pGenerateMipsParams);

/// This function is called by The Compressonator before enabling the mip generation options dialog specific to this plugin. Its purpose is to do the setup before the mip generation options dialog is called.
/// \param[in] hMipper A HMIPPER as returned to this plugin when by TC_AppRegisterMipGenerator. Can be used to determine between multiple mippers registered by the same plugin.
/// \param[in, out] ppTemplate A pointer to a LPTSTR that this plugin will assign a dialog resource to.
/// \param[in, out] ppfnDialogProc A pointer to a dialog proc. This plugin will assign the message handler for this plugin's mip generation dialog to ppfnDialogProc.
/// \param[in, out] pGenerateMipsParams A pointer to a set of TC_GenerateMipsParams. This plugin will assign the address of a static TC_GenerateMipsParams variable to pGenerateMipsParams.
/// \return TRUE if successful, FALSE otherwise.
/// \remarks If this function returns false, The Compressonator will not enable the mip generation options dialog specific to this plugin. The static TC_GenerateMipsParams variable that is assigned to pGenerateMipsParams should contain an already allocated pPluginSpecific object, whatever that object is (you decide).
/// \par Example:
/// \code
/// bool TC_PluginMipGetGenerateMipLevelsParametersDialog(const HMIPPER hMipper, LPTSTR* ppTemplate, GenerateMipsDialogProc* ppfnDialogProc, TC_GenerateMipsParams* pGenerateMipsParams)
/// {
///    //All you should have to do is change the dialog resource name
///    assert(hMipper == g_hMipper);
///    assert(ppTemplate);
///    assert(ppfnDialogProc);
///    if(hMipper != g_hMipper)
///       return false;
/// 
///    *ppTemplate = MAKEINTRESOURCE(IDD_MIP_PARAMETERS);
///    *ppfnDialogProc = &GenerateMipsParametersDialogProc;
///    ASSERT(pGenerateMipsParams);
///    if(pGenerateMipsParams)
///    {
///       pGenerateMipsParams->pPluginSpecific = &g_GenerateMipsParams;
///       ASSERT(pGenerateMipsParams->pPluginSpecific);
///    }
///    return true;
/// }
/// \endcode
/// \sa \link MipperPlugins \endlink, GenerateMipsDialogProc, TC_GenerateMipsParams
//bool TC_PluginMipGetGenerateMipLevelsParametersDialog(const HMIPPER hMipper, LPTSTR* ppTemplate, GenerateMipsDialogProc* ppfnDialogProc, TC_GenerateMipsParams* pGenerateMipsParams);

/// Called by The Compressonator to determine if this plugin can generate mipmaps based on the format of pMipSet.
/// \param[in] hMipper A HMIPPER as returned to this plugin when by TC_AppRegisterMipGenerator. Can be used to determine between multiple mippers registered by the same plugin.
/// \param[in] pMipSet Pointer to the MipSet that contains the format information.
/// \return TRUE if this plugin can generate mipmaps for the given format, FALSE otherwise.
/// \remarks Don't assume things like pMipSet being non-NULL. See the example code.
/// \par Example:
/// If the plugin supports all 8 bit RGBA formats:
/// \code
/// BOOL TC_PluginMipSupportsFormat(const HMIPPER hMipper, const MipSet* pMipSet)
/// {
///    if(pMipSet && pMipSet->m_ChannelFormat == CF_8bit)
///    {
///       return TRUE;
///    }
///    return FALSE;
/// }
/// \endcode
/// \sa \link MipperPlugins \endlink, \link TC_PluginMipGenerateMipLevels() TC_PluginMipGenerateMipLevels\endlink
bool TC_PluginMipSupportsFormat(const HMIPPER hMipper, const MipSet* pMipSet);

//View plugin functions

/// \internal
/// 
/// \param[in]
/// \return 
/// \remarks
/// \par Example:
/// 
/// \code
/// \endcode
TC_PluginError TC_PluginViewCreateView(const HVIEWTYPE hViewType, HWND hParent, HWND* phWnd, HVIEW* phView, TC_ViewMode viewMode);

/// \internal
/// 
/// \param[in]
/// \return 
/// \remarks
/// \par Example:
/// 
/// \code
/// \endcode
TC_PluginError TC_PluginViewUpdateView(const HVIEW hView, const MipSet* pMipSetSourceRGBA, const MipSet* pMipSetCompressedRGBA, const MipSet* pMipSetCompressedData, const MipSet* pMipSetDiff);

/// \internal
/// 
/// \param[in]
/// \return 
/// \remarks
/// \par Example:
/// 
/// \code
/// \endcode
TC_PluginError TC_PluginViewUpdateViewTitles(const HVIEW hView, const TCHAR* pszTitle, const TCHAR* pszSourceTitle, const TCHAR* pszCompressedTitle, const TCHAR* pszDiffTitle);

/// \internal
/// 
/// \param[in]
/// \return 
/// \remarks
/// \par Example:
/// 
/// \code
/// \endcode
TC_PluginError TC_PluginViewReleaseView(HVIEW hView);


/// Application Functions

/// For getting the major version number of the API, not The Compressonator.
/// \return The major version number.
/// \remarks Use this function to turn features on and off depending on what version of The Compressonator API the user has (or if you have another use for this function, do that).
/// \par Example:
/// Lets say the plugin can't work with API versions older than 1.1, so in our TC_PluginLoad function we place the following lines:
/// \code
/// if(TC_AppGetAppAPIVersionMajor() < 1)
/// {
///    return PE_Unknown;
/// }
/// else if(TC_AppGetAppAPIVersionMajor() == 1)
/// {
///    if(TC_AppGetAppAPIVersionMinor() < 1)
///    {
///       return PE_Unknown;
///    }
///    return PE_OK;
/// }
/// else
/// {
///    return PE_OK;
/// }
/// \endcode
/// \sa \link TC_AppGetAppAPIVersionMinor() TC_AppGetAppAPIVersionMinor\endlink
CMP_DWORD TC_AppGetAppAPIVersionMajor();

/// For getting the minor version number of the API, not The Compressonator.
/// \return The minor version number.
/// \remarks Use this function to turn features on and off depending on what version of The Compressonator API the user has (or if you have another use for this function, do that).
/// \par Example:
/// Lets say the plugin can't work with API versions older than 1.1, so in our TC_PluginLoad function we place the following lines:
/// \code
/// if(TC_AppGetAppAPIVersionMajor() < 1)
/// {
///    return PE_Unknown;
/// }
/// else if(TC_AppGetAppAPIVersionMajor() == 1)
/// {
///    if(TC_AppGetAppAPIVersionMinor() < 1)
///    {
///       return PE_Unknown;
///    }
///    return PE_OK;
/// }
/// else
/// {
///    return PE_OK;
/// }
/// \endcode
/// \sa \link TC_AppGetAppAPIVersionMajor() TC_AppGetAppAPIVersionMajor\endlink
CMP_DWORD TC_AppGetAppAPIVersionMinor();

/// Displays a message box indicating that an error has occurred.
/// \param[in] errorLevel A TC_ErrorLevel indicating the severity of the error.
/// \param[in] pszCaption Caption of the message box.
/// \param[in] pszText Text of the message box.
/// \return A TC_ErrorResponse indicating the user's response.
/// \remarks Will display a message box the buttons OK, Cancel, and Ignore. The return value indicates which the user clicked.
/// \sa \link TC_AppDebugString() TC_AppDebugString\endlink
TC_ErrorResponse TC_AppHandleError(TC_ErrorLevel errorLevel, const TCHAR* pszCaption, const TCHAR* pszText);

/// Outputs a string to the debug stream.
/// \param[in] pszString Text to output.
/// \return A TC_ErrorResponse indicating the user's response.
/// \sa \link TC_AppHandleError() TC_AppHandleError\endlink
void TC_AppDebugString(const TCHAR* pszString);

//Plugin type-specific registration

/// Called by File plugins during their initialization process to let The Compressonator know what file extensions this plugin can handle.
/// \param[in] pszFileTypeDescription A string description of what kinds of texture files this plugin handles.
/// \param[in] pszFileTypeExtensions An array of strings that are the file extensions this plugin can handle. The last string in the array must be the empty string (just a null terminator).
/// \param[in] bSaveFile A bool that is true if this plugin can also save textures to those file extensions, false if it can't. It is assumed that this plugin can load those file extensions.
/// \return A HFILETYPE that must be saved by this plugin to use in a future call to TC_AppUnregisterFileType.
/// \remarks Call this function in your TC_PluginLoad function. The HFILETYPE that this function returns your plugin must save for TC_AppUnregisterFileType, which your plugin will call sometime before the end of TC_PluginUnload (assuming this function call succeeded). If the HFILETYPE returned is NULL, then the registration failed, and your plugin should react accordingly.
/// \par Example:
/// The following example is from a File Plugin's TC_PluginLoad function. It registers the file extension 'GIF' such that all files named '*.GIF' (case is not important) will be opened by the plugin (through a call to the plugin's TC_PluginFileLoadTexture). If the returned HFILETYPE is NULL then the plugin returns PE_Unknown (indicating failure).
/// \code
/// TCHAR* szExtensions[] = {"GIF", ""};
/// ASSERT(g_FileType==NULL);
/// g_FileType = TC_AppRegisterFileType("GIF Textures", &szExtensions[0], true);
/// if(g_FileType != NULL)
/// {
///    return PE_OK;
/// }
/// else
/// {
///    OutputDebugString("GIF Plugin- failed to register file type\n");
///    return PE_Unknown;
/// }
/// \endcode
/// \sa \link FilePlugins File Plugins\endlink, 
/// \link TC_PluginLoad() TC_PluginLoad \endlink, 
/// \link TC_AppUnregisterFileType() TC_AppUnregisterFileType \endlink, 
/// \link TC_PluginFileLoadTexture() TC_PluginFileLoadTexture \endlink, 
/// \link TC_PluginFileSaveTexture() TC_PluginFileSaveTexture \endlink
HFILETYPE TC_AppRegisterFileType(TCHAR* pszFileTypeDescription, TCHAR* pszFileTypeExtensions[], bool bSaveFile);

/// Usually called from a File Plugin's TC_PluginUnload, this function informs The Compressonator that this plugin can no longer handle the file extensions previously registered with TC_AppRegisterFileType.
/// \param[in] hFileType A HFILETYPE indicating what File Plugin (and its associated file extensions) is being unregistered.
/// \remarks The programmer must call this function at some point before the end of TC_PluginUnload (assuming the previous call to TC_AppRegisterFileType was successful). Pass in the HFILETYPE that was returned from the call to TC_AppRegisterFileType.
/// \par Example:
/// This code could appear in your plugin's TC_PluginUnload. g_FileType has the value returned from the call to TC_AppRegisterFileType.
/// \code
/// if(g_FileType != NULL)
/// {
///    TC_AppUnregisterFileType(g_FileType);
///    g_FileType = NULL;
/// }
/// \endcode
/// \sa \link FilePlugins File Plugins\endlink, 
/// \link TC_PluginUnload() TC_PluginUnload \endlink, 
/// \link TC_AppRegisterFileType() TC_AppRegisterFileType \endlink
void TC_AppUnregisterFileType(HFILETYPE hFileType);

/// Called by Codec Plugins during their initialization process to let The Compressonator know what FourCCs this plugin can handle.
/// \param[in] pszCodecDescription A string description of what compression types this plugin handles.
/// \param[in] dwFourCCs An array of FourCCs generated by the macro MAKEFOURCC. The last element of the array must be 0 to indicate the end.
/// \param[in] bDecompressorOnly Set to true if this plugin can only decompress the FourCCs passed in. Set to false if it can compress to them as well.
/// \return A HCODEC that must be saved by this plugin to use in a future call to TC_AppUnregisterCodec.
/// \remarks Call this function in your TC_PluginLoad function. The HCODEC that this function returns your plugin must save for TC_AppUnregisterCodec, which your plugin must call sometime before the end of TC_PluginUnload (assuming this function call succeeded). If the HCODEC returned is NULL, then the registration failed, and your plugin should react accordingly. Multiple plugins can register to handle the same FourCC code. The plugin to perform the compression is selected by the user, The Compressonator selects which plugin to use to perform decompression.
/// \par Example:
/// The following example is from a Codec Plugin's TC_PluginLoad function. It registers the P8 FourCC such that textures going to or from P8 can go through this plugin (through a call to the plugin's TC_PluginCodecCompressTexture and TC_PluginCodecDecompressTexture, respectively). If the returned HCODEC is NULL then the plugin returns PE_Unknown (indicating failure).
/// \code
/// DWORD dwFourCCs[] = {CMP_MAKEFOURCC('P', '8', ' ', ' '), 0};
/// ASSERT(g_Codec==NULL);
/// g_Codec = TC_AppRegisterCodec("Palettised Texture Compression", &dwFourCCs[0], false);
/// if(g_Codec == NULL)
/// {
///    OutputDebugString("Palettized Plugin- failed to register codec\n");
///    return PE_Unknown;
/// }
/// \endcode
/// \sa \link CodecPlugins Codec Plugins\endlink, 
/// \link TC_PluginLoad() TC_PluginLoad \endlink, 
/// \link TC_AppUnregisterCodec() TC_AppUnregisterCodec \endlink, 
/// \link TC_PluginCodecCompressTexture() TC_PluginCodecCompressTexture \endlink, 
/// \link TC_PluginCodecDecompressTexture() TC_PluginCodecDecompressTexture \endlink
HCODEC TC_AppRegisterCodec(TCHAR* pszCodecDescription, CMP_DWORD dwFourCCs[], bool bDecompressorOnly);

/// Usually called from a Codec Plugin's TC_PluginUnload, this function informs The Compressonator that this plugin can no longer act as a codec for the compression formats previously registered through TC_AppRegisterCodec.
/// \param[in] hCodec A HCODEC indicating what codec (and its associated FourCCs) is being unregistered.
/// \remarks The programmer must call this function at some point before the end of TC_PluginUnload (assuming the previous call to TC_AppRegisterCodec was successful). Pass in the HCODEC that was returned from the call to TC_AppRegisterCodec.
/// \par Example:
/// This code could appear in your plugin's TC_PluginUnload. g_Codec has the value returned from the call to TC_AppRegisterCodec.
/// \code
/// if(g_Codec != NULL)
/// {
///    TC_AppUnregisterCodec(g_Codec);
///    g_Codec = NULL;
/// }
/// \endcode
/// \sa \link CodecPlugins Codec Plugins \endlink, 
/// \link TC_PluginUnload() TC_PluginUnload \endlink, 
/// \link TC_AppRegisterCodec() TC_AppRegisterCodec \endlink
void TC_AppUnregisterCodec(HCODEC hCodec);

/// Called by Mipper Plugins during their initialization process to let The Compressonator know that this plugin can generate mipmaps.
/// \param[in] pszMipGeneratorDescription A string description of the mipmap generator.
/// \return A HMIPPER that must be saved by this plugin to use in a future call to
/// TC_AppUnregisterMipGenerator if successful, otherwise false.
/// \remarks Call this function in your TC_PluginLoad function. The HMIPPER that this function returns your plugin must save for 
/// TC_AppUnregisterMipGenerator, which your plugin must call sometime before the end of TC_PluginUnload (assuming this function 
/// call succeeded). If the HMIPPER returned is NULL, then the registration failed, and your plugin should react accordingly.
/// \par Example:
/// The following example is from a Mipper Plugin's TC_PluginLoad function. It registers the plugin as a mipmap generator so that users can generate mipmaps with the plugin (through a call to the plugin's TC_PluginMipGenerateMipLevels). If the returned HMIPPER is NULL then the plugin returns PE_Unknown (indicating failure).
/// \code
/// ASSERT(g_hMipper==NULL);
/// g_hMipper = TC_AppRegisterMipGenerator("Box-Filter");
/// if(g_hMipper == NULL)
/// {
///    Error("BoxFilter Plugin", EL_Error, IDS_ERROR_REGISTER_CODEC, "Box-Filter Mip-Level Generator");
///    return PE_Unknown;
/// }
/// \endcode
/// \sa \link MipperPlugins Mipper Plugins\endlink, 
/// \link TC_PluginLoad() TC_PluginLoad \endlink, 
/// \link TC_AppUnregisterMipGenerator() TC_AppUnregisterMipGenerator \endlink, 
/// \link TC_PluginMipGenerateMipLevels() TC_PluginMipGenerateMipLevels \endlink
HMIPPER TC_AppRegisterMipGenerator(TCHAR* pszMipGeneratorDescription);

/// Usually called from a Mipper Plugin's TC_PluginUnload, this function informs The Compressonator that this plugin can no longer generate mipmaps.
/// \param[in] hMipper A HMIPPER indicating what mipmap generator is being unregistered.
/// \remarks The programmer must call this function at some point before the end of TC_PluginUnload (assuming the previous call to TC_AppRegisterMipGenerator was successful). Pass in the HMIPPER that was returned from the call to TC_AppRegisterMipGenerator.
/// \par Example:
/// This code could appear in your plugin's TC_PluginUnload. g_hMipper has the value returned from the call to TC_AppRegisterMipGenerator.
/// \code
/// if(g_hMipper != NULL)
/// {
///    TC_AppUnregisterMipGenerator(g_hMipper);
///    g_hMipper = NULL;
/// }
/// \endcode
/// \sa \link MipperPlugins Mipper Plugins \endlink, 
/// \link TC_PluginUnload() TC_PluginUnload \endlink, 
/// \link TC_AppRegisterMipGenerator() TC_AppRegisterMipGenerator \endlink
void TC_AppUnregisterMipGenerator(HMIPPER hMipper);

/// \internal
/// Called by View Plugins during their initialization process to let The Compressonator know that this plugin can
/// can support viewing textures.
/// \param[in] pszViewDescription A string description of the view type.
/// \param[in] bDefaultView Set to true if this view type should be the default.
/// \return A HVIEWTYPE that must be saved by this plugin to use in a future call to
/// TC_AppUnregisterViewType if successful, otherwise false.
/// \remarks This functionality is not currently supported in the plugin SDK.
HVIEWTYPE TC_AppRegisterViewType(TCHAR* pszViewDescription, bool bDefaultView);

/// \internal
/// Usually called from a View Plugin's TC_PluginUnload, this function informs The Compressonator that this
/// view type is no longer available.
/// \param[in] hViewType A HVIEWTYPE indicating what view type is being unregistered.
/// \remarks This functionality is not currently supported in the plugin SDK.
void TC_AppUnregisterViewType(HVIEWTYPE hViewType);


/// This function is used to get a pointer to a given MipLevel from a MipSet.
/// \param[in] pMipSet A pointer to the MipSet that contains the MipLevel that you want a pointer to.
/// \param[in] nMipLevel An int that indicates which MipLevel of the texture you want.
/// \param[in] nFaceOrSlice An int that indicates which face of the Cube Map or which slice of the Volume Texture if pMipSet is pointing to a Cube Map or Volume Texture, respectively. If your MipSet is TT_2D, then this should either be 0, or not passed in so that it will default to 0. Non-zero values for TT_2D textures will cause an error. See Remarks for further explanation of this parameter for Cube Maps and Volume Textures.
/// \return A pointer to the requested MipLevel.
/// \remarks You should probably cache the value returned by this function instead of calling it over and over from the same function (such as inside a loop).
/// \remarks The value of nFaceOrSlice for Cube Maps must be less than the nDepth argument passed to TC_AppAllocateMipSet. All miplevels (I use that term in the more general texture sense, not referring to the data structure usually meant by this documentation) of a Cube Map have the same depth.
/// \remarks For Volume Textures, nFaceOrSlice must be less than (nDepth / 2^nMipLevel) truncated to int, until that calculation equals 1, in which case nFaceOrSlice is never less than 0. The reason is each successive miplevel (I use that term in the more general texture sense, not referring to the data structure usually meant by this documentation) of a Volume Texture has half the depth of the previous, rounded down, to a minimum of 1. And since nFaceOrSlice is 0 based, 0 is the minimum value it takes (obviously).
/// \par Example:
/// To get the topmost miplevel:
/// \code
/// MipLevel* m_pMipLevelZero = TC_AppGetMipLevel(m_pMipSet, 0, 0);
/// \endcode
/// \sa \link TC_AppAllocateMipSet() TC_AppAllocateMipSet \endlink
MipLevel* TC_AppGetMipLevel(const MipSet* pMipSet, int nMipLevel, int nFaceOrSlice = 0);

//Memory (de)allocation

/// Usually called by Codec Plugins in both the compression and decompression processs and by File Plugins' TC_PluginFileLoadTexture. Used to allocate memory for an uninitialized MipSet.
/// \param[in, out] pMipSet A pointer to the MipSet you are allocating memory for.
/// \param[in] channelFormat The ChannelFormat of pMipSet, will be assigned to pMipSet->m_ChannelFormat.
/// \param[in] textureDataType The TextureDataType of pMipSet, will be assigned to pMipSet->m_TextureDataType.
/// \param[in] textureType The TextureType of pMipSet, will be assigned to pMipSet->m_TextureType.
/// \param[in] nWidth An int that denotes the width (in pixels) of the topmost MipLevel of the MipSet you are passing in, will be used in determining how much space to allocate for the MipSet, and is assigned to pMipSet->m_nWidth.
/// \param[in] nHeight An int that denotes the height (in pixels) of the topmost MipLevel of the MipSet you are passing in, will be used in determining how much space to allocate for the MipSet, and is assigned to pMipSet->m_nHeight.
/// \param[in] nDepth An int that depends on textureType. Must be 1 for TT_2D. It denotes the number of faces for TT_CubeMap, or number of slices of the highest miplevel for TT_VolumeTexture. Also used to determine how much space to allocate for the MipSet, and is assigned to pMipSet->m_nDepth.
/// \return True if the MipSet was allocated successfully, false otherwise.
/// \remarks The programmer must call this function before calling TC_AppGetMipLevel (or either of the MipLevelData allocation functions). Basically, call this function before doing anything with an unitialized MipSet.
/// \remarks nDepth must be 1 for all 2D textures.
/// \remarks nDepth determines the number of faces that each mip level has in the case of Cube Maps.
/// \remarks In the case of TT_VolumeTexture textures, nDepth determines how many MipLevels comprise the first mip level of the volume texture. It might be better to talk of them as 'slices' within the first mip level of the volume texture, but the function is named TC_AppGetMipLevel because most of the time 2D textures and some cube maps are what's used, so it was deemed clearer to just use the term mip level. Regardless, there will be nDepth 'slices' in the top miplevel of a volume texture, half that many slices in the next miplevel, and so on down to a minimum of one slice per miplevel. See Remarks in MipSet for further explanation.
/// \par Example:
/// Code similar to this could appear in your TC_PluginCodecCompressTexture, TC_PluginCodecDecompressTexture, or TC_PluginFileLoadTexture function.
/// \code
/// if(!TC_AppAllocateMipSet(m_pMipSet, CF_Compressed, TDT_XRGB, TT_2D, imageWidth, imageHeight, 1))
/// {
///    return PE_Unknown;
/// }
/// \endcode
/// \sa \link TC_AppGetMipLevel() TC_AppGetMipLevel \endlink, 
/// \link TC_AppAllocateMipLevelData() TC_AppAllocateMipLevelData \endlink, 
/// \link TC_AppAllocateCompressedMipLevelData() TC_AppAllocateCompressedMipLevelData \endlink
bool TC_AppAllocateMipSet(MipSet* pMipSet, ChannelFormat channelFormat, TextureDataType textureDataType, TextureType textureType, int nWidth, int nHeight, int nDepth = 1);

/// This function is called by plugins to allocate memory whenever an uncompressed MipLevel is being created. Usually needed by Codec Plugins' TC_PluginCodecDecompressTexture function when they are decompressing textures and some File Plugins' TC_PluginFileLoadTexture when they are converting a file on disk into an uncompressed texture in memory.
/// \param[in, out] pMipLevel A pointer to the MipLevel whose m_pbData pointer will point to the allocated memory after this function call.
/// \param[in] nWidth An int that denotes the width (in pixels) of the MipLevel you are passing in, will be assigned to pMipLevel->m_nWidth.
/// \param[in] nHeight An int that denotes the height (in pixels) of the MipLevel you are passing in, will be assigned to pMipLevel->m_nHeight.
/// \param[in] channelFormat A ChannelFormat such as CF_8bit or CF_Float32 that indicates the size in bytes of each color channel. Passing CF_Compressed in will cause errors. Use TC_AppAllocateCompressedMipLevelData if you need compressed texture memory.
/// \param[in] textureDataType A TextureDataType such as TDT_ARGB or TDT_NORMAL_MAP indicating what each channel of the texture represents.
/// \return True if the memory requested was successfully allocated, false otherwise.
/// \remarks Use the pointer returned by TC_AppGetMipLevel as your first argument to this function.
/// \remarks Call this function once for each MipLevel of the texture that you are creating (so if you are only making the highest MipLevel, only call this function once). Do not call this function for memory that is being used solely inside the plugin (if you need temporary memory). Use the standard malloc or new allocators (or your own if you have one) in such a case. If you need memory for a compressed texture, use TC_AppAllocateCompressedMipLevelData instead. Do not try and unallocate memory allocated by this function yourself. Use TC_AppFreeMipLevelData instead.
/// \remarks The channelFormat and textureDataType arguments will be used by this function to determine how much memory to allocate, and this value in bytes is assigned to pMipLevel->dwLinearSize. So for example, if you passed this function 128 for nWidth, 64 for nHeight, CF_8bit for channelFormat and TT_ARGB for textureType, the size of memory allocated for you in bytes would be 128*64*1*4=32768. Each scan line would be byte-aligned, as described in Internal Texture Formats. So the algorithm in this example really is bitsPerPixel=8*4  pitch=((nWidth*bitsPerPixel + 7)/8) truncated to an integer  size of allocated memory=nHeight*pitch, which would still turn out to be 32768. But you don't need to worry about any of this as long as you pass in correct nWidth, nHeight, channelFormat and textureDataType arguments.
/// \par Example:
/// \code
/// if(!TC_AppAllocateMipLevelData(TC_AppGetMipLevel(pMipSetOut, nMipLevel, 0), dwWidth, dwHeight, CF_8bit, TDT_XRGB))
/// {
///    return PE_Unknown;
/// }
/// \endcode
/// \sa \link TC_AppGetMipLevel() TC_AppGetMipLevel \endlink, 
/// \link TC_AppAllocateMipSet() TC_AppAllocateMipSet \endlink, 
/// \link TC_AppAllocateCompressedMipLevelData() TC_AppAllocateCompressedMipLevelData \endlink,
/// \link TC_AppFreeMipLevelData() TC_AppFreeMipLevelData \endlink, 
bool TC_AppAllocateMipLevelData(MipLevel* pMipLevel, int nWidth, int nHeight, ChannelFormat channelFormat, TextureDataType textureDataType);

/// This function is called by plugins to allocate memory whenever a compressed MipLevel is being created. Usually needed by Codec Plugins' TC_PluginCodecCompressTexture function when they are compressing textures and some File Plugins' TC_PluginFileLoadTexture when they are converting a file on disk into a compressed texture in memory.
/// \param[in, out] pMipLevel A pointer to the MipLevel whose m_pbData pointer will point to the allocated memory after this function call.
/// \param[in] nWidth An int that denotes the width (in pixels) of the MipLevel you are passing in, will be assigned to pMipLevel->m_nWidth.
/// \param[in] nHeight An int that denotes the height (in pixels) of the MipLevel you are passing in, will be assigned to pMipLevel->m_nHeight.
/// \param[in] dwSize A DWORD that is the size in bytes of memory you want allocated, will be assigned to pMipLevel->m_dwLinearSize.
/// \return True if the memory requested was successfully allocated, false otherwise.
/// \remarks Use the pointer returned by TC_AppGetMipLevel as your first argument to this function.
/// \remarks Call this function once for each MipLevel of the compressed texture that you are creating (so if you are only making the highest MipLevel, only call this function once). Do not call this function for memory that is being used solely inside the plugin (if you need temporary memory). Use the standard malloc or new allocators (or your own if you have one) in such a case. If you need memory for an uncompressed texture, use TC_AppAllocateMipLevelData instead. Do not try and unallocate memory allocated by this function yourself. Use TC_AppFreeMipLevelData instead.
/// \par Example:
/// \code
/// if(!TC_AppAllocateCompressedMipLevelData(m_pMipLevelZero, GetWidth(), GetHeight(), dwTotalSize))
/// {
///    return PE_Unknown;
/// }
/// \endcode
/// \sa \link TC_AppGetMipLevel() TC_AppGetMipLevel \endlink, 
/// \link TC_AppAllocateMipSet() TC_AppAllocateMipSet \endlink, 
/// \link TC_AppAllocateMipLevelData() TC_AppAllocateMipLevelData \endlink,
/// \link TC_AppFreeMipLevelData() TC_AppFreeMipLevelData \endlink, 
bool TC_AppAllocateCompressedMipLevelData(MipLevel* pMipLevel, int nWidth, int nHeight, CMP_DWORD dwSize);

/// Deallocates a chunk of memory previously allocated by TC_AppAllocateMipLevelData or TC_AppAllocateCompressedMipLevelData.
/// \param[in] pMipLevel Pointer to the MipLevel whose m_pbData member points to the memory to be unallocated.
/// \remarks If your plugin needs to unallocate memory allocated with either  TC_AppAllocateMipLevelData or TC_AppAllocateCompressedMipLevelData, use this function, do not try an unallocate it yourself. Plugins do not need to unallocate textures that they have created when those textures are closed by the user, that is the job of The Compressonator. Plugins should use this function when they they want to create a texture in a MipLevel, but its m_pbData pointer points to a valid memory block. So for example a plugin that generates mipmaps should check that all the MipLevels that it is going to create data in are not already pointing to something, and if they are, the plugin should call this function to free that memory, not overwrite the pointer. This function will assign the value NULL to the m_pbData pointer after unallocating the memory, so comparing the m_pbData pointer to NULL is the recommended way to determine if a m_pbData pointer points to a valid chunk of memory.
/// \par Example:
/// \code
/// TC_AppFreeMipLevelData(TC_AppGetMipLevel(pMipSet, 0));
/// \endcode
/// \sa \link TC_AppAllocateMipLevelData() TC_AppAllocateMipLevelData \endlink,
/// \link TC_AppAllocateCompressedMipLevelData() TC_AppAllocateCompressedMipLevelData \endlink, 
void TC_AppFreeMipLevelData(MipLevel* pMipLevel);

//Other

/// This function is used during texture compression to let The Compressonator update the progress meter and text at the bottom of the window.
/// \param[in] fProgress A percentage indicating the current progress of compression.
/// \param[in] pUser1 An integer indicating which MipLevel is currently being compressed.
/// \param[in] pUser2 An integer indicating the total number of MipLevels that are going to be compressed.
/// \return True if the compression should be aborted, false otherwise.
/// \remarks fProgress should restart for each MipLevel being compressed. So it should go from 0.0 to 100.0 for each MipLevel being compressed.
/// \par Example:
/// If this was the compression loop, and it looped over each scan line of the image:
/// \code
/// //pseudocode
/// for(int i=0; i<textureHeight; i++)
/// {
///    //compress the current scanline here
///    TC_AppCompressTextureCallback(100 * (float)i / textureHeight, currentMipLevel, totalMipLevels)
/// }
/// \endcode
/// \sa \link TC_PluginCodecCompressTexture() TC_PluginCodecCompressTexture \endlink
bool TC_AppCompressTextureCallback(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2);

//View plugin communication

/// \internal
/// 
/// \param[in]
/// \return 
/// \remarks
/// \par Example:
/// 
/// \code
/// \endcode
void TC_AppUpdateViewParent(HWND hParent, unsigned int nWidth, unsigned int nHeight);

/// \internal
/// 
/// \param[in]
/// \return 
/// \remarks
/// \par Example:
/// 
/// \code
/// \endcode
void TC_AppEnableCommand(HWND hParent, unsigned int nCommand, bool bEnable);

/// \internal
/// 
/// \param[in]
/// \return 
/// \remarks
/// \par Example:
/// 
/// \code
/// \endcode
void TC_AppCheckCommand(HWND hParent, unsigned int nCommand, bool bCheck);

#ifdef __cplusplus
};
#endif

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(_TC_PLUGINAPI_INCLUDED_)
