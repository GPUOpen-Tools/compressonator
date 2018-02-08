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
//

#include "BC7_Definitions.h"

//
// Block encoding information for all block types
// {Component Encoding, PartitionBits, RotationBits, indexSwapBits,
//  scalarBits, vectorBits, pBitType, subsetCount, {index0Bits, index1Bits}}
//
CMP_BTI bti[NUM_BLOCK_TYPES] =
{
    {NO_ALPHA,          4, 0, 0, 0, 12, TWO_PBIT, 3, {3, 0}},  // Format Mode 0
    {NO_ALPHA,          6, 0, 0, 0, 18, ONE_PBIT, 2, {3, 0}},  // Format Mode 1	
    {NO_ALPHA,          6, 0, 0, 0, 15, NO_PBIT,  3, {2, 0}},  // Format Mode 2
    {NO_ALPHA,          6, 0, 0, 0, 21, TWO_PBIT, 2, {2, 0}},  // Format Mode 3
    {SEPARATE_ALPHA,    0, 2, 1, 6, 15, NO_PBIT,  1, {2, 3}},  // Format Mode 4
    {SEPARATE_ALPHA,    0, 2, 0, 8, 21, NO_PBIT,  1, {2, 2}},  // Format Mode 5
    {COMBINED_ALPHA,    0, 0, 0, 0, 28, TWO_PBIT, 1, {4, 0}},  // Format Mode 6
    {COMBINED_ALPHA,    6, 0, 0, 0, 20, TWO_PBIT, 2, {2, 0}}   // Format Mode 7
};
