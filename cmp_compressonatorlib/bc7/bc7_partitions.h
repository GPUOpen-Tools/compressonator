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

#ifndef _BC7_PARTITIONS_H_
#define _BC7_PARTITIONS_H_

#include "bc7_definitions.h"
#include "debug.h"

extern CMP_DWORD BC7_PARTITIONS_CPU[MAX_SUBSETS][MAX_PARTITIONS][MAX_SUBSET_SIZE];  // cpu
extern CMP_DWORD BC7_FIXUPINDICES[MAX_SUBSETS][MAX_PARTITIONS][3];

extern void Partition(CMP_DWORD partition,
                      double    in[][MAX_DIMENSION_BIG],
                      double    subsets[MAX_SUBSETS][MAX_SUBSET_SIZE][MAX_DIMENSION_BIG],
                      CMP_DWORD count[MAX_SUBSETS],
                      CMP_DWORD blockType,
                      int       dimension);

#endif