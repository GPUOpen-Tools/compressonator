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

#include <string>
#include <vector>

#include "Hologram.h"

namespace {

Game *create_game(int argc, char **argv)
{
    std::vector<std::string> args(argv, argv + argc);
    return new Hologram(args);
}

} // namespace

#if defined(VK_USE_PLATFORM_XCB_KHR)

#include "ShellXcb.h"

int main(int argc, char **argv)
{
    Game *game = create_game(argc, argv);
    {
        ShellXcb shell(*game);
        shell.run();
    }
    delete game;

    return 0;
}

#elif defined(VK_USE_PLATFORM_ANDROID_KHR)

#include <android/log.h>
#include "ShellAndroid.h"

void android_main(android_app *app)
{
    Game *game = create_game(0, nullptr);

    try {
        ShellAndroid shell(*app, *game);
        shell.run();
    } catch (const std::runtime_error &e) {
        __android_log_print(ANDROID_LOG_ERROR, game->settings().name.c_str(),
                "%s", e.what());
    }

    delete game;
}

#elif defined(VK_USE_PLATFORM_WIN32_KHR)

#include "ShellWin32.h"

int main(int argc, char **argv)
{
    Game *game = create_game(argc, argv);
    {
        ShellWin32 shell(*game);
        shell.run();
    }
    delete game;

    return 0;
}

#endif // VK_USE_PLATFORM_XCB_KHR
