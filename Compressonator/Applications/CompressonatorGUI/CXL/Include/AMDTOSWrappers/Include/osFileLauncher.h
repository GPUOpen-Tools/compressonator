//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osFileLauncher.h
///
//=====================================================================

//------------------------------ osFileLauncher.h ------------------------------

#ifndef __OSFILELAUNCHER
#define __OSFILELAUNCHER

// Infra:
#include <AMDTBaseTools/Include/gtString.h>

// Local:
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>


// ----------------------------------------------------------------------------------
// Class Name:           OS_API osFileLauncher
// General Description:
//   Enables launching a File in an internet browser.
// Author:      AMD Developer Tools Team
// Creation Date:        29/7/2004
// ----------------------------------------------------------------------------------
class OS_API osFileLauncher
{
public:
    osFileLauncher(const gtString& FileToBeLaunched, bool launchFileInDifferentThread = true);
    osFileLauncher(const gtString& FileToBeLaunched, const gtString& commandLineParameters, bool launchFileInDifferentThread = true);
    bool launchFile();

private:
    // Do not allow the use of the default constructor:
    osFileLauncher();

private:
    // The path / URL of the file to be launched:
    gtString _fileToBeLaunched;

    gtString _commandLineParameters;

    // Will the file be launched in a different thread:
    bool _launchFileInDifferentThread;
};

// A function that waits for the launched file and gets its return code:
bool OS_API osLaunchFileAndGetReturnCode(const gtString& fileToBeLaunched, const gtString& commandLineParameters, long& retCode);

#endif  // __OSFILELAUNCHER
