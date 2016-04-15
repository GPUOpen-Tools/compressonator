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

void BC6H_init_ramps ();


double BC6H_ep_shaker_2_( 
    double data[BC6H_MAX_ENTRIES][BC6H_DIMENSION], 
    int numEntries, 
    int index_[BC6H_MAX_ENTRIES],
    double out[BC6H_MAX_ENTRIES][BC6H_DIMENSION],
    int epo_code[2][BC6H_DIMENSION],
    int size,
    int Mi_,                // last cluster
    int bits[3],            // including parity
    BC6H_qt type 
    ); 


double BC6H_ep_shaker_2_d( 
    double data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG], 
    int numEntries, 
    int index_[BC6H_MAX_ENTRIES],
    double out[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],
    int epo_code[2][BC6H_MAX_DIMENSION_BIG],
    int size,
    int Mi_,                // last cluster
    int bits,               // including parity
    int dimension,
    double epo[2][BC6H_MAX_DIMENSION_BIG]
    ); 

double BC6H_ep_shaker_( 
    double data[BC6H_MAX_ENTRIES][BC6H_DIMENSION], 
    int numEntries, 
    int index_[BC6H_MAX_ENTRIES],
    double out[BC6H_MAX_ENTRIES][BC6H_DIMENSION],
    int epo_code[2][BC6H_DIMENSION],
    int size,
    int Mi_,                // last cluster
    int bits[3],            // including parity
    BC6H_qt type 
    ); 


double BC6H_ep_shaker_d( 
    double data[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG], 
    int numEntries, 
    int index_[BC6H_MAX_ENTRIES],
    double out[BC6H_MAX_ENTRIES][BC6H_MAX_DIMENSION_BIG],
    int epo_code[2][BC6H_MAX_DIMENSION_BIG],
    int Mi_,                // last cluster
    int bits[4],            // including parity
    BC6H_qt type ,
    int dimension
    ); 

#define BC6H_MAX_PARITY_CASES     8

#ifdef USE_BC6H
extern int BC6H_npv_nd[][2*MAX_DIMENSION_BIG];
extern int BC6H_par_vectors_nd[][2*MAX_DIMENSION_BIG][(1<<(2*MAX_DIMENSION_BIG-1))][2][MAX_DIMENSION_BIG];
#endif
