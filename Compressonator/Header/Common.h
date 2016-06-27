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
// THE SOFTWARE.
//=====================================================================

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <tchar.h>
#include <string>

#include "Compressonator.h"

using namespace std;

#define UNREFERENCED_PARAMETER(P)          (P)

//#define USE_DBGTRACE                                //  Show messages via Win Debug 
//#define BC7_DEBUG_TO_RESULTS_TXT                    //  Send debug info to a results text file
//#define DXT5_COMPDEBUGGER                           //  Remote connect data to Comp Debugger views
//#define BC6H_COMPDEBUGGER                           //  Remote connect data to Comp Debugger views
//#define BC7_COMPDEBUGGER                            //  Remote connect data to Comp Debugger views
//#define BC6H_NO_OPTIMIZE_ENDPOINTS                  //  Turn off BC6H optimization of endpoints - useful for debugging quantization and mode checking
//#define BC6H_DEBUG_TO_RESULTS_TXT                   //  Generates a Results.txt file on exe working directory; MultiThreading is turned off for debuging to text file
//#define BC6H_DECODE_DEBUG                           //  Enables debug info on decoder

//#define MAKE_FORMAT_COMPATIBLE                      //  Byte<->Float to make all source and dest compatible

// Internal experimental code
// #define ARGB_32_SUPPORT
// #define SUPPORT_ETC_ALPHA

#endif // !COMMON_H
