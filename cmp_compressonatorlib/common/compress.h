//=====================================================================
// Copyright (c) 2007-2014    Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006    ATI Technologies Inc.
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
// THE SOFTWARE./// \file Compress.h
//
/// \brief Declares the interface to the Compressonator library.
//
//=====================================================================

#ifndef COMPRESS_H
#define COMPRESS_H

#include "codec.h"
#include "float.h"

#define FP_EXCEPTION_MASK _MCW_EM // Disable fp exceptions

#ifdef _MSC_VER
#if _MSC_VER < 1400
#define DISABLE_FP_EXCEPTIONS \
        UINT nCurrentFP = _controlfp(0, 0); \
        _controlfp(FP_EXCEPTION_MASK, _MCW_EM);
#define RESTORE_FP_EXCEPTIONS \
        _controlfp(nCurrentFP, _MCW_EM);
#else // >= 1400
#define DISABLE_FP_EXCEPTIONS \
        unsigned int nCurrentFP = 0; \
        _controlfp_s(&nCurrentFP, FP_EXCEPTION_MASK, _MCW_EM);
#define RESTORE_FP_EXCEPTIONS \
        _controlfp_s(NULL, nCurrentFP, _MCW_EM);
#endif // >= 1400
#else
#define DISABLE_FP_EXCEPTIONS
#define RESTORE_FP_EXCEPTIONS
#endif

/*******************************
For prototyping Multithreaded Compression is been disabled
Navin May 2014
was #define THREADED_COMPRESS
*******************************/
#define THREADED_COMPRESS

#ifdef THREADED_COMPRESS
#include <thread>

#define MAX_THREADS 64u
static CMP_DWORD GetProcessorCount() {
    return std::thread::hardware_concurrency();
}

const CMP_DWORD f_dwProcessorCount = GetProcessorCount();
#endif




#endif // !COMPRESS_H
