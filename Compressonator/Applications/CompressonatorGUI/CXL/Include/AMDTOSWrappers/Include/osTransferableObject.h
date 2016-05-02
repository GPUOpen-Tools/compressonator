//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osTransferableObject.h
///
//=====================================================================

//------------------------------ osTransferableObject.h ------------------------------

#ifndef __OSTRANSFERABLEOBJECT
#define __OSTRANSFERABLEOBJECT

// Pre-declarations:
class osChannel;

// Infra:
#include <AMDTBaseTools/Include/gtAutoPtr.h>

// Local:
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>
#include <AMDTOSWrappers/Include/osTransferableObjectType.h>



// ------------------------------ Notice ---------------------------------
//
//      Each sub-class of osTransferableObject should:
//      a. Have an enum value in osTransferableObjectType.
//      b. Register it's creator in osTransferableObjectsCreatorsManager.
//         Using an osTransferableObjectCreator object.
//
// ------------------------------ Notice ---------------------------------



// ----------------------------------------------------------------------------------
// Class Name:           OS_API osTransferableObject
//
// General Description:
//   Base class for classes that are "transferable" through channels.
//   Sub-classes are forced to implement "type", "writeSelfIntoChannel" and "readSelfFromChannel"
//   methods. This enables their transfer through a the channel.
//
// Author:      AMD Developer Tools Team
// Creation Date:        31/1/2004
// ----------------------------------------------------------------------------------
class OS_API osTransferableObject
{
public:
    osTransferableObject() {};
    virtual ~osTransferableObject();

public:
    // Get the transferable object type:
    virtual osTransferableObjectType type() const = 0;

    // Am I an apParameter ?
    virtual bool isParameterObject() const;

    // Am I an apEvent ?
    virtual bool isEventObject() const;

    // Am I an apCLEnqueuedCommand ?
    virtual bool isCLEnqueuedCommandObject() const;

    // Write to / Read from an IPC channel:

    // ------------------------------ Notice ---------------------------------
    //
    //  Even in failure cases, sub-classes must complete their read / write operations.
    //  (Stopping the read / write operation on failure can yield inconsistent
    //   read and write, which leads to a corrupted channel !! ).
    //
    // ------------------------------ Notice ---------------------------------
    virtual bool writeSelfIntoChannel(osChannel& ipcChannel) const = 0;
    virtual bool readSelfFromChannel(osChannel& ipcChannel) = 0;

    osTransferableObject* clone() const;
};


// ----  Write into / read from IPC channel operators ---- :

OS_API osChannel& operator<<(osChannel& ipcChannel, const osTransferableObject& transferableObj);
OS_API osChannel& operator>>(osChannel& ipcChannel, gtAutoPtr<osTransferableObject>& aptrTransferableObj);


// ---------------------------------------------------------------------------
// Name:        osReadTransferableObjectFromChannel
//
// Description:
//   Reads a transferable object from a channel.
//   Use this function instead of operator>> if you know the type of the
//   transferable object that is about to be read.
//
// Template arguments:
//   TransferableObjType - The type of the transferable object to be read.
//
// Arguments:  ipcChannel - The channel from which we will read the transferable obj.
//             aptrTransferableObj - Will get the created and read transferable obj.
//
// Return Val:  bool - Success / failure.
// Author:      AMD Developer Tools Team
// Date:        16/5/2004
// ---------------------------------------------------------------------------
template<class TransferableObjType>
bool osReadTransferableObjectFromChannel(osChannel& ipcChannel, gtAutoPtr<TransferableObjType>& aptrTransferableObj)
{
    bool retVal = false;

    // Read a transferable object from the channel:
    gtAutoPtr<osTransferableObject> aptrReadTransferableObj;
    ipcChannel >> aptrReadTransferableObj;

    // Verify that we read an object of the right type:
    TransferableObjType tempObj;

    if (tempObj.type() == aptrReadTransferableObj->type())
    {
        // Output the read transferable object:
        aptrTransferableObj = (TransferableObjType*)(aptrReadTransferableObj.releasePointedObjectOwnership());
        retVal = true;
    }

    return retVal;
}


#endif  // __OSTRANSFERABLEOBJECT
