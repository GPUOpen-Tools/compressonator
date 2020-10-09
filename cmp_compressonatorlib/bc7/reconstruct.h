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

#ifndef RECONSTRUCT_H_INCLUDED
#define RECONSTRUCT_H_INCLUDED

#include "bc7_definitions.h"

// continious recoustruction unclumped
/*********************************/
double reconstruct(
    double data[MAX_ENTRIES][DIMENSION],  // input
    int numEntries,                            // input
    int index_[MAX_ENTRIES],            // input
    double out[MAX_ENTRIES][DIMENSION], // output
    int ns,                                // input
    double direction [DIMENSION],        // output
    double *step                        // output
);
/*********************************/


double reconstruct_rnd(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],int ns,
    double direction [DIMENSION],double *step
);
int block_mean_rnd(
    double data_[MAX_ENTRIES][DIMENSION],
    int numEntries,
    int partition[MAX_ENTRIES],
    int ns,

    double mean[MAX_SUBSETS][DIMENSION],
    double *clip,
    double mm[DIMENSION],
    double ni[MAX_SUBSETS]  // norm


);
/*****************************************/

double ep_shaker_2(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries,
    int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],
    double direction [DIMENSION],
    double *step,
    int epo_code[2][DIMENSION],
    int bits[3],
    int bcc,
    int nClusters,
    int size
) ;

/*****************************************/

double ep_shaker_2__(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],int ns,
    double direction [DIMENSION],double *step
);

// good but broken (first one) ?????????????????????
double ep_shaker(
    double data[MAX_ENTRIES][DIMENSION],
    int numEntries, int index_[MAX_ENTRIES],
    double out[MAX_ENTRIES][DIMENSION],int ns,
    double direction [DIMENSION],double *step,
    int lock
);


void printStep (void);
void printCnt (void);
void printStepHisto (void);
#endif