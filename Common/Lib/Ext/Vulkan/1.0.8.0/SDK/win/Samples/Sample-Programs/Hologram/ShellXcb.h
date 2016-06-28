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

#ifndef SHELL_XCB_H
#define SHELL_XCB_H

#include <xcb/xcb.h>
#include "Shell.h"

class ShellXcb : public Shell {
public:
    ShellXcb(Game &game);
    ~ShellXcb();

    void run();
    void quit() { quit_ = true; }

private:
    void init_connection();

    PFN_vkGetInstanceProcAddr load_vk();
    bool can_present(VkPhysicalDevice phy, uint32_t queue_family);

    void create_window();
    VkSurfaceKHR create_surface(VkInstance instance);

    void handle_event(const xcb_generic_event_t *ev);
    void loop_wait();
    void loop_poll();

    xcb_connection_t *c_;
    xcb_screen_t *scr_;
    xcb_window_t win_;

    xcb_atom_t wm_protocols_;
    xcb_atom_t wm_delete_window_;

    void *lib_handle_;

    bool quit_;
};

#endif // SHELL_XCB_H
