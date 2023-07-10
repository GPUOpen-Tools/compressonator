//=====================================================================
// Copyright (c) 2021-2023    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file cpu_timing.cpp
//
//=====================================================================

#include "cpu_timing.h"

#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#else
#include <sys/time.h>
#include <string.h>
#endif

long long query_timer::m_frequency;
int query_timer::m_elapsedCount;

void query_timer::Initialize()
{
#ifdef _WIN32
    LARGE_INTEGER temp;
    QueryPerformanceFrequency(&temp);

    m_frequency = temp.QuadPart;
#endif
}

query_timer::query_timer(char const* label) : m_label(label)
{
#ifdef _WIN32
    LARGE_INTEGER temp;
    QueryPerformanceCounter(&temp);

    m_start = temp.QuadPart;
#else
    timeval temp;
    gettimeofday(&temp, 0);

    m_start = temp.tv_sec*1000 + temp.tv_usec/1000;
#endif
}

query_timer::~query_timer()
{
#ifdef _WIN32
    LARGE_INTEGER temp;
    QueryPerformanceCounter(&temp);

    m_end = temp.QuadPart;
#else
    timeval temp;
    gettimeofday(&temp, 0);

    m_end = temp.tv_sec*1000 + temp.tv_usec/1000;
#endif

    m_elapsedCount = static_cast<int>(m_end - m_start);
}

cpu_timer::cpu_timer() : m_frequency(), m_startTimes(), m_endTimes()
{
#ifdef _WIN32
    for (int i = 0; i < CMP_TIMER_REFMAX; i++)
        m_startTimes[i] = 0;

    LARGE_INTEGER temp;
    QueryPerformanceFrequency(&temp);

    m_frequency = 1.0/(double)temp.QuadPart;
#endif
}

void cpu_timer::Start(unsigned int id)
{
    assert(id < CMP_TIMER_REFMAX);

    if (id >= CMP_TIMER_REFMAX)
        id = 0;
    
    m_endTimes[id] = 0;

#ifdef _WIN32    
    LARGE_INTEGER temp;
    QueryPerformanceCounter(&temp);

    m_startTimes[id] = temp.QuadPart;
#else
    timeval temp;
    gettimeofday(&temp, 0);

    m_startTimes[id] = temp.tv_sec*1000 + temp.tv_usec/1000;
#endif
}

void cpu_timer::Stop(unsigned int id)
{
    assert(id < CMP_TIMER_REFMAX);

    if (id >= CMP_TIMER_REFMAX)
        id = 0;
    
    if (m_startTimes[id] == 0)
    {
        m_endTimes[id] = 0;
        return;
    }

#ifdef _WIN32
    LARGE_INTEGER temp;
    QueryPerformanceCounter(&temp);

    m_endTimes[id] = temp.QuadPart;
#else
    timeval temp;
    gettimeofday(&temp, 0);

    m_endTimes[id] = temp.tv_sec*1000 + temp.tv_usec/1000;
#endif
}

void cpu_timer::Reset(unsigned int id)
{
    assert(id < CMP_TIMER_REFMAX);

#ifdef _WIN32
    m_startTimes[id] = 0;
    m_endTimes[id] = 0;
#endif
}


double cpu_timer::GetTimeMS(unsigned int id)
{
    assert(id < CMP_TIMER_REFMAX);

    if (id >= CMP_TIMER_REFMAX)
        id = 0;
    
    if (m_startTimes[id] <= 0)
        return 0;

#ifdef _WIN32
    long long elapsedTime = m_endTimes[id] - m_startTimes[id];
    double diff = (double)elapsedTime * m_frequency;
    return diff*1000.0;
#else
    return m_endTimes[id] - m_startTimes[id];
#endif
}


stopwatch::stopwatch() : m_startTimes(), m_endTimes(), m_elapsedTimes()
{
#ifdef _WIN32
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    m_msPerTick = 1000.0/frequency.QuadPart;
#else
    m_msPerTick = 1.0/1000.0;
#endif
}

void stopwatch::Start(unsigned int refId)
{
    assert(refId < MAX_ENTRIES);

#ifdef _WIN32
    LARGE_INTEGER tempValue;
    QueryPerformanceCounter(&tempValue);
    m_startTimes[refId] = tempValue.QuadPart;
#else
    timeval tempValue;
    gettimeofday(&tempValue, 0);
    m_startTimes[refId] = tempValue.tv_sec*1000000 + tempValue.tv_usec;
#endif
}

void stopwatch::Stop(unsigned int refId)
{
    assert(refId < MAX_ENTRIES);

#ifdef _WIN32
    LARGE_INTEGER tempValue;
    QueryPerformanceCounter(&tempValue);
    m_endTimes[refId] = tempValue.QuadPart;
#else
    timeval tempValue;
    gettimeofday(&tempValue, 0);
    m_endTimes[refId] = tempValue.tv_sec*1000000 + tempValue.tv_usec;
#endif

    m_elapsedTimes[refId] += m_endTimes[refId] - m_startTimes[refId];
}

void stopwatch::Reset(unsigned int refId)
{
    assert(refId < MAX_ENTRIES);

    m_startTimes[refId] = 0;
    m_endTimes[refId] = 0;
    m_elapsedTimes[refId] = 0;
}

void stopwatch::ResetAll()
{
    memset(m_startTimes, 0, sizeof(m_startTimes));
    memset(m_endTimes, 0, sizeof(m_endTimes));
    memset(m_elapsedTimes, 0, sizeof(m_elapsedTimes));
}

double stopwatch::GetTimeMS(unsigned int refId)
{
    assert(refId < MAX_ENTRIES);

    return (double)(m_elapsedTimes[refId]*m_msPerTick);
}