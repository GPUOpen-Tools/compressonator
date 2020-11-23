#ifndef HDR_Encode_H
#define HDR_Encode_H

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
//  File Name:   HDR_Encode.h
//  Description: Reserved utils function for HDR process
//
//////////////////////////////////////////////////////////////////////////////

namespace HDR_Encode {
#define MAX_SUBSET_SIZE         16                       // Largest possible size for an individual subset
#define MAX_SUBSETS             3                        // Maximum number of possible subsets
#define MAX_INDEX_BITS          4                        // Maximum number of index bits
#define MAX_PARTITIONS          64                       // Maximum number of partition types
#define NUM_BLOCK_TYPES         8                        // Number of block types in the format
#define COMPRESSED_BLOCK_SIZE   16                       // Size of a compressed block in bytes
#define MAX_DIMENSION_BIG       4                        // Maximun number of Channels per Texel,  
// BC6H uses 3 channels in current encoder, 4th is reserved for future use
#define F16HMAX                 0x7bff                   // Max 16bit half float value (0x7BFF) + 1
#define MAX_END_POINTS          2                        // BC6H Maximum number of end point pairs (AB)

static int g_aWeights3[] = { 0, 9, 18, 27, 37, 46, 55, 64 };                                // 3 bit color Indices
static int g_aWeights4[] = { 0, 4, 9, 13, 17, 21, 26, 30, 34, 38, 43, 47, 51, 55, 60, 64 }; // 4 bit color indices

extern float lerpf(float a, float b, int i, int denom);
extern inline int NBits(int n, bool bIsSigned);
extern int QuantizeToInt(short value, int prec, bool signedfloat16, float exposure);
extern int Unquantize(int comp, unsigned char uBitsPerComp, bool bSigned);

extern  void    Partition(  int       shape,
                            float    in[][MAX_DIMENSION_BIG],
                            float    subsets[MAX_SUBSETS][MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],
                            int       count[MAX_SUBSETS],
                            int       ShapeTableToUse,
                            int       dimension);

// Used by optQuantAnD_d
#define MAX_ENTRIES                             64
#define MAX_TRY                                 4000
#define HDR_FLT_MAX_EXP                         128 // DBL_MAX_EXP_ = 1024
#define MAX_PARTITIONS_TABLE                    (1+64+64)

// Out contains all endpoints (out) calaculated for the input shape (data) and pattern (index)
extern  float optQuantAnD_d(
    float data[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int numEntries,
    int numClusters,
    int index[MAX_ENTRIES],
    float out[MAX_ENTRIES][MAX_DIMENSION_BIG],
    float direction[MAX_DIMENSION_BIG],
    float *step,
    int dimension,
    float quality
);

extern  void  GetEndPoints(float EndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], float outB[MAX_SUBSETS][MAX_SUBSET_SIZE][MAX_DIMENSION_BIG], int max_subsets, int entryCount[MAX_SUBSETS]);
extern  int   PARTITIONS[MAX_SUBSETS][MAX_PARTITIONS][MAX_SUBSET_SIZE];

extern  float ep_shaker_HD(
    float data[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int numEntries,
    int index_[MAX_ENTRIES],
    float out[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int epo_code[2][MAX_DIMENSION_BIG],
    int Mi_,                // last cluster
    int bits[3],            // including parity
    int dimension
);


//===========================================================================================
extern void init_ramps();
extern void deinit_ramps();

extern float ep_shaker_2_d(
    float data[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int numEntries,
    int index_[MAX_ENTRIES],
    float out[MAX_ENTRIES][MAX_DIMENSION_BIG],
    int epo_code[2][MAX_DIMENSION_BIG],
    int size,
    int Mi_,             // last cluster
    int bits,            // total for all channels
    // defined by total numbe of bits and dimensioin
    int dimension,
    float epo[2][MAX_DIMENSION_BIG]
);

} // name space HDR_Encode
#endif
