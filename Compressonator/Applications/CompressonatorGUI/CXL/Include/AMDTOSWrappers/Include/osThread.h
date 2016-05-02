//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osThread.h
///
//=====================================================================

//------------------------------ osThread.h ------------------------------

#ifndef __OSTHREAD
#define __OSTHREAD

// Infra:
#include <AMDTBaseTools/Include/gtASCIIString.h>
#include <AMDTBaseTools/Include/AMDTDefinitions.h>
#include <AMDTBaseTools/Include/gtString.h>

// Local:
#include <AMDTOSWrappers/Include/osCriticalSection.h>
#include <AMDTOSWrappers/Include/osOSDefinitions.h>
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>

#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    #include <TlHelp32.h>
#endif

class osTimeInterval;
class osTime;

// ----------------------------------------------------------------------------------
// Class Name:           osThread
// General Description:
//   Represents an OS thread (a path of execution through a program).
//
//   --- NOTICE ---
//   This class must be created on the heap !!!
//   Creating it on the stack caused undefined behavior !!!
//
// Author:      AMD Developer Tools Team
// Creation Date:        6/12/2003
// ----------------------------------------------------------------------------------
class OS_API osThread
{
public:
    osThread(const gtString& threadName, bool syncTermination = false, bool joinable = false);
    virtual ~osThread();

    osThreadId id() const { return _threadId; }
    osThreadHandle getHandle() const { return _threadHandle; }

    bool execute();
    bool breakExecution();
    bool resumeExecution();
    bool terminate();

    // Waits for the thread to end or until the timeout expires.
    // Returns true if thread ended in time. Returns false if timeout
    // was reached before the thread ended, or if the thread is not joinable
    bool waitForThreadEnd(const osTimeInterval& maxTimeToWait);

    bool isAlive() const;

private:
    // Do not allow the use of my default constructor:
    osThread();

protected:
    // Should be implemented by sub-classes:
    virtual int entryPoint() = 0;
    virtual void beforeTermination();

private:
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    static unsigned int OS_STD_CALL_CONVENTION threadEntryPoint(void* pParam);
    static void debugLogCreatedThread(osThreadHandle createdThreadHandle, const gtString& threadName);
#elif AMDT_BUILD_TARGET == AMDT_LINUX_OS
    static void* threadEntryPoint(void* pParam);
    static void debugLogCreatedThread(osThreadId createdThreadId, const gtString& threadName);
#else
#error Unknown build target!
#endif

    static void debugLogThreadRunStarted(osThreadId threadId, const gtString& threadName);

public:
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    // Thread naming supported only in Windows:
    static void setThreadNamingPrefix(const gtASCIIString& prefix) {ms_threadNamingPrefix = prefix;};
private:
    static gtASCIIString ms_threadNamingPrefix;
#else
    // Leave an empty function for other architectures:
    static void setThreadNamingPrefix(const gtASCIIString& prefix) {(void)prefix;};
#endif

protected:
    // Thread name (for logging):
    gtString _threadName;

private:
    // The OS id of the tread that this class represents:
    osThreadId _threadId;

    // The OS handle of the tread that this class represents:
    osThreadHandle _threadHandle;

    // Contains true after the thread was terminated:
    bool _wasThreadTerminated;

    // A critical section that guards the thread's termination:
    osCriticalSection _threadTerminationCS;

    // Finish termination only if thread is actually terminated:
    bool m_syncTermination;

    // Determine if the thread is joinable. Relevant only for Linux as all threads are joinable on Windows.
    bool m_isJoinable;
};

class OS_API osProcessThreadsEnumerator
{
public:
    osProcessThreadsEnumerator();
    ~osProcessThreadsEnumerator();

    bool initialize(osProcessId pid);
    void deinitialize();

    bool next(gtUInt32& threadId);

private:
    void* m_pEnumHandler;

#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    THREADENTRY32 m_te32;
#endif
};

OS_API osThreadId osGetCurrentThreadId();
OS_API osThreadId osGetUniqueCurrentThreadId();
OS_API osThreadHandle osGetCurrentThreadHandle();
OS_API void osThreadIdAsString(osThreadId threadId, gtString& threadIdAsString);
OS_API bool osAreThreadHandlesEquivalent(const osThreadHandle& handle1, const osThreadHandle& handle2);
OS_API bool osWaitForFlagToTurnOff(bool& flag, unsigned long timeOutMsec);
OS_API bool osWaitForFlagToTurnOn(bool& flag, unsigned long timeOutMsec);
OS_API void osSleep(unsigned long miliseconds);

#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    OS_API void osNameThreadInDebugger(osThreadId threadId, const gtASCIIString& threadName);
    OS_API bool osGetThreadStartTime(osThreadId threadId, osTime& startTime);
#endif

#endif  // __OSTHREAD
