//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acApplicationComponentsDLLBuild.h 
/// 
//================================================================================== 

//------------------------------ acApplicationComponentsDLLBuild.h ------------------------------

#ifndef __ACAPPLICATIONCOMPONENTSDLLBUILD
#define __ACAPPLICATIONCOMPONENTSDLLBUILD

// Under Win32 builds - define: AC_API to be:
// - When building GRApplicationComponents.dll:     __declspec(dllexport).
// - When building other projects:                  __declspec(dllimport).

#if defined(_WIN32)
    #if defined(AMDTAPPLICATIONCOMPONENTS_EXPORTS)
        #define AC_API __declspec(dllexport)
    #else
        #define AC_API __declspec(dllimport)
    #endif
#else
    #define AC_API
#endif


#endif  // __ACAPPLICATIONCOMPONENTSDLLBUILD
