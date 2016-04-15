//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file AMDTDefinitions.h 
/// 
//=====================================================================

//------------------------------ amdtDefinitions.h ------------------------------

#ifndef __AMDT_DEFINITIONS
#define __AMDT_DEFINITIONS

#include <CXLBaseTools/Include/gtGRBaseToolsDLLBuild.h>

// Standard C definitions (including NULL definition):
#if defined(__linux__)
    #define __STDC_LIMIT_MACROS
#endif
#include <stddef.h>

// STL:
#include <string>

// A large unsigned integer that is returned by the sizeof() operator:
typedef size_t gtSizeType;

#ifndef FCC
#define FCC(ch4) ((((gtUInt32)(ch4) & 0xFF) << 24) |     \
                  (((gtUInt32)(ch4) & 0xFF00) << 8) |    \
                  (((gtUInt32)(ch4) & 0xFF0000) >> 8) |  \
                  (((gtUInt32)(ch4) & 0xFF000000) >> 24))
#endif

// ------------------- Linux Variant ----------------------

// The Linux variant on which the software is compiled:
// - AMDT_GENERIC_LINUX_VARIANT - Generic Linux variant (Red Hat, Suse, etc)
// - AMDT_MAC_OS_X_LINUX_VARIANT - Mac OS X
#define AMDT_GENERIC_LINUX_VARIANT 1
#define AMDT_MAC_OS_X_LINUX_VARIANT 2
#define AMDT_ANDROID_LINUX_VARIANT 3


// ------------------- Build targets ----------------------

// The supported build targets (operating system to which we build our binaries):
// - GR_WIN32 - Window
// - GR_LINUX - Linux
#define AMDT_WINDOWS_OS   1
#define AMDT_LINUX_OS     2

// The current build target will be contained in AMDT_BUILD_TARGET:
#if defined(_WIN32)
    #define AMDT_BUILD_TARGET AMDT_WINDOWS_OS
#elif defined (__APPLE__)
    #define AMDT_BUILD_TARGET AMDT_LINUX_OS
    #define AMDT_LINUX_VARIANT AMDT_MAC_OS_X_LINUX_VARIANT
#elif defined(__ANDROID__)
    #define AMDT_BUILD_TARGET AMDT_LINUX_OS
    #define AMDT_LINUX_VARIANT AMDT_ANDROID_LINUX_VARIANT
#elif defined(__linux__)
    #define AMDT_BUILD_TARGET AMDT_LINUX_OS
    #define AMDT_LINUX_VARIANT AMDT_GENERIC_LINUX_VARIANT
#else
    #error Error: build target is not defined!!
#endif


// ------------------- Address space ----------------------

// The supported address spaces:
// - AMDT_32_BIT_ADDRESS_SPACE - 32 bit address space.
// - AMDT_64_BIT_ADDRESS_SPACE - 64 bit address space.
#define AMDT_32_BIT_ADDRESS_SPACE     1
#define AMDT_64_BIT_ADDRESS_SPACE     2

// The address space will be contained in AMDT_ADDRESS_SPACE_TYPE:
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    #ifdef _WIN64
        #define AMDT_ADDRESS_SPACE_TYPE AMDT_64_BIT_ADDRESS_SPACE
    #else
        #define AMDT_ADDRESS_SPACE_TYPE AMDT_32_BIT_ADDRESS_SPACE
    #endif
#elif AMDT_BUILD_TARGET == AMDT_LINUX_OS
    #include <stdint.h>
    #include <stdlib.h>
    #include <string.h>
    #include <limits.h>

    #if __WORDSIZE == 32
        #define AMDT_ADDRESS_SPACE_TYPE AMDT_32_BIT_ADDRESS_SPACE
    #elif __WORDSIZE == 64
        #define AMDT_ADDRESS_SPACE_TYPE AMDT_64_BIT_ADDRESS_SPACE
    #else
        #error Error: Unknown address space size!
    #endif

#endif


// ------------------- Used compiler ----------------------

#define AMDT_VISUAL_CPP_COMPILER  1
#define AMDT_GNU_CPP_COMPILER     2

#if defined(_MSC_VER)
    #define AMDT_CPP_COMPILER AMDT_VISUAL_CPP_COMPILER
    #define AMDT_VISUAL_CPP_COMPILER_VERSION _MSC_VER
#elif defined(__GNUC__)
    #define AMDT_CPP_COMPILER AMDT_GNU_CPP_COMPILER
    #define AMDT_GNU_CPP_COMPILER_VERSION __GNUC__
    #define AMDT_GNU_CPP_COMPILER_MINOR_VERSION __GNUC_MINOR__
#else
    #error Error: Unknown compiler
#endif

#if AMDT_CPP_COMPILER == AMDT_VISUAL_CPP_COMPILER
    #define AMDT_HAS_CPP0X 1
#else
    #if defined(__GXX_EXPERIMENTAL_CXX0X__)
        #define AMDT_HAS_CPP0X 1
    #endif
#endif


// ------------------- Build configurations ----------------------

// The supported build configurations:
#define AMDT_DEBUG_BUILD 1
#define AMDT_RELEASE_BUILD 2

// The used build configuration will be contained in AMDT_BUILD_CONFIGURATION:
#if defined(_DEBUG)
    #define AMDT_BUILD_CONFIGURATION AMDT_DEBUG_BUILD
#elif defined(NDEBUG)
    #define AMDT_BUILD_CONFIGURATION AMDT_RELEASE_BUILD
#else
    #error Error: Unknown build configuration!
#endif

// ------------------- Build Access ----------------------

// The supported build configurations:
#define AMDT_PUBLIC_ACCESS 1
#define AMDT_NDA_ACCESS 2
#define AMDT_INTERNAL_ACCESS 3

#if defined (GDT_INTERNAL)
    #define AMDT_BUILD_ACCESS AMDT_INTERNAL_ACCESS
#elif defined (GDT_NDA)
    #define AMDT_BUILD_ACCESS AMDT_NDA_ACCESS
#elif defined (GDT_PUBLIC)
    #define AMDT_BUILD_ACCESS AMDT_PUBLIC_ACCESS
#else
    #error Error: Unknown build access!
#endif

// ------------------- Data Types ----------------------

#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    #include <WinError.h>

    // Represents fixed size data types:
    typedef char gtByte;
    typedef unsigned char gtUByte;
    typedef __int16 gtInt16;
    typedef unsigned __int16 gtUInt16;
    typedef __int32 gtInt32;
    typedef unsigned __int32 gtUInt32;
    typedef __int64 gtInt64;
    typedef unsigned __int64 gtUInt64;
    typedef float gtFloat32;
    typedef size_t gtSize_t;
    typedef uintptr_t gtUIntPtr;
    typedef intptr_t gtIntPtr;

    // Maximal values per fixed data type:
    #define GT_INT16_MAX _I16_MAX
    #define GT_UINT16_MAX _UI16_MAX
    #define GT_INT32_MAX _I32_MAX
    #define GT_UINT32_MAX _UI32_MAX
    #define GT_INT64_MAX _I64_MAX
    #define GT_UINT64_MAX _UI64_MAX
    #define GT_FLOAT32_MAX FLT_MAX


    // HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)
    #define E_NOFILE _HRESULT_TYPEDEF_(0x80070002L)

    // HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)
    #define E_INVALIDPATH _HRESULT_TYPEDEF_(0x80070003L)

    // HRESULT_FROM_WIN32(ERROR_INVALID_DATA)
    #define E_INVALIDDATA _HRESULT_TYPEDEF_(0x8007000DL)

    // HRESULT_FROM_WIN32(ERROR_RESOURCE_NOT_AVAILABLE)
    #define E_NOTAVAILABLE _HRESULT_TYPEDEF_(0x80075006L)

    // HRESULT_FROM_WIN32(ERROR_NO_DATA)
    #define E_NODATA _HRESULT_TYPEDEF_(0x800700E8L)

    // HRESULT_FROM_WIN32(ERROR_LOCK_VIOLATION)
    #define E_LOCKED _HRESULT_TYPEDEF_(0x80070021L)

    // HRESULT_FROM_WIN32(ERROR_TIMEOUT)
    #define E_TIMEOUT _HRESULT_TYPEDEF_(0x800705B4L)

    // HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)
    #define E_NOTSUPPORTED  _HRESULT_TYPEDEF_(0x80070032)

#else

    // Represents fixed size data types:
    typedef float gtFloat32;
    typedef char gtByte;
    typedef char gtByte;
    typedef unsigned char gtUByte;
    typedef __int16_t gtInt16;
    typedef __uint16_t gtUInt16;
    typedef __int32_t gtInt32;
    typedef __uint32_t gtUInt32;
    typedef __int64_t gtInt64;
    typedef __uint64_t gtUInt64;
    typedef size_t gtSize_t;
    typedef uintptr_t gtUIntPtr;
    typedef intptr_t gtIntPtr;

    // Char type:
    #ifdef GR_UNICODE
        typedef wchar_t gtChar;
    #else
        typedef char gtChar;
    #endif

    // Maximal values per fixed data type:
    #define GT_INT16_MAX INT16_MAX
    #define GT_UINT16_MAX UINT16_MAX
    #define GT_INT32_MAX INT32_MAX
    #define GT_UINT32_MAX UINT32_MAX
    #define GT_INT64_MAX INT64_MAX
    #define GT_UINT64_MAX UINT64_MAX
    #define GT_FLOAT32_MAX FLT_MAX

    // This value is missing from gcc 4.1.2 for some reason (4.1.1 has it):
    #ifndef FLT_MAX
        #define FLT_MAX __FLT_MAX__
    #endif
    #ifndef DBL_MAX
        #define DBL_MAX __DBL_MAX__
    #endif


    typedef gtInt32           HRESULT;

    #define SUCCEEDED(hr)   (((HRESULT)(hr)) >= 0)

    // The convention seems to be to take the value of the error (windows system error codes)
    // and then to add 0x80070000 to it to make the linux equivalent.
    // but that isn't even consistently done here.
    #define _HRESULT_TYPEDEF_(sc) ((HRESULT)sc)

    #define S_OK            _HRESULT_TYPEDEF_(0)
    #define S_FALSE         _HRESULT_TYPEDEF_(1)
    #define E_FAIL          _HRESULT_TYPEDEF_(0x80004005)
    #define E_INVALIDARG    _HRESULT_TYPEDEF_(0x80070057)
    #define E_OUTOFMEMORY   _HRESULT_TYPEDEF_(0x8007000E)
    #define E_UNEXPECTED    _HRESULT_TYPEDEF_(0x8000FFFF)
    #define E_ACCESSDENIED  _HRESULT_TYPEDEF_(0x80070005)
    #define E_HANDLE        _HRESULT_TYPEDEF_(0x80070006)
    #define E_ABORT         _HRESULT_TYPEDEF_(0x80004004)
    #define E_NOTIMPL       _HRESULT_TYPEDEF_(0x80004001)
    #define E_NOFILE        _HRESULT_TYPEDEF_(0x80070002)   // HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)
    #define E_INVALIDPATH   _HRESULT_TYPEDEF_(0x80070003)   // HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND)
    #define E_INVALIDDATA   _HRESULT_TYPEDEF_(0x8007000D)   // HRESULT_FROM_WIN32(ERROR_INVALID_DATA)
    #define E_NOTAVAILABLE  _HRESULT_TYPEDEF_(0x80075006)   // HRESULT_FROM_WIN32(ERROR_RESOURCE_NOT_AVAILABLE)
    #define E_NODATA        _HRESULT_TYPEDEF_(0x800700E8)   // HRESULT_FROM_WIN32(ERROR_NO_DATA)
    #define E_LOCKED        _HRESULT_TYPEDEF_(0x80070021)   // HRESULT_FROM_WIN32(ERROR_LOCK_VIOLATION)
    #define E_TIMEOUT       _HRESULT_TYPEDEF_(0x800705B4)   // HRESULT_FROM_WIN32(ERROR_TIMEOUT)
    #define E_PENDING       _HRESULT_TYPEDEF_(0x8000000A)
    #define E_NOTSUPPORTED  _HRESULT_TYPEDEF_(0x80070032)   // HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)

#endif

// Relative Virtual Address
typedef gtUInt32 gtRVAddr;
// Virtual Address
typedef gtUInt64 gtVAddr;

#define GT_INVALID_RVADDR ((gtRVAddr)(-1))
#define GT_INVALID_VADDR ((gtVAddr)(0))

#define GT_UNREFERENCED_PARAMETER(p) ((void)(p))


#define GT_CONTAINING_RECORD(address, type, field) ((type *)((gtByte*)(address) - (gtUIntPtr)(&((type *)0)->field)))

template <bool flag, class IsTrue, class IsFalse>
struct ConditionalType;

template <class IsTrue, class IsFalse>
struct ConditionalType<true, IsTrue, IsFalse>
{
    typedef IsTrue type;
};

template <class IsTrue, class IsFalse>
struct ConditionalType<false, IsTrue, IsFalse>
{
    typedef IsFalse type;
};

//
// Converts error codes to error description strings.
// The error codes are defined in AMDTDefinitions.h (above) and WinError.h.
//
GT_API const wchar_t* gtGetErrorString(const HRESULT errCode);

#endif  // __AMDT_DEFINITIONS
