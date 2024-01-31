//===============================================================================
// Copyright (c) 2007-2024  Advanced Micro Devices, Inc. All rights reserved.
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

void init_ramps();

double ep_shaker_2_(double     data[MAX_ENTRIES][DIMENSION],
                    int        numEntries,
                    int        index_[MAX_ENTRIES],
                    double     out[MAX_ENTRIES][DIMENSION],
                    int        epo_code[2][DIMENSION],
                    int        size,
                    int        Mi_,      // last cluster
                    int        bits[3],  // including parity
                    CMP_qt_cpu type);

double ep_shaker_2_d(double data[MAX_ENTRIES][MAX_DIMENSION_BIG],
                     int    numEntries,
                     int    index_[MAX_ENTRIES],
                     double out[MAX_ENTRIES][MAX_DIMENSION_BIG],
                     int    epo_code[2][MAX_DIMENSION_BIG],
                     int    size,
                     int    Mi_,   // last cluster
                     int    bits,  // [3],            // including parity
                                   //   CMP_qt type,
                     int    dimension,
                     double epo[2][MAX_DIMENSION_BIG]);

double ep_shaker_(double     data[MAX_ENTRIES][DIMENSION],
                  int        numEntries,
                  int        index_[MAX_ENTRIES],
                  double     out[MAX_ENTRIES][DIMENSION],
                  int        epo_code[2][DIMENSION],
                  int        size,
                  int        Mi_,      // last cluster
                  int        bits[3],  // including parity
                  CMP_qt_cpu type);

double ep_shaker_d(double data[MAX_ENTRIES][MAX_DIMENSION_BIG],
                   int    numEntries,
                   int    index_[MAX_ENTRIES],
                   double out[MAX_ENTRIES][MAX_DIMENSION_BIG],
                   int    epo_code[2][MAX_DIMENSION_BIG],
                   // Note:    int size,
                   int        Mi_,      // last cluster
                   int        bits[3],  // including parity
                   CMP_qt_cpu type,
                   int        dimension);

#define MAX_PARITY_CASES 8

#ifdef USE_BC7
extern int npv_nd[][2 * MAX_DIMENSION_BIG];
extern int par_vectors_nd[][2 * MAX_DIMENSION_BIG][(1 << (2 * MAX_DIMENSION_BIG - 1))][2][MAX_DIMENSION_BIG];
#endif
