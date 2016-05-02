//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file apExceptionEvent.h 
/// 
//==================================================================================

//------------------------------ apExceptionEvent.h ------------------------------

#ifndef __APEXCEPTIONEVENT
#define __APEXCEPTIONEVENT

// Forward declarations:
template <class TransferableObjectType> class osTransferableObjectCreator;

// Infra:
#include <AMDTBaseTools/Include/AMDTDefinitions.h>
#include <AMDTBaseTools/Include/gtString.h>
#include <AMDTOSWrappers/Include/osExceptionReason.h>

// Local:
#include <AMDTAPIClasses/Include/Events/apEvent.h>


// ----------------------------------------------------------------------------------
// Class Name:           AP_API apExceptionEvent
// General Description:
//   Represents the event of an exception that is thrown by one of the debugged
//   process threads.
// Author:  AMD Developer Tools Team
// Creation Date:        30/3/2004
// ----------------------------------------------------------------------------------
class AP_API apExceptionEvent : public apEvent
{
public:
    apExceptionEvent(osThreadId triggeringThreadId, osExceptionReason exceptionReason,
                     osInstructionPointer exceptionAddress, bool isSecondChance = false);

    // Overrides osTransferableObject:
    virtual osTransferableObjectType type() const;
    virtual bool writeSelfIntoChannel(osChannel& ipcChannel) const;
    virtual bool readSelfFromChannel(osChannel& ipcChannel);

    // Overrides apEvent:
    virtual EventType eventType() const;
    virtual apEvent* clone() const;

    // Self functions:
    osExceptionReason exceptionReason() const { return _exceptionReason; };
    osInstructionPointer exceptionAddress() const { return _exceptionAddress; };
    bool isSecondChance() const { return _isSecondChance; };
    bool isFatalLinuxSignal() const;

private:
    friend class osTransferableObjectCreator<apExceptionEvent>;

    // Do not allow the use of the default constructor:
    apExceptionEvent();

private:
    // The exception reason:
    osExceptionReason _exceptionReason;

    // The address in which the exception occur:
    osInstructionPointer _exceptionAddress;

    // Contains true iff this is a second chance exception:
    // (I.E: The debugged process is about to die)
    bool _isSecondChance;
};


#endif  // __APEXCEPTIONEVENT
