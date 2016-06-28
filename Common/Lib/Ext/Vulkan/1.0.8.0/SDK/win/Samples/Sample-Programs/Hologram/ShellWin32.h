/*
 * Copyright (C) 2016 Google, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef SHELL_WIN32_H
#define SHELL_WIN32_H

#include <windows.h>
#include "Shell.h"

class ShellWin32 : public Shell {
public:
    ShellWin32(Game &game);
    ~ShellWin32();

    void run();
    void quit();

private:

    PFN_vkGetInstanceProcAddr load_vk();
    bool can_present(VkPhysicalDevice phy, uint32_t queue_family);

    void create_window();
    VkSurfaceKHR create_surface(VkInstance instance);

    static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        ShellWin32 *shell = reinterpret_cast<ShellWin32 *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        // called from constructor, CreateWindowEx specifically.  But why?
        if (!shell)
            return DefWindowProc(hwnd, uMsg, wParam, lParam);

        return shell->handle_message(uMsg, wParam, lParam);
    }
    LRESULT handle_message(UINT msg, WPARAM wparam, LPARAM lparam);

    HINSTANCE hinstance_;
    HWND hwnd_;

    HMODULE hmodule_;
};

#endif // SHELL_WIN32_H
