//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osCriticalSectionLocker.h
///
//=====================================================================

//------------------------------ osCriticalSectionLocker.h ------------------------------

#ifndef __OSCRITICALSECTIONLOCKER
#define __OSCRITICALSECTIONLOCKER

// Local:
#include <AMDTOSWrappers/Include/osCriticalSection.h>


// ----------------------------------------------------------------------------------
// Class Name:           OS_API osCriticalSectionLocker
// General Description:
//   Aid class that enables "exception safe" locking of a critical section object.
//   Its constructor "enters" the critical section and destructor "leaves" the critical section.
//   This causes critical section to be "left" in case of an exception.
//   Example:
//     void foo(osCriticalSection& myCriticalSection)
//     {
//         osCriticalSectionLocker criticalSectionLucker(myCriticalSection);
//
//         < doing something >
//
//     }
//
//     In the above example, the critical section will be "left" in the following scenarios:
//     a. The thread exits the function: Exiting the function executes the osCriticalSectionLucker
//        destructor, which leaves the critical section.
//     b. An exception is thrown while < doing something > is executed.
//        If there is no exception handler in the function, the exception will be "thrown out"
//        of the function, calling all the destructors of the function stack variables.
//        Among these destructors is the osCriticalSectionLucker destructor, which will "leave"
//        the critical section.
//
//     The user can also call leaveCriticalSection() manually to "leave the critical section
//     before the osCriticalSectionLucker destructor is called.
//
// Author:      AMD Developer Tools Team
// Creation Date:        18/5/2005
// ----------------------------------------------------------------------------------
class OS_API osCriticalSectionLocker
{
public:
    osCriticalSectionLocker(osCriticalSection& criticalSectionObj);
    ~osCriticalSectionLocker();

    void leaveCriticalSection();

private:
    // Disallow use of the default constructor and assignment operator:
    osCriticalSectionLocker(const osCriticalSectionLocker& other) = delete;
    osCriticalSectionLocker& operator=(const osCriticalSectionLocker& other) = delete;

private:
    // The critical section on which this class operates:
    osCriticalSection& _criticalSection;

    // Contains true iff the critical section was already "left" by this class:
    bool _wasCriticalSectionLeft;
};


// ----------------------------------------------------------------------------------
// Class Name:           OS_API osCriticalSectionDelayedLocker
// General Description:
//   Aid class that enables "exception safe" locking of a critical section object.
//   Operates similar to osCriticalSectionLocker, only allows late setting of the CS object.
//   Destructor "leaves" the critical section.
//   This causes critical section to be "left" in case of an exception.
//   Example:
//     void foo(osCriticalSection& myCriticalSection)
//     {
//         osCriticalSectionDelayedLocker criticalSectionDelayedLocker();
//
//         < doing something >
//
//         criticalSectionDelayedLocker.attachToCriticalSection(myCriticalSection);
//
//         < doing something else >
//
//     }
//
//     The user can also call leaveCriticalSection() manually to "leave the critical section
//     before the osCriticalSectionLucker destructor is called.
//
//     The leaveCriticalSection() method also clears the member pointer, allowing the locker object to be re-used.
//     However, since this class itself is not guarded, it is not recommended to use it on two different
//     CS objects, rather only re-using it to re-lock the same critical section.
//
// Author:      AMD Developer Tools Team
// Creation Date:        5/7/2015
// ----------------------------------------------------------------------------------
class OS_API osCriticalSectionDelayedLocker
{
public:
    osCriticalSectionDelayedLocker();
    ~osCriticalSectionDelayedLocker();

    bool attachToCriticalSection(osCriticalSection& criticalSectionObj);
    void leaveCriticalSection();

private:
    // Disallow use of the default constructor and assignment operator:
    osCriticalSectionDelayedLocker(const osCriticalSectionDelayedLocker& other) = delete;
    osCriticalSectionDelayedLocker& operator=(const osCriticalSectionDelayedLocker& other) = delete;

private:
    // The critical section on which this class operates:
    osCriticalSection* m_pCriticalSection;
};


#endif  // __OSCRITICALSECTIONLOCKER
