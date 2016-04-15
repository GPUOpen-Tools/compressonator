//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osApplication.h
///
//=====================================================================

//------------------------------ osApplication.h ------------------------------

#ifndef __OSAPPLICATION
#define __OSAPPLICATION

// Pre-declarations:
struct osProductVersion;
class osFilePath;

// Infra:
#include <CXLBaseTools/Include/gtString.h>
#include <CXLBaseTools/Include/gtList.h>

// Local:
#include <CXLOSWrappers/Include/osOSWrappersDLLBuild.h>
#include <CXLOSWrappers/Include/osFile.h>
#include <CXLOSWrappers/Include/osModuleArchitecture.h>

extern osFilePath* os_stat_applicationDllsPath;


// Defines the executed application configuration:
enum OS_API osExecutedApplicationType
{
    OS_STANDALONE_APPLICATION_TYPE,     // CodeXL stand alone application.
    OS_VISUAL_STUDIO_PLUGIN_TYPE        // CodeXL Visual Studio plugin.
};


struct WindowsStoreAppInfo
{
    gtString m_name;
    gtString m_userModelId;
    gtString m_packageDirectory;
    osFilePath m_logoPath;
    gtUInt32 m_backgroundColor;
};


OS_API bool osGetCurrentApplicationPath(osFilePath& applicationPath, bool convertToLower = true);
OS_API bool osGetCurrentApplicationName(gtString& applicationName);
OS_API void osGetApplicationVersion(osProductVersion& applicationVersion);
OS_API void osGetApplicationVersionFromMacros(osProductVersion& applicationVersion);

OS_API void osSetCurrentApplicationDllsPath(const osFilePath& dllsPath);
/// Retrieve the path to the DLLs that this app is using, if it has been set.
/// This is used when running as a Visual Studio extension, in which case the
/// CodeXL DLLs are not located in the same folder as the application executable.
/// \param specificArchitecture specifies if the retrieved path should be of DLLs with
///                             a specific architecture. DLLs specific to 64-bit architecture
///                             reside in <DLLs path>/x64 while 32-bit specific DLLs
///                             reside in <DLLs path>/x86
/// \return true if the DLLs path has been set
OS_API bool osGetCurrentApplicationDllsPath(osFilePath& dllsPath, osModuleArchitecture specificArchitecture = OS_UNKNOWN_ARCHITECTURE);

OS_API bool osCheckForOutputRedirection(gtString& commandLine, gtString& fileName, bool& appendMode);
OS_API bool osCheckForInputRedirection(gtString& commandLine, gtString& fileName);

OS_API bool osSupportWindowsStoreApps();
OS_API bool osLaunchSuspendedWindowsStoreApp(const gtString& userModelId,
                                             const gtString& arguments,
                                             osProcessId& processId,
                                             osProcessHandle& processHandle,
                                             osFilePath& executablePath);
OS_API bool osResumeSuspendedWindowsStoreApp(const osProcessHandle& processHandle, bool closeHandle);
OS_API bool osEnumerateInstalledWindowsStoreApps(gtList<WindowsStoreAppInfo>& storeApps);
OS_API bool osDetermineIsWindowsStoreApp64Bit(const gtString& userModelId);
OS_API bool osGetWindowsStoreAppExecutable(const gtString& userModelId, gtString& exeFullPath);

/// \return the type of application we're running inside - standalone or Visual Studio plug-in
OS_API osExecutedApplicationType GetExecutedApplicationType();

/// By default the app type is standalone, so only the VS plug-in should call
/// this function to let interested code know we're running inside Visual Studio
OS_API void SetExecutedApplicationType(osExecutedApplicationType appType);


// This is used to get some of the predefined paths, which are relative to the iPhone app bundle.
#ifdef _GR_IPHONE_DEVICE_BUILD
    #include <CXLOSWrappers/Include/osFilePath.h>
    bool osGetiPhoneApplicationSpecialPath(osFilePath::osPreDefinedFilePaths predefinedPath, gtString& fullPathAsString);
#endif


#endif  // __OSAPPLICATION
