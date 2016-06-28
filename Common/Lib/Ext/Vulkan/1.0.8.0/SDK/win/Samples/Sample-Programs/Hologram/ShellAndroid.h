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

#ifndef SHELL_ANDROID_H
#define SHELL_ANDROID_H

#include <android_native_app_glue.h>
#include "Shell.h"

class ShellAndroid : public Shell {
public:
    ShellAndroid(android_app &app, Game &game);
    ~ShellAndroid();

    void log(LogPriority priority, const char *msg) const;

    void run();
    void quit();

private:
    PFN_vkGetInstanceProcAddr load_vk();
    bool can_present(VkPhysicalDevice phy, uint32_t queue_family) { return true; }

    VkSurfaceKHR create_surface(VkInstance instance);

    void on_app_cmd(int32_t cmd);
    int32_t on_input_event(const AInputEvent *event);

    static inline void on_app_cmd(android_app *app, int32_t cmd);
    static inline int32_t on_input_event(android_app *app, AInputEvent *event);

    android_app &app_;

    void *lib_handle_;
};

void ShellAndroid::on_app_cmd(android_app *app, int32_t cmd)
{
    auto android = reinterpret_cast<ShellAndroid *>(app->userData);
    android->on_app_cmd(cmd);
}

int32_t ShellAndroid::on_input_event(android_app *app, AInputEvent *event)
{
    auto android = reinterpret_cast<ShellAndroid *>(app->userData);
    return android->on_input_event(event);
}

#endif // SHELL_ANDROID_H
