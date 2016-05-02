//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osProductVersion.h
///
//=====================================================================

//------------------------------ osProductVersion.h ------------------------------

#ifndef __OSPRODUCTVERSION
#define __OSPRODUCTVERSION

class gtASCIIString;
class gtString;

// Local:
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>


// ----------------------------------------------------------------------------------
// Struct Name:          osProductVersion
// General Description: Represents a product version.
// Author:      AMD Developer Tools Team
// Creation Date:        29/6/2004
// ----------------------------------------------------------------------------------
struct OS_API osProductVersion
{
public:
    osProductVersion();

    void initToZeroVersion();
    bool fromString(const gtString& version);
    bool fromString(const gtASCIIString& version);
    gtString toString(bool fullNumber = true) const;
    bool operator>(const osProductVersion& otherVersion) const;
    bool operator<(const osProductVersion& otherVersion) const;
    bool operator==(const osProductVersion& otherVersion) const;

    // The product major version:
    int _majorVersion;

    // The product minor version:
    int _minorVersion;

    // The patch number (patch level):
    int _patchNumber;

    // The revision number (Source Control revision number):
    int _revisionNumber;
};


#endif  // __OSPRODUCTVERSION
