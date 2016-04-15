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

#include "bc6h_partitions.h"

//
// Reversible partitioning operation for BC6H
//

void    BC6H_Partition(
                  int        shape,
                  double    in[][BC6H_MAX_DIMENSION_BIG],
                  double    subsets[BC6H_MAX_SUBSETS][BC6H_MAX_SUBSET_SIZE][BC6H_MAX_DIMENSION_BIG],
                  DWORD        count[BC6H_MAX_SUBSETS],
                  int        ShapeTableToUse,
                  int        dimension)
{
#ifdef USE_DBGTRACE
    DbgTrace(());
#endif
    int   i,j;
    DWORD   *table = NULL;

    // Dont use memset: this is better for now
    for(i=0;i<BC6H_MAX_SUBSETS;i++) count[i] = 0;
    
    switch(ShapeTableToUse)
    {
        case    0:
        case    1:
            table = &(BC7_PARTITIONS[0][0][0]);
            break;
        case    2:
            table = &(BC7_PARTITIONS[1][shape][0]);
            break;
        default:
            break;
    }

    // Nothing to do!!: Must indicate an error to user
    if (table == NULL) return; 

    for(i=0; i<BC6H_MAX_SUBSET_SIZE; i++)
    {
        DWORD   subset = table[i];
        for(j=0; j<dimension; j++)
        {
            subsets[subset][count[subset]][j] = in[i][j];
        }
        if(dimension < BC6H_MAX_DIMENSION_BIG)
        {
            subsets[subset][count[subset]][j] = 0.0;
        }
        count[subset]++;
    }
}


