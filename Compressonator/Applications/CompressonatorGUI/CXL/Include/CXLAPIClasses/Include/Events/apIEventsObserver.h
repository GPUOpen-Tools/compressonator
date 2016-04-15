//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file apIEventsObserver.h 
/// 
//==================================================================================

//------------------------------ apIEventsObserver.h ------------------------------

#ifndef __APIEVENTSOBSERVER_H
#define __APIEVENTSOBSERVER_H

// Local:
#include <CXLAPIClasses/Include/Events/apEvent.h>
#include <CXLAPIClasses/Include/apAPIClassesDLLBuild.h>


// ----------------------------------------------------------------------------------
// Interface Name:      AP_API apIEventsObserver
// General Description:
//  Interface that enables listening to the debugged process events.
//
// Author:  AMD Developer Tools Team
// Creation Date:       9/11/2003
// ----------------------------------------------------------------------------------
class AP_API apIEventsObserver
{
public:
    virtual ~apIEventsObserver();

    // Debugged process events callback function:
    // (This function should be implemented by sub classes)
    virtual void onEvent(const apEvent& eve, bool& vetoEvent) = 0;

    // Should be implemented in sub classes - is used for log:
    virtual const wchar_t* eventObserverName() const = 0;

    virtual void onEventRegistration(apEvent& eve, bool& vetoEvent);

};


#endif //__APIEVENTSOBSERVER_H

