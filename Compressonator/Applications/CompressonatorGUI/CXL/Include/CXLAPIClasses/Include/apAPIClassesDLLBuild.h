//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file apAPIClassesDLLBuild.h
///
//==================================================================================

//------------------------------ apAPIClassesDLLBuild.h ------------------------------

#ifndef __APAPICLASSESDLLBUILD
#define __APAPICLASSESDLLBUILD

// Under Win32 builds - define: AP_API to be:
// - When building APIClasses.dll:     __declspec(dllexport).
// - When building other projects:     __declspec(dllimport).

#if defined(_WIN32)
    #if defined(AMDTAPICLASSES_EXPORTS)
        #define AP_API __declspec(dllexport)
    #else
        #define AP_API __declspec(dllimport)
    #endif
#else
    #define AP_API
#endif


#endif  // __APAPICLASSESDLLBUILD
