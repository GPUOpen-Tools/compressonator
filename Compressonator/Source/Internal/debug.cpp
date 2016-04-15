#include <windows.h>
#include <stdarg.h>
#include <stdio.h>

#include <time.h>
#include <errno.h>
#include <tchar.h>

#include "debug.h"

DBGENTRY g_DbgTable[DBG_NUM_CLASSES] =
{
 //   DPF LEVEL                         PREFIX/KEY                              FILENAME        DEFAULT MODE      BREAK
    { DBG_ASSERTION,                    _T("DBG_Assert"),                       _T(""),         DBGOUT_PRINT,     TRUE   },
    { DBG_ERROR_MESSAGE,                _T("DBG_Error"),                        _T(""),         DBGOUT_PRINT,     FALSE  },
}; 

//BOOL g_csDebugInitialized = FALSE;
CRITICAL_SECTION g_csDebug;

static const TCHAR *endTStr = _T("\r\n");

static void dbgVOutf(LPCTSTR pfileName, EDbgOutput dbgOutput, LPCTSTR fmtStr, va_list ap)
{
    int stringSize = _vsctprintf(fmtStr, ap) + 1;
    TCHAR *pdebugString = new TCHAR[stringSize];

    _vstprintf_s(pdebugString,stringSize,fmtStr,ap);

    if (dbgOutput == DBGOUT_PRINT)
    {
        OutputDebugString(pdebugString);
    }

    if (dbgOutput == DBGOUT_FILE)
    {
        FILE *pFile;
        __time64_t Now;
        struct tm When;

        // Get the time stamp
        _time64(&Now);
        _localtime64_s(&When,&Now);

        if(_tfopen_s(&pFile,pfileName,_T("a+t")) != EINVAL)
        {
            // Output to the file
            _ftprintf(pFile,_T("%02d-%02d-%02d  %02d:%02d:%02d> (Thread = 0x%lx) %s"),
                When.tm_year + 1900,
                When.tm_mon + 1,
                When.tm_mday,
                When.tm_hour,
                When.tm_min,
                When.tm_sec,
                GetCurrentThreadId(),
                pdebugString);

            // Close the file
            fclose(pFile);
        }
    }

    delete [] pdebugString;

    return;
}

static void dbgOutf(LPCTSTR pfileName, EDbgOutput dbgOutput, LPCTSTR fmtStr, ...)
{
    va_list ap;
    va_start(ap,fmtStr);
    dbgVOutf(pfileName,dbgOutput,fmtStr,ap);
    va_end(ap);
}

void dbgInternalPrintf(int debugLevel, LPCTSTR fmtStr, va_list ap)
{
    EDbgClass maskedDebugLevel = (EDbgClass)(debugLevel & DBG_MASK);
	static int iInit = 0;
    // Make it thread safe
    // TODO: Vista driver has serious issues loading if this is global
    //       I really want thread safety, so try and get it working later
    if(iInit == 0)
    {
        InitializeCriticalSection(&g_csDebug);
		iInit++;
    }

    EnterCriticalSection(&g_csDebug);
    // print the prefix, if appropriate
    if(!( debugLevel & DBG_NO_PREFIX ))
    {
        dbgOutf(
            g_DbgTable[maskedDebugLevel].filename,
            g_DbgTable[maskedDebugLevel].dbgEnable,
            _T("%s: "),
            g_DbgTable[maskedDebugLevel].keyName);
    }

    // Print the output requested by the caller
    dbgVOutf(
        g_DbgTable[maskedDebugLevel].filename,
        g_DbgTable[maskedDebugLevel].dbgEnable,
        fmtStr,
        ap);

    // Add a cr/lf if appropriate
    if(!(debugLevel & DBG_NO_CRLF))
    {
        dbgOutf(
            g_DbgTable[maskedDebugLevel].filename,
            g_DbgTable[maskedDebugLevel].dbgEnable,
            endTStr);
    }

    if (g_DbgTable[maskedDebugLevel].breakEnable)
    {
        dbgBreak();
    }

    LeaveCriticalSection(&g_csDebug);
}

void dbgPrintGUID(int debugLevel, const GUID *pGuid)
{
    dbgPrintf(
        debugLevel,
        _T("%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
        pGuid->Data1,
        pGuid->Data2,
        pGuid->Data3,
        pGuid->Data4[0],
        pGuid->Data4[1],
        pGuid->Data4[2],
        pGuid->Data4[3],
        pGuid->Data4[4],
        pGuid->Data4[5],
        pGuid->Data4[6],
        pGuid->Data4[7]);
}

/// @TODO   Need mechanism to read from registry, the keys above and update the table when the function is called