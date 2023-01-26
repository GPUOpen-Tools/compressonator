//=====================================================================
// Copyright 2016-2020 (c), Advanced Micro Devices, Inc. All rights reserved.
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
/// \version 3.1
/// \brief Declares the interface to the Compressonator & ArchitectMF SDK
//
//=====================================================================

#ifndef _PLUGINBASE_H
#define _PLUGINBASE_H

#include <memory>

#ifdef _WIN32
#include <windows.h>
#else
typedef int  HWND;
typedef int* GUID;
#endif

//===========================================================================================
// START OF: BASIC PLUGIN INTERFACE ----- DO NOT CHANGE CODE BELOW FOR BACKWARD SUPPORT------
//===========================================================================================

typedef struct _TC_PluginVersion {
    GUID	 guid;
    uint32_t dwAPIVersionMajor;         // Do not load plugin with greater API major version than app
    uint32_t dwAPIVersionMinor;
    uint32_t dwPluginVersionMajor;
    uint32_t dwPluginVersionMinor;
} TC_PluginVersion;

/// Error codes returned by application & plugin functions.
/// We should obviously return more meaningful error codes than we currently like.
typedef enum {
    PE_OK,            // No error - success.
    PE_AlreadyLoaded, // The plugin is already loaded.
    PE_Unknown,       // An error occured.
    PE_InitErr,       // An error occured during init
    PE_DeviceCreate,  // An error occured durind a device create
} TC_PluginError;

/// Indicates the error level of an error message.
typedef enum {
    EL_Error,      ///< The error message is for an error.
    EL_Warning,    ///< The error message is for a warning.
} TC_ErrorLevel;

//-------------------------
// User defined Interfaces
//------------------------

//#define DECLARE_PLUGIN(x)     extern "C"{__declspec(dllexport) std::unique_ptr<x> makePlugin()  { return std::move(std::make_unique<x>()); }}
#define DECLARE_PLUGIN(x)         extern "C"{__declspec(dllexport) void * makePlugin()       { return new x;}}
#define SET_PLUGIN_TYPE(x)        extern "C"{__declspec(dllexport) char * getPluginType()    { return x; }}
#define SET_PLUGIN_NAME(x)        extern "C"{__declspec(dllexport) char * getPluginName()    { return x; }}
#define SET_PLUGIN_UUID(x)        extern "C"{__declspec(dllexport) char * getPluginUUID()    { return x; }}
#define SET_PLUGIN_CATEGORY(x)    extern "C"{__declspec(dllexport) char * getPluginCategory(){ return x; }}
#define SET_PLUGIN_OPTIONS(x)     extern "C"{__declspec(dllexport) unsigned long  getPluginOptions() { return x; }}

class PluginBase {
  public:
    PluginBase() {}
    virtual ~PluginBase() {}
    virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)=0;
};

typedef void				*(*PLUGIN_FACTORYFUNC)();
typedef char				*(*PLUGIN_TEXTFUNC)();
typedef unsigned long        (*PLUGIN_ULONGFUNC)();

#endif
