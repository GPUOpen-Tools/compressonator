
#include "cmp_fileio.h"

#ifdef _WIN32
#pragma warning(disable : 4244)
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

// For dev work on C++ 11 code
#if defined _CMP_CPP17_ || defined _CMP_CPP14_ 
// #undef _CMP_CPP17_
// #undef _CMP_CPP14_
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

uintmax_t CMP_GetFileSize(std::string& source_file)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_ 
    return sfs::file_size(source_file);
#else
    return 0;
#endif
}

void CMP_FileCopy(std::string& source_file, std::string& destination_file)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_ 
    sfs::copy(source_file, destination_file);
#else
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
    return (false);
#endif
}

bool CMP_FileExists(const std::string& abs_filename)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    return sfs::exists(abs_filename);
#else
    #ifdef _WIN32
        bool    ret = false;
        FILE*   fp;
        errno_t err = fopen_s(&fp, abs_filename.c_str(), "rb");
        if (err != 0)
        {
            return false;
        }
    
        if (fp)
        {
            ret = true;
            fclose(fp);
        }
    
        return ret;
    #else
        return false;
    #endif
#endif
}

bool CMP_CreateDir(std::string sPath)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    bool success = sfs::create_directory(sfs::absolute(sPath));
    return (success);
#else
    return false;
#endif
}

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

std::string CMP_GetFilePathExtension(const std::string& FileName)
{
    if (FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".") + 1);
    return "";
}

std::string CMP_GetBaseDir(const std::string& srcfileDirpath)
{
    if (srcfileDirpath.find_last_of(FILE_SPLIT_PATH) != std::string::npos)
        return srcfileDirpath.substr(0, srcfileDirpath.find_last_of(FILE_SPLIT_PATH));
    return "";
}

std::string CMP_GetFileName(const std::string& srcfileNamepath)
{
    int pos = (int)srcfileNamepath.find_last_of(FILE_SPLIT_PATH);
    return srcfileNamepath.substr(pos + 1);
}

std::string CMP_GetFileNameAndExt(const std::string& FilePathName)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    return sfs::path(FilePathName).filename().string();
#else
    return "";
#endif
}

std::string CMP_GetJustFileName(const std::string& SourceFile)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    sfs::path   fp(SourceFile);
    std::string file_name = fp.stem().string();
    return file_name;
#else
    return "";
#endif
}

std::string CMP_GetPath(const std::string& SourceFile)
{
    std::string strpath = "";
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    sfs::path fp(SourceFile);
    strpath = fp.parent_path().string();
    return strpath;
#else
    size_t found;
    found   = SourceFile.find_last_of("/\\");
    strpath = SourceFile.substr(0, found);
    return strpath;
#endif
}

std::string CMP_GetJustFileExt(const std::string& SourceFile)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    sfs::path   fp(SourceFile);
    std::string file_ext = fp.extension().string();
    return file_ext;
#else
    // Aternate Code
   // std::string fn = file;
   // string file_extension;
   // if (incDot)
   //     file_extension = fn.substr(fn.find_last_of("."));
   // else
   //     file_extension = fn.substr(fn.find_last_of(".") + 1);
   // if (upperCase)
   //     std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(),::toupper);
   // else
   //     std::transform(file_extension.begin(), file_extension.end(), file_extension.begin(), ::tolower);
   // return file_extension;
    return "";
#endif
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

void CMP_CLoseTextFile(FILE* fp)
{
    if (fp)
        fclose(fp);
}

bool CMP_IsHidden(const std::string& fullpath)
{
#ifdef _WIN32
    bool  IsHidden = false;
    DWORD Result   = GetFileAttributesA(fullpath.c_str());
    if (Result != 0xFFFFFFFF)
    {
        IsHidden = !!(Result & FILE_ATTRIBUTE_HIDDEN);
    }

    return IsHidden;
#else
    #if defined _CMP_CPP17_ || defined _CMP_CPP14_
        sfs::path path(fullpath);
        if (path.filename().string().find(".") == 0)
            return true;
    #else
        return false;
    #endif
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

void CMP_GetDirList(const std::string& directory, std::vector<std::string>& files, std::string filter)
{
#ifdef _WIN32
    WIN32_FIND_DATAA data;
    HANDLE           hFind;
    std::string      path(directory);
    std::string      fullpath;
    path.append("\\*");
    if ((hFind = FindFirstFileA(path.c_str(), &data)) != INVALID_HANDLE_VALUE)
    {
        do
        {
            fullpath = directory;
            fullpath.append("\\");
            fullpath.append(data.cFileName);
            if (CMP_PathType(fullpath.c_str()) == CMP_PATH_IS_FILE)
            {
                // check file attribute is not hidden
                bool  IsHidden = false;
                DWORD Result   = GetFileAttributesA(fullpath.c_str());
                if (Result != 0xFFFFFFFF)
                {
                    IsHidden = !!(Result & FILE_ATTRIBUTE_HIDDEN);
                }

                if (!IsHidden)
                {
                    // Get the file extension if a file filter is suppiled
                    if (filter.length() > 0)
                    {
                        std::string FileName(data.cFileName);
                        std::string ext = CMP_GetFilePathExtension(FileName);
                        std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
                        if (filter.find(ext) != std::string::npos)
                            files.push_back(fullpath);
                    }
                    else
                        files.push_back(fullpath);
                }
            }
        } while (FindNextFileA(hFind, &data) != 0);
        FindClose(hFind);
    }
#else
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    for (const sfs::directory_entry& entry : sfs::directory_iterator(directory))
    {
        if (CMP_IsHidden(entry.path().string()))
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
//file = "C:/Images/DevTestImages/results/13_shape32_BMP_BC7_1.DDS"
std::string CMP_GetFullPath(std::string file)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    sfs::path p(file);
    sfs::path fullpath = sfs::absolute(p);
    return fullpath.generic_string();
   //return sfs::absolute(file).string();
#else
    return "";
#endif
}

CMP_PATHTYPES CMP_PathType(const char* path)
{
//#if defined _CMP_CPP17_ || defined _CMP_CPP14_
//    // works only if file or dir exists!
//    if (sfs::is_directory(path))
//    {
//        return CMP_PATHTYPES::CMP_PATH_IS_DIR;
//    }
//    else if (sfs::is_regular_file(path))
//    {
//        return CMP_PATHTYPES::CMP_PATH_IS_FILE;
//    }
//#else
//#ifdef _WIN32
//    DWORD attrib = GetFileAttributesA(path);
//    if (attrib != INVALID_FILE_ATTRIBUTES)
//    {
//        if (attrib & FILE_ATTRIBUTE_DIRECTORY)
//        {
//            return CMP_PATHTYPES::CMP_PATH_IS_DIR;
//        }
//        else if (attrib & FILE_ATTRIBUTE_ARCHIVE)
//        {
//            return CMP_PATHTYPES::CMP_PATH_IS_FILE;
//        }
//        return CMP_PATH_IS_UNKNOWN;
//    }
//#endif
//#endif

    // a none existant file or dir
    std::string unkn = path;

    std::string basedir  = CMP_GetBaseDir(unkn);
    std::string ext      = CMP_GetFilePathExtension(unkn);
    std::string filename = CMP_GetFileName(unkn);

    // files should have an extension
    if (ext.length() > 0)
    {
        if ((  // we only support a limited relative path upto 2 levels up!
                (basedir.compare(".") == 0) || (basedir.compare("..\\..") == 0) || (basedir.compare("../..") == 0) || (basedir.compare("..") == 0)) &&
            ((ext.compare(0, 1, "\\") == 0) || (ext.compare(0, 1, "/") == 0)))
            return CMP_PATH_IS_DIR;
        return CMP_PATH_IS_FILE;
    }
    else
    {  // must be a folder or file with no extension
        if ((basedir.length() > 0))
            return CMP_PATH_IS_DIR;
        if (filename.length() > 0)
            return CMP_PATH_IS_FILE;
    }

    return CMP_PATHTYPES::CMP_PATH_IS_UNKNOWN;
}
