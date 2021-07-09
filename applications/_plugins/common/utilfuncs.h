//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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
// UtilFuncs.h : header file for utility functions
//

#ifndef _AMD_TEX_UTILSFUNCS_H_INCLUDED_
#define _AMD_TEX_UTILSFUNCS_H_INCLUDED_

#include <string>

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
HWND FindTopLevelWindow(TCHAR *pszName);
#endif

#include "cmp_fileio.h"

#define CMP_STATENUM 14 //14 characters for the state
#define CMP_COPY "# CMP_filecopy"
#define CMP_PROCESSED "# CMP_fileproc"
#define CMP_ORIGINAL ""
#define CMP_FILE_ERROR "fileIOError"

void SwizzleBytes(void *src, unsigned long numBytes);
float HalfToFloat(uint16_t h);
void getFileNameExt(const char *FilePathName, char *fnameExt, int maxbuffsize);
#ifdef _WIN32
bool writeObjFileState(std::string filename, std::string state);
std::string readObjFileState(std::string filename);
#endif
#endif // !defined(_AMD_TEX_UTILSFUNCS_H_INCLUDED_)
