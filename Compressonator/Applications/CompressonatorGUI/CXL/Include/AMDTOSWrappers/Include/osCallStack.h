//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osCallStack.h
///
//=====================================================================

//------------------------------ osCallStack.h ------------------------------

#ifndef __OSCALLSTACK
#define __OSCALLSTACK

// Infra:
#include <AMDTBaseTools/Include/gtVector.h>

// Local:
#include <AMDTOSWrappers/Include/osTransferableObject.h>
#include <AMDTOSWrappers/Include/osCallStackFrame.h>


// ----------------------------------------------------------------------------------
// Class Name:           AP_API osCallStack
// General Description:
//   Represents a thread call stack.
//
// Author:      AMD Developer Tools Team
// Creation Date:        8/4/2004
// ----------------------------------------------------------------------------------
class OS_API osCallStack : public osTransferableObject
{
public:
    osCallStack(osThreadId threadId = OS_NO_THREAD_ID, bool is64BitCallStack = false);

    osThreadId threadId() const { return _threadId; };
    bool is64BitCallStack() const { return _is64BitCallStack; };
    int amountOfStackFrames() const { return (int)_stackFrames.size(); };
    void reserveStack(int stackSize) { _stackFrames.reserve(stackSize); }
    const osCallStackFrame* stackFrame(int frameIndex) const;
    void asString(gtString& callStackBriefString, gtString& callStackString, bool& isSpyModuleRelatedCallStack, bool allowDifferentSystemPathInSpy = true) const;

    void clearStack();
    void setThreadId(osThreadId threadId) { _threadId = threadId; };
    void setAddressSpaceType(bool is64BitCallStack) { _is64BitCallStack = is64BitCallStack; };
    void addStackFrame(const osCallStackFrame& stackFrame);
    bool setStackFrame(const osCallStackFrame& stackFrame, int index);
    void append(const osCallStack& subStack);

    // Overrides osTransferableObject
    virtual osTransferableObjectType type() const {return OS_TOBJ_ID_CALLS_STACK;};
    virtual bool writeSelfIntoChannel(osChannel& ipcChannel) const;
    virtual bool readSelfFromChannel(osChannel& ipcChannel);

private:
    // The id of the thread to which the call stack belongs:
    osThreadId _threadId;

    // true for 64 bit address space call stack, false for 32 bit address space call stack:
    bool _is64BitCallStack;

    // The call stack frames:
    gtVector<osCallStackFrame> _stackFrames;
};


#endif  // __OSCALLSTACK
