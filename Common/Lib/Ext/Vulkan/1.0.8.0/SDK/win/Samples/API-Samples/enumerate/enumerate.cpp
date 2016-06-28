/*
 * Vulkan Samples
 *
 * Copyright (C) 2015-2016 Valve Corporation
 * Copyright (C) 2015-2016 LunarG, Inc.
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

/*
VULKAN_SAMPLE_SHORT_DESCRIPTION
enumerate physical devices
*/

#include <cassert>
#include <cstdlib>
#include <util_init.hpp>

int main(int argc, char *argv[]) {
    struct sample_info info = {};
    init_instance(info, "vulkansamples_enumerate");

    /* VULKAN_KEY_START */

    uint32_t gpu_count = 1;
    VkResult U_ASSERT_ONLY res =
        vkEnumeratePhysicalDevices(info.inst, &gpu_count, NULL);
    assert(gpu_count);
    info.gpus.resize(gpu_count);
    res = vkEnumeratePhysicalDevices(info.inst, &gpu_count, info.gpus.data());
    assert(!res && gpu_count >= 1);

    /* VULKAN_KEY_END */

    vkDestroyInstance(info.inst, NULL);

    return 0;
}
