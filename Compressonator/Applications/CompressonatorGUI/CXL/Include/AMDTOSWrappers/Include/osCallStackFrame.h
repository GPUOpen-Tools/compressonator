//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osCallStackFrame.h
///
//=====================================================================

//------------------------------ osCallStackFrame.h ------------------------------

#ifndef __OSCALLSTACKFRAME
#define __OSCALLSTACKFRAME

// Local:
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>
#include <AMDTOSWrappers/Include/osOSDefinitions.h>
#include <AMDTOSWrappers/Include/osTransferableObject.h>
#include <AMDTOSWrappers/Include/osFilePath.h>


// ----------------------------------------------------------------------------------
// Class Name:           AP_API osCallStackFrame
// General Description:
//   Represents a call stack frame (a single function call data).
//
// Author:      AMD Developer Tools Team
// Creation Date:        8/4/2004
// ----------------------------------------------------------------------------------
class OS_API osCallStackFrame : public osTransferableObject
{
public:
    osCallStackFrame();

    virtual ~osCallStackFrame();

    const osFilePath& moduleFilePath() const { return _moduleFilePath; };
    osInstructionPointer functionStartAddress() const { return _functionStartAddress; };
    osInstructionPointer moduleStartAddress() const { return _moduleStartAddress; };
    osInstructionPointer instructionCounterAddress() const { return _instructionCounterAddress; };
    const gtString& functionName() const { return _functionName; };
    bool isSpyFunction() const { return _isSpyFunction; };
    const osFilePath& sourceCodeFilePath() const { return _sourceCodeFilePath; };
    int sourceCodeFileLineNumber() const { return _sourceCodeFileLineNumber; };
    bool isKernelSourceCode() const {return _isKernelSourceCode;};

    void setModuleFilePath(const osFilePath& filePath) { _moduleFilePath = filePath; };
    void setFunctionStartAddress(osInstructionPointer functionStartAddress) { _functionStartAddress = functionStartAddress; };
    void setModuleStartAddress(osInstructionPointer moduleStartAddress) { _moduleStartAddress = moduleStartAddress; };
    void setInstructionCounterAddress(osInstructionPointer instructionCounterAddress) { _instructionCounterAddress = instructionCounterAddress; };
    void setFunctionName(const gtString& functionName) { _functionName = functionName; };
    void markAsSpyFunction(bool isSpyFunc = true) { _isSpyFunction = isSpyFunc; };
    void setSourceCodeFilePath(const osFilePath& sourceCodeFilePath) { _sourceCodeFilePath = sourceCodeFilePath; };
    void setSourceCodeFileLineNumber(int lineNumber) { _sourceCodeFileLineNumber = lineNumber; };
    void setKernelSourceCode(bool isKernelCode) {_isKernelSourceCode = isKernelCode;};

    // Overrides osTransferableObject
    virtual osTransferableObjectType type() const {return OS_TOBJ_ID_CALLS_STACK_FRAME;};
    virtual bool writeSelfIntoChannel(osChannel& ipcChannel) const;
    virtual bool readSelfFromChannel(osChannel& ipcChannel);

private:
    // The path of the module file (dll / exe) that contains the function that is
    // executed in this stack frame:
    // (It can contain a full or partial file path)
    osFilePath _moduleFilePath;

    // The starting address of assembly code of the function that is executed in this stack
    // frame. (This parameter will contain NULL when the function symbol info is not found)
    osInstructionPointer _functionStartAddress;

    // The starting address of the module image in memory where the function was called from.
    // (This parameter will contain NULL when the function symbol info is not found)
    osInstructionPointer _moduleStartAddress;

    // A address of the machine code instruction on which the instruction pointer
    // register points in this stack frame:
    osInstructionPointer _instructionCounterAddress;

    // The name of the function that this call stack frame executes:
    // (If there is no appropriate debug info this name will be empty).
    gtString _functionName;

    // Contains true iff the function belongs to a "spy" dll:
    bool _isSpyFunction;

    // The path of the source code file associated with this stack frame:
    // (If there is no appropriate debug info it will be an empty path).
    osFilePath _sourceCodeFilePath;

    // The source code file line number associated with this stack frame:
    // (If there is no appropriate debug info it contain -1).
    int _sourceCodeFileLineNumber;

    // Is this kernel source or c++ source?
    bool _isKernelSourceCode;
};


#endif  // __OSCALLSTACKFRAME
