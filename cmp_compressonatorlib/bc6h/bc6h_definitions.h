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

#ifndef _BC6H_DEFINITIONS_H_
#define _BC6H_DEFINITIONS_H_

#include "hdr_encode.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>

typedef std::uint8_t BYTE;
typedef std::uint32_t DWORD;

#define TRUE 1
#define FALSE 0

#define BC6H_NUM_BLOCK_TYPES            14
#define BC6H_MAX_PARTITIONS             32
#define BC6H_MAX_SUBSETS                2
#define BC6H_MAX_SUBSET_SIZE            16
#define BC6H_COMPRESSED_BLOCK_SIZE      16
#define BC6H_MAX_PARTITIONS_TABLE       (1+64+64)
#define BC6H_MAX_INDEX_BITS             4        // Maximum number of index bits: This should be set to 3
#define NCHANNELS                       3
#define NINDICES                        16
#define MAXENDPOINTS                    2

// these are the limits that can be found in half.h
#define F16_MAX                         65504.0f
#define F16_MAX_NEGATIVE                -65504.0f

// F16 limits in bits, converted to floating point numbers. Useful for our implementation.
#define F16_MAX_BITS                    ((float)0x7BFF)
#define F16_MAX_NEGATIVE_BITS           (-F16_MAX_BITS)

#define F16S_MASK                       0x8000   // f16 sign mask
#define F16EM_MASK                      0x7fff   // f16 exp & mantissa mask
#define U16MAX                          0xffff
#define S16MIN                          (-0x8000)
#define S16MAX                          0x7fff
#define INT16_MASK                      0xffff
#define BC6H_WEIGHT_MAX                 64       // scale from 0 to 64
#define BC6H_WEIGHT_SHIFT               6        // Using shift operator (Multipler or divider by 64)
#define BC6H_WEIGHT_ROUND               32       // take care of round up errors
#define BC6H_OFFSET_MAG                 31       // This offsets the colors from the actual line by a factor mag/32 or mag/64
#define MAX_TWOREGION_MODES             10
#define MAX_BC6H_MODES                  14
#define MAX_BC6H_PARTITIONS             32
#define HIGH_INDEX_BIT                  4
#define ONE_REGION_INDEX_OFFSET         65        // bit location to start saving color index values for single region shape
#define TWO_REGION_INDEX_OFFSET         82        // bit location to start saving color index values for two region shapes
#define MIN_MODE_FOR_ONE_REGION         11        // Two regions shapes use modes 1..9 and single use 11..14
#define R_0(ep)                         (ep)[0][0][i]
#define R_1(ep)                         (ep)[0][1][i]
#define R_2(ep)                         (ep)[1][0][i]
#define R_3(ep)                         (ep)[1][1][i]
#define MASK(n)                         ((1<<(n))-1)
#define SIGN_EXTEND(w,tbits)            ((((signed(w))&(1<<((tbits)-1)))?((~0)<<(tbits)):0)|(signed(w)))
#define REGION(x,y,si)                  shapes[((si)&3)*4+((si)>>2)*64+(x)+(y)*16]
#define BC6H_NREGIONS                   2                 // shapes for two regions

typedef unsigned int uint;
typedef enum _BC6H_COMPONENT {
    BC6H_COMP_RED   = 0,
    BC6H_COMP_GREEN = 1,
    BC6H_COMP_BLUE =  2,
    BC6H_COMP_ALPHA = 3
} BC6H_COMPONENT;

enum {
    BC6_ONE = 0,
    BC6_TWO
};

enum {
    C_RED = 0,
    C_GREEN,
    C_BLUE
};

enum {
    UNSIGNED_F16 = 1,
    SIGNED_F16     = 2
};

enum EField {
    NA, // N/A
    M,  // Mode
    D,  // Shape
    RW,
    RX,
    RY,
    RZ,
    GW,
    GX,
    GY,
    GZ,
    BW,
    BX,
    BY,
    BZ,
};

struct EndPointPair {
    int A;
    int B;
};


struct BC6H_Vec3 {
    int x,y,z;
};

struct BC6H_Vec3f {
    float x, y, z;
};

struct UShortVec3 {
    unsigned short x,y,z;
};


struct ModePartitions {
    int nbits;              // Number of bits
    int prec[3];            // precission of the Qunatized RGB endpoints
    int transformed;        // if 0, deltas are unsigned and no transform; otherwise, signed and transformed
    int modebits;           // number of mode bits
    int IndexPrec;          // Index Precision
    int mode;               // Mode value to save
    int lowestPrec;         // Step size of each precesion incriment
};

static ModePartitions ModePartition[MAX_BC6H_MODES +1] = {
    0,    0,0,0,        0,    0,    0,    0,     0,   // Mode = Invaild

    // Two region Partition
    10,   5,5,5,        1,    2,    3,    0x00,  31,    // Mode = 1
    7,    6,6,6,        1,    2,    3,    0x01,  248,   // Mode = 2
    11,   5,4,4,        1,    5,    3,    0x02,  15,    // Mode = 3
    11,   4,5,4,        1,    5,    3,    0x06,  15,    // Mode = 4
    11,   4,4,5,        1,    5,    3,    0x0a,  15,    // Mode = 5
    9,    5,5,5,        1,    5,    3,    0x0e,  62,    // Mode = 6
    8,    6,5,5,        1,    5,    3,    0x12,  124,   // Mode = 7
    8,    5,6,5,        1,    5,    3,    0x16,  124,   // Mode = 8
    8,    5,5,6,        1,    5,    3,    0x1a,  124,   // Mode = 9
    6,    6,6,6,        0,    5,    3,    0x1e,  496,   // Mode = 10

    // One region Partition
    10,   10,10,10,     0,    5,    4,    0x03,  31,    // Mode = 11
    11,   9,9,9,        1,    5,    4,    0x07,  15,    // Mode = 12
    12,   8,8,8,        1,    5,    4,    0x0b,  7,     // Mode = 13
    16,   4,4,4,        1,    5,    4,    0x0f,  1,     // Mode = 14
};


//  Shape, Fixup Index locations for region 1's (subset = 2)
// locations accroding to BC6H documentation on a 4x4 block
//        15,15,15, 15,
//        15,15,15, 15,
//        15,15,15, 15,
//        15,15,15, 15,
//        15, 2, 8,  2,
//         2, 8, 8, 15,
//         2, 8, 2,  2,
//         8, 8, 2,  2,
//
// The Region2FixUps are for our index[subset = 2][16][3] locations
// indexed by shape region 2
static const int g_Region2FixUp[32] = {
    7, 3, 11, 7,
    3, 11, 9, 5,
    2, 12, 7, 3,
    11, 7, 11, 3,
    7, 1, 0, 1,
    0, 1, 0, 7,
    0, 1, 1, 0,
    4, 4, 1, 0,
};

// Indexed by all shape regions
// Partition Set Fixups for region 1 note region 0 is always at 0
// that means normally we use 3 bits to define an index value
// if its at the fix up location then its one bit less
static const int g_indexfixups[32] = {
    15,15,15,15,
    15,15,15,15,
    15,15,15,15,
    15,15,15,15,
    15, 2, 8, 2,
    2, 8, 8,15,
    2, 8, 2, 2,
    8, 8, 2, 2,
};


//------------------------------------------------------------------------------


/*============================================================================================================
MODE FORMAT TABLE:

Mode    Partition Indices    Partition    Color Endpoints                        Mode Bits            Dec value
1        46 bits                5 bits        75 bits (10.555, 10.555, 10.555)   2 bits (00)           0
2        46 bits                5 bits        75 bits (7666, 7666, 7666)         2 bits (01)           1
3        46 bits                5 bits        72 bits (11.555, 11.444, 11.444)   5 bits (00010)        2
4        46 bits                5 bits        72 bits (11.444, 11.555, 11.444)   5 bits (00110)        6
5        46 bits                5 bits        72 bits (11.444, 11.444, 11.555)   5 bits (01010)        10
6        46 bits                5 bits        72 bits (9555, 9555, 9555)         5 bits (01110)        14
7        46 bits                5 bits        72 bits (8666, 8555, 8555)         5 bits (10010)        18
8        46 bits                5 bits        72 bits (8555, 8666, 8555)         5 bits (10110)        22
9        46 bits                5 bits        72 bits (8555, 8555, 8666)         5 bits (11010)        26
10       46 bits                5 bits        72 bits (6666, 6666, 6666)         5 bits (11110)        30
11       63 bits                0 bits        60 bits (10.10, 10.10, 10.10)      5 bits (00011)        3
12       63 bits                0 bits        60 bits (11.9, 11.9, 11.9)         5 bits (00111)        7
13       63 bits                0 bits        60 bits (12.8, 12.8, 12.8)         5 bits (01011)        11
14       63 bits                0 bits        60 bits (16.4, 16.4, 16.4)         5 bits (01111)        15
============================================================================================================*/

struct END_Points {
    int A[NCHANNELS];
    int B[NCHANNELS];
};

struct AMD_BC6H_Format {
    unsigned short region;             // one or two
    unsigned short m_mode;             // m
    int d_shape_index;                 // d
    int rw;                            // endpt[0].A[0]
    int rx;                            // endpt[0].B[0]
    int ry;                            // endpt[1].A[0]
    int rz;                            // endpt[1].B[0]
    int gw;                            // endpt[0].A[1]
    int gx;                            // endpt[0].B[1]
    int gy;                            // endpt[1].A[1]
    int gz;                            // endpt[1].B[1]
    int bw;                            // endpt[0].A[2]
    int bx;                            // endpt[0].B[2]
    int by;                            // endpt[1].A[2]
    int bz;                            // endpt[1].B[2]

    union {
        std::uint8_t indices[4][4];            // Indices data after header block
        std::uint8_t indices16[16];
    };

    float         din[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG];   // Original data input
    END_Points    EC[MAXENDPOINTS];    // compressed endpoints expressed as endpt[0].A[] and endpt[1].B[]
    END_Points    E[MAXENDPOINTS];     // decompressed endpoints
    bool          issigned;            // Format is 16 bit signed floating point
    bool          istransformed;       // region two: all modes = true except mode=10
    short         wBits;               // number of bits for the root endpoint
    short         tBits[NCHANNELS];    // number of bits used for the transformed endpoints
    int           format;              // floating point format are we using for decompression
    BC6H_Vec3      Palete[2][16];
    BC6H_Vec3f     Paletef[2][16];

    int           index;               // for debugging
    float         fEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG];
    float         cur_best_fEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG];
    int           shape_indices[MAX_SUBSETS][MAX_SUBSET_SIZE];
    int           cur_best_shape_indices[MAX_SUBSETS][MAX_SUBSET_SIZE];
    int           entryCount[MAX_SUBSETS];
    int           cur_best_entryCount[MAX_SUBSETS];
    float         partition[MAX_SUBSETS][MAX_SUBSET_SIZE][MAX_DIMENSION_BIG];
    float         cur_best_partition[MAX_SUBSETS][MAX_SUBSET_SIZE][MAX_DIMENSION_BIG];
    bool          optimized;           // were end points optimized during final encoding
};

extern unsigned short    g_BC6H_FORMAT;

#endif
