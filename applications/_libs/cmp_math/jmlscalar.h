//=====================================================================
// Copyright 2006-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================
#ifndef _JML_SCALAR_H_
#define _JML_SCALAR_H_

#include <xmmintrin.h>
#include <stdlib.h>

namespace JML
{

inline float RandomFloat()
{
    return (float)rand() / (float)RAND_MAX;
};

#ifdef CMP_USE_RSQ_RSQR
inline float FastSQRT(float v)
{
    __m128 val = _mm_load1_ps(&v);
    val        = _mm_sqrt_ss(val);
    return val.m128_f32[0];
};

inline float FastRSQ(float v)
{
    __m128 val = _mm_load1_ps(&v);
    val        = _mm_rsqrt_ss(val);
    float frsq = val.m128_f32[0];
    return (0.5f * frsq) * (3.0f - (v * frsq) * frsq);
};
#else
inline float FastSQRT(float v)
{
    __m128 val = _mm_set_ss(v);
    val        = _mm_sqrt_ss(val);
    return (val.m128_f32[0]);
};

inline float FastRSQ(float v)
{
    __m128 val  = _mm_set_ss(v);
    __m128 val1 = _mm_set_ss(1.0f);
    val         = _mm_sqrt_ss(val);
    val         = _mm_div_ss(val1, val);
    float frsq  = val.m128_f32[0];
    return (0.5f * frsq) * (3.0f - (v * frsq) * frsq);
};
#endif

};  // namespace JML

#endif