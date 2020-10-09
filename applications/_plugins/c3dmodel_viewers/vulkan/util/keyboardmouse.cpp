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

#include "keyboardmouse.h"

static bool m_keyDown[256];
static int m_mouseButton;
static POINT m_lastMousePos, m_mouseDelta;
static int m_lastMouseWheelDelta, m_mouseWheelDelta;

void kbmInit() {
    memset(m_keyDown, 0, sizeof(m_keyDown));
    m_mouseDelta.x = m_mouseDelta.y = 0;
    GetCursorPos(&m_lastMousePos);
}

static void inputEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_KILLFOCUS: {
        ZeroMemory(m_keyDown, sizeof(m_keyDown));
        break;
    }

    case WM_KEYDOWN: {
        m_keyDown[wParam & 0xFF] = true;
        break;
    }

    case WM_KEYUP: {
        m_keyDown[wParam & 0xFF] = false;
        break;
    }

    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK: {
        m_mouseButton |= MOUSE_RIGHT_BUTTON;
        SetCapture(hWnd);
        GetCursorPos(&m_lastMousePos);
        break;
    }
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK: {
        m_mouseButton |= MOUSE_MIDDLE_BUTTON;
        SetCapture(hWnd);
        GetCursorPos(&m_lastMousePos);
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK: {
        m_mouseButton |= MOUSE_LEFT_BUTTON;
        SetCapture(hWnd);
        GetCursorPos(&m_lastMousePos);
        break;
    }

    case WM_RBUTTONUP: {
        m_mouseButton &= ~MOUSE_RIGHT_BUTTON;
        if (m_mouseButton == 0) {
            ReleaseCapture();
        }
        break;
    }
    case WM_MBUTTONUP: {
        m_mouseButton &= ~MOUSE_MIDDLE_BUTTON;
        if (m_mouseButton == 0) {
            ReleaseCapture();
        }
        break;
    }
    case WM_LBUTTONUP: {
        m_mouseButton &= ~MOUSE_LEFT_BUTTON;
        if (m_mouseButton == 0) {
            ReleaseCapture();
        }
        break;
    }

    case WM_CAPTURECHANGED: {
        if ((HWND)lParam != hWnd) {
            if (m_mouseButton != 0) {
                m_mouseButton = 0;
                ReleaseCapture();
            }
        }
        break;
    }

    case WM_MOUSEWHEEL: {
        // Update member var state
        m_mouseWheelDelta += (short)HIWORD(wParam);
        break;
    }
    }
}

bool kbmOnEvent(MSG msg) {
    switch (msg.message) {
    case WM_KILLFOCUS:
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_CAPTURECHANGED:
    case WM_MOUSEWHEEL:
        inputEvent(msg.hwnd, msg.message, msg.wParam, msg.lParam);
        return true;
        break;
    }
    return false;
}

void kbmGetMouseDelta(POINT *pDelta, int *pMouseWheelDelta, int *pMouseButton) {
    // Get current position of mouse
    POINT mousePos;
    GetCursorPos(&mousePos);

    // calc delta
    pDelta->x = mousePos.x - m_lastMousePos.x;
    pDelta->y = mousePos.y - m_lastMousePos.y;

    *pMouseButton = m_mouseButton;
    *pMouseWheelDelta = m_mouseWheelDelta - m_lastMouseWheelDelta;

    // Record current position for next time
    m_lastMousePos = mousePos;
    m_lastMouseWheelDelta = m_mouseWheelDelta;
}

bool kbmKeyState(int key) {
    if (key < 0 || key > 255)
        return false;

    return m_keyDown[key];
}

bool *kbmKeyStateArray() {
    return m_keyDown;
}
