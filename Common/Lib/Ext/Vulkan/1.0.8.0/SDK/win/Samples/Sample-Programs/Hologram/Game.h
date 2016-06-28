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

#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

class Shell;

class Game {
public:
    Game(const Game &game) = delete;
    Game &operator=(const Game &game) = delete;
    virtual ~Game() {}

    struct Settings {
        std::string name;
        int initial_width;
        int initial_height;
        int queue_count;
        int back_buffer_count;
        int ticks_per_second;
        bool vsync;
        bool animate;

        bool validate;
        bool validate_verbose;

        bool no_tick;
        bool no_render;
        bool no_present;
    };
    const Settings &settings() const { return settings_; }

    virtual void attach_shell(Shell &shell) { shell_ = &shell; }
    virtual void detach_shell() { shell_ = nullptr; }

    virtual void attach_swapchain() {}
    virtual void detach_swapchain() {}

    enum Key {
        // virtual keys
        KEY_SHUTDOWN,
        // physical keys
        KEY_UNKNOWN,
        KEY_ESC,
        KEY_UP,
        KEY_DOWN,
        KEY_SPACE,
        KEY_F,
    };
    virtual void on_key(Key key) {}
    virtual void on_tick() {}

    virtual void on_frame(float frame_pred) {}

protected:
    Game(const std::string &name, const std::vector<std::string> &args)
        : settings_(), shell_(nullptr)
    {
        settings_.name = name;
        settings_.initial_width = 1280;
        settings_.initial_height = 1024;
        settings_.queue_count = 1;
        settings_.back_buffer_count = 1;
        settings_.ticks_per_second = 30;
        settings_.vsync = true;
        settings_.animate = true;

        settings_.validate = false;
        settings_.validate_verbose = false;

        settings_.no_tick = false;
        settings_.no_render = false;
        settings_.no_present = false;

        parse_args(args);
    }

    Settings settings_;
    Shell *shell_;

private:
    void parse_args(const std::vector<std::string> &args)
    {
        for (auto it = args.begin(); it != args.end(); ++it) {
            if (*it == "-b") {
                settings_.vsync = false;
            } else if (*it == "-w") {
                ++it;
                settings_.initial_width = std::stoi(*it);
            } else if (*it == "-h") {
                ++it;
                settings_.initial_height = std::stoi(*it);
            } else if (*it == "-v") {
                settings_.validate = true;
            } else if (*it == "-vv") {
                settings_.validate = true;
                settings_.validate_verbose = true;
            } else if (*it == "-nt") {
                settings_.no_tick = true;
            } else if (*it == "-nr") {
                settings_.no_render = true;
            } else if (*it == "-np") {
                settings_.no_present = true;
            }
        }
    }
};

#endif // GAME_H
