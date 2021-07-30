//=====================================================================
// Copyright (c) 2021    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file query_timer.cpp
//
//=====================================================================

#include "query_timer.h"

cmp_timeval  query_timer::m_frequency;
int          query_timer::m_elapsed_count;

void cmp_cputimer::initialize() {
#ifdef _WIN32
    for (int i=0; i<CMP_TIMER_REFMAX; i++) m_start[i].QuadPart = 0;
    QueryPerformanceFrequency(&m_cpufrequency);
    m_fcpufrequency = 1.0f/float(m_cpufrequency.QuadPart);
#endif
}

void cmp_cputimer::Start(CMP_INT ref) {
    if (ref > CMP_TIMER_REFMAX) ref = 0;
#ifdef _WIN32
    m_end[ref].QuadPart = 0;
    QueryPerformanceCounter(&m_start[ref]);
#else
    gettimeofday(&m_start[ref], NULL);
#endif
}

void cmp_cputimer::Stop(CMP_INT ref) {
    if (ref > CMP_TIMER_REFMAX) ref = 0;
#ifdef _WIN32
    if (m_start[ref].QuadPart == 0) m_end[ref].QuadPart = 0;
    else {
        QueryPerformanceCounter(&m_end[ref]);
    }
#else
    gettimeofday(&m_end[ref], NULL);
#endif
}

void cmp_cputimer::Reset(int ref) {
#ifdef _WIN32
    m_start[ref].QuadPart = 0;
    m_end[ref].QuadPart = 0;
#endif
}


CMP_FLOAT cmp_cputimer::GetMS(CMP_INT ref) {
    if (ref > CMP_TIMER_REFMAX) ref = 0;
#ifdef _WIN32
    if (ref > CMP_TIMER_REFMAX) ref = 0;
    if (m_start[ref].QuadPart <= 0) return 0;
    float diff;
    float tickResolution = 1.0f/(float)m_cpufrequency.QuadPart;
    LARGE_INTEGER  ElapsedMicroseconds;
    ElapsedMicroseconds.QuadPart = (m_end[ref].QuadPart - m_start[ref].QuadPart);
    diff = (float)ElapsedMicroseconds.QuadPart * tickResolution;
    return diff*1000.0f;
#else
    return ((m_end[ref].tv_sec - m_start[ref].tv_sec) * 1000 + (m_end[ref].tv_usec - m_start[ref].tv_usec) / 1000);
#endif
}


