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


#include "cmp_frameworkwindowsvk.h"

// LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// static FrameworkWindows* dxSample;

static bool bIsFullScreen = false;
static bool bIsMinimized = false;
int32_t lBorderedStyle = 0;
int32_t lBorderlessStyle = 0;

// // this is the main message handler for the program
// LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// {
//     // sort through and find what code to run for the message given
//     switch (message)
//     {
//         case WM_DESTROY:
//         {
//            PostQuitMessage(0);
//            return 0;
//         }
//
//         case WM_KEYDOWN:
//         {
//             if (wParam == VK_ESCAPE)
//             {
//                 PostQuitMessage(0);
//             }
//             break;
//         }
//
//         case WM_SIZE:
//         {
//             if (dxSample)
//             {
//                 RECT clientRect = {};
//                 GetClientRect(hWnd, &clientRect);
//                 dxSample->OnResize(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);
//
//                 bIsMinimized = (IsIconic(hWnd)==TRUE);
//
//                 return 0;
//             }
//             break;
//         }
//
//         case  WM_SYSKEYDOWN:
//         {
//             if ((wParam == VK_RETURN) && (lParam & (1 << 29)))
//             {
//                 bIsFullScreen = !bIsFullScreen;
//                 SetWindowLong(hWnd, GWL_STYLE, bIsFullScreen ? lBorderlessStyle : lBorderedStyle);
//                 dxSample->SetFullScreen(bIsFullScreen);
//             }
//             break;
//         }
//
//     }
//
//     MSG msg;
//     msg.hwnd = hWnd;
//     msg.message = message;
//     msg.wParam = wParam;
//     msg.lParam = lParam;
//     dxSample->OnEvent(msg);
//
//     // Handle any messages the switch statement didn't
//     return DefWindowProc(hWnd, message, wParam, lParam);
// }
