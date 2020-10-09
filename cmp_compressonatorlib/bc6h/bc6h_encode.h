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

#ifndef _BC6H_ENCODE_H_
#define _BC6H_ENCODE_H_

#include "compressonator.h"
#include "bc6h_definitions.h"

#include <float.h>

//#define DEBUG_PATTERNS                // Define if you want to debug pattern matching
//#define USE_KNOWN_PATTERNS            // Enable this if you want to bipass using user images and use the known 32 BC6H patterns

#ifdef DEBUG_PATTERNS
#define TOP_OFFSET          0
#define BASE_OFFSET         0
#define RANDOM_NOISE_LEVEL 10
#endif

#define NOISE_BAND_WIDTH 0.2
#define LEVEL_BAND_GAP   0.2

#define DELTA_UP         0
#define DELTA_RIGHT      1
#define DELTA_DOWN       2
#define DELTA_LEFT       3


class BC6HBlockEncoder {
  public:

    BC6HBlockEncoder(CMP_BC6H_BLOCK_PARAMETERS user_options) {
        m_quality                 = user_options.fQuality;
        m_useMonoShapePatterns    = user_options.bUsePatternRec;
        m_isSigned                = user_options.bIsSigned;
        m_ModeMask                = user_options.dwMask;
        m_Exposure                = user_options.fExposure;
        m_bAverageEndPoint        = true;
        m_DiffLevel               = 0.01f;
    };

    ~BC6HBlockEncoder() {};

    float   CompressBlock(float in[MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],BYTE   out[COMPRESSED_BLOCK_SIZE]);
    void    clampF16Max(float EndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG]);
    void    AverageEndPoint(float EndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], float iEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int max_subsets, int mode);
    void    QuantizeEndPointToF16Prec(float  EndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int iEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int max_subsets, int prec);
    void    SwapIndices(int iEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int iIndices[3][MAX_SUBSET_SIZE], int  entryCount[MAX_SUBSETS], int max_subsets, int mode, int shape_pattern);
    bool   TransformEndPoints(AMD_BC6H_Format &BC6H_data, int iEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG], int oEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG],int max_subsets, int mode);

  private:

#ifdef DEBUG_PATTERNS
    float DoPixelNoise();                        // Used for debugging... adds existing shape patterns with random noise
#endif

    /*
    Reserved Feature MONOSHAPE_PATTERNS
    int BC6HBlockEncoder::FindPattern();
    */

    float    FindBestPattern(AMD_BC6H_Format &BC6H_data,
                             bool    TwoRegionShapes,
                             int    shape_pattern);


    float    EncodePattern(AMD_BC6H_Format &BC6H_data,
                           float  error);

    void    SaveCompressedBlockData(AMD_BC6H_Format &BC6H_data,
                                    int oEndPoints[MAX_SUBSETS][MAX_END_POINTS][MAX_DIMENSION_BIG],
                                    int iIndices[3][MAX_SUBSET_SIZE],
                                    int max_subsets,
                                    int mode);


    // ==========================================================
    // Reserved Feature
    // ==========================================================
    bool    m_useMonoShapePatterns;

    // ==========================================================
    // Bulky temporary data used during compression of a block
    // ==========================================================
    int     m_sortedModes[MAX_PARTITIONS];

    // This stores the min and max for the components of the block, and the ranges
    float  m_blockMin[MAX_DIMENSION_BIG];
    float  m_blockMax[MAX_DIMENSION_BIG];
    float  m_blockRange[MAX_DIMENSION_BIG];
    float  m_blockMaxRange;

    // data setup at initialization time
    float  m_quality;
    DWORD   m_ModeMask;
    bool    m_isSigned;
    float  m_Exposure;
    bool    m_bAverageEndPoint;         // Enables Averaging Endpoints for low bits modes
    float   m_DiffLevel;                // Threashhold for Channel diferance to set Averages value of channels on Endpoints
};

#endif
