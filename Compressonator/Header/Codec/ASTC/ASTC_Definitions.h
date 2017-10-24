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
//////////////////////////////////////////////////////////////////////////////////

#ifndef _ASTC_DEFINITIONS_H_
#define _ASTC_DEFINITIONS_H_

#include "astc_codec_internals.h"
#include "softfloat.h"
#include <stdio.h>
#include <math.h>
#include <cstdint>

typedef std::uint8_t BYTE;

#define TRUE 1
#define FALSE 0

// notes: All of these definitions need changing some need to be variable!

// Size of a compressed block in bytes : 128 bits
#define ASTC_COMPRESSED_BLOCK_SIZE   16

//How many block candidates would be evaluated for every mode
#define ASTC_SCB_CANDIDATES 4

//How many partitioning candidates would be evaluated for every mode
#define ASTC_PARTITION_CANDIDATES_PER_MODE 2
#define ASTC_PARTITIONS_CANDIDATES_1PLANE (ASTC_PARTITION_CANDIDATES_PER_MODE * 3)
#define ASTC_PARTITIONS_CANDIDATES_2PLANES (ASTC_PARTITION_CANDIDATES_PER_MODE * 2)

// ASTC Specification based data

// OpenGL 4.0, table 3.2 (page 162) 
#define ASTC_GL_COMPRESSED          0x0000
#define ASTC_GL_UNSIGNED_SHORT      0x1403
#define ASTC_GL_HALF_FLOAT          0x140B
#define ASTC_GL_FLOAT               0x1406

// Define this to be 1 to allow "illegal" block sizes
#define DEBUG_ALLOW_ILLEGAL_BLOCK_SIZES 0

#endif