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

#include <cstdint>
#include <cstddef>


// class FrameworkWindows
// {
// public:
//
//     FrameworkWindows(int width, int height, LPCWSTR name) : m_Width(width), m_Height(height), m_Name(name) {};
//     virtual ~FrameworkWindows() {}
//
//     LPCWSTR GetName() {    return m_Name;    }
//     LONG GetWidth() { return m_Width; }
//     LONG GetHeight() { return m_Height; }
//
//     // Pure virtual functions
//
//     virtual void OnCreate(HWND hWnd) = 0;
//     virtual void OnDestroy() = 0;
//     virtual void OnRender() = 0;
//     virtual bool OnEvent(MSG msg) = 0;
//     virtual void OnResize(std::uint32_t Width, std::uint32_t Height) = 0;
//     virtual void SetFullScreen(bool fullscreen) = 0;
//
// protected:
//     // sample name
//     LPCWSTR m_Name;
//
//     // viewport metrics
//     LONG m_Width;
//     LONG m_Height;
// };
//
//
// int RunFramework(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow, FrameworkWindows *pFramework);

// *1*
class FrameworkWindows {
  protected:
    // sample name
    wchar_t m_Name;

    // viewport metrics
    int32_t m_Width;
    int32_t m_Height;

  public:

    FrameworkWindows(int width, int height) : m_Width(width), m_Height(height) {};
    virtual ~FrameworkWindows() {}

    int32_t GetWidth() {
        return m_Width;
    }
    int32_t GetHeight() {
        return m_Height;
    }

    // Pure virtual functions

    virtual int  OnCreate(void* hWnd) = 0;
    virtual void OnDestroy() = 0;
    virtual void OnRender() = 0;
    virtual bool OnEvent(void* msg) = 0;
    virtual void OnResize(uint32_t Width, uint32_t Height) = 0;
    virtual void SetFullScreen(bool fullscreen) = 0;
};
