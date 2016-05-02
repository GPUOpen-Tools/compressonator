//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osChannelOperators.h
///
//=====================================================================

//------------------------------ osChannelOperators.h ------------------------------

#ifndef __OSCHANNELOPERATORS
#define __OSCHANNELOPERATORS

// Forward declaration:
class osChannel;
class osTime;
class osRawMemoryStream;

// Infra:
#include <AMDTBaseTools/Include/gtString.h>
#include <AMDTBaseTools/Include/gtASCIIString.h>

// Local:
#include <AMDTOSWrappers/Include/osOSDefinitions.h>
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>


// ----  Write into IPC channel operators ---- :
OS_API osChannel& operator<<(osChannel& ipcChannel, signed char c);
OS_API osChannel& operator<<(osChannel& ipcChannel, unsigned char uc);
OS_API osChannel& operator<<(osChannel& ipcChannel, short s);
OS_API osChannel& operator<<(osChannel& ipcChannel, unsigned short us);
OS_API osChannel& operator<<(osChannel& ipcChannel, bool b);
OS_API osChannel& operator<<(osChannel& ipcChannel, int i);
OS_API osChannel& operator<<(osChannel& ipcChannel, unsigned int ui);
OS_API osChannel& operator<<(osChannel& ipcChannel, long l);
OS_API osChannel& operator<<(osChannel& ipcChannel, unsigned long ul);
OS_API osChannel& operator<<(osChannel& ipcChannel, long long ll);
OS_API osChannel& operator<<(osChannel& ipcChannel, unsigned long long ll);
OS_API osChannel& operator<<(osChannel& ipcChannel, float f);
OS_API osChannel& operator<<(osChannel& ipcChannel, double d);
OS_API osChannel& operator<<(osChannel& ipcChannel, const wchar_t* pString);
OS_API osChannel& operator<<(osChannel& ipcChannel, const gtString& str);
OS_API osChannel& operator<<(osChannel& ipcChannel, const gtASCIIString& str);
OS_API osChannel& operator<<(osChannel& ipcChannel, const osTime& timeToBeSent);
OS_API osChannel& operator<<(osChannel& ipcChannel, osRawMemoryStream& rawMemoryStream);

// ----  Read from IPC channel operators ---- :
OS_API osChannel& operator>>(osChannel& ipcChannel, signed char& c);
OS_API osChannel& operator>>(osChannel& ipcChannel, unsigned char& uc);
OS_API osChannel& operator>>(osChannel& ipcChannel, short& s);
OS_API osChannel& operator>>(osChannel& ipcChannel, unsigned short& us);
OS_API osChannel& operator>>(osChannel& ipcChannel, bool& b);
OS_API osChannel& operator>>(osChannel& ipcChannel, int& i);
OS_API osChannel& operator>>(osChannel& ipcChannel, unsigned int& ui);
OS_API osChannel& operator>>(osChannel& ipcChannel, long& l);
OS_API osChannel& operator>>(osChannel& ipcChannel, unsigned long& ul);
OS_API osChannel& operator>>(osChannel& ipcChannel, long long& ll);
OS_API osChannel& operator>>(osChannel& ipcChannel, unsigned long long& ll);
OS_API osChannel& operator>>(osChannel& ipcChannel, float& f);
OS_API osChannel& operator>>(osChannel& ipcChannel, double& d);
// OS_API osChannel& operator>>(osChannel& ipcChannel, void*& p);
// OS_API osChannel& operator>>(osChannel& ipcChannel, void**& p);
OS_API osChannel& operator>>(osChannel& ipcChannel, gtString& str);
OS_API osChannel& operator>>(osChannel& ipcChannel, gtASCIIString& str);
OS_API osChannel& operator>>(osChannel& ipcChannel, osTime& timeToBeRecieved);

#endif  // __OSCHANNELOPERATORS
