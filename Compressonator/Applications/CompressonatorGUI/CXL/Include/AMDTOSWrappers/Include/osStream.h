//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osStream.h
///
//=====================================================================

//------------------------------ osStream.h ------------------------------

#ifndef __OSSTREAM
#define __OSSTREAM

// Local:
#include <AMDTOSWrappers/Include/osChannel.h>


// ----------------------------------------------------------------------------------
// Class Name:           OS_API osStream : public osChannel
//
// General Description:
//   Represents a stream. A stream in a channel that enables seek operations.
//
// Author:      AMD Developer Tools Team
// Creation Date:        15/5/2004
// ----------------------------------------------------------------------------------
class OS_API osStream : public osChannel
{
public:
    // Predefined stream positions:
    enum osStreamPosition
    {
        OS_STREAM_BEGIN,
        OS_STREAM_CURRENT_POSITION,
        OS_STREAM_END
    };

    bool seekCurrentPosition(gtSize_t offset);
    gtSize_t currentPosition() const;

    // Must be implemented by sub-classes:
    virtual bool isOpened() const = 0;
    virtual bool seekCurrentPosition(osStreamPosition seekStartPosition, gtSize_t offset) = 0;
    virtual bool currentPosition(osStreamPosition positionReference, gtSize_t& offset) const = 0;
};


#endif  // __OSSTREAM
