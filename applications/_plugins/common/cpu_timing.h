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
/// \file cpu_timing.h
//
//=====================================================================


#ifndef __CPU_TIMING_H
#define __CPU_TIMING_H


#define ENABLE_QUERY_TIMER

#ifdef ENABLE_QUERY_TIMER
#define QUERY_PERFORMANCE(label) query_timer __query_timer_##label__(label)
#else
#define QUERY_PERFORMANCE(label)
#endif

// This timer was originally created to try to simplify timing blocks of code.
// The general usage is to first call query_timer::Initialize() at some point in your code before timing starts,
// then whenever you want to time something you use the above macro QUERY_PERFORMANCE("example_label") which will
// create a new temporary timer object that will be destroyed at the end of the block. To retrieve the final time
// for the block, you can get the value at query_timer::m_elapsedCount.
// NOTE: Only one final time can be stored at once, so be sure to save the previous time before calling the macro again.
struct query_timer
{
    static void Initialize();

    query_timer(char const* label);
    ~query_timer();

    static int m_elapsedCount;

private:
    static long long m_frequency;
    long long m_start;
    long long m_end;
    char const* m_label;
};

#define CMP_TIMER_REFMAX 15     // Max number of timer references used for timing a function call

// The most basic timer which provides the ability to time up to CMP_TIMER_REFMAX number of sections with one timer object
class cpu_timer
{
  public:
    cpu_timer();

    void Reset(unsigned int id);
    void Start(unsigned int id);
    void Stop(unsigned int id);
    
    double GetTimeMS(unsigned int id);

  private:
    double m_frequency;
    long long m_startTimes[CMP_TIMER_REFMAX];
    long long m_endTimes[CMP_TIMER_REFMAX];
};

// Similar to "cpu_timer", but with slightly different behaviour. Rather than reset whenever you call the "Start" method, the time will continue
// to accumulate like a stopwatch. Also, unlike the "cpu_timer" class, you don't need to manually initialize the timer object.
struct stopwatch
{
    static const unsigned int MAX_ENTRIES = 64;

    stopwatch();

    void Start(unsigned int refId);
    void Stop(unsigned int refId);
    void Reset(unsigned int refId);

    void ResetAll();

    double GetTimeMS(unsigned int refId);

  private:
    long long m_startTimes[MAX_ENTRIES];
    long long m_endTimes[MAX_ENTRIES];
    long long m_elapsedTimes[MAX_ENTRIES];

    double m_msPerTick;
};

#endif // __CPU_TIMING_H
