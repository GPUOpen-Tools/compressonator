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

#ifndef _BC6H_3DQUANT_H_INCLUDED

#define _BC6H_3DQUANT_H_INCLUDED

typedef struct {
    double    d;
    int        i;
} BC6H_a;

#define BC6H_EPSILON        0.000001
#define BC6H_MAX_TRY        20
#define BC6H_DBL_MAX_EXP    1024
#undef TRACE

void BC6H_optQuantAnD_d(
    double data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],  // 0-255
    int numEntries, int numClusters, int index[BC6H_MAX_ENTRIES],
    double out[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],
    double direction [BC6H_MAX_DIMENSION_BIG],double *step,
    int dimension
    );


double BC6H_optQuantTrace_d(
    double data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],    // input data 
    int numEntries,                            // number of input points above (not clear about 1, better to avoid)  
    int numClusters,                        // number of clusters on the ramp, max 8 (not clear about 1, better to avoid)  
    int index[BC6H_MAX_ENTRIES],                    // output index, if not all points of the ramp used, 0 may not be assigned
    double out[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],        // resulting quantization
    double direction [BC6H_MAX_DIMENSION_BIG],            // direction vector of the ramp (check normalization) 
    double *step,                            // step size (check normalization) 
    int dimension);

void BC6H_sortProjection(double projection[BC6H_MAX_ENTRIES], int order[BC6H_MAX_ENTRIES], int numEntries);

#endif

 

