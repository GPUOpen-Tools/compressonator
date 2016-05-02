//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtIAssertionFailureHandler.h 
/// 
//=====================================================================

//------------------------------ gtIAssertionFailureHandler.h ------------------------------

#ifndef __GTIASSERTIONFAILUREHANDLER
#define __GTIASSERTIONFAILUREHANDLER

#include <string>


// Local:
#include <AMDTBaseTools/Include/gtGRBaseToolsDLLBuild.h>


// ----------------------------------------------------------------------------------
// Class Name:           gtIAssertionFailureHandler
// General Description:
//  An interface that enables a class to server as an assertion failure handler.
//  When an assertion failure appends, this class onAssertionFailure method will be called.
//  A class instance can register itself as the assertion failure handler by calling
//  gtRegisterAssertHandler.
// Author:      AMD Developer Tools Team
// Creation Date:        30/8/2005
// ----------------------------------------------------------------------------------
class GT_API gtIAssertionFailureHandler
{
public:
    virtual void onAssertionFailure(const wchar_t* functionName, const wchar_t* fileName,
                                    int lineNumber, const wchar_t* message) = 0;
    virtual ~gtIAssertionFailureHandler();
};


#endif  // __GTIASSERTIONFAILUREHANDLER
