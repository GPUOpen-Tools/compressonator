//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osOSDefinitions.h
///
//=====================================================================

//------------------------------ osOSDefinitions.h ------------------------------

#ifndef __OSOSDEFINITIONS
#define __OSOSDEFINITIONS

//////////////////////////////////////////////////////////////////////////
// A few notes about some of the types defined in this file:            //
//////////////////////////////////////////////////////////////////////////
// osThreadId - a numeric ID of a thread, supplied by the system or the
//      debugger. The osThreadId <-> application thread relation is
//      supposed to be 1-1 and should remain so when transferring from
//      the spy to the app.
// osThreadHandle - an opaque system internal handle to a thread. We
//      cannot assume anything about this type in non-platform-dependent
//      functions, as in Windows it is a handle/number, in Mac it is a
//      pointer to another opaque struct, and in Linux it is variant
//      dependent and can be a pointer, a struct or a number.
//      While an osThreadHandle represents a single thread, the same
//      thread can have several different osThreadHandle-s, thus we
//      should never compare osThreadHandles with ==, but rather use the
//      osAreThreadHandlesEquivalent() function. Also note that there
//      isn't always a way to get an osThreadId relating to an
//      osThreadHandle, nor to get an osThreadHandle relating to an
//      osThreadId (this is mostly true in the spy). Also note that
//      unlike osThreadId-s, the osThreadHandle might be symbolic or
//      relative, and as such they can not be transferred between
//      threads or processes.
// osProcedureAddress - a pointer to a function in the local process
//      address space. This should NEVER be passed between the spy and
//      the debugger, as theit address spaces may be of a different size.
// osProcedureAddress64 - an osProcedureAddress cast to a 64-bit sized int
//      variable. Making the size fixed and lossless allows us to tranfer
//      this between the spy and debugger and back (such as in
//      makeThreadExecuteFunction).
// osInstructionPointer - a variable that can contain the same data as the
//      RIP / EIP register in the spy. This is used only to describe spy
//      addresses, so in Linux we can assume it is a void*. In Windows /
//      Mac, where the 32-bit debugger can be used with the 64-bit spy,
//      this has to be a 64-bit wide pointer.
//////////////////////////////////////////////////////////////////////////

// Infra:
#include <AMDTBaseTools/Include/AMDTDefinitions.h>

// ------------------------ Common definitions ------------------------

// XML fields values:
#define OS_STR_EmptyXMLString "CodeXLEmptyString"
#define OS_STR_EmptyXMLStringUnicode L"CodeXLEmptyString"
#define OS_STR_TrueXMLValue "true"
#define OS_STR_TrueXMLValueUnicode L"true"
#define OS_STR_FalseXMLValue "false"
#define OS_STR_FalseXMLValueUnicode L"false"

// Application data directory into which we write none-application specific data:
#define OS_STR_amdAppDataDirectory L"AMD"

// Maximum length of file name component
#define OS_MAX_FNAME 256

// Maximum length of full pathname
#define OS_MAX_PATH 260

#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    #define WINDOWS_SWITCH(x, y) x
#else
    #define WINDOWS_SWITCH(x, y) y
#endif


#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS

    // ------------------------ Win32 definitions ------------------------

    // Exclude rarely used items from Win32 header files:
    // (And thus make builds faster)
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN 1
    #endif

    // Main Win32 include file:
    #include <Windows.h>

    // STD call type definition:
    #define OS_STD_CALL_CONVENTION __stdcall

    // System error code type:
    typedef DWORD osSystemErrorCode;

    // Process id and handle types:
    typedef DWORD osProcessId;
    typedef HANDLE osProcessHandle;

    // Thread related data types:
    typedef DWORD osThreadId;
    typedef HANDLE osThreadHandle;
    typedef DWORD osTheadLocalDataHandle;

    // The OS file handle:
    typedef HANDLE osFileHandle;

    // The OS module handle. A module is a binary entity that can be loaded
    // and linked to dynamically (dll / exe / etc)
    typedef HMODULE osModuleHandle;

    // Represents NULL module handle:
    #define OS_NO_MODULE_HANDLE NULL

    // Represents OS socket descriptor:
    typedef UINT_PTR osSocketDescriptor;

    // The OS timer handle:
    typedef HANDLE osTimerHandle;

    // Mutex handle:
    typedef HANDLE osMutexHandle;

    // Pipe handle:
    typedef HANDLE osPipeHandle;

    // Represents a NULL thread id:
    #define OS_NO_THREAD_ID 0

    // Represents a NULL thread handle:
    #define OS_NO_THREAD_HANDLE 0

    // Pointer to a function:
    typedef int (*osProcedureAddress)(void);

    // "Pointer" to a function in the spy (we do not know if the spy has the same address space as we do):
    typedef gtUInt64 osProcedureAddress64;
    #define OS_NULL_PROCEDURE_ADDRESS_64 ((osProcedureAddress64)NULL)

    // A pointer to a machine code instruction:
    typedef DWORD64 osInstructionPointer;

    // Represents an exception code:
    typedef DWORD osExceptionCode;

    // When raising an exception with this code, it is a signal to the debugger that the current thread
    // wants to name itself
    #define OS_THREAD_NAMING_EXCEPTION_CODE 0x406D1388

    // Specific function names in the spies:
    #define OS_SPIES_BREAKPOINT_FUNCTION_NAME L"suBreakpointsManager::triggerBreakpointException"

    // Triggers a breakpoint exception:
    // (On Intel 32 bit CPUs - int 3 triggers a breakpoint exception)
    #define OS_TRIGGER_BREAKPOINT_EXCEPTION __asm int 3

    // Module file extension string
    #define OS_MODULE_EXTENSION L"dll"

    // The spies sub-directory name:
    #define OS_SPIES_SUB_DIR_NAME L"spies" GDT_BUILD_SUFFIX_W
    #define OS_SPIES_64_SUB_DIR_NAME L"spies64" GDT_BUILD_SUFFIX_W

    // OpenGL dll name:
    #define OS_OPENGL_MODULE_NAME L"opengl32.dll"

    // Spy utilities dll name:
    #define OS_SPY_UTILS_FILE_PREFIX L"cxlserverutilities"

    // Our OpenGL Module name:
    #define OS_GREMEDY_OPENGL_SERVER_MODULE_NAME OS_OPENGL_MODULE_NAME

    // OpenGL ES dlls names:
    #define OS_OPENGL_ES_COMMON_DLL_NAME L"libgles_cm.dll"
    #define OS_OPENGL_ES_COMMON_DLL_FILE_NAME L"libgles_cm"
    #define OS_OPENGL_ES_COMMON_LITE_DLL_NAME L"libgles_cl.dll"
    #define OS_OPENGL_ES_COMMON_LITE_DLL_FILE_NAME L"libgles_cl"
    #define OS_OPENGL_ES_DEVICE_COMMON_DLL_NAME L"unknown dll name"
    #define OS_OPENGL_ES_LOADER_DLL_NAME L""

    // OpenCL ICD module name:
    #define OS_OPENCL_ICD_MODULE_NAME L"OpenCL.dll"
    #define OS_OPENCL_ICD_MODULE_ALTERNATIVE_NAME L"opencl.dll"

    // OpenCL runtime module name:
    #if AMDT_ADDRESS_SPACE_TYPE == AMDT_32_BIT_ADDRESS_SPACE
        #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME L"amdocl.dll"
        #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME_OTHER_BITNESS L"amdocl64.dll"
    #elif AMDT_ADDRESS_SPACE_TYPE == AMDT_64_BIT_ADDRESS_SPACE
        #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME L"amdocl64.dll"
        #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME_OTHER_BITNESS L"amdocl.dll"
    #else
        #error Unknown address space!
    #endif

    // Our OpenGL Module name:
    #define OS_GREMEDY_OPENCL_SERVER_MODULE_NAME L"opencl.dll"
    #define OS_OPENCL_MODULE_NAME L"OpenCL"

    // Our HSA Module name
    #define OS_CODEXL_HSA_SERVER_MODULE_NAME L"cxlhsaserver.dll"

    // NVIDIA driver dll names:
    #define OS_NVIDIA_OGL_DRIVER_DLL_NAME L"nvoglnt"
    #define OS_NVIDIA_OGL32_DRIVER_DLL_NAME L"nvogl32"
    #define OS_NVIDIA_VISTA32_OGL_DRIVER_DLL_NAME L"nvoglv32"
    #define OS_NVIDIA_VISTA64_OGL_DRIVER_DLL_NAME L"nvoglv64"
    #define OS_NVIDIA_CUDA_DRIVER_DLL_NAME L"NVCUDA"

    // Intel driver dll names:
    #define OS_INTEL_32_DRIVER_DLL_NAME L"ig4icd32"
    #define OS_INTEL_64_DRIVER_DLL_NAME L"ig4icd64"

    // ATI DLL names:
    #define OS_ATI_OGL_DRIVER_DLL_NAME L"atioglxx"
    #define OS_ATI_O6_DRIVER_DLL_NAME L"atio6axx"
    #define OS_ATI_CFX32_DRIVER_DLL_NAME L"aticfx32"
    #define OS_ATI_CFX64_DRIVER_DLL_NAME L"aticfx64"
    #define OS_ATI_OCL_DRIVER_DLL_NAME L"atiocl"
    #define OS_ATI_CALDD_DRIVER_DLL_NAME L"aticaldd"
    #define OS_ATI_CALDD64_DRIVER_DLL_NAME L"aticaldd64"
    #define OS_ATI_CALCL_DRIVER_DLL_NAME L"aticalcl"
    #define OS_ATI_CALCL64_DRIVER_DLL_NAME L"aticalcl64"
    #define OS_ATI_CALRT_DRIVER_DLL_NAME L"aticalrt"
    #define OS_ATI_CALRT64_DRIVER_DLL_NAME L"aticalrt64"
    #define OS_ATI_OVDECODE_DRIVER_DLL_NAME L"OVDecode"
    #define OS_ATI_OVDECODE64_DRIVER_DLL_NAME L"OVDecode64"
    #define OS_ATI_OCL64_DRIVER_DLL_NAME L"atiocl64"
    #define OS_ATI_CALXY_DRIVER_DLL_NAME L"atiadlxy"
    #define OS_ATI_CALXX_DRIVER_DLL_NAME L"atiadlxx"
    #define OS_ATI_CALRT_DRIVER_DLL_NAME L"aticalrt"
    #define OS_ATI_G632_DRIVER_DLL_NAME L"atigktxx"
    #define OS_ATI_G664_DRIVER_DLL_NAME L"atig6txx"
    #define OS_AMD_OCL_DRIVER_DLL_NAME L"amdocl"
    #define OS_AMD_OCL64_DRIVER_DLL_NAME L"amdocl64"
    // The name of the directory that contains the system's OpenGL dll:
    #define OS_SYSTEM_32_FOLDER_NAME L"system32"
    #define OS_SYSTEM_WOW64_FOLDER_NAME L"syswow64"

#elif AMDT_BUILD_TARGET == AMDT_LINUX_OS

    // ------------------------ Linux definitions ------------------------

    // Linux data types:
    #include <sys/types.h>

    // System error code type:
    typedef int osSystemErrorCode;

    // Process id:
    typedef pid_t osProcessId;
    typedef void* osProcessHandle; // Not really used in Linux

    // Thread related data types:
    typedef pthread_key_t osTheadLocalDataHandle;

    // The OS module handle. A module is a binary entity that can be loaded
    // and linked to dynamically (shared library / etc)
    typedef void* osModuleHandle;

    // Represents NULL module handle:
    #define OS_NO_MODULE_HANDLE NULL

    // Represents OS socket descriptor:
    typedef int osSocketDescriptor;

    // Pipe handle:
    typedef int osPipeHandle;

    // Represents a NULL thread id:
    #define OS_NO_THREAD_ID 0

    // Represents a NULL thread handle:
    #define OS_NO_THREAD_HANDLE 0

    // Represents an exception code (signal id):
    typedef int osExceptionCode;

    // Timer handle:
    typedef void* osTimerHandle;

    // Specific function names in the spies:
    #define OS_SPIES_BREAKPOINT_FUNCTION_NAME L"suBreakpointsManager::triggerBreakpointException"

    // Module file extension string
    #define OS_MODULE_EXTENSION L"so"

    // The spies sub-directory name:
    #define OS_SPIES_SUB_DIR_NAME L"spies"

    #if AMDT_LINUX_VARIANT == AMDT_GENERIC_LINUX_VARIANT || AMDT_LINUX_VARIANT == AMDT_ANDROID_LINUX_VARIANT

        // ----------------- Generic Linux variants only -----------------

        // Pointer to a machine code instruction:
        typedef void* osInstructionPointer;

        // Pointer to a function:
        typedef int (*osProcedureAddress)(void);

        // "Pointer" to a function in the spy (we do not know if the spy has the same address space as we do):
        typedef gtUInt64 osProcedureAddress64;
        #define OS_NULL_PROCEDURE_ADDRESS_64 ((osProcedureAddress64)NULL)

        // Thread related data types:
        typedef pthread_t osThreadId;
        typedef pthread_t osThreadHandle;

        // Mutex handle:
        typedef pthread_mutex_t osMutexHandle;

        // OpenGL dll name:
        // (We are using the runtime name - see http://www.opengl.org/registry/ABI/ - chapter 3 - "Libraries")
        #define OS_OPENGL_MODULE_NAME L"libGL.so.1"

        // Spy utilities dll name:
        #define OS_SPY_UTILS_FILE_PREFIX L"libcxlserverutilities"

        // Our OpenGL Module name:
        #define OS_GREMEDY_OPENGL_SERVER_MODULE_NAME OS_OPENGL_MODULE_NAME

        // OpenGL ES dlls names:
        // TO_DO: LNX replace the names to appropriate Linux names
        #define OS_OPENGL_ES_COMMON_DLL_NAME L"libgles_cm.dll"
        #define OS_OPENGL_ES_COMMON_LITE_DLL_NAME L"libgles_cl.dll"
        #define OS_OPENGL_ES_DEVICE_COMMON_DLL_NAME L"Unknown module name"
        #define OS_OPENGL_ES_LOADER_DLL_NAME L""

        // OpenCL dll name:
        #define OS_OPENCL_ICD_MODULE_NAME L"libOpenCL.so.1"
        #define OS_OPENCL_ICD_MODULE_ALTERNATIVE_NAME L"libOpenCL.so"

        // OpenCL runtime module name:
        #if AMDT_ADDRESS_SPACE_TYPE == AMDT_32_BIT_ADDRESS_SPACE
            #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME L"libamdocl.so"
            #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME_OTHER_BITNESS L"libamdocl64.so"
        #elif AMDT_ADDRESS_SPACE_TYPE == AMDT_64_BIT_ADDRESS_SPACE
            #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME L"libamdocl64.so"
            #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME_OTHER_BITNESS L"libamdocl.so"
        #else
            #error Unknown address space!
        #endif

        // Our OpenCL Module name:
        #define OS_GREMEDY_OPENCL_SERVER_MODULE_NAME OS_OPENCL_ICD_MODULE_NAME

        // Our HSA Module name:
        #define OS_CODEXL_HSA_SERVER_MODULE_NAME L"libCXLHsaServer.so"
        // ATI DLL Names
        #define OS_ATI_CALXY_DRIVER_DLL_NAME L"libatiadlxy"
        #define OS_ATI_CALXX_DRIVER_DLL_NAME L"libatiadlxx"
        #define OS_AMD_DRIVER_DIRECTORY L"fglrx"

    #elif AMDT_LINUX_VARIANT == AMDT_MAC_OS_X_LINUX_VARIANT

        // ----------------- Mac OS X only  -----------------

        // Mach kernel:
        #include <mach/mach_init.h>

        // Thread related data types:
        // - On Mac OS X we mainly use pthreads. However, since pthread_t on Mac is a pointer to a struct,
        //   we prefer using the Mach kernel thread id as our thread id. The pthread_t is held as the thread handle.
        typedef mach_port_t osThreadId;
        typedef pthread_t osThreadHandle;

        #ifndef _GR_IPHONE_BUILD
            // OpenGL Framework path:
            #define OS_OPENGL_FRAMEWORK_PATH L"/System/Library/Frameworks/OpenGL.framework"

            // OpenCL Framework path:
            #define OS_OPENCL_FRAMEWORK_PATH L"/System/Library/Frameworks/OpenCL.framework"
        #else // _GR_IPHONE_BUILD
            // OpenGL Framework path:
            #define OS_OPENGL_FRAMEWORK_PATH L"N/A"
            /* "/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator3.1.sdk/System/Library/Frameworks/OpenGLES.framework" */

            // OpenCL Framework path:
            #define OS_OPENCL_FRAMEWORK_PATH L"N/A"
        #endif
        // Mutex handle:
        typedef pthread_mutex_t osMutexHandle;

        // Pointer to a function:
        typedef void* osProcedureAddress;

        // "Pointer" to a function in the spy (we do not know if the spy has the same address space as we do):
        typedef gtUInt64 osProcedureAddress64;
        #define OS_NULL_PROCEDURE_ADDRESS_64 ((osProcedureAddress64)NULL)
        // OpenGL module name:
        #define OS_OPENGL_MODULE_NAME L"OpenGL"
        #define OS_OPENGL_ES_MODULE_NAME L"OpenGLES"

        // Spy utilities dll name:
        #define OS_SPY_UTILS_FILE_PREFIX L"libcxlserverutilities"

        // Our OpenGL Module name:
        #define OS_GREMEDY_OPENGL_SERVER_MODULE_NAME L"libGROpenGLServer.dylib"

        // OpenGL framework module name:
        #define OS_OPENGL_FRAMWRORK_MODULE_NAME L"OpenGL"

        // OpenGL ES module names:
        #define OS_OPENGL_ES_COMMON_DLL_NAME L"libGROpenGLESServer.dylib"
        #define OS_OPENGL_ES_COMMON_LITE_DLL_NAME L"unknown module name"
        #define OS_OPENGL_ES_LOADER_DLL_NAME L"libGROpenGLESServerLoader.dylib"
        #define OS_OPENGL_ES_DEVICE_COMMON_DLL_NAME L"libGROpenGLESDeviceServer.dylib"

        // OpenCL dll name:
        #define OS_OPENCL_ICD_MODULE_NAME L"OpenCL"
        #define OS_OPENCL_ICD_MODULE_ALTERNATIVE_NAME L"OpenCL"

        // OpenCL runtime module name:
        #if AMDT_ADDRESS_SPACE_TYPE == AMDT_32_BIT_ADDRESS_SPACE
            #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME L"libamdocl.dylib"
            #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME_OTHER_BITNESS L"libamdocl64.dylib"
        #elif AMDT_ADDRESS_SPACE_TYPE == AMDT_64_BIT_ADDRESS_SPACE
            #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME L"libamdocl64.dylib"
            #define OS_AMD_OPENCL_RUNTIME_MODULE_NAME_OTHER_BITNESS L"libamdocl.dylib"
        #else
            #error Unknown address space!
        #endif

        // Our OpenGL Module name:
        #define OS_GREMEDY_OPENCL_SERVER_MODULE_NAME L"libGROpenCLServer.dylib"

        // Our HSA Module name:
        #define OS_CODEXL_HSA_SERVER_MODULE_NAME L"libCXLHsaServer.dylib"
        // Mac OS X application bundle sub folder names:
        #define OS_MAC_APPLICATION_BUNDLE_FILE_EXTENSION L"app"
        #define OS_MAC_APPLICATION_BUNDLE_INTERNAL_PATH1 L"Contents"
        #define OS_MAC_APPLICATION_BUNDLE_INTERNAL_PATH2 L"MacOS"
        #define OS_MAC_XCODE_PROJECT_BUNDLE_FILE_EXTENSION L"xcodeproj"

        // iPhone simulator related paths:
        #define OS_IPHONE_SIMULATOR_APP_PATH L"/Developer/Platforms/iPhoneSimulator.platform/Developer/Applications/iPhone Simulator.app"
        #define OS_IPHONE_SIMULATOR_PROCESS_NAME L"iPhone Simulator"

    #endif

#endif // AMDT_BUILD_TARGET == AMDT_LINUX_OS



#endif  // __OSOSDEFINITIONS
