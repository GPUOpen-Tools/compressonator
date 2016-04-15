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

#include <float.h>

#include "BC6H_definitions.h"
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

#include "Compressonator.h"

class BC6HBlockEncoder
{
public:

    BC6HBlockEncoder(CMP_BC6H_BLOCK_PARAMETERS user_options) {
                        m_quality                 = user_options.fQuality;
                        m_useMonoShapePatterns    = user_options.bUsePatternRec;
                        m_isSigned                = user_options.bIsSigned;
                        m_ModeMask                = user_options.dwMask;
                        m_Exposure                = user_options.fExposure;
                    };

    ~BC6HBlockEncoder(){};

    double    optimize_endpts(
                            AMD_BC6H_Format &BC6H_data,
                            float    best_EndPoints[2][2][3], 
                            int      best_Indices[2][16],
                            int      max_subsets, 
                            int      mode, 
                            double   error);


    float   CompressBlock(float in[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG],BYTE   out[BC6H_COMPRESSED_BLOCK_SIZE]);
    void    clampF16Max(float EndPoints[2][2][3]);
    void    QuantizeEndPoint(float    EndPoints[2][2][3], int iEndPoints[2][2][3], int max_subsets, int prec);
    void    SwapIndices(int iEndPoints[2][2][3], int iIndices[3][BC6H_MAX_SUBSET_SIZE], DWORD  entryCount[BC6H_MAX_SUBSETS], int max_subsets, int mode, int shape_pattern);
    void    CompressEndPoints(int iEndPoints[2][2][3], int oEndPoints[2][2][3],int max_subsets, int mode);
    void    BlockSetup(DWORD blockMode);

private:

#ifdef DEBUG_PATTERNS
    float DoPixelNoise();                        // Used for debugging... adds existing shape patterns with random noise
#endif

    /*
    Reserved Feature MONOSHAPE_PATTERNS
    int BC6HBlockEncoder::FindPattern();
    */

    double    FindBestPattern(AMD_BC6H_Format &BC6H_data, 
                              bool    TwoRegionShapes, 
                              int    shape_pattern, 
                              double in[BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG]);


    double    EncodePattern(AMD_BC6H_Format &BC6H_data, 
                            double  error);

    void    SaveCompressedBlockData(AMD_BC6H_Format &BC6H_data, 
                                    int oEndPoints[2][2][3],
                                    int iIndices[3][BC6H_MAX_SUBSET_SIZE], 
                                    int max_subsets, 
                                    int mode);

    // ==========================================================
    // Reserved Feature
    // ==========================================================
    bool    m_useMonoShapePatterns;

    // ==========================================================
    // Bulky temporary data used during compression of a block
    // ==========================================================
    int     m_sortedModes[BC6H_MAX_PARTITIONS];

    // This stores the min and max for the components of the block, and the ranges
    double  m_blockMin[BC6H_MAX_DIMENSION_BIG];
    double  m_blockMax[BC6H_MAX_DIMENSION_BIG];
    double  m_blockRange[BC6H_MAX_DIMENSION_BIG];
    double  m_blockMaxRange;

    // data setup at initialization time
    double  m_quality;
    DWORD   m_ModeMask;
    bool    m_isSigned;
    double  m_Exposure;

    // Data for compressing a particular block mode
    DWORD m_parityBits;
    DWORD m_clusters[2];
    DWORD m_componentBits[BC6H_MAX_DIMENSION_BIG];

};

#endif
