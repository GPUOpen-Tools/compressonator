#ifndef _COMMON_DEFINITIONS_H
#define _COMMON_DEFINITIONS_H

//===============================================================================
// Copyright (c) 2007-2017  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//===============================================================================
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
//
//  File Name:   Common_Def.h
//  Description: common definitions used for future
//
//////////////////////////////////////////////////////////////////////////////


//#define USE_BLOCK_LINEAR

// BC7 build options- reserved for future bc7 kernel development
//#define USE_OPT_QUANTIZER     // High Quality
#define USE_OPT_QUANTIZER2      // Poor Quality : use this as default for BC7
#define USE_optQUANT            // Good Quality

#ifndef __OPENCL_VERSION__
#define USE_QSORT               // Default use non qsort, enable to use qsort (qsort function only work for cpu)
#endif


//#define USE_DOUBLE         // Default use float, enable to use double

//### Automated section set by ComputeLib SDK at run time, do not edit
//
//###
#endif
