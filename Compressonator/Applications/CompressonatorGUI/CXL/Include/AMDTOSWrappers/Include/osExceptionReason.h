//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osExceptionReason.h
///
//=====================================================================

//------------------------------ osExceptionReason.h ------------------------------

#ifndef __OSEXCEPTIONREASON_H
#define __OSEXCEPTIONREASON_H

// Infra:
#include <AMDTBaseTools/Include/gtString.h>

// Local:
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>
#include <AMDTOSWrappers/Include/osOSDefinitions.h>


// ----------------------------------------------------------------------------------
// Class Name:           osExceptionReason
// General Description:
//   Represents an exception reason.
//
// Author:      AMD Developer Tools Team
// Creation Date:        30/3/2004
// ----------------------------------------------------------------------------------
enum osExceptionReason
{
    OS_UNKNOWN_EXCEPTION_REASON,    // An unknown exception was encountered.

    OS_STANDALONE_THREAD_STOPPED,   // Exception on standalone thread was stopped

    OS_INSUFFICIENT_MEMORY,         // Insufficient memroy (used in cases an exception like std::bad_alloc is being thrown).

    OS_ACCESS_VIOLATION,            // The thread tried to read from or write to a virtual address
    // to which it does not have access.

    OS_ARRAY_BOUNDS_EXCEEDED,       // The thread tried to access an array element that is out of
    // bounds and the underlying hardware supports bounds checking.

    OS_DATATYPE_MISALIGNMENT,       // The thread tried to read or write data that is misaligned
    // on hardware that does not provide alignment.
    // For example, 16-bit values must be aligned on 2-byte boundaries;
    // 32-bit values on 4-byte boundaries, and so on.

    OS_FLT_DENORMAL_OPERAND,        // One of the operands in a floating-point operation is denormal.
    // A denormal value is one that is too small to represent as a
    // standard floating-point value.

    OS_FLT_DIVIDE_BY_ZERO,          // The thread tried to divide a floating-point value by a
    // floating-point divisor of zero.

    OS_FLT_INEXACT_RESULT,          // The result of a floating-point operation cannot be represented
    // exactly as a decimal fraction.

    OS_FLT_INVALID_OPERATION,       // This exception represents any floating-point exception not
    // included in this list.

    OS_FLT_OVERFLOW,                // The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.

    OS_FLT_STACK_CHECK,             // The stack overflowed or underflowed as the result of a floating-point operation.

    OS_FLT_UNDERFLOW,               // The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.

    OS_ILLEGAL_INSTRUCTION,         // The thread tried to execute an invalid instruction.

    OS_IN_PAGE_ERROR,               // The thread tried to access a page that was not present, and
    // the system was unable to load the page.
    // For example, this exception might occur if a network connection
    // is lost while running a program over the network.

    OS_INT_DIVIDE_BY_ZERO,          // The thread tried to divide an integer value by an integer
    // divisor of zero.

    OS_INT_OVERFLOW,                // The result of an integer operation caused a carry out of
    // the most significant bit of the result.

    OS_INVALID_DISPOSITION,         // An exception handler returned an invalid disposition to the
    // exception dispatcher. Programmers using a high-level language
    // such as C should never encounter this exception.

    OS_NONCONTINUABLE_EXCEPTION,    // The thread tried to continue execution after a noncontinuable
    // exception occurred.

    OS_PRIV_INSTRUCTION,            // The thread tried to execute an instruction whose operation
    // is not allowed in the current machine mode.

    OS_SINGLE_STEP,                 // A trace trap or other single-instruction mechanism signaled
    // that one instruction has been executed.

    OS_STACK_OVERFLOW,              // The thread used up its stack.

    OS_DLL_NOT_FOUND,               // A required dll file was not found.

    // Linux signals:

    OS_SIGHUP_SIGNAL,           // Hangup detected on controlling terminal.

    OS_SIGINT_SIGNAL,           // Interrupt from keyboard

    OS_SIGQUIT_SIGNAL,          // Quit from keyboard

    OS_SIGILL_SIGNAL,           // Illegal Instruction

    OS_SIGTRAP_SIGNAL,          // Trace/breakpoint trap

    OS_SIGABRT_SIGNAL,          // Abort signal from abort(3)

    OS_SIGBUS_SIGNAL,           // Bus error.

    OS_SIGFPE_SIGNAL,           // Floating point exception.

    OS_SIGKILL_SIGNAL,          // Kill signal.

    OS_SIGSEGV_SIGNAL,          // Invalid memory reference.

    OS_SIGPIPE_SIGNAL,          // Broken pipe: write to pipe with no readers.

    OS_SIGALRM_SIGNAL,          // Timer signal from alarm(2).

    OS_SIGTERM_SIGNAL,          // Termination signal.

    OS_SIGUSR1_SIGNAL,          // User-defined signal 1.

    OS_SIGUSR2_SIGNAL,          // User-defined signal 2.

    OS_SIGEMT_SIGNAL,           // Emulation trap

    OS_SIGSYS_SIGNAL,           // Bad system call

    OS_SIGURG_SIGNAL,           // Urgent I/O condition

    OS_SIGSTOP_SIGNAL,          // Stopped (signal)

    OS_SIGTSTP_SIGNAL,          // Stopped (user)

    OS_SIGCONT_SIGNAL,          // Continued

    OS_SIGCHLD_SIGNAL,          // Child status changed

    OS_SIGTTIN_SIGNAL,          // Stopped (tty input)

    OS_SIGTTOU_SIGNAL,          // Stopped (tty output)

    OS_SIGIO_SIGNAL,            // I/O possible

    OS_SIGXCPU_SIGNAL,          // CPU time limit exceeded

    OS_SIGXFSZ_SIGNAL,          // File size limit exceeded

    OS_SIGVTALRM_SIGNAL,        // Virtual timer expired

    OS_SIGPROF_SIGNAL,          // Profiling timer expired

    OS_SIGWINCH_SIGNAL,         // Window size changed

    OS_SIGLOST_SIGNAL,          // Resource lost

    OS_SIGPWR_SIGNAL,           // Power fail/restart

    OS_SIGPOLL_SIGNAL,          // Pollable event occurred

    OS_SIGWIND_SIGNAL,          // SIGWIND

    OS_SIGPHONE_SIGNAL,         // SIGPHONE

    OS_SIGWAITING_SIGNAL,       // Process's LWPs are blocked

    OS_SIGLWP_SIGNAL,           // Signal LWP

    OS_SIGDANGER_SIGNAL,        // Swap space dangerously low

    OS_SIGGRANT_SIGNAL,         // Monitor mode granted

    OS_SIGRETRACT_SIGNAL,       // Need to relinquish monitor mode

    OS_SIGMSG_SIGNAL,           // Monitor mode data available

    OS_SIGSOUND_SIGNAL,         // Sound completed

    OS_SIGSAK_SIGNAL,           // Secure attention

    OS_SIGPRIO_SIGNAL,          // SIGPRIO

    OS_SIGCANCEL_SIGNAL,        // LWP internal signal

    OS_EXC_BAD_ACCESS_SIGNAL,   // Could not access memory

    OS_EXC_BAD_INSTRUCTION_SIGNAL,  // Illegal instruction/operand

    OS_EXC_ARITHMETIC_SIGNAL,   // Arithmetic exception

    OS_EXC_EMULATION_SIGNAL,    // Emulation instruction

    OS_EXC_SOFTWARE_SIGNAL,     // Software generated exception

    OS_EXC_BREAKPOINT_SIGNAL,   // Breakpoint

    OS_SIG32_SIGNAL,            // Real-time event 32

    OS_SIG33_SIGNAL,            // Real-time event 33

    OS_SIG34_SIGNAL,            // Real-time event 34

    OS_SIG35_SIGNAL,            // Real-time event 35

    OS_SIG36_SIGNAL,            // Real-time event 36

    OS_SIG37_SIGNAL,            // Real-time event 37

    OS_SIG38_SIGNAL,            // Real-time event 38

    OS_SIG39_SIGNAL,            // Real-time event 39

    OS_SIG40_SIGNAL,            // Real-time event 40

    OS_SIG41_SIGNAL,            // Real-time event 41

    OS_SIG42_SIGNAL,            // Real-time event 42

    OS_SIG43_SIGNAL,            // Real-time event 43

    OS_SIG44_SIGNAL,            // Real-time event 44

    OS_SIG45_SIGNAL,            // Real-time event 45

    OS_SIG46_SIGNAL,            // Real-time event 46

    OS_SIG47_SIGNAL,            // Real-time event 47

    OS_SIG48_SIGNAL,            // Real-time event 48

    OS_SIG49_SIGNAL,            // Real-time event 49

    OS_SIG50_SIGNAL,            // Real-time event 50

    OS_SIG51_SIGNAL,            // Real-time event 51

    OS_SIG52_SIGNAL,            // Real-time event 52

    OS_SIG53_SIGNAL,            // Real-time event 53

    OS_SIG54_SIGNAL,            // Real-time event 54

    OS_SIG55_SIGNAL,            // Real-time event 55

    OS_SIG56_SIGNAL,            // Real-time event 56

    OS_SIG57_SIGNAL,            // Real-time event 57

    OS_SIG58_SIGNAL,            // Real-time event 58

    OS_SIG59_SIGNAL,            // Real-time event 59

    OS_SIG60_SIGNAL,            // Real-time event 60

    OS_SIG61_SIGNAL,            // Real-time event 61

    OS_SIG62_SIGNAL,            // Real-time event 62

    OS_SIG63_SIGNAL,            // Real-time event 63

    OS_SIG64_SIGNAL,            // Real-time event 64

    OS_SIG65_SIGNAL,            // Real-time event 65

    OS_SIG66_SIGNAL,            // Real-time event 66

    OS_SIG67_SIGNAL,            // Real-time event 67

    OS_SIG68_SIGNAL,            // Real-time event 68

    OS_SIG69_SIGNAL,            // Real-time event 69

    OS_SIG70_SIGNAL,            // Real-time event 70

    OS_SIG71_SIGNAL,            // Real-time event 71

    OS_SIG72_SIGNAL,            // Real-time event 72

    OS_SIG73_SIGNAL,            // Real-time event 73

    OS_SIG74_SIGNAL,            // Real-time event 74

    OS_SIG75_SIGNAL,            // Real-time event 75

    OS_SIG76_SIGNAL,            // Real-time event 76

    OS_SIG77_SIGNAL,            // Real-time event 77

    OS_SIG78_SIGNAL,            // Real-time event 78

    OS_SIG79_SIGNAL,            // Real-time event 79

    OS_SIG80_SIGNAL,            // Real-time event 80

    OS_SIG81_SIGNAL,            // Real-time event 81

    OS_SIG82_SIGNAL,            // Real-time event 82

    OS_SIG83_SIGNAL,            // Real-time event 83

    OS_SIG84_SIGNAL,            // Real-time event 84

    OS_SIG85_SIGNAL,            // Real-time event 85

    OS_SIG86_SIGNAL,            // Real-time event 86

    OS_SIG87_SIGNAL,            // Real-time event 87

    OS_SIG88_SIGNAL,            // Real-time event 88

    OS_SIG89_SIGNAL,            // Real-time event 89

    OS_SIG90_SIGNAL,            // Real-time event 90

    OS_SIG91_SIGNAL,            // Real-time event 91

    OS_SIG92_SIGNAL,            // Real-time event 92

    OS_SIG93_SIGNAL,            // Real-time event 93

    OS_SIG94_SIGNAL,            // Real-time event 94

    OS_SIG95_SIGNAL,            // Real-time event 95

    OS_SIG96_SIGNAL,            // Real-time event 96

    OS_SIG97_SIGNAL,            // Real-time event 97

    OS_SIG98_SIGNAL,            // Real-time event 98

    OS_SIG99_SIGNAL,            // Real-time event 99

    OS_SIG100_SIGNAL,           // Real-time event 100

    OS_SIG101_SIGNAL,           // Real-time event 101

    OS_SIG102_SIGNAL,           // Real-time event 102

    OS_SIG103_SIGNAL,           // Real-time event 103

    OS_SIG104_SIGNAL,           // Real-time event 104

    OS_SIG105_SIGNAL,           // Real-time event 105

    OS_SIG106_SIGNAL,           // Real-time event 106

    OS_SIG107_SIGNAL,           // Real-time event 107

    OS_SIG108_SIGNAL,           // Real-time event 108

    OS_SIG109_SIGNAL,           // Real-time event 109

    OS_SIG110_SIGNAL,           // Real-time event 110

    OS_SIG111_SIGNAL,           // Real-time event 111

    OS_SIG112_SIGNAL,           // Real-time event 112

    OS_SIG113_SIGNAL,           // Real-time event 113

    OS_SIG114_SIGNAL,           // Real-time event 114

    OS_SIG115_SIGNAL,           // Real-time event 115

    OS_SIG116_SIGNAL,           // Real-time event 116

    OS_SIG117_SIGNAL,           // Real-time event 117

    OS_SIG118_SIGNAL,           // Real-time event 118

    OS_SIG119_SIGNAL,           // Real-time event 119

    OS_SIG120_SIGNAL,           // Real-time event 120

    OS_SIG121_SIGNAL,           // Real-time event 121

    OS_SIG122_SIGNAL,           // Real-time event 122

    OS_SIG123_SIGNAL,           // Real-time event 123

    OS_SIG124_SIGNAL,           // Real-time event 124

    OS_SIG125_SIGNAL,           // Real-time event 125

    OS_SIG126_SIGNAL,           // Real-time event 126

    OS_SIG127_SIGNAL,           // Real-time event 127

    OS_SIGINFO_SIGNAL,          // Information request

    OS_AMOUNT_OF_EXCEPTION_REASONS
};

OS_API osExceptionReason osExceptionCodeToExceptionReason(osExceptionCode exceptionCode);
OS_API osExceptionCode osExceptionReasonToExceptionCode(osExceptionReason exceptionReason);
OS_API void osExceptionReasonToString(osExceptionReason exceptionReason, gtString& reasonAsString);
OS_API void osExceptionReasonToExplanationString(osExceptionReason exceptionReason, gtString& reasonAsExplanationString);

#endif //__OSEXCEPTIONREASON_H

