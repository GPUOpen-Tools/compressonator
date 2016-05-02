//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtIgnoreCompilerWarnings.h
///
//=====================================================================

//------------------------------ gtIgnoreCompilerWarnings.h ------------------------------

#ifndef __GTIGNORECOMPILERWARNINGS_H
#define __GTIGNORECOMPILERWARNINGS_H

// Local:
#include <AMDTBaseTools/Include/AMDTDefinitions.h>


// ----------------------------------------------------------------------------------
// File Name:            gtIgnoreCompilerWarnings
// General Description:
//  This file contains commands that instruct the compiler to ignore warnings
//  that we thing should be GLOBALLY ignored.
//
//  If you would like to have a single warning ignored for a given code, please
//  use #pragma warning( push ) and #pragma warning( pop ) instead.
//
// Author:      AMD Developer Tools Team
// Creation Date:        11/10/2006
// ----------------------------------------------------------------------------------

// If this is a Microsoft compiler build:
#if AMDT_CPP_COMPILER == AMDT_VISUAL_CPP_COMPILER

    // TO_DO: Unicode - should we skip this one?
    #pragma warning( disable : 4005 )

    // Eliminate CRT deprecation warnings:
    // (We cannot use Microsoft's CRT Security Enhancements, since they are not supported on Linux).
    #pragma warning( disable : 4996 )

    // Error description: 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    // However, this warning is caused by private members that do not have DLL exports definitions. Therefore, we disable
    // this warning globally.
    #pragma warning( disable : 4251 )

    // Error description:'identifier' : identifier was truncated to 'number' characters.
    // However the use of STL causes the generation of a lot of such errors. Therefore, we disable
    // this warning globally.
    #pragma warning( disable : 4786 )

    // Error description: error C4592: symbol will be dynamically initialized.
    // cAn be ignored http://stackoverflow.com/questions/34013930/error-c4592-symbol-will-be-dynamically-initialized-vs2015-1-static-const-std
    #pragma warning( disable : 4592 )

#endif // AMDT_CPP_COMPILER



#endif //__GTIGNORECOMPILERWARNINGS_H
