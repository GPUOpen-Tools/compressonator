//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtAssert.h 
/// 
//=====================================================================

//------------------------------ gtAssert.h ------------------------------

#ifndef __GTASSERT
#define __GTASSERT

// Standard C:
#include <assert.h>
#include <stdio.h>
#include <string>

// Local:
#include <AMDTBaseTools/Include/gtAutoPtr.h>
#include <AMDTBaseTools/Include/gtGRBaseToolsDLLBuild.h>
#include <AMDTBaseTools/Include/gtStringConstants.h>
#include <AMDTBaseTools/Include/gtString.h>

// Forward decelerations:
class gtIAssertionFailureHandler;

GT_API void gtFreeReservedMemory();
GT_API void gtRegisterAssertionFailureHandler(gtIAssertionFailureHandler* pAssertionFailureHandler);
GT_API void gtUnRegisterAssertionFailureHandler(gtIAssertionFailureHandler* pAssertionFailureHandler);
GT_API void gtTriggerAssertonFailureHandler(const wchar_t* functionName, const wchar_t* fileName, int lineNumber, const wchar_t* message);
// Expose this function as a C function so it can be called from the VS Extension modules that use different C runtimes
extern "C"
{
    GT_API void gtTriggerAssertonFailureHandler(const char* functionName, const char* fileName, int lineNumber, const wchar_t* message);
}
GT_API void gtExitCurrentProcess();
GT_API void gtCrashDialog();
GT_API void gtUnregsiterAllAssertionFailureHandlers();

#if AMDT_BUILD_TARGET == AMDT_LINUX_OS
    #undef _T
    #define _T(x) x
#elif AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    // instead of #include <atlbase.h> this is all we need from it
    #define _T(x)       __T(x)
    #define _TEXT(x)    __T(x)
    #define __T(x)      L ## x
#endif

// ---------------------------------------------------------------------------
// Name:        GT_ASSERT
// Description:
//   Inputs a boolean expression and evaluates it. If the expression value is false:
//   - In release configuration: Calls the registered assertion handler (if exists).
//   - In debug configurations: Also raise the assertion failure dialog.
//
// Author:      AMD Developer Tools Team
// Date:        30/8/2005
// ---------------------------------------------------------------------------
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
#define GT_ASSERT( booleanExpression )                                                                                                          \
    {                                                                                                                                           \
        __pragma(warning(push))                                                                                                                 \
        __pragma(warning(disable : 4127))                                                                                                       \
        if (!( booleanExpression ))                                                                                                             \
        {                                                                                                                                       \
            gtTriggerAssertonFailureHandler(__FUNCTION__, __FILE__, __LINE__,                                                                   \
                                            GT_STR_ASSERTION_FAILURE_MSG_PREFIX _T(# booleanExpression) GT_STR_ASSERTION_FAILURE_MSG_SUFFIX );  \
        }                                                                                                                                       \
        __pragma(warning(pop))                                                                                                                  \
    }
#else
#define GT_ASSERT( booleanExpression )                                                                                                          \
    {                                                                                                                                           \
        if (!( booleanExpression ))                                                                                                             \
        {                                                                                                                                       \
            gtTriggerAssertonFailureHandler(__FUNCTION__, __FILE__, __LINE__,                                                                   \
                                            GT_STR_ASSERTION_FAILURE_MSG_PREFIX _T(# booleanExpression) GT_STR_ASSERTION_FAILURE_MSG_SUFFIX );  \
        }                                                                                                                                       \
    }
#endif

// ---------------------------------------------------------------------------
// Name:        GT_ASSERT_EX
// Description:
//   The same as GT_ASSERT, but also enables writing a failure message
//   that will be sent to the assertion handler (in case of failure).
//
// Author:      AMD Developer Tools Team
// Date:        30/8/2005
// ---------------------------------------------------------------------------
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
#define GT_ASSERT_EX(booleanExpression, failureMessage)                                         \
    {                                                                                           \
        __pragma(warning(push))                                                                 \
        __pragma(warning(disable : 4127))                                                       \
        if (!(booleanExpression))                                                               \
        {                                                                                       \
            gtTriggerAssertonFailureHandler(__FUNCTION__, __FILE__, __LINE__, failureMessage);  \
        }                                                                                       \
        __pragma(warning(pop))                                                                  \
    }
#else
#define GT_ASSERT_EX(booleanExpression, failureMessage)                                         \
    {                                                                                           \
        if (!(booleanExpression))                                                               \
        {                                                                                       \
            gtTriggerAssertonFailureHandler(__FUNCTION__, __FILE__, __LINE__, failureMessage);  \
        }                                                                                       \
    }
#endif

// ---------------------------------------------------------------------------
// Name:        GT_IF_WITH_ASSERT
// Description: An if condition that also performs gtAssert on its boolean expression.
// Author:      AMD Developer Tools Team
// Date:        17/10/2005
// ---------------------------------------------------------------------------
#define GT_IF_WITH_ASSERT_AID2( booleanExpression , lineNum )                                                                               \
    bool gtAssertBoolExpr_##lineNum = ( booleanExpression );                                                                                \
    if (! gtAssertBoolExpr_##lineNum )                                                                                                      \
    {                                                                                                                                       \
        gtTriggerAssertonFailureHandler(__FUNCTION__, __FILE__, __LINE__,                                                                   \
                                        GT_STR_ASSERTION_FAILURE_MSG_PREFIX _T(# booleanExpression) GT_STR_ASSERTION_FAILURE_MSG_SUFFIX );  \
    }                                                                                                                                       \
    if (gtAssertBoolExpr_##lineNum)

#define GT_IF_WITH_ASSERT_AID1( booleanExpression , lineNum ) GT_IF_WITH_ASSERT_AID2( booleanExpression , lineNum )
#define GT_IF_WITH_ASSERT( booleanExpression ) GT_IF_WITH_ASSERT_AID1( booleanExpression , __LINE__ )


// ---------------------------------------------------------------------------
// Name:        GT_IF_WITH_ASSERT_EX
// Description: An if condition that also performs gtAssert on its boolean
//              expression. If the assertion fails, a failure message is sent
//              to the assertion handler.
// Author:      AMD Developer Tools Team
// Date:        17/10/2005
// ---------------------------------------------------------------------------
#define GT_IF_WITH_ASSERT_EX_AID2( booleanExpression , failureMessage , lineNum)            \
    bool gtAssertBoolExpr_##lineNum = ( booleanExpression );                                \
    if (! gtAssertBoolExpr_##lineNum )                                                      \
    {                                                                                       \
        gtTriggerAssertonFailureHandler(__FUNCTION__, __FILE__, __LINE__, failureMessage);  \
    }                                                                                       \
    if (gtAssertBoolExpr_##lineNum)

#define GT_IF_WITH_ASSERT_EX_AID1( booleanExpression , failureMessage , lineNum ) GT_IF_WITH_ASSERT_EX_AID2( booleanExpression , failureMessage , lineNum )
#define GT_IF_WITH_ASSERT_EX( booleanExpression , failureMessage ) GT_IF_WITH_ASSERT_EX_AID1( booleanExpression , failureMessage , __LINE__ )


// ---------------------------------------------------------------------------
// Name:        GT_RETURN_WITH_ASSERT
// Description: Performs an assertion test on booleanExpression and returns it.
// Author:      AMD Developer Tools Team
// Date:        2/1/2006
// ---------------------------------------------------------------------------
#define GT_RETURN_WITH_ASSERT( booleanExpression ) \
    GT_ASSERT( booleanExpression ); return booleanExpression;

#endif  // __GTASSERT
