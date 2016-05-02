//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osCallsStackReader.h
///
//=====================================================================

//------------------------------ osCallsStackReader.h ------------------------------

#ifndef __OSCALLSSTACKREADER_H
#define __OSCALLSSTACKREADER_H

// Foreward declarations:
class osCallStack;

// Local:
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>

// ----------------------------------------------------------------------------------
// Class Name:           OS_API osCallsStackReader
// General Description:
//  A class that reads a thread's calls stack into an osCallStack Parameter.
//  Note that this class should USE platform-specific implementations (such as
//  osWin32CallsStackReader so they should not inherit it.
//
// Author:      AMD Developer Tools Team
// Creation Date:        23/10/2008
// ----------------------------------------------------------------------------------
class OS_API osCallsStackReader
{
public:
    osCallsStackReader();
    ~osCallsStackReader();

    bool getCurrentCallsStack(osCallStack& callStack, bool shouldIgnoreSpyFrames = true, bool forceWindowsSymbolLoading = false);
    bool getCallStack(osCallStack& callStack, void* pThreadExecutionContext, bool shouldIgnoreSpyFrames = true);
};

#endif //__OSCALLSSTACKREADER_H

