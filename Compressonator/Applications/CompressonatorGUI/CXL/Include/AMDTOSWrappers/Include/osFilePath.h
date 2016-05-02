//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osFilePath.h
///
//=====================================================================

//------------------------------ osFilePath.h ------------------------------

#ifndef __OSFILEPATH
#define __OSFILEPATH

// Pre-declarations:
class osDirectory;

// Infra:
#include <AMDTBaseTools/Include/gtStringTokenizer.h>

// Local:
#include <AMDTOSWrappers/Include/osTransferableObject.h>

// The function and struct names differ on Windows and Linux / Mac
// So we use wrapper names
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    #define osStatStructure struct _stat
#elif AMDT_BUILD_TARGET == AMDT_LINUX_OS
    #include <sys/stat.h>
    #define osStatStructure  struct stat
#endif



// ----------------------------------------------------------------------------------
// Class Name:           OS_API osFilePath
//
// General Description:
//   Represents a file / directory path (Example: C:\TEMP\foo.txt).
//   Responsible for holding the path in the syntax that match the current operating
//   system (/ or \, removes trailing /, etc)
//
// Author:      AMD Developer Tools Team
// Creation Date:        15/5/2004
// ----------------------------------------------------------------------------------
class OS_API osFilePath : public osTransferableObject
{
public:
    // Pre-defined OS directory / file paths
    enum osPreDefinedFilePaths
    {
        OS_SYSTEM_DIRECTORY,             // - Directory where the system is installed.
        OS_SYSTEM_X86_DIRECTORY,         // - Directory where the x86 system is installed.
        OS_TEMP_DIRECTORY,               // - Temporary directory, deleted periodically on few OSs.
        OS_USER_APPLICATION_DATA,        // - Per user application data storage.
        OS_ROOT_USER_APPLICATION_DATA,   // - The "root" user application data storage (Available on Linux only).
        OS_USER_DOCUMENTS,               // - Per user documents default folder.
        OS_COMMON_APPLICATION_DATA,      // - Common (shared across all users) application data storage.
        OS_CURRENT_DIRECTORY,            // - The current application directory.
        OS_BROWSE_EXECUTABLES_DIRECTORY, // - The directory where we start browsing for executables.
        OS_USER_DOWNLOADS,               // - Current User Download folder
        OS_PUBLIC_DOCUMENTS                  // - ProgramData folder
    };

    enum osApplicationSpecialDirectories
    {
        OS_CODEXL_BINARIES_PATH,
        OS_CODEXL_EULA_PATH,
        OS_CODEXL_EXAMPLES_PATH,
        OS_CODEXL_HELP_FILE,
        OS_CODEXL_TUTORIAL_FILE,
        OS_CODEXL_QUICK_START_FILE,
        OS_CODEXL_VS_PACKAGE_HELP_FILE,
        OS_CODEXL_VS_PACKAGE_TUTORIAL_FILE,
        OS_CODEXL_SERVERS_PATH,
        OS_CODEXL_TEAPOT_SOURCES_PATH,
        OS_CODEXL_TEAPOT_SOURCES_LIB_PATH,
        OS_CODEXL_TEAPOT_SAMPLE_PATH,
        OS_CODEXL_MAT_MUL_SAMPLE_PATH,
        OS_CODEXL_D3D_MT_SAMPLE_PATH,
        OS_CODEXL_DATA_PATH,
    };

    // The char that is used to separate between path segments
    // ('/' On UNIX, '\' on Windows, etc)
    static const wchar_t osPathSeparator;

    // The char that is used to separate between file name and the file extension
    // (Usually ".")
    static const wchar_t osExtensionSeparator;

    // The char that is used to separate between different paths in environment
    // variables that constitute a path list (';' in Windows, ':' in UNIX)
    static const wchar_t osEnvironmentVariablePathsSeparator;

    osFilePath();
    osFilePath(osPreDefinedFilePaths predefinedfilePath, bool applyRedirection = false);
    osFilePath(osApplicationSpecialDirectories predefinedFilePath, bool convertToLower = true);
    osFilePath(const gtString& fileFullPath);
    osFilePath(osPreDefinedFilePaths predefinedfilePath, const gtString& fileName, const gtString& fileExtension, bool applyRedirection = false);
    osFilePath(const osFilePath& other);

#if AMDT_HAS_CPP0X
    osFilePath(osFilePath&& other);
    osFilePath& operator=(osFilePath&& other);
#endif

    osFilePath& operator=(const osFilePath& other);

    bool operator==(const osFilePath& other) const;
    bool operator!=(const osFilePath& other) const { return !(operator==(other)); };
    bool operator<(const osFilePath& otherFile) const;

    const gtString& asString(bool appendSeparatorToDir = false) const;

    osFilePath& clear();
    bool exists() const;
    bool isDirectory() const;
    bool isRegularFile() const;
    bool isExecutable() const;
    bool isRelativePath() const;

    bool getFileDirectory(osDirectory& fileDirectory) const;
    bool getFileName(gtString& fileName) const;
    bool getFileExtension(gtString& fileExtension) const;

    const gtString& fileDirectoryAsString() const { return _fileDirectory; };
    void getFileNameAndExtension(gtString& fileNameWithExt) const;
    bool setPath(osPreDefinedFilePaths predefinedfilePath, bool applyRedirection = false);

    // Set path based on installed path of CodeXL
    bool SetInstallRelatedPath(osApplicationSpecialDirectories predefinedFilePath, bool convertToLower = true);

    osFilePath& setFullPathFromString(const gtString& fullPathAsString, bool adjustToOS = true);
    osFilePath& setFileDirectory(const gtString& fileDirectory);
    osFilePath& setFileDirectory(const osDirectory& fileDirectory);
    osFilePath& appendSubDirectory(const gtString& subDirRelativePathString);
    osFilePath& setFileName(const gtString& fileName);
    osFilePath& setFileExtension(const gtString& fileExtension);
    osFilePath& setFromOtherPath(const osFilePath& other, bool setDirectory = true, bool setName = true, bool setExtension = true);
    osFilePath& reinterpretAsDirectory();
    osFilePath& clearFileName() {_fileName.makeEmpty(); return *this;};
    osFilePath& clearFileExtension() {_fileExtension.makeEmpty(); return *this;};
    osFilePath& resolveToAbsolutePath();

    // Overrides osTransferableObject:
    virtual osTransferableObjectType type() const;
    virtual bool writeSelfIntoChannel(osChannel& channel) const;
    virtual bool readSelfFromChannel(osChannel& channel);

    bool isEmpty() const {return _fileName.isEmpty();};
    bool isWritable() const;
    bool makeWritable();

    static void adjustStringToCurrentOS(gtString& filePathString);


    /// If the user is supporting user app data paths with unicode characters, this function should be called with "true".
    /// The default behavior is to use temporary path in case of unicode characters:
    static void SupportUnicodeCharactersInUserAppData(bool shouldSupport) { ms_supportUnicodeInUserAppData = shouldSupport; };

    /// Checks if current file path extension is one of specified extensions
    /// \param[in] extensionsStr - space-delimited string containing all possible extensions to check
    /// \return true if current file extension is found in the list
    bool IsMatchingExtension(const gtString& extensionsString) const;

    bool Rename(const gtString& newNameFullPath);
private:
    osFilePath& adjustToCurrentOS();

    /// Utility for samples paths
    void SetSamplePath(osApplicationSpecialDirectories predefinedFilePath);

    /// Initializes the user app data file path for a user with unicode characters:
    void InitializeUnicodeCharactersUserFilePath(bool applyRedirection);

    /// Return the native user application app data folder, without the unicode workaround:
    virtual bool GetUserAppDataFilePath(osFilePath& userAppDataFilePath, bool applyRedirection);

private:
    gtString _fileDirectory;
    gtString _fileName;
    gtString _fileExtension;

    // This variable is calculated only when it is needed:
    gtString _fileFullPathString;

    /// In case that this flag is false, return temp folder as user application data, in case that the
    /// user name contain unicode characters:
    static bool ms_supportUnicodeInUserAppData;

    /// What will be the alternative file path for user with unicode characters?
    static osFilePath ms_userAppDataFilePath;

};


bool osGenerateUniqueFileName(const osFilePath& directoryPath, const gtString& fileNamePrefix,
                              const gtString& fileExtension, osFilePath& uniqueFileNamePath);

int OS_API osWStat(const gtString& fileName, osStatStructure& fileStruct);

#endif  // __OSFILEPATH
