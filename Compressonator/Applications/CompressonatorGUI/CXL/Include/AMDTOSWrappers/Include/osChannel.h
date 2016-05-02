//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osChannel.h
///
//=====================================================================

//------------------------------ osChannel.h ------------------------------

#ifndef __OSCHANNEL
#define __OSCHANNEL

// Local:
#include <AMDTOSWrappers/Include/osOSDefinitions.h>
#include <AMDTOSWrappers/Include/osChannelOperators.h>

// Infinite time out:
#define OS_CHANNEL_INFINIT_TIME_OUT LONG_MAX

// Use no time out (return immediately):
#define OS_CHANNEL_NO_TIME_OUT 0

// Use the per channel type default time out:
#define OS_CHANNEL_DEFAULT_TIME_OUT -1

// ----------------------------------------------------------------------------------
// Class Name:           osChannel
// General Description:
//   Base class for all channel types.
//   A channel enables communication between two entities (processes / algorithms / etc).
//   Data can be written into one side of the channel and read on the other side of
//   the channel.
//
// Author:      AMD Developer Tools Team
// Creation Date:        6/12/2003
// ----------------------------------------------------------------------------------
class OS_API osChannel
{

public:
    // Describes the channel type.
    enum osChannelType
    {
        OS_BINARY_CHANNEL,
        OS_ASCII_TEXT_CHANNEL,
        OS_UNICODE_TEXT_CHANNEL
    };

    // Self functions:
    osChannel();
    virtual ~osChannel();

    long readOperationTimeOut() const { return _readOperationTimeOut;}
    void setReadOperationTimeOut(long timeout);

    long writeOperationTimeOut() const { return _writeOperationTimeOut; }
    void setWriteOperationTimeOut(long timeout);
    void excludeFromCommunicationDebug(bool isExcluded) { _isExcludedFromCommunicationDebug = isExcluded; }

    // Set whether this channel may be accessed by multiple threads (param = false) or just the one that created it (param = true)
    void setSingleThreadAccess(bool isSingleThreadAccess) { _isSingleThreadAccess = isSingleThreadAccess; }

    // Can be overridden by sub-classes:
    virtual long defaultReadOperationTimeOut() const;
    virtual long defaultWriteOperationTimeOut() const;

    // Must be implemented by sub-classes:
    virtual osChannelType channelType() const = 0;
    bool write(const gtByte* pDataBuffer, gtSize_t dataSize);
    bool read(gtByte* pDataBuffer, gtSize_t dataSize);
    bool readAvailableData(gtByte* pDataBuffer, gtSize_t bufferSize, gtSize_t& amountOfDataRead);

    bool writeString(const gtString& str);
    bool readString(gtString& str);

    bool writeString(const gtASCIIString& str);
    bool readString(gtASCIIString& str);

protected:
    /// Derived classes implement the details of writing data through this channel
    virtual bool writeImpl(const gtByte* pDataBuffer, gtSize_t dataSize) = 0;

    /// Derived classes implement the details of reading data through this channel
    virtual bool readImpl(gtByte* pDataBuffer, gtSize_t dataSize) = 0;

    /// Derived classes implement the details of reading data through this channel
    virtual bool readAvailableDataImpl(gtByte* pDataBuffer, gtSize_t bufferSize, gtSize_t& amountOfDataRead) = 0;

    virtual bool writeStringImpl(const gtString& str);
    virtual bool readStringImpl(gtString& str);

    virtual bool writeStringImpl(const gtASCIIString& str);
    virtual bool readStringImpl(gtASCIIString& str);


protected:
    // Read and write operations time out (measured in milliseconds):
    long _readOperationTimeOut;
    long _writeOperationTimeOut;

    bool _isExcludedFromCommunicationDebug;
    gtString _selfDetailsForDebug;
    bool _isSingleThreadAccess;
    osThreadId _idOfThreadThatCreatedMe;

private:
    // Send debug info to debug server before calling write()
    void beforeWrite(const gtByte* pDataBuffer, gtSize_t dataSize);

    // Send debug info to debug server after calling write()
    void afterWrite(gtSize_t dataSize, bool opReturnValue);

    // Send debug info to debug server before calling read()
    void beforeRead(gtSize_t dataSize);

    // Send debug info to debug server after calling read()
    void afterRead(gtByte* pDataBuffer, gtSize_t dataSize, bool opReturnValue);

    // Send debug info to debug server before calling readAvailableData()
    void beforeReadAvailableData(gtSize_t bufferSize);

    // Send debug info to debug server after calling readAvailableData()
    void afterReadAvailableData(gtByte* pDataBuffer, gtSize_t bufferSize, gtSize_t& amountOfDataRead, bool opReturnValue);

    // Send debug info to debug server before calling writeString()
    void beforeWriteString(const gtString& str);

    // Send debug info to debug server after calling writeString()
    void afterWriteString(const gtString& str, bool opReturnValue);

    // Send debug info to debug server before calling readString()
    void beforeReadString();

    // Send debug info to debug server after calling readString()
    void afterReadString(const gtString& str, bool opReturnValue);

    // Send debug info to debug server before calling the ASCII version of writeString()
    void beforeWriteString(const gtASCIIString& str);

    // Send debug info to debug server after calling  the ASCII version of writeString()
    void afterWriteString(const gtASCIIString& str, bool opReturnValue);

    // Send debug info to debug server after calling the ASCII version of readString()
    void afterReadString(const gtASCIIString& str, bool opReturnValue);

};


#endif  // __OSCHANNEL
