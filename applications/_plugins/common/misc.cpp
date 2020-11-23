// AMD AMDUtils code
//
// Copyright(c) 2020 Advanced Micro Devices, Inc.All rights reserved.
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

#include "misc.h"

#ifndef _WIN32
#include <chrono>
static std::chrono::time_point<std::chrono::steady_clock> misc_start = std::chrono::steady_clock::now();
#endif

double MillisecondsNow() {
#ifdef _WIN32
    static LARGE_INTEGER s_frequency;
    static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    double milliseconds = 0;

    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        milliseconds = double(1000.0 * now.QuadPart) / s_frequency.QuadPart;
    } else {
        milliseconds = double(GetTickCount());
    }

    return milliseconds;
#else
    std::chrono::duration<double, std::milli> diff = std::chrono::steady_clock::now() - misc_start;
    return diff.count();
#endif
}

#ifdef _WIN32
// align uLocation to the next multiple of uAlign
SIZE_T Align(SIZE_T uOffset, SIZE_T uAlign) {
    if ((0 == uAlign) || (uAlign & (uAlign - 1))) {
        return -1;
    }

    return ((uOffset + (uAlign - 1)) & ~(uAlign - 1));
}

#endif