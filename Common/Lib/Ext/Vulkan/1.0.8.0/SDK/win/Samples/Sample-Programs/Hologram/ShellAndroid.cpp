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

#include <cassert>
#include <dlfcn.h>
#include <time.h>
#include <android/log.h>

#include "Helpers.h"
#include "Game.h"
#include "ShellAndroid.h"

namespace {

// copied from ShellXCB.cpp
class PosixTimer {
public:
    PosixTimer()
    {
        reset();
    }

    void reset()
    {
        clock_gettime(CLOCK_MONOTONIC, &start_);
    }

    double get() const
    {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        constexpr long one_s_in_ns = 1000 * 1000 * 1000;
        constexpr double one_s_in_ns_d = static_cast<double>(one_s_in_ns);

        time_t s = now.tv_sec - start_.tv_sec;
        long ns;
        if (now.tv_nsec > start_.tv_nsec) {
            ns = now.tv_nsec - start_.tv_nsec;
        } else {
            assert(s > 0);
            s--;
            ns = one_s_in_ns - (start_.tv_nsec - now.tv_nsec);
        }

        return static_cast<double>(s) + static_cast<double>(ns) / one_s_in_ns_d;
    }

private:
    struct timespec start_;
};

} // namespace

ShellAndroid::ShellAndroid(android_app &app, Game &game) : Shell(game), app_(app)
{
    instance_extensions_.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);

    app_dummy();
    app_.userData = this;
    app_.onAppCmd = on_app_cmd;
    app_.onInputEvent = on_input_event;

    init_vk();
}

ShellAndroid::~ShellAndroid()
{
    cleanup_vk();
    dlclose(lib_handle_);
}

void ShellAndroid::log(LogPriority priority, const char *msg) const
{
    int prio;

    switch (priority) {
    case LOG_DEBUG:
        prio = ANDROID_LOG_DEBUG;
        break;
    case LOG_INFO:
        prio = ANDROID_LOG_INFO;
        break;
    case LOG_WARN:
        prio = ANDROID_LOG_WARN;
        break;
    case LOG_ERR:
        prio = ANDROID_LOG_ERROR;
        break;
    default:
        prio = ANDROID_LOG_UNKNOWN;
        break;
    }

    __android_log_write(prio, settings_.name.c_str(), msg);
}

PFN_vkGetInstanceProcAddr ShellAndroid::load_vk()
{
    const char filename[] = "libvulkan.so";
    void *handle = nullptr, *symbol = nullptr;

    handle = dlopen(filename, RTLD_LAZY);
    if (handle)
        symbol = dlsym(handle, "vkGetInstanceProcAddr");
    if (!symbol) {
        if (handle)
            dlclose(handle);

        throw std::runtime_error(dlerror());
    }

    lib_handle_ = handle;

    return reinterpret_cast<PFN_vkGetInstanceProcAddr>(symbol);
}

VkSurfaceKHR ShellAndroid::create_surface(VkInstance instance)
{
    VkAndroidSurfaceCreateInfoKHR surface_info = {};
    surface_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surface_info.window = app_.window;

    VkSurfaceKHR surface;
    vk::assert_success(vk::CreateAndroidSurfaceKHR(instance, &surface_info, nullptr, &surface));

    return surface;
}

void ShellAndroid::on_app_cmd(int32_t cmd)
{
    switch (cmd) {
    case APP_CMD_INIT_WINDOW:
        create_context();
        resize_swapchain(0, 0);
        break;
    case APP_CMD_TERM_WINDOW:
        destroy_context();
        break;
    case APP_CMD_WINDOW_RESIZED:
        resize_swapchain(0, 0);
        break;
    case APP_CMD_STOP:
        ANativeActivity_finish(app_.activity);
        break;
    default:
        break;
    }
}

int32_t ShellAndroid::on_input_event(const AInputEvent *event)
{
    if (AInputEvent_getType(event) != AINPUT_EVENT_TYPE_MOTION)
        return false;

    bool handled = false;

    switch (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK) {
    case AMOTION_EVENT_ACTION_UP:
        game_.on_key(Game::KEY_SPACE);
        handled = true;
        break;
    default:
        break;
    }

    return handled;
}

void ShellAndroid::quit()
{
    ANativeActivity_finish(app_.activity);
}

void ShellAndroid::run()
{
    PosixTimer timer;

    double current_time = timer.get();

    while (true) {
        struct android_poll_source *source;
        while (true) {
            int timeout = (settings_.animate && app_.window) ? 0 : -1;
            if (ALooper_pollAll(timeout, nullptr, nullptr,
                    reinterpret_cast<void **>(&source)) < 0)
                break;

            if (source)
                source->process(&app_, source);
        }

        if (app_.destroyRequested)
            break;

        if (!app_.window)
            continue;

        acquire_back_buffer();

        double t = timer.get();
        add_game_time(static_cast<float>(t - current_time));

        present_back_buffer();

        current_time = t;
    }
}
