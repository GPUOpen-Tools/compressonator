//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef _BC7_DEFINITIONS_H_
#define _BC7_DEFINITIONS_H_

#include "Compressonator.h"

#include "3dquant_constants.h"

#include <cstdint>

//typedef std::uint8_t    BYTE;
//typedef std::uint32_t   DWORD;
//typedef std::int32_t    BOOL;

#define TRUE 1
#define FALSE 0

// Largest possible size for an individual subset
#define MAX_SUBSET_SIZE         16

// Maximum number of possible subsets
#define MAX_SUBSETS             3

// Maximum number of index bits
#define MAX_INDEX_BITS          4
 
// Maximum number of partition types
#define MAX_PARTITIONS          64

// Number of block types in the format
#define NUM_BLOCK_TYPES         8

// Size of a compressed block in bytes
#define COMPRESSED_BLOCK_SIZE   16

// If this define is set then 6-bit weights will be used for the ramp.
// Otherwise the ramp will use a pure linear interpolation
#define USE_FINAL_BC7_WEIGHTS   1

// If this is defined, ramp calculation is done via math floor and division.
// Otherwise, ramp calculation is done by bit shifting
#define USE_HIGH_PRECISION_INTERPOLATION_BC7 

#define MAX_PARTITIONS_TABLE (1+64+64)

#define MAX_ENTRIES_QUANT_TRACE     16
#define MAX_CLUSTERS_QUANT_TRACE    8

typedef enum _COMPONENT
{
    COMP_RED   = 0,
    COMP_GREEN = 1,
    COMP_BLUE =  2,
    COMP_ALPHA = 3
} COMPONENT;

// Block component encoding
typedef enum
{
    NO_ALPHA,
    COMBINED_ALPHA,
    SEPARATE_ALPHA
} CMP_BCE;

// Endpoint encoding type
typedef enum
{
    NO_PBIT,
    ONE_PBIT,
    TWO_PBIT,
    THREE_PBIT,
    FOUR_PBIT,
    FIVE_PBIT
} CMP_PBIT; 

// Descriptor structure for block encodings
typedef struct
{
    CMP_BCE encodingType;           // Type of block
    CMP_DWORD   partitionBits;          // Number of bits for partition data
    CMP_DWORD   rotationBits;           // Number of bits for component rotation
    CMP_DWORD   indexModeBits;          // Number of bits for index selection
    CMP_DWORD   scalarBits;             // Number of bits for one scalar endpoint
    CMP_DWORD   vectorBits;             // Number of bits for one vector endpoint(excluding P bits)
    CMP_PBIT  pBitType;               // Type of P-bit encoding
    CMP_DWORD   subsetCount;            // Number of subsets
    CMP_DWORD   indexBits[2];           // Number of bits per index in each index set
} CMP_BTI;

extern CMP_BTI bti[NUM_BLOCK_TYPES];

#include "MathMacros.h"

#endif