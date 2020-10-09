// AMD AMDUtils code
//
// Copyright(c) 2017 Advanced Micro Devices, Inc.All rights reserved.
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

#include "error.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <iostream>
#endif

void ShowErrorMessageBox(int result) {
#ifdef _WIN32
    wchar_t err[256];
    memset(err, 0, 256);
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, static_cast<HRESULT>(result), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
    int msgboxID = MessageBoxW(NULL, err, (LPCWSTR)L"Error", MB_OK);
#else
    std::cerr << "Error " << result << std::endl;
#endif
}