//=====================================================================
// Copyright 2023 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#include <immintrin.h>

// Horizontal add on a single SSE register. Computes the sum of all values in the input, returning a 128-bit value with the resulting sum in each lane.
static inline __m128 _mm_hadd_ps(__m128 v)
{
    __m128 shuffled = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 1, 0, 3));
    __m128 intermediateSum = _mm_add_ps(v, shuffled);

    __m128 shuffledTwice = _mm_shuffle_ps(intermediateSum, intermediateSum, _MM_SHUFFLE(1, 0, 3, 2));
    __m128 sum = _mm_add_ps(intermediateSum, shuffledTwice);

    return sum;
}

// Horizontal add on a single AVX register. Computes the sum of all values in the input, returning a 256-bit value with the resulting sum in each lane.
static inline __m256 _mm256_hadd_ps(__m256 v)
{
    __m128 v1 = _mm256_extractf128_ps(v, 0);
    __m128 v2 = _mm256_extractf128_ps(v, 1);

    __m128 sumV1 = _mm_hadd_ps(v1);
    __m128 sumV2 = _mm_hadd_ps(v2);

    __m128 sum = _mm_add_ps(sumV1, sumV2);
    
    return _mm256_set_m128(sum, sum);
}

// Horizontal minimum on a single SSE register. Calculates the min value in the input and returns a 128-bit value with the minimum value in each lane.
static inline __m128 _mm_hmin_ps(__m128 v)
{
    __m128 shuffled = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 1, 0, 3));
    __m128 intermediateMin = _mm_min_ps(v, shuffled);

    __m128 shuffledTwice = _mm_shuffle_ps(intermediateMin, intermediateMin, _MM_SHUFFLE(1, 0, 3, 2));
    __m128 min = _mm_min_ps(intermediateMin, shuffledTwice);

    return min;
}

// Horizontal minimum on a single AVX register. Calculates the min value in the input and returns a 256-bit value with the minimum value in each lane.
static inline __m256 _mm256_hmin_ps(__m256 v)
{
    __m128 v1 = _mm256_extractf128_ps(v, 0);
    __m128 v2 = _mm256_extractf128_ps(v, 1);

    __m128 minV1 = _mm_hmin_ps(v1);
    __m128 minV2 = _mm_hmin_ps(v2);

    __m128 min = _mm_min_ps(minV1, minV2);

    return _mm256_set_m128(min, min);
}

// Horizontal maximum on a single SSE register. Calculates the max value in the input and returns a 128-bit value with the maximum value in each lane.
static inline __m128 _mm_hmax_ps(__m128 v)
{
    __m128 shuffled = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 1, 0, 3));
    __m128 intermediateMax = _mm_max_ps(v, shuffled);

    __m128 shuffledTwice = _mm_shuffle_ps(intermediateMax, intermediateMax, _MM_SHUFFLE(1, 0, 3, 2));
    __m128 max = _mm_max_ps(intermediateMax, shuffledTwice);

    return max;
}

// Horizontal maximum on a single AVX register. Calculates the max value in the input and returns a 256-bit value with the maximum value in each lane.
static inline __m256 _mm256_hmax_ps(__m256 v)
{
    __m128 v1 = _mm256_extractf128_ps(v, 0);
    __m128 v2 = _mm256_extractf128_ps(v, 1);

    __m128 maxV1 = _mm_hmax_ps(v1);
    __m128 maxV2 = _mm_hmax_ps(v2);

    __m128 max = _mm_max_ps(maxV1, maxV2);
    
    return _mm256_set_m128(max, max);
}

// Horizontal minimum on a single SSE register. Calculates the min value in the input and returns a 128-bit value with the minimum value in each lane.
static __m128i _mm_hmin_epi8(__m128i v)
{
    static const alignas(16) unsigned char shuffle8Bit[16] = { 0x0F, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E };
    static const alignas(16) unsigned char shuffle16Bit[16] = { 0x0E, 0x0F, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D};

    __m128i shuffle8BitVector = _mm_load_si128((__m128i*)shuffle8Bit);
    __m128i shuffle16BitVector = _mm_load_si128((__m128i*)shuffle16Bit);

    // Shuffle and min 8-bit portion
    __m128i shuffled8Bit = _mm_shuffle_epi8(v, shuffle8BitVector);
    __m128i min8Bit = _mm_min_epi8(v, shuffled8Bit);

    // Shuffle and min 16-bit portion
    __m128i shuffled16Bit = _mm_shuffle_epi8(min8Bit, shuffle16BitVector);
    __m128i min16Bit = _mm_min_epi8(min8Bit, shuffled16Bit);

    // Shuffle and min 32-bit portion
    __m128i shuffled32Bit = _mm_shuffle_epi32(min16Bit, _MM_SHUFFLE(2, 1, 0, 3));
    __m128i min32Bit = _mm_min_epi8(min16Bit, shuffled32Bit);

    // Shuffle and min 64-bit portion
    __m128i shuffled64Bit = _mm_shuffle_epi32(min32Bit, _MM_SHUFFLE(1, 0, 3, 2));
    __m128i min64Bit = _mm_min_epi8(min32Bit, shuffled64Bit);

    return min64Bit;
}