//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
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
/// \file PluginInterface.h
/// \version 2.20
/// \brief Declares the interface to the AMDCompress library.
//
//=====================================================================

#ifndef _PLUGININTERFACE_H
#define _PLUGININTERFACE_H

#include "stdafx.h"
#include "Compressonator.h"

//===========================================================================================
// START OF: BASIC PLUGIN INTERFACE ----- DO NOT CHANGE CODE BELOW FOR BACKWARD SUPPORT------
//===========================================================================================

#define TC_API_VERSION_MAJOR 1
#define TC_API_VERSION_MINOR 4

typedef struct _TC_PluginVersion
{
   GUID  guid;
   DWORD dwAPIVersionMajor;		// Do not load plugin with greater API major version than app
   DWORD dwAPIVersionMinor;
   DWORD dwPluginVersionMajor;
   DWORD dwPluginVersionMinor;
} TC_PluginVersion;

/// Error codes returned by application & plugin functions.
/// We should obviously return more meaningful error codes than we currently like.
typedef enum
{
	PE_OK,            ///< No error - success.
	PE_AlreadyLoaded, ///< The plugin is already loaded. 
	PE_Unknown,       ///< An error occured.
} TC_PluginError;

/// Indicates the error level of an error message.
typedef enum
{
	EL_Error,      ///< The error message is for an error.
	EL_Warning,    ///< The error message is for a warning.
} TC_ErrorLevel;

#define DECLARE_PLUGIN(x)		extern "C"{__declspec(dllexport) void * makePlugin()   { return new x;}}
#define SET_PLUGIN_TYPE(x)		extern "C"{__declspec(dllexport) char * getPluginType(){ return x;}}
#define SET_PLUGIN_NAME(x)		extern "C"{__declspec(dllexport) char * getPluginName(){ return x;}}

class PluginBase 
{
public:
		PluginBase(){}
		virtual ~PluginBase(){}
		virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)=0;
};

typedef PluginBase*		(*PLUGIN_FACTORYFUNC)();
typedef char * (*PLUGIN_TEXTFUNC)();

//===========================================================================================
// END OF: BASIC PLUGIN INTERFACE ----- DO NOT CHANGE CODE ABOVE FOR BACKWARD SUPPORT------
//===========================================================================================

#include "Texture.h"
#include "pluginManager.h"

#include "MIPS.h"

typedef DWORD_PTR TC_HANDLE;  ///< Generic Texture API handle
typedef TC_HANDLE HFILETYPE;  ///< Handle to a FileType.
typedef TC_HANDLE HCODEC;     ///< Handle to a Codec.
typedef TC_HANDLE HMIPPER;    ///< Handle to a Mipper.
typedef TC_HANDLE HVIEWTYPE;  ///< \internal Handle to a ViewType.
typedef TC_HANDLE HVIEW;      ///< \internal Handle to a View.


class PluginInterface_Image : PluginBase
{
public:
		PluginInterface_Image(){}
		virtual ~PluginInterface_Image(){}
		virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)=0;
		virtual int TC_PluginSetSharedIO(void* Shared)=0;

		virtual int TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet) = 0;
		virtual int TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet) = 0;
		virtual int TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture) = 0;
		virtual int TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture) = 0;
};


class PluginInterface_Codec : PluginBase
{
public:
		PluginInterface_Codec(){}
		virtual ~PluginInterface_Codec(){}
		virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)=0;
};


// These type of plugins are used to Analyze images
class PluginInterface_Analysis : PluginBase
{
public:
		PluginInterface_Analysis(){}
		virtual ~PluginInterface_Analysis(){}
		virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)=0;
		virtual int TC_ImageDiff(const char *in1, const char *in2, const char *out, char *resultsFile, void *pluginManager, void **cmipImages) { (void)in1, (void)in2, (void)out, (void)resultsFile; (void)pluginManager; (void*)cmipImages; return 0; };
        virtual int TC_PSNR_MSE(const char *in1, const char *in2, char *resultsFile, void *pluginManager) { (void)in1, (void)in2, (void)resultsFile; (void)pluginManager; return 0; };
        virtual int TC_SSIM(const char *in1, const char *in2, char *resultsFile, void *pluginManager) { (void)in1, (void)in2, (void)resultsFile; (void)pluginManager; return 0; };
};


// These type of plugins are used to Generate or transform images
class PluginInterface_Filters : PluginBase
{
public:
		PluginInterface_Filters(){}
		virtual ~PluginInterface_Filters(){}
		virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)=0;
		virtual int TC_GenerateMIPLevels(MipSet *pMipSet, int nMinSize)=0;
};


// Feature driven classes based on Base Plugin interface
typedef PluginInterface_Codec*		(*PLUGIN_FACTORYFUNC_CODEC)();
typedef PluginInterface_Image*		(*PLUGIN_FACTORYFUNC_IMAGE)();
typedef PluginInterface_Analysis*	(*PLUGIN_FACTORYFUNC_ANALYSIS)();
typedef PluginInterface_Filters*	(*PLUGIN_FACTORYFUNC_FILTERS)();

#endif