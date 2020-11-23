
//=====================================================================
// Copyright (c) 2016    Advanced Micro Devices, Inc. All rights reserved.
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
/// \file GPU_DecodeBase.cpp
//
//=====================================================================

#ifdef _WIN32
#include "common.h"
#include "compressonator.h"
#include "gpu_decodebase.h"

using namespace GPU_Decode;

LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;
    switch (message) {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



void RenderWindow::EnableWindowContext(HWND hWnd, HDC * hDC, HGLRC * hRC) {
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    // get the device context (DC)
    *hDC = GetDC(hWnd);

    // set the pixel format for the DC
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, iFormat, &pfd);

    // create and enable the render context (RC)
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}

// Disable OpenGL
void RenderWindow::DisableWindowContext(HWND hWnd, HDC hDC, HGLRC hRC) {
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}

HRESULT RenderWindow::InitWindow(int width, int height,WNDPROC callback) {
    if (m_hInstance == 0) {
        m_hInstance = GetModuleHandle(NULL);
        snprintf(m_strWindowName, sizeof(m_strWindowName),"%s_%llx_%d_%d", str_WindowName, (unsigned long long)m_hInstance,width,height);
        snprintf(m_strWindowClassName,sizeof(m_strWindowClassName), "%s_%llx_%d_%d", str_WindowsClassName, (unsigned long long)m_hInstance, width, height);
    }

    if (!FindWindowA(m_strWindowClassName, m_strWindowName)) {
        // Register class
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        if (callback)
            wcex.lpfnWndProc = callback;
        else
            wcex.lpfnWndProc = WndProc2;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = m_hInstance;
        wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = m_strWindowClassName;
        wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

        if (!RegisterClassEx(&wcex)) {
            PrintInfo("Error: RegisterClass failed.\n");
            fprintf(stderr, "[OpenGL Decode] Error: CreateWindow Failed.\n");
            return E_FAIL;
        }
    }

    // Create window
    m_hWnd = CreateWindowEx(
                 WS_EX_APPWINDOW,
                 m_strWindowClassName,
                 m_strWindowName,
                 WS_POPUP,// WS_OVERLAPPEDWINDOW,
                 0, 0,
                 width,
                 height,
                 nullptr,
                 nullptr,
                 m_hInstance,
                 nullptr);

    if (!m_hWnd) {
        PrintInfo("Error: CreateWindow Failed.\n");
        fprintf(stderr, "[OpenGL Decode] Error: CreateWindow Failed.\n");
        return E_FAIL;
    }

    return S_OK;
}

#endif

