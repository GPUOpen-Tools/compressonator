//===============================================================================
// Copyright (c) 2023-2024  Advanced Micro Devices, Inc. All rights reserved.
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
//////////////////////////////////////////////////////////////////////////////

#ifndef _MATH_MACROS_H_
#define _MATH_MACROS_H_

// requires C++14 and above
// #pragma warning( push )
// #pragma warning(disable:4018)
//
// template <typename primNumType, typename secNumType>
// inline decltype(auto) cmp_minT( const primNumType& a, const secNumType& b ) {
//     return ( a < b ? a : b );
// }
// template <typename primNumType, typename secNumType>
// inline decltype(auto) cmp_maxT( const primNumType& a, const secNumType& b ) {
//     return ( a > b ? a : b );
// }
//
// #pragma warning(pop)

#ifndef cmp_maxT
#define cmp_maxT(x, y) (((x) > (y)) ? (x) : (y))
#endif
//
#ifndef cmp_minT
#define cmp_minT(x, y) (((x) < (y)) ? (x) : (y))
#endif

#endif  //_MATH_MACROS_H_