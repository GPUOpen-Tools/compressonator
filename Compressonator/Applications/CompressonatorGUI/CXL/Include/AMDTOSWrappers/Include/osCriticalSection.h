//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osCriticalSection.h
///
//=====================================================================

//------------------------------ osCriticalSection.h ------------------------------

#ifndef __OSCRITICALSECTION
#define __OSCRITICALSECTION

// Pre-declarations:
class osCriticalSectionImpl;

// Local:
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>


// ----------------------------------------------------------------------------------
// Class Name:           osCriticalSection
//
// General Description:
//   A "critical section" object. Critical section objects enables coordinated access to
//   a resource that is shared by few threads.
//   Only one thread can "enter" the critical section at a time. Other threads that
//   will try to "enter" the critical section will halt until the critical section is
//   "left" by the thread that "owns" it.
//
//   Notice:
//   a. Prefer using osCriticalSectionLocker on using osCriticalSection directly.
//      (See osCriticalSectionLocker for more details).
//
//   b. On Windows, critical sections are only visible inside one process and more
//      efficient than mutexes, since they use only "user mode" execution when a thread
//      tries to "enter" a critical section object that is not "owned" by another thread.
//      (For more details, see MSDN article "A Quick and Versatile Synchronization Object" ->
//       "Critical Section" section).
//      I.E: On Windows:
//      - If you need to synchronize threads within one process - use critical section objects.
//      - If you need to synchronize threads within few process - use mutex objects.
//
// Author:      AMD Developer Tools Team
// Creation Date:        29/3/2004
// ----------------------------------------------------------------------------------
class OS_API osCriticalSection
{
public:
    osCriticalSection();
    virtual ~osCriticalSection();
    void enter();
    bool tryEntering();
    void leave();

private:
    // This class OS specific implementation:
    osCriticalSectionImpl* _pImplementation;
};


#endif  // __OSCRITICALSECTION
