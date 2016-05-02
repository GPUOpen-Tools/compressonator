//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osOSWrappersDLLBuild.h
///
//=====================================================================

//------------------------------ osOSWrappersDLLBuild.h ------------------------------

#ifndef __OSOSWRAPPERSDLLBUILD
#define __OSOSWRAPPERSDLLBUILD

// Under Win32 builds - define: OW_API to be:
// - When building OSWrappers.lib:        default
// - When building OSWrappersDLL.dll:     __declspec(dllexport).
// - When building other projects:        __declspec(dllimport).

#if defined(_WIN32)
    #if defined(AMDTOSWRAPPERS_EXPORTS)
        #define OS_API __declspec(dllexport)
    #elif defined(AMDTOSWRAPPERS_STATIC)
        #define OS_API
    #else
        #define OS_API __declspec(dllimport)
    #endif
#else
    #define OS_API
#endif


#endif  // __OSOSWRAPPERSDLLBUILD
