//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osDebugLog.h
///
//=====================================================================

//------------------------------ osDebugLog.h ------------------------------

#ifndef __OSDEBUGLOG
#define __OSDEBUGLOG

// Standard C:
#include <string>

// Forward decelerations:
class osFilePath;

// Infra:
#include <AMDTBaseTools/Include/gtString.h>
#include <AMDTBaseTools/Include/gtQueue.h>
#include <AMDTBaseTools/Include/gtIAssertionFailureHandler.h>

// Local:
#include <AMDTOSWrappers/Include/osCriticalSection.h>
#include <AMDTOSWrappers/Include/osSynchronizedQueue.h>
#include <AMDTOSWrappers/Include/osFile.h>

// Defined debug log printouts severities:
enum osDebugLogSeverity
{
    OS_DEBUG_LOG_ERROR,     // An error printout.
    OS_DEBUG_LOG_INFO,      // An informative printout, aimed for users.
    OS_DEBUG_LOG_DEBUG,     // An informative printout, aimed for developers.
    OS_DEBUG_LOG_EXTENSIVE  // An extensive printout, aimed for user bugs that are difficult to detect
};

OS_API const wchar_t* osDebugLogSeverityToString(osDebugLogSeverity severity);
OS_API osDebugLogSeverity osStringToDebugLogSeverity(const wchar_t* severityString);

#ifdef _GR_IPHONE_DEVICE_BUILD
    // This function is used internally only, so we do not need the OS_API prefix:
    bool osAddPrintoutToiPhoneConsole(const gtString& printoutString, osDebugLogSeverity printoutSeverity);
#endif

// ----------------------------------------------------------------------------------
// Class Name:           OS_API osDebugLog : public gtIAssertionFailureHandler
// General Description:
//   A debug log into which an application can output debug strings and errors.
//
// Author:      AMD Developer Tools Team
// Creation Date:        29/8/2005
// ----------------------------------------------------------------------------------
class OS_API osDebugLog : public gtIAssertionFailureHandler
{
public:
    static osDebugLog& instance();

    bool initialize(const gtString& logFileName, const wchar_t* pszProductDescription = nullptr, const wchar_t* pszOSDescription = nullptr, const osFilePath& logFilePath = osFilePath());
    bool initialize(const osFilePath& logFilePath);
    void terminate();

    void setLoggedSeverity(osDebugLogSeverity loggedSeverity);
    osDebugLogSeverity loggedSeverity() const { return _loggedSeverity; };

    // Check if a message severity exceeds the logged severity threshold, i.e. should a message with this severity be saved to the log
    bool isAboveLoggedSeverityThreshold(osDebugLogSeverity severity) const;

    void setLoggedSeverityChangedToHigh(bool loggedSeverityChangedToHigh);
    bool loggedSeverityChangedToHigh() const { return _loggedSeverityChangedToHigh; };

    void setProductDescription(const gtString& productDescriptionString) { _productDescriptionString = productDescriptionString; };
    const gtString& productDescriptionString() const { return _productDescriptionString; };

    void setOSDescriptionString(const gtString& osDescriptionString) { _osDescriptionString = osDescriptionString; };
    const gtString& osDescriptionString() const { return _osDescriptionString; };

    void addPrintout(const wchar_t* functionName, const wchar_t* fileName, int lineNumber,
                     const wchar_t* message, osDebugLogSeverity severity);
    void addPrintout(const char* functionName, const char* fileName, int lineNumber,
                     const wchar_t* message, osDebugLogSeverity severity);

    // Overrides gtIAssertionFailureHandler:
    virtual void onAssertionFailure(const wchar_t* functionName, const wchar_t* fileName,
                                    int lineNumber, const wchar_t* message);

    static const wchar_t* loggedSeverityAsString(osDebugLogSeverity severity);

    static void calculateLogFilePath(const gtString& logFileName, osFilePath& logFilePath);

    /// Start a session:
    void StartSession();

    /// End the current session:
    void EndSession();

private:
    // Represents a debug printout:
    struct osDebugLogPrintout
    {
        osDebugLogPrintout(): _printoutSeverity(OS_DEBUG_LOG_INFO) {};
        gtString _printoutString;
        osDebugLogSeverity _printoutSeverity;
    };

private:
    // Only my instance method can create me:
    osDebugLog();

    // Only osSingeltonsDelete can delete me:
    friend class osSingeltonsDelete;
    virtual ~osDebugLog();

    void registerSelfAsAssertionHandler();
    osFile::osOpenMode calculateLogFileOpenMode() const;
    bool outputLogFileHeader();
    bool outputSessionHeader();
    bool updateOSString();
    void addSynchronizedPrintout(const osDebugLogPrintout& printoutString);

private:
    // The debug log file:
    osFile _debugLogFile;

    // The logged severity:
    osDebugLogSeverity _loggedSeverity;

    // Indicate if _loggedSeverity is changed to high from low
    bool _loggedSeverityChangedToHigh;

    // Contains true iff the log file was initialized:
    bool _isInitialized;

    // A critical section object that synchronize the log writes among
    // different threads:
    osCriticalSection _writeCriticalSection;

    // A thread synchronized queue of debug print messages that are pending to be written:
    gtQueue<osDebugLogPrintout> _pendingDebugPrintouts;
    osCriticalSection _pendingDebugPrintoutsCriticalSection;

    // This class single instance:
    static osDebugLog* _pMySingleInstance;

    // OS Short description:
    gtString _osShortDescriptionString;

    // OS description:
    gtString _osDescriptionString;

    // Product description:
    gtString _productDescriptionString;

    /// Current session start time:
    gtString m_currentSessionStartTime;
};

#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    #define OS_PREPEND_L2(x) L ## x
    #define OS_PREPEND_L(x)  OS_PREPEND_L2(x)
#else
    #define OS_PREPEND_L2(x) x
    #define OS_PREPEND_L(x)  x
#endif


// A macro that output a debug log printout:
#define OS_OUTPUT_DEBUG_LOG( logMessage, logMessageSeverity )                   \
    {                                                                               \
        osDebugLog::instance().addPrintout( OS_PREPEND_L(__FUNCTION__) ,            \
                                            OS_PREPEND_L(__FILE__) , __LINE__ ,     \
                                            logMessage , logMessageSeverity ) ;     \
    }

#define OS_OUTPUT_FORMAT_DEBUG_LOG( logMessageSeverity, logFormat, ... )        \
    {                                                                               \
        gtString _logMessage;                                                       \
        _logMessage.appendFormattedString(logFormat, __VA_ARGS__);                  \
        osDebugLog::instance().addPrintout( OS_PREPEND_L(__FUNCTION__) ,            \
                                            OS_PREPEND_L(__FILE__) , __LINE__ ,     \
                                            _logMessage.asCharArray() ,             \
                                            logMessageSeverity ) ;                  \
    }

class OS_API osDebugLogTrace
{
public:
    osDebugLogTrace(const wchar_t* funcName);
    osDebugLogTrace(const wchar_t* funcName, bool& retVal);
    osDebugLogTrace(const char* funcName);
    osDebugLogTrace(const char* funcName, bool& retVal);

    ~osDebugLogTrace();

private:
    gtString m_funcName;
    bool* m_pRetVal;
};

#define OS_DEBUG_LOG_TRACER osDebugLogTrace tracer(OS_PREPEND_L(__FUNCTION__));
#define OS_DEBUG_LOG_TRACER_WITH_RETVAL(X) osDebugLogTrace tracer(OS_PREPEND_L(__FUNCTION__), X);

#endif  // __OSDEBUGLOG
