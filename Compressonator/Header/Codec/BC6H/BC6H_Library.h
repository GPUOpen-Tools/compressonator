//===============================================================================
// Copyright (c) 2014-2016  Advanced Micro Devices, Inc. All rights reserved.
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
// File : BC6H_Library.h
//
// Reference implementation of a multithreaded BC6H block compressor.
//
// Version 0.1
//
//-----------------------------------------------------------------------------

#ifndef _BC6H_LIBRARY_H_
#define _BC6H_LIBRARY_H_

#include "Compressonator.h"
#include "BC6H_Definitions.h"

// Maximum number of threads supported by the compressor
#define BC6H_MAX_THREADS     128

// Number of image components
#define BC6H_COMPONENT_COUNT 4

// Number of pixels in a BC6H block
#define BC6H_BLOCK_PIXELS   16

typedef enum _BC6H_ERROR
{
    BC6H_ERROR_NONE                = 0,
    BC6H_ERROR_OUT_OF_MEMORY    = 1,
} BC6H_ERROR;

typedef enum _BC6H_MODES
{
    BC6H_ONE = 0,
    BC6H_TWO = 1,
} TYPE_BC6H_MODES;

// Descriptor structure for block encodings
typedef struct
{
    enum _BC6H_MODES  type;   // Type of block
} BC6MODE;

#endif
