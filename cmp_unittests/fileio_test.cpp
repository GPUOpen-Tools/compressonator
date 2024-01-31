//=====================================================================
// Copyright (c) 2023-2024, Advanced Micro Devices, Inc. All rights reserved.
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

#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

#include "single_include/catch2/catch.hpp"

#include "compressonator.h"
#include "cmp_fileio.h"
#include "test_constants.h"
#include "direct.h"
#include "stdio.h"

TEST_CASE("CreateDirectory_EmptyName", "[fileio]")
{
    const std::string outputFolder = TEST_DATA_PATH + std::string("/ ");
    bool              status       = CMP_CreateDir(outputFolder);
    REQUIRE(status == false);
}

TEST_CASE("GetFileName_Basic", "[fileio]")
{
    const std::string filePath = TEST_DATA_PATH + std::string("/mipmap_128x512.png");
    SECTION("Get Just File Name")
    {
        const std::string result = CMP_GetJustFileName(filePath);
        REQUIRE(result == "mipmap_128x512");
    }

    SECTION("Get File Name")
    {
        const std::string result = CMP_GetFileName(filePath);
        REQUIRE(result == "mipmap_128x512.png");
    }
}

TEST_CASE("GetFileName_EmptyName", "[fileio]")
{
    const std::string filePath = TEST_DATA_PATH + std::string("/ ");
    SECTION("Get Just File Name")
    {
        const std::string result = CMP_GetJustFileName(filePath);
        REQUIRE(result == " ");
    }

    SECTION("Get File Name")
    {
        const std::string result = CMP_GetFileName(filePath);
        REQUIRE(result == " ");
    }
}

TEST_CASE("GetFileName_GivenWeirdName", "[fileio]")
{
    const std::string filePath = TEST_DATA_PATH + std::string("/mipmap_128x512./.png");
    SECTION("Get Just File Name")
    {
        const std::string result = CMP_GetJustFileName(filePath);
        REQUIRE(result == "");
    }

    SECTION("Get File Name")
    {
        const std::string result = CMP_GetFileName(filePath);
        REQUIRE(result == ".png");
    }
}

TEST_CASE("GetFileName_NoExt", "[fileio]")
{
    const std::string filePath = TEST_DATA_PATH + std::string("testCreateDir");
    SECTION("Get Just File Name")
    {
        const std::string result = CMP_GetJustFileName(filePath);
        REQUIRE(result == "test_datatestCreateDir");
    }

    SECTION("Get File Name")
    {
        const std::string result = CMP_GetFileName(filePath);
        REQUIRE(result == "test_datatestCreateDir");
    }
}

TEST_CASE("GetFileSize", "[fileio]")
{
    const std::string filePath = TEST_DATA_PATH + std::string("/mipmap_128x512.png");
    uintmax_t         result   = CMP_GetFileSize(filePath);
    REQUIRE(result == 1419);
}

TEST_CASE("GetFileSize_FileDoesNotExist", "[fileio]")
{
    const std::string filePath = TEST_DATA_PATH + std::string("/hello.txt");
    uintmax_t         result   = 0;
    REQUIRE_THROWS(result = CMP_GetFileSize(filePath));
}

bool CompareFiles(std::string& sourceFile, std::string& destinationFile)
{
    FILE*   fpSrc;
    errno_t err = fopen_s(&fpSrc, sourceFile.c_str(), "rb");
    if (err != 0)
        return false;

    fseek(fpSrc, 0, SEEK_END);
    int sourceSize = ftell(fpSrc);
    rewind(fpSrc);

    FILE* fpDest;
    err = fopen_s(&fpDest, destinationFile.c_str(), "rb");
    if (err != 0)
    {
        fclose(fpSrc);
        return false;
    }

    fseek(fpDest, 0, SEEK_END);
    int destSize = ftell(fpDest);
    rewind(fpDest);

    if (sourceSize != destSize)
    {
        fclose(fpSrc);
        fclose(fpDest);
        return false;
    }

    int  c1, c2;
    bool filesEqual = true;
    while (1)
    {
        c1 = getc(fpSrc);
        c2 = getc(fpDest);

        if (c1 != c2)
        {
            filesEqual = false;
            break;
        }

        if ((c1 == EOF) || (c2 == EOF))
            break;
    }

    fclose(fpSrc);
    fclose(fpDest);

    return filesEqual;
}

TEST_CASE("FileCopy", "[fileio]")
{
    std::string sourceFile = TEST_DATA_PATH + std::string("/mipmap_128x512.png");
    std::string destFile   = TEST_DATA_PATH + std::string("/copy_mipmap_128x512.png");

    if (CMP_FileExists(destFile))
        remove(destFile.c_str());

    CMP_FileCopy(sourceFile, destFile);
    bool compare = CompareFiles(sourceFile, destFile);
    REQUIRE(compare == true);
}

TEST_CASE("FileCopy_SourceDoesNotExist", "[fileio]")
{
    std::string sourceFile = TEST_DATA_PATH + std::string("/cat.png");
    std::string destFile   = TEST_DATA_PATH + std::string("/copy_mipmap_128x512.png");

    if (CMP_FileExists(destFile))
        remove(destFile.c_str());

    REQUIRE_THROWS(CMP_FileCopy(sourceFile, destFile));
    //bool compare = CompareFiles(sourceFile, destFile);
    //REQUIRE_FALSE(compare);
}

TEST_CASE("DirExists_Basic", "[fileio]")
{
    const std::string directory = TEST_DATA_PATH + std::string("/testCreateDir");
    int               error     = 0;
    if (CMP_DirExists(directory))
        error = _rmdir(directory.c_str());
    REQUIRE(error == 0);

    error = _mkdir(directory.c_str());
    REQUIRE(error == 0);

    bool dirExist = CMP_DirExists(directory);

    REQUIRE(dirExist == true);
}

TEST_CASE("DirExists_DirDoesNotExist", "[fileio]")
{
    const std::string directory = TEST_DATA_PATH + std::string("/hello");
    bool              dirExist  = CMP_DirExists(directory);
    REQUIRE(dirExist == false);
}

TEST_CASE("DirExists_GivenFile", "[fileio]")
{
    const std::string directory = TEST_DATA_PATH + std::string("/createTextFile.txt");
    bool              dirExist  = CMP_DirExists(directory);
    REQUIRE(dirExist == false);
}

TEST_CASE("FileExists_Basic", "[fileio]")
{
    std::string file   = TEST_DATA_PATH + std::string("/mipmap_128x512.png");
    bool        isFile = CMP_FileExists(file);
    REQUIRE(isFile == true);
}

TEST_CASE("FileExists_FileDoesNotExist", "[fileio]")
{
    std::string file      = TEST_DATA_PATH + std::string("/hello.png");
    bool        fileExist = CMP_FileExists(file);
    REQUIRE(fileExist == false);
}

TEST_CASE("CreateDir_ValidName", "[fileio]")
{
    const std::string directory = TEST_DATA_PATH + std::string("/testCreateDir");
    int               error     = 0;

    bool dirExist = CMP_DirExists(directory);
    if (dirExist == true)
        error = _rmdir(directory.c_str());

    REQUIRE(error == 0);

    bool dirCreated = CMP_CreateDir(directory);
    REQUIRE(dirCreated == true);
}

TEST_CASE("CreateDir_InvalidName", "[fileio]")
{
    const std::string directory = TEST_DATA_PATH + std::string("/testCreateDir?bad:");
    int               error     = 0;

    bool dirExist = CMP_DirExists(directory);
    if (dirExist == true)
        error = _rmdir(directory.c_str());

    REQUIRE(error == 0);

    bool dirCreated = CMP_CreateDir(directory);
    REQUIRE(dirCreated == false);
}

TEST_CASE("GetFileNameAndExt", "[fileio]")
{
    std::string path       = TEST_DATA_PATH + std::string("/mipmap_128x512.png");
    std::string nameAndExt = CMP_GetFileNameAndExt(path);
    REQUIRE(nameAndExt == "mipmap_128x512.png");
}

TEST_CASE("GetFileNameAndExt_EmptyString", "[fileio]")
{
    std::string path       = TEST_DATA_PATH + std::string(" ");
    std::string nameAndExt = CMP_GetFileNameAndExt(path);
    REQUIRE(nameAndExt == "test_data ");
}

TEST_CASE("FileExt", "[fileio]")
{
    const std::string path = TEST_DATA_PATH + std::string("/mipmap_128x512.png");

    SECTION("GetJustFileExt")
    {
        std::string extention = CMP_GetJustFileExt(path);
        REQUIRE(extention == ".png");
    }

    SECTION("GetFilePathExtension")
    {
        std::string extention = CMP_GetFilePathExtension(path);
        REQUIRE(extention == "png");
    }

    SECTION("GetFileExtension")
    {
        bool        incDot        = true;
        bool        upperCase     = true;
        const char* pathChar      = path.c_str();
        std::string fileExtension = CMP_GetFileExtension(pathChar, incDot, upperCase);
        REQUIRE(fileExtension == ".PNG");
    }
}

TEST_CASE("FileExt_GivenDir", "[fileio]")
{
    const std::string path = TEST_DATA_PATH + std::string("testCreateDir");

    SECTION("GetJustFileExt")
    {
        std::string extention = CMP_GetJustFileExt(path);
        REQUIRE(extention == "");
    }

    SECTION("GetFilePathExtension")
    {
        std::string extention = CMP_GetFilePathExtension(path);
        REQUIRE(extention == "");
    }

    SECTION("GetFileExtension")
    {
        bool        incDot        = true;
        bool        upperCase     = true;
        const char* pathChar      = path.c_str();
        std::string fileExtension = CMP_GetFileExtension(pathChar, incDot, upperCase);
        REQUIRE(fileExtension == "");
    }
}

TEST_CASE("GetPath", "[fileio]")
{
    std::string path = TEST_DATA_PATH + std::string("/mipmap_128x512.png");
    std::string returnedPath;

    SECTION("GetParentPath")
    {
        returnedPath = CMP_GetPath(path);
        REQUIRE(returnedPath == "./test_data");
    }

    SECTION("GetFullPath")
    {
        // TO-DO
        // Cannot test absolute path since the test will only work on author's local device
    }

    SECTION("GetBaseDir")
    {
        returnedPath = CMP_GetBaseDir(path);
        REQUIRE(returnedPath == "./test_data");
    }
}

TEST_CASE("IsHidden_Basic", "[fileio]")
{
    std::string path     = TEST_DATA_PATH + std::string("/mipmap_128x512.png");
    bool        IsHidden = CMP_IsHidden(path);
    REQUIRE(IsHidden == false);
}

TEST_CASE("IsHidden_FileDoesNotExist", "[fileio]")
{
    std::string path     = TEST_DATA_PATH + std::string("/hello.png");
    bool        IsHidden = CMP_IsHidden(path);
    REQUIRE(IsHidden == false);
}

TEST_CASE("GetAllDirFilesList", "[fileio]")
{
    std::string path = TEST_DATA_PATH;
    // The current format of file path is needed because of the nature of the functions called. CMP_GetAllFilesInDirectory manually adds "\" to the file path name.
    std::vector<std::string> files = {"./test_data\\16bit_image.png",
                                      "./test_data\\mipmap_128x128.png",
                                      "./test_data\\mipmap_128x512.png",
                                      "./test_data\\mipmap_512x128.png",
                                      "./test_data\\ruby.bmp"};

    std::string filter;

    SECTION("CMP_GetAllFilesInDirectory")
    {
        std::vector<std::string> returnedFiles = CMP_GetAllFilesInDirectory(path, filter);

        for (std::string& expectedFile : files)
        {
            CHECK(std::find(returnedFiles.begin(), returnedFiles.end(), expectedFile) != returnedFiles.end());
        }
    }
}

//=================== Generic for all platforms and c++ standards ===============================

TEST_CASE("CMP_ExpandFilePath", "[fileio]")
{
    std::string path   = TEST_DATA_PATH + std::string("/mipmap_128x512.png");
    std::string result = CMP_ExpandFilePath(path);
    REQUIRE(result == "./test_data/mipmap_128x512.png");
}

TEST_CASE("JoinPath_OnlyP1Slash", "[fileio]")
{
    std::string path0  = TEST_DATA_PATH;
    std::string path1  = "/mipmap_128x512.png";
    std::string result = CMP_JoinPath(path0, path1);
    REQUIRE(result == "./test_data/mipmap_128x512.png");
}

TEST_CASE("JoinPath_BothNoSlash", "[fileio]")
{
    std::string path0  = TEST_DATA_PATH;
    std::string path1  = "mipmap_128x512.png";
    std::string result = CMP_JoinPath(path0, path1);
    REQUIRE(result == "./test_data/mipmap_128x512.png");
}

TEST_CASE("JoinPath_BothSlash", "[fileio]")
{
    std::string path0  = TEST_DATA_PATH + std::string("/");
    std::string path1  = "/mipmap_128x512.png";
    std::string result = CMP_JoinPath(path0, path1);
    REQUIRE(result == "./test_data/mipmap_128x512.png");
}

TEST_CASE("JoinPath_OnlyP0Slash", "[fileio]")
{
    std::string path0  = TEST_DATA_PATH + std::string("/");
    std::string path1  = "mipmap_128x512.png";
    std::string result = CMP_JoinPath(path0, path1);
    REQUIRE(result == "./test_data/mipmap_128x512.png");
}

TEST_CASE("FindFile", "[fileio]")
{
    const std::vector<std::string> paths    = {"./images", "./plugins", "./projects", "./resources", "./test_data"};
    std::string                    filepath = "/mipmap_128x512.png";
    std::string                    result   = CMP_FindFile(paths, filepath);
    REQUIRE(result == "./test_data/mipmap_128x512.png");
}

TEST_CASE("FindFile_FileDoesNotExist", "[fileio]")
{
    const std::vector<std::string> paths    = {"./images", "./plugins", "./projects", "./resources", "./test_data"};
    std::string                    filepath = "/hello.png";
    std::string                    result   = CMP_FindFile(paths, filepath);
    REQUIRE(result == "");
}

TEST_CASE("CMP_CreateTextFile", "[fileio]")
{
    std::string sourceFile = TEST_DATA_PATH + std::string("/createTextFile.txt");
    int         error      = 0;
    bool        exist      = CMP_FileExists(sourceFile);
    if (exist == true)
        error = remove(sourceFile.c_str());

    REQUIRE(error == 0);

    CMP_CreateTextFile(sourceFile);
    bool fileCreated = CMP_FileExists(sourceFile);
    REQUIRE(fileCreated == true);
}

TEST_CASE("CMP_OpenTextFile", "[fileio]")
{
    std::string sourceFile     = TEST_DATA_PATH + std::string("/createTextFile.txt");
    char*       sourceFileChar = &sourceFile[0];
    FILE*       fp             = CMP_OpenTextFile(sourceFileChar, "r");
    REQUIRE(fp != NULL);
    CMP_CloseTextFile(fp);
}

TEST_CASE("CMP_PathType_Basic", "[fileio]")
{
    std::string   path     = TEST_DATA_PATH + std::string("/createTextFile.txt");
    const char*   pathChar = path.c_str();
    CMP_PATHTYPES pathtype = CMP_PathType(pathChar);
    REQUIRE(pathtype == CMP_PATH_IS_FILE);
}

TEST_CASE("CMP_PathType_RelativePath", "[fileio]")
{
    std::string   path     = "..\..\testing.txt";
    const char*   pathChar = path.c_str();
    CMP_PATHTYPES pathtype = CMP_PathType(pathChar);
    REQUIRE(pathtype == CMP_PATH_IS_FILE);
}

TEST_CASE("CMP_PathType_NoExtensionFile", "[fileio]")
{
    std::string   path     = TEST_DATA_PATH + std::string("/createTextFile");
    const char*   pathChar = path.c_str();
    CMP_PATHTYPES pathtype = CMP_PathType(pathChar);
    REQUIRE(pathtype == CMP_PATH_IS_FILE);
}

TEST_CASE("CMP_ReplaceExt", "[fileio]")
{
    std::string file   = TEST_DATA_PATH + std::string("/createTextFile.txt");
    std::string newExt = "pdf";
    CMP_ReplaceExt(file, newExt);
    std::string newFile = TEST_DATA_PATH + std::string("/createTextFile.pdf");
    REQUIRE(file == newFile);
}