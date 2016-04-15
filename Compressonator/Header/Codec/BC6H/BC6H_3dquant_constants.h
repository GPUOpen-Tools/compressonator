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

#ifndef _BC6H_3DQUANT_CONSTANTS_H_
#define _BC6H_3DQUANT_CONSTANTS_H_

#define BC6H_MAX_DIMENSION_BIG                    4

#define BC6H_DIMENSION                            4
#define BC6H_MAX_ENTRIES                        64
#define BC6H_MAX_ENTRIES_QUANT_TRACE            16
#define BC6H_MAX_CLUSTERS_BIG                    16
#define BC6H_MAX_CLUSTERS                        8
#define BC6H_MAX_SHAKE_SIZE                        32

typedef enum 
{
    BC6H_CART,
    BC6H_SAME_PAR,
    BC6H_BCC,
    BC6H_SAME_FCC,
    BC6H_FCC,
    BC6H_FCC_SAME_BCC,
} BC6H_qt; 
#endif 