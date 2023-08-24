
#include "cmp_fileio.h"

#ifdef _WIN32
#pragma warning(disable : 4244)
#pragma warning(disable : 4996) // 'fopen': This function or variable may be unsafe. 
#pragma warning(disable : 4100) // unreferenced formal parameter
#include "windows.h"
#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <iterator>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <wordexp.h>
#endif

#include<fstream>

// For dev work on older C++ code
#if defined _CMP_CPP17_ || defined _CMP_CPP14_ 
 //#undef _CMP_CPP17_
 //#undef _CMP_CPP14_
#endif

#if defined _CMP_CPP17_  // Build code using std::c++17
    #include <filesystem>
    namespace sfs = std::filesystem;
#else
    #if defined _CMP_CPP14_  // Build code using std::c++14
        #ifndef _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
        #define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
        #endif
        #include <experimental/filesystem>
        namespace sfs = std::experimental::filesystem;
    #endif
#endif

#include <algorithm>
#include <sys/stat.h> 

bool CMP_CreateDirectory(std::string OutputFolder)
{
    int         nError = 0;
#if defined(_WIN32)
    nError        = _mkdir(OutputFolder.c_str());
#else
    mode_t nMode = 0733;                                // permissions
    nError       = mkdir(OutputFolder.c_str(), nMode);
#endif
    if (nError != 0)
        return false;
    else
        return true;
}

std::string CMP_GetJustFileName(const std::string& filePath)
{
    std::size_t dotPos = filePath.rfind('.');
    std::size_t sepPos = filePath.find_last_of(FILE_SPLIT_PATH);

    if (sepPos == std::string::npos)
        return filePath.substr(0, dotPos);
    else
        return filePath.substr(sepPos + 1, dotPos - sepPos - 1);
}

std::string CMP_GetFileName(const std::string& filePath)
{
    std::size_t dirPos = filePath.find_last_of(FILE_SPLIT_PATH);

    if (dirPos == std::string::npos)
        return filePath;
    else
        return filePath.substr(dirPos + 1);
}

uintmax_t CMP_GetFileSize(const std::string& fileName)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_ 
    return sfs::file_size(fileName);
#else
    std::ifstream fstrm(fileName.c_str(), std::ifstream::ate | std::ifstream::binary); 
    fstrm.seekg(0, std::ifstream::end);
    return fstrm.tellg();
#endif
}

void CMP_FileCopy(std::string& source_file, std::string& destination_file)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_ 
    sfs::copy(source_file, destination_file);
#else
    FILE* src = fopen(source_file.c_str(), "rb");
    if (src == NULL) 
        return;
    FILE* dst = fopen(destination_file.c_str(), "wb");
    if (dst == NULL)
    {
        fclose(src);
        return;
    }
    for (int i = getc(src); i != EOF; i = getc(src))
    {
        putc(i, dst);
    }
    fclose(dst);
    fclose(src);
#endif
}

bool CMP_DirExists(const std::string& abs_dir)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    if (sfs::exists(abs_dir))
        return sfs::is_directory(abs_dir);
    else
        return false;
#else
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributesA(abs_dir.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    DIR* dir = opendir(abs_dir.c_str());
    if (dir)
    {
        closedir(dir);
        return true;
    }
    return (false);
#endif
#endif
}

bool CMP_FileExists(const std::string& fileName)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    return sfs::exists(fileName) && !sfs::is_directory(fileName);
#else
    bool ret = false;
    FILE* fp;
#ifdef _WIN32
    errno_t err = fopen_s(&fp, fileName.c_str(), "rb");
    if (err != 0)
        return false;
#else
    fp = fopen(fileName.c_str(), "rb");
#endif
    if (fp)
    {
        ret = true;
        fclose(fp);
    }
    return ret;
#endif
}

bool CMP_CreateDir(std::string sPath)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    bool success = sfs::create_directory(sfs::absolute(sPath));
    return (success);
#else
    int Err;
#ifdef _WIN32
    Err = _mkdir(sPath.c_str());
#else
    mode_t nMode = 0733;  // UNIX style permissions
    Err = mkdir(sPath.c_str(),nMode);
#endif
    return (Err == 0);
#endif
}

std::string CMP_GetFileNameAndExt(const std::string& filePathName)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    return sfs::path(filePathName).filename().string();
#else
    //std::size_t dotPos = filePathName.rfind('.');
    std::size_t sepPos = filePathName.rfind('/');
    if (sepPos == 0)
        sepPos = filePathName.rfind('\\');
    if (sepPos != std::string::npos)
    {
        return filePathName.substr(sepPos + 1, filePathName.size() - 1);
    }
    return "";
#endif
}

std::string CMP_GetJustFileExt(const std::string& sourceFile)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    sfs::path   fp(sourceFile);
    std::string file_ext = fp.extension().string();
    return file_ext;
#else
    // Aternate Code
    std::string file_extension;
    file_extension = sourceFile.substr(sourceFile.find_last_of('.') + 1);
    return file_extension;
#endif
}

std::string CMP_GetPath(const std::string& sourceFile)
{
    std::string strpath = "";
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    sfs::path fp(sourceFile);
    strpath = fp.parent_path().string();
    return strpath;
#else
    size_t found;
    found   = sourceFile.find_last_of("/\\");
    strpath = sourceFile.substr(0, found);
    return strpath;
#endif
}

bool CMP_IsHidden(const std::string& fullPath)
{
#ifdef _WIN32
    bool  IsHidden = false;
    DWORD Result   = GetFileAttributesA(fullPath.c_str());
    if (Result != 0xFFFFFFFF)
    {
        IsHidden = !!(Result & FILE_ATTRIBUTE_HIDDEN);
    }

    return IsHidden;
#else
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    sfs::path path(fullPath);
    if (path.filename().string().find(".") == 0)
        return true;
#else
    return false;
#endif
#endif
}


std::string CMP_GetFullPath(std::string file)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    sfs::path p(file);
    sfs::path fullpath = sfs::absolute(p);
    return fullpath.generic_string();
    //return sfs::absolute(file).string();
#else
    return file;
#endif
}

#if defined _CMP_CPP17_ || defined _CMP_CPP14_
using recursive_directory_iterator = sfs::recursive_directory_iterator;
#endif

void CMP_GetAllDirFilesList(const std::string& directory, std::vector<std::string>& files, std::string filter)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    std::string path(directory);
    for (const auto& dirEntry : recursive_directory_iterator(path))
    {
        if (sfs::is_regular_file(dirEntry))
        {
            std::string FileNamePath = dirEntry.path().string();
            // Get the file extension if a file filter is suppiled
            if (filter.length() > 0)
            {
                std::string FileName = CMP_GetFileName(FileNamePath);
                std::string ext      = CMP_GetFilePathExtension(FileName);
                std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
                if (filter.find(ext) != std::string::npos)
                {
                    //std::cout << FileNamePath << std::endl;
                    files.push_back(FileNamePath);
                }
            }
            else
            {
                //std::cout << FileNamePath << std::endl;
                files.push_back(FileNamePath);
            }
        }
    }
#endif
}

void CMP_GetFilesInDirectory(const std::string& directory, std::vector<std::string>& files, std::string filter)
{
#ifdef _WIN32
    WIN32_FIND_DATAA data;
    HANDLE hFind;

    std::string path(directory);
    path.append("\\*");

    if ((hFind = FindFirstFileA(path.c_str(), &data)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            std::string fullPath = directory;
            fullPath.append("\\");
            fullPath.append(data.cFileName);

            // check file attribute is not hidden
            bool isHidden = (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
            bool isFile = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;

            if (!isFile || isHidden)
                continue;

            // Get the file extension if a file filter is supplied
            if (filter.length() > 0)
            {
                std::string FileName(data.cFileName);

                std::string ext = CMP_GetFilePathExtension(FileName);
                std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);

                if (filter.find(ext) != std::string::npos)
                    files.push_back(fullPath);
            }
            else
                files.push_back(fullPath);
        } while (FindNextFileA(hFind, &data) != 0);
        FindClose(hFind);
    }
#else
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    for (const sfs::directory_entry& entry : sfs::directory_iterator(directory))
    {
        if (CMP_IsHidden(entry.path().string()) || sfs::is_directory(entry.path().string()))
            continue;

        if (filter.length() > 0)
        {
            std::string ext = entry.path().extension();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
            if (filter.find(ext) != std::string::npos)
                files.push_back(entry.path());
        }
        else
        {
            files.push_back(entry.path());
        }
    }
#endif
#endif
}

//=================== Generic for all platforms and c++ standards ===============================
std::string CMP_ExpandFilePath(const std::string& filepath)
{
#ifdef _WIN32
    DWORD len = ExpandEnvironmentStringsA(filepath.c_str(), NULL, 0);
    char* str = new char[len];
    ExpandEnvironmentStringsA(filepath.c_str(), str, len);

    std::string s(str);

    delete[] str;

    return s;
#else

#if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR) || defined(__ANDROID__)
    // no expansion
    std::string s = filepath;
#else
    std::string s;
    wordexp_t   p;

    if (filepath.empty())
    {
        return "";
    }

    // char** w;
    int ret = wordexp(filepath.c_str(), &p, 0);
    if (ret)
    {
        // err
        s = filepath;
        return s;
    }

    // Use first element only.
    if (p.we_wordv)
    {
        s = std::string(p.we_wordv[0]);
        wordfree(&p);
    }
    else
    {
        s = filepath;
    }

#endif

    return s;
#endif
}

std::string CMP_JoinPath(const std::string& path0, const std::string& path1)
{
    if (path0.empty())
    {
        return path1;
    }
    else
    {
        // check '/'
        char lastChar = *path0.rbegin();
        if (lastChar != '/')
        {
            return path0 + std::string("/") + path1;
        }
        else
        {
            return path0 + path1;
        }
    }
}

std::string CMP_FindFile(const std::vector<std::string>& paths, const std::string& filepath)
{
    for (size_t i = 0; i < paths.size(); i++)
    {
        std::string absPath = CMP_ExpandFilePath(CMP_JoinPath(paths[i], filepath));
        if (CMP_FileExists(absPath))
        {
            return absPath;
        }
    }
    return std::string();
}

std::string CMP_GetFilePathExtension(const std::string& fileName)
{
    std::size_t extIndex = fileName.find_last_of('.');

    if (extIndex != std::string::npos)
    {
        // check the special case where the '.' character is part of a relative path
        std::size_t slashIndex = fileName.find_last_of(FILE_SPLIT_PATH);
        if (slashIndex != std::string::npos && slashIndex > extIndex)
        {
            return "";
        }

        return fileName.substr(extIndex + 1);
    }

    return "";
}

std::string CMP_GetBaseDir(const std::string& srcfileDirpath)
{
    if (srcfileDirpath.find_last_of(FILE_SPLIT_PATH) != std::string::npos)
        return srcfileDirpath.substr(0, srcfileDirpath.find_last_of(FILE_SPLIT_PATH));
    return "";
}

std::string CMP_GetFileExtension(const char* file, bool incDot, bool upperCase)
{
    std::string file_extension = CMP_GetJustFileExt(file);
    if (upperCase)
    {
        for (char& c : file_extension)
            c = (char)(toupper(c));
    }
    else
    {
        for (char& c : file_extension)
            c = (char)(tolower(c));
    }

    if (!incDot)
    {
        file_extension.erase(std::remove(file_extension.begin(), file_extension.end(), '.'), file_extension.end());
    }

    return file_extension;
}

void CMP_CreateTextFile(std::string& source_file)
{
    FILE* fp;
#ifdef _WIN32
    fopen_s(&fp, source_file.c_str(), "a");
#else
    fp = fopen(source_file.c_str(), "a");
#endif
    if (fp) 
        fclose(fp);
}

FILE* CMP_OpenTextFile(char* SourceFile, const char* mode)
{
    FILE* fp;
#ifdef _WIN32
    fopen_s(&fp, SourceFile, mode);
#else
    fp = fopen(SourceFile, mode);
#endif
    return fp;
}

void CMP_CloseTextFile(FILE* fp)
{
    if (fp)
        fclose(fp);
}

CMP_PATHTYPES CMP_PathType(const char* path)
{
    std::string pathString = path;

    std::string basedir = CMP_GetBaseDir(pathString);
    std::string ext = CMP_GetFilePathExtension(pathString);
    std::string filename = CMP_GetFileName(pathString);

    if (ext.length() > 0)
        return CMP_PATH_IS_FILE;

    // checking for the following three cases: '*/test/', '*/.', and '*/..'
    if (filename.length() == 0 || filename.back() == '.' || filename.compare(filename.length() - 2, 2, "..") == 0)
        return CMP_PATH_IS_DIR;
    else // no extension, but still a valid file
        return CMP_PATH_IS_FILE;


    return CMP_PATHTYPES::CMP_PATH_IS_UNKNOWN;
}



void CMP_ReplaceExt(std::string& s, const std::string& newExt)
{
    std::string::size_type i = s.rfind('.', s.length());

    if (i != std::string::npos)
    {
        s.replace(i + 1, newExt.length(), newExt);
    }
}