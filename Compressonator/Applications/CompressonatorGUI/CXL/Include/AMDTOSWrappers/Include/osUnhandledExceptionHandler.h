//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osUnhandledExceptionHandler.h
///
//=====================================================================

//------------------------------ osUnhandledExceptionHandler.h ------------------------------

#ifndef __OSUNHANDLEDEXCEPTIONHANDLER_H
#define __OSUNHANDLEDEXCEPTIONHANDLER_H

// Infra:
#include <AMDTBaseTools/Include/AMDTDefinitions.h>

// Local:
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>
#include <AMDTOSWrappers/Include/osOSDefinitions.h>


// ----------------------------------------------------------------------------------
// Class Name:           OS_API osUnhandledExceptionHandler
// General Description:
//   Catches unhandles exceptions that should of cause the CodeXL application to crash
//   and enables handling them a sub class.
//
// Author:      AMD Developer Tools Team
// Creation Date:        21/4/2009
// ----------------------------------------------------------------------------------
class OS_API osUnhandledExceptionHandler
{
public:
    virtual ~osUnhandledExceptionHandler();
    static osUnhandledExceptionHandler* instance();

protected:
    static void registerSingleInstance(osUnhandledExceptionHandler& singleInstance);
    virtual void onUnhandledException(osExceptionCode exceptionCode, void* pExceptionContext) = 0;

    // Only my sub-classes should be able to create me:
    osUnhandledExceptionHandler();

private:
    friend class osSingeltonsDelete;

    // Holds the single instance of this class:
    static osUnhandledExceptionHandler* _pMySingleInstance;

#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    // Allow the below function access to this class:
    friend long osUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* pExceptionInfo);
#endif
};

#endif //__OSUNHANDLEDEXCEPTIONHANDLER_H

