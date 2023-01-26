// Copyright(c) 2018 Advanced Micro Devices, Inc.All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef _CMP_FILEIO_H_
#define _CMP_FILEIO_H_

#include <string>
#include <vector>

enum CMP_PATHTYPES
{
    CMP_PATH_IS_UNKNOWN,
    CMP_PATH_IS_DIR,
    CMP_PATH_IS_FILE,
};

#define FILE_SPLIT_PATH "/\\"

bool CMP_CreateDirectory(std::string OutputFolder);

// Given a path string, determine the type of the path.
// This is done by purely through string processing, so the path doesn't need to actually exist
// NOTE: Paths that don't end in a slash ('/' or '\') are assumed to be files, even if they don't have extensions
CMP_PATHTYPES CMP_PathType(const char* path);

void  CMP_CreateTextFile(std::string& source_file);
FILE* CMP_OpenTextFile(char* SourceFile, const char* mode);
void  CMP_CloseTextFile(FILE* fp);

uintmax_t CMP_GetFileSize(const std::string& fileName);
bool  CMP_FileExists(const std::string& abs_filename);
void  CMP_FileCopy(std::string& source_file, std::string& destination_file);
bool  CMP_DirExists(const std::string& abs_dir);
bool  CMP_CreateDir(std::string sPath);

// Fills the "files" list with the file names of every file in the given directory (not recursive).
void  CMP_GetFilesInDirectory(const std::string& directory, std::vector<std::string>& files, std::string filter);

void  CMP_GetAllDirFilesList(const std::string& directory, std::vector<std::string>& files, std::string filter);

// This function assumes the input is a file path and returns the parent directory of SourceFile
std::string CMP_GetPath(const std::string& SourceFile);

std::string CMP_FindFile(const std::vector<std::string>& paths, const std::string& filepath);
std::string CMP_ExpandFilePath(const std::string& filepath);
std::string CMP_JoinPath(const std::string& path0, const std::string& path1);

std::string CMP_GetFilePathExtension(const std::string& fileName);
std::string CMP_GetFileExtension(const char* file, bool incDot, bool upperCase);
std::string CMP_GetJustFileExt(const std::string& SourceFile);

std::string CMP_GetBaseDir(const std::string& srcfileDirpath);

// Returns the file name and extension without any preceeding directory paths
std::string CMP_GetFileName(const std::string& filePath);
// Same as above, but also removes the file extension from the result
std::string CMP_GetJustFileName(const std::string& filePath);

std::string CMP_GetFileNameAndExt(const std::string& filePathName);

std::string CMP_GetFullPath(std::string file);

void CMP_ReplaceExt(std::string& s, const std::string& newExt);

#endif
