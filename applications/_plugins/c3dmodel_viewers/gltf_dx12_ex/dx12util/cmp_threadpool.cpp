// AMD AMDUtils code
//
// Copyright(c) 2017 Advanced Micro Devices, Inc.All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#include "cmp_threadpool.h"

static ThreadPool t;

ThreadPool *GetThreadPool() {
    return &t;
}

//#define ENABLE_MULTI_THREADING

ThreadPool::ThreadPool() {
#ifdef ENABLE_MULTI_THREADING
    Num_Threads = std::thread::hardware_concurrency();
    bExiting = false;
    for (int ii = 0; ii < Num_Threads; ii++) {
        Pool.push_back(std::thread(&ThreadPool::Infinite_loop_function, GetThreadPool()));
    }
#endif
}

ThreadPool::~ThreadPool() {
#ifdef ENABLE_MULTI_THREADING
    bExiting = true;
    condition.notify_all();
    for (int ii = 0; ii < Num_Threads; ii++) {
        Pool[ii].join();
    }
#endif
}

void ThreadPool::Infinite_loop_function() {
#ifdef ENABLE_MULTI_THREADING
    while (true) {
        std::function<void()> Job;
        {
            std::unique_lock<std::mutex> lock(Queue_Mutex);

            condition.wait(lock, [this] {return bExiting || (!Queue.empty()); });
            if (bExiting)
                return;
            Job = Queue.front();
            Queue.pop();
        }
        Job(); // function<void()> type
    }
#endif
};

void ThreadPool::Add_Job(std::function<void()> New_Job) {
#ifdef ENABLE_MULTI_THREADING
    if (bExiting == false) {
        {
            std::unique_lock<std::mutex> lock(Queue_Mutex);
            Queue.push(New_Job);
        }
        condition.notify_one();
    }
#else
    New_Job();
#endif
}
