//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtGRBaseToolsDLLBuild.h 
/// 
//=====================================================================

//------------------------------ gtGRBaseToolsDLLBuild.h ------------------------------

#ifndef __GTGRBASETOOLSDLLBUILD_H
#define __GTGRBASETOOLSDLLBUILD_H

// Under Win32 builds - define: GT_API to be:
// - When building BaseTools.lib:    default
// - When building GRBaseTools.dll:  __declspec(dllexport).
// - When building other projects:   __declspec(dllimport).
//         Also add -D "AMDTBASETOOLS_EXTERN" for Linux and HSA build infrastructure

#if defined(_WIN32)
    #if defined(AMDTBASETOOLS_EXPORTS)
        #define GT_API __declspec(dllexport)
        #define GT_API_EXTERN __declspec(dllexport)
    #elif defined(AMDTBASETOOLS_STATIC)
        #define GT_API
        #define GT_API_EXTERN
    #else
        #define GT_API __declspec(dllimport)
        #define GT_API_EXTERN __declspec(dllimport)
    #endif
#else
    #define GT_API
#endif
#if defined(AMDTBASETOOLS_EXTERN)
    #define GT_API_EXTERN extern
#endif

#endif //__GTGRBASETOOLSDLLBUILD_H

