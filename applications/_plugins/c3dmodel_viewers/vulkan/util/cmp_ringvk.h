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

#include <windows.h>

#include <cstdint>
#include <cassert>

// This is the typical ring buffer, it is used by resources that will be reused.
// For example the command Lists, the 'dynamic' constant buffers, etc..
//
class Ring {
    std::uint32_t m_Head;
    std::uint32_t m_AllocatedSize;
    std::uint32_t m_TotalSize;
  public:
    void Create(std::uint32_t TotalSize) {
        m_Head = 0;
        m_AllocatedSize = 0;
        m_TotalSize = TotalSize;
    }

    std::uint32_t GetSize() {
        return m_AllocatedSize;
    }
    std::uint32_t GetHead() {
        return m_Head;
    }
    std::uint32_t GetTail() {
        return (m_Head + m_AllocatedSize) % m_TotalSize;
    }

    //helper to avoid allocating chunks that span across the head and the tail of the ring.
    std::uint32_t PaddingToAvoidCrossOver(std::uint32_t size) {
        int tail = GetTail();
        if ((tail + size) > m_TotalSize)
            return (m_TotalSize - tail);
        else
            return 0;
    }

    bool Alloc(std::uint32_t size, std::uint32_t *pOut) {
        if (m_AllocatedSize + size <= m_TotalSize) {
            if (pOut)
                *pOut = GetTail();

            m_AllocatedSize += size;
            return true;
        }

        assert(false);
        return false;
    }

    bool Free(std::uint32_t size) {
        if (m_AllocatedSize > size) {
            m_Head = (m_Head + size) % m_TotalSize;
            m_AllocatedSize -= size;
            return true;
        }
        return false;
    }

};

//
// This class can be thought as ring buffer inside a ring buffer. The outer ring is for ,
// the frames and the internal one is for the resources that were allocated for that frame.
// The size of the outer ring is typically the number of back buffers.
//
// When the outer ring is full, for the next allocation it automatically frees the entries
// of the oldest frame and makes those entries available for the next frame. This happens
// when you call 'OnBeginFrame()'
//
class RingWithTabs {
    //internal ring buffer
    Ring m_mem;

    //this is the external ring buffer (I could have reused the Ring class though)
    std::uint32_t m_frame;
    std::uint32_t m_numberOfBackBuffers;

    std::uint32_t m_memAllocatedInFrame;
    std::uint32_t m_allocatedMemPerBackBuffer[4];
  public:

    void OnCreate(std::uint32_t numberOfBackBuffers, std::uint32_t memTotalSize) {
        m_frame = 0;
        m_numberOfBackBuffers = numberOfBackBuffers;

        //init mem per frame tracker
        m_memAllocatedInFrame = 0;
        for (int i = 0; i < 4; i++)
            m_allocatedMemPerBackBuffer[i] = 0;

        m_mem.Create(memTotalSize);
    }

    void OnDestroy() {
        m_mem.Free(m_mem.GetSize());
    }


    bool Alloc(std::uint32_t size, std::uint32_t *pOut) {
        std::uint32_t padding = m_mem.PaddingToAvoidCrossOver(size);
        if (padding > 0) {
            m_memAllocatedInFrame += padding;

            if (m_mem.Alloc(padding, NULL) == false) { //alloc chunk to avoid crossover, ignore offset
                return false;  //no mem, cannot allocate apdding
            }
        }

        if (m_mem.Alloc(size, pOut) == true) {
            m_memAllocatedInFrame += size;
            return true;
        }
        return false;
    }

    void OnBeginFrame() {
        m_allocatedMemPerBackBuffer[m_frame] = m_memAllocatedInFrame;
        m_memAllocatedInFrame = 0;

        m_frame = (m_frame + 1) % m_numberOfBackBuffers;

        // free all the entries for the oldest buffer in one go
        std::uint32_t memToFree = m_allocatedMemPerBackBuffer[m_frame];
        m_mem.Free(memToFree);
    }
};
