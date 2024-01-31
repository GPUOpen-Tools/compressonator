//=====================================================================
// Copyright 2020-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//=====================================================================

#include "cmp_fileio.h"

#ifdef _WIN32
#pragma warning(disable : 4244)
#pragma warning(disable : 4996)  // 'fopen': This function or variable may be unsafe.
#pragma warning(disable : 4100)  // unreferenced formal parameter
#include "windows.h"
#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <iterator>
#include <string>
#else
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <wordexp.h>
#endif

#include <fstream>

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

bool CMP_DirExists(const std::string& directory)
{
#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    if (sfs::exists(directory))
        return sfs::is_directory(directory);
    else
        return false;
#else
#ifdef _WIN32
    DWORD dwAttrib = GetFileAttributesA(directory.c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    DIR* dir = opendir(directory.c_str());
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
    bool  ret = false;
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

bool CMP_CreateDir(std::string dirName)
{
    bool success = true;

#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    try
    {
        success = sfs::create_directory(sfs::absolute(dirName));
    }
    catch (std::exception exception)
    {
        success = false;
    }
#else
    int error = 0;
#ifdef _WIN32
    error = _mkdir(dirName.c_str());
#else
    mode_t nMode = 0733;  // UNIX style permissions
    error        = mkdir(dirName.c_str(), nMode);
#endif
    success = error == 0;
#endif

    return success;
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
    std::string fileExt = "";

#if defined _CMP_CPP17_ || defined _CMP_CPP14_
    sfs::path fp(sourceFile);
    fileExt = fp.extension().string();
#else
    // Alternate Code
    fileExt = sourceFile.substr(sourceFile.find_last_of('.') + 1);
#endif

    return fileExt;
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

static std::vector<std::string> GetFilesInSubdirectory(const std::string& directory, const std::string& filter)
{
    std::vector<std::string> resultFiles;

#if defined(_CMP_CPP17_) || defined(_CMP_CPP14_)
    for (const sfs::directory_entry& entry : sfs::directory_iterator(directory))
    {
        if (CMP_IsHidden(entry.path().string()))
            continue;

        if (sfs::is_directory(entry.path()))
        {
            std::vector<std::string> subDirFiles = GetFilesInSubdirectory(entry.path().string(), filter);
            resultFiles.insert(resultFiles.end(), subDirFiles.begin(), subDirFiles.end());
        }
        else if (sfs::is_regular_file(entry.path()))
        {
            if (filter.length() > 0)
            {
                std::string extension = CMP_GetFileExtension(entry.path().string().c_str(), false, true);

                if (filter.find(extension) != std::string::npos)
                    resultFiles.push_back(entry.path().string());
            }
            else
                resultFiles.push_back(entry.path().string());
        }
    }
#elif defined(_WIN32)
    std::string path = directory + "\\*";

    WIN32_FIND_DATAA fileData = {};

    HANDLE fileHandle = FindFirstFileA(path.c_str(), &fileData);

    if (fileHandle == INVALID_HANDLE_VALUE)
        return {};

    do
    {
        std::string fullPath = directory + "\\" + fileData.cFileName;

        bool isHidden    = (fileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0;
        bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (fileData.cFileName[0] == '.' && fileData.cFileName[1] == 0)
            continue;
        if (fileData.cFileName[0] == '.' && fileData.cFileName[1] == '.' && fileData.cFileName[2] == 0)
            continue;

        if (isHidden)
            continue;

        if (isDirectory)
        {
            std::string subDir = directory + "\\" + fileData.cFileName;

            std::vector<std::string> subDirFiles = GetFilesInSubdirectory(subDir, filter);
            resultFiles.insert(resultFiles.end(), subDirFiles.begin(), subDirFiles.end());
        }
        else
        {
            if (filter.length() > 0)
            {
                std::string extension = CMP_GetFileExtension(fileData.cFileName, false, true);

                if (filter.find(extension) != std::string::npos)
                    resultFiles.push_back(fullPath);
            }
            else
                resultFiles.push_back(fullPath);
        }
    } while (FindNextFileA(fileHandle, &fileData) != 0);

    FindClose(fileHandle);
#endif

    return resultFiles;
}

std::vector<std::string> CMP_GetAllFilesInDirectory(const std::string& directory, const std::string& filter)
{
    if (!CMP_DirExists(directory))
        return {};

    return GetFilesInSubdirectory(directory, filter);
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
        return path1;
    if (path1.empty())
        return path0;

    // check '/'
    char lastChar  = *path0.rbegin();
    char firstChar = path1[0];

    if (lastChar != '/' && firstChar != '/')
        return path0 + std::string("/") + path1;

    else if (lastChar == '/' && firstChar == '/')
        return path0 + path1.substr(1, path1.size() - 1);

    else
        return path0 + path1;
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

std::string CMP_GetFileExtension(const char* fileName, bool includeDot, bool upperCase)
{
    std::string fileExt = CMP_GetJustFileExt(fileName);

    if (upperCase)
    {
        for (char& c : fileExt)
            c = (char)(toupper(c));
    }
    else
    {
        for (char& c : fileExt)
            c = (char)(tolower(c));
    }

    if (!includeDot)
    {
        fileExt.erase(std::remove(fileExt.begin(), fileExt.end(), '.'), fileExt.end());
    }

    return fileExt;
}

void CMP_CreateTextFile(std::string& sourceFile)
{
    FILE* fp;

#ifdef _WIN32
    fopen_s(&fp, sourceFile.c_str(), "a");
#else
    fp = fopen(sourceFile.c_str(), "a");
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

    std::string basedir  = CMP_GetBaseDir(pathString);
    std::string ext      = CMP_GetFilePathExtension(pathString);
    std::string filename = CMP_GetFileName(pathString);

    if (ext.length() > 0)
        return CMP_PATH_IS_FILE;

    // checking for the following three cases: '*/test/', '*/.', and '*/..'
    if (filename.length() == 0 || filename.back() == '.' || filename.compare(filename.length() - 2, 2, "..") == 0)
        return CMP_PATH_IS_DIR;
    else  // no extension, but still a valid file
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