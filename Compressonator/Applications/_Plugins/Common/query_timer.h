//=====================================================================
// Copyright (c) 2020    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file query_timer.h
//
//=====================================================================


#ifndef __QUERY_PERFORMANCE_H
#define __QUERY_PERFORMANCE_H

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
typedef LARGE_INTEGER   cmp_timeval;
#else
#include <sys/time.h>
typedef timeval         cmp_timeval;
#endif

#define ENABLE_QUERY_TIMER

#ifdef ENABLE_QUERY_TIMER
#define QUERY_PERFORMANCE(label) query_timer __query_timer_##label__(label)
#else
#define QUERY_PERFORMANCE(label)
#endif

#include "Compressonator.h"
#include "Common.h"

struct query_timer 
{
    static void initialize()
    {
#ifdef _WIN32
        QueryPerformanceFrequency(&m_frequency);
#endif
    }

    query_timer(char const* p_label):m_label(p_label)
    {
#ifdef _WIN32
        QueryPerformanceCounter(&m_start);
#else
        gettimeofday(&m_start, NULL);
#endif
    }

    ~query_timer()
    {
#ifdef _WIN32
        QueryPerformanceCounter(&m_end);
        m_elapsed_count = static_cast< int >(m_end.QuadPart - m_start.QuadPart);
#else
        gettimeofday(&m_end, NULL);
        const int64_t seconds = (m_end.tv_sec - m_start.tv_sec) * 1000;
        const int64_t milliseconds = (m_end.tv_usec - m_start.tv_usec) / 1000;
        m_elapsed_count = seconds + milliseconds;
#endif
    }

    static cmp_timeval    m_frequency;
    static int            m_elapsed_count;
    cmp_timeval           m_start;
    cmp_timeval           m_end;
    char const*           m_label;
};

#define CMP_TIMER_REFMAX 15     // Max number of timer references used for timing a function call

class cmp_cputimer
{
public:
    void initialize();
    void Reset(CMP_INT ref);
    void Start(CMP_INT ref);
    void Stop(CMP_INT ref);
    CMP_FLOAT GetMS(CMP_INT ref);

private:
    float                 m_fcpufrequency;
    cmp_timeval           m_cpufrequency;
    cmp_timeval           m_start[CMP_TIMER_REFMAX];
    cmp_timeval           m_end[CMP_TIMER_REFMAX];
};




#endif // __QUERY_PERFORMANCE_H
