//=====================================================================
// Copyright (c) 2019    Advanced Micro Devices, Inc. All rights reserved.
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

#define ENABLE_QUERY_TIMER

#ifdef ENABLE_QUERY_TIMER
#define QUERY_PERFORMANCE(label,cmips) query_timer __query_timer_##label__(label,cmips)
#else
#define QUERY_PERFORMANCE(label)
#endif

#include "Compressonator.h"
#include "Common.h"

struct query_timer 
{
    static void initialize()
    {
        QueryPerformanceFrequency(&m_frequency);
    }

    query_timer(char const* p_label,CMIPS *CMips):m_label(p_label), m_cmips(CMips)
    {
        QueryPerformanceCounter(&m_start);
    }

    ~query_timer()
    {
        QueryPerformanceCounter(&m_end);
        m_elapsed_time = (m_end.QuadPart - m_start.QuadPart) / static_cast< double >(m_frequency.QuadPart);
        if (m_cmips)
            m_cmips->Print("%s : %3.3f seconds\n", m_label, m_elapsed_time);
        printf(""); // This Bug need fixing!! (distructor needs to use printf(), could be timing issue!

    }

    static LARGE_INTEGER    m_frequency;
    LARGE_INTEGER           m_start;
    LARGE_INTEGER           m_end;
    static double           m_elapsed_time;
    char const*             m_label;
    CMIPS*                  m_cmips = NULL;
};
#endif

#endif // __QUERY_PERFORMANCE_H
