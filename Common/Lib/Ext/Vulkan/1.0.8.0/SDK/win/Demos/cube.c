/*
 * Copyright (c) 2015-2016 The Khronos Group Inc.
 * Copyright (c) 2015-2016 Valve Corporation
 * Copyright (c) 2015-2016 LunarG, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and/or associated documentation files (the "Materials"), to
 * deal in the Materials without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Materials, and to permit persons to whom the Materials are
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice(s) and this permission notice shall be included in
 * all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR THE
 * USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 * Author: Chia-I Wu <olv@lunarg.com>
 * Author: Courtney Goeltzenleuchter <courtney@LunarG.com>
 * Author: Ian Elliott <ian@LunarG.com>
 * Author: Jon Ashburn <jon@lunarg.com>
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <signal.h>

#ifdef _WIN32
#pragma comment(linker, "/subsystem:windows")
#define APP_NAME_STR_LEN 80
#endif // _WIN32

#include <vulkan/vulkan.h>

#include <vulkan/vk_sdk_platform.h>
#include "linmath.h"

#define DEMO_TEXTURE_COUNT 1
#define APP_SHORT_NAME "cube"
#define APP_LONG_NAME "The Vulkan Cube Demo Program"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

#ifdef _WIN32
#define ERR_EXIT(err_msg, err_class)                                           \
    do {                                                                       \
        MessageBox(NULL, err_msg, err_class, MB_OK);                           \
        exit(1);                                                               \
    } while (0)

#else // _WIN32

#define ERR_EXIT(err_msg, err_class)                                           \
    do {                                                                       \
        printf(err_msg);                                                       \
        fflush(stdout);                                                        \
        exit(1);                                                               \
    } while (0)
#endif // _WIN32

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                               \
    {                                                                          \
        demo->fp##entrypoint =                                                 \
            (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
        if (demo->fp##entrypoint == NULL) {                                    \
            ERR_EXIT("vkGetInstanceProcAddr failed to find vk" #entrypoint,    \
                     "vkGetInstanceProcAddr Failure");                         \
        }                                                                      \
    }

static PFN_vkGetDeviceProcAddr g_gdpa = NULL;

#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                  \
    {                                                                          \
        if (!g_gdpa)                                                           \
            g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(           \
                demo->inst, "vkGetDeviceProcAddr");                            \
        demo->fp##entrypoint =                                                 \
            (PFN_vk##entrypoint)g_gdpa(dev, "vk" #entrypoint);                 \
        if (demo->fp##entrypoint == NULL) {                                    \
            ERR_EXIT("vkGetDeviceProcAddr failed to find vk" #entrypoint,      \
                     "vkGetDeviceProcAddr Failure");                           \
        }                                                                      \
    }

/*
 * structure to track all objects related to a texture.
 */
struct texture_object {
    VkSampler sampler;

    VkImage image;
    VkImageLayout imageLayout;

    VkMemoryAllocateInfo mem_alloc;
    VkDeviceMemory mem;
    VkImageView view;
    int32_t tex_width, tex_height;
};

static char *tex_files[] = {"lunarg.ppm"};

static int validation_error = 0;

struct vkcube_vs_uniform {
    // Must start with MVP
    float mvp[4][4];
    float position[12 * 3][4];
    float color[12 * 3][4];
};

struct vktexcube_vs_uniform {
    // Must start with MVP
    float mvp[4][4];
    float position[12 * 3][4];
    float attr[12 * 3][4];
};

//--------------------------------------------------------------------------------------
// Mesh and VertexFormat Data
//--------------------------------------------------------------------------------------
// clang-format off
struct Vertex
{
    float     posX, posY, posZ, posW;    // Position data
    float     r, g, b, a;                // Color
};

struct VertexPosTex
{
    float     posX, posY, posZ, posW;    // Position data
    float     u, v, s, t;                // Texcoord
};

#define XYZ1(_x_, _y_, _z_)         (_x_), (_y_), (_z_), 1.f
#define UV(_u_, _v_)                (_u_), (_v_), 0.f, 1.f

static const float g_vertex_buffer_data[] = {
    -1.0f,-1.0f,-1.0f,  // -X side
    -1.0f,-1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,

    -1.0f,-1.0f,-1.0f,  // -Z side
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,

    -1.0f,-1.0f,-1.0f,  // -Y side
     1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,

    -1.0f, 1.0f,-1.0f,  // +Y side
    -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,

     1.0f, 1.0f,-1.0f,  // +X side
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,

    -1.0f, 1.0f, 1.0f,  // +Z side
    -1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
};

static const float g_uv_buffer_data[] = {
    0.0f, 0.0f,  // -X side
    1.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,  // -Z side
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,

    1.0f, 1.0f,  // -Y side
    1.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,

    1.0f, 1.0f,  // +Y side
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,

    1.0f, 1.0f,  // +X side
    0.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    0.0f, 1.0f,  // +Z side
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
};
// clang-format on

void dumpMatrix(const char *note, mat4x4 MVP) {
    int i;

    printf("%s: \n", note);
    for (i = 0; i < 4; i++) {
        printf("%f, %f, %f, %f\n", MVP[i][0], MVP[i][1], MVP[i][2], MVP[i][3]);
    }
    printf("\n");
    fflush(stdout);
}

void dumpVec4(const char *note, vec4 vector) {
    printf("%s: \n", note);
    printf("%f, %f, %f, %f\n", vector[0], vector[1], vector[2], vector[3]);
    printf("\n");
    fflush(stdout);
}

VKAPI_ATTR VkBool32 VKAPI_CALL
dbgFunc(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
        uint64_t srcObject, size_t location, int32_t msgCode,
        const char *pLayerPrefix, const char *pMsg, void *pUserData) {
    char *message = (char *)malloc(strlen(pMsg) + 100);

    assert(message);

    if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        sprintf(message, "ERROR: [%s] Code %d : %s", pLayerPrefix, msgCode,
                pMsg);
        validation_error = 1;
    } else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
        // We know that we're submitting queues without fences, ignore this
        // warning
        if (strstr(pMsg,
                   "vkQueueSubmit parameter, VkFence fence, is null pointer")) {
            return false;
        }
        sprintf(message, "WARNING: [%s] Code %d : %s", pLayerPrefix, msgCode,
                pMsg);
        validation_error = 1;
    } else {
        validation_error = 1;
        return false;
    }

#ifdef _WIN32
    MessageBox(NULL, message, "Alert", MB_OK);
#else
    printf("%s\n", message);
    fflush(stdout);
#endif
    free(message);

    /*
     * false indicates that layer should not bail-out of an
     * API call that had validation failures. This may mean that the
     * app dies inside the driver due to invalid parameter(s).
     * That's what would happen without validation layers, so we'll
     * keep that behavior here.
     */
    return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL
BreakCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
              uint64_t srcObject, size_t location, int32_t msgCode,
              const char *pLayerPrefix, const char *pMsg,
              void *pUserData) {
#ifndef WIN32
    raise(SIGTRAP);
#else
    DebugBreak();
#endif

    return false;
}

typedef struct _SwapchainBuffers {
    VkImage image;
    VkCommandBuffer cmd;
    VkImageView view;
} SwapchainBuffers;

struct demo {
#ifdef _WIN32
#define APP_NAME_STR_LEN 80
    HINSTANCE connection;        // hInstance - Windows Instance
    char name[APP_NAME_STR_LEN]; // Name to put on the window/icon
    HWND window;                 // hWnd - window handle
#else                            // _WIN32
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_intern_atom_reply_t *atom_wm_delete_window;
#endif                           // _WIN32
    VkSurfaceKHR surface;
    bool prepared;
    bool use_staging_buffer;

    VkInstance inst;
    VkPhysicalDevice gpu;
    VkDevice device;
    VkQueue queue;
    uint32_t graphics_queue_node_index;
    VkPhysicalDeviceProperties gpu_props;
    VkQueueFamilyProperties *queue_props;
    VkPhysicalDeviceMemoryProperties memory_properties;

    uint32_t enabled_extension_count;
    uint32_t enabled_layer_count;
    char *extension_names[64];
    char *device_validation_layers[64];

    int width, height;
    VkFormat format;
    VkColorSpaceKHR color_space;

    PFN_vkGetPhysicalDeviceSurfaceSupportKHR
        fpGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
        fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR
        fpGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
        fpGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
    PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
    PFN_vkQueuePresentKHR fpQueuePresentKHR;
    uint32_t swapchainImageCount;
    VkSwapchainKHR swapchain;
    SwapchainBuffers *buffers;

    VkCommandPool cmd_pool;

    struct {
        VkFormat format;

        VkImage image;
        VkMemoryAllocateInfo mem_alloc;
        VkDeviceMemory mem;
        VkImageView view;
    } depth;

    struct texture_object textures[DEMO_TEXTURE_COUNT];

    struct {
        VkBuffer buf;
        VkMemoryAllocateInfo mem_alloc;
        VkDeviceMemory mem;
        VkDescriptorBufferInfo buffer_info;
    } uniform_data;

    VkCommandBuffer cmd; // Buffer for initialization commands
    VkPipelineLayout pipeline_layout;
    VkDescriptorSetLayout desc_layout;
    VkPipelineCache pipelineCache;
    VkRenderPass render_pass;
    VkPipeline pipeline;

    mat4x4 projection_matrix;
    mat4x4 view_matrix;
    mat4x4 model_matrix;

    float spin_angle;
    float spin_increment;
    bool pause;

    VkShaderModule vert_shader_module;
    VkShaderModule frag_shader_module;

    VkDescriptorPool desc_pool;
    VkDescriptorSet desc_set;

    VkFramebuffer *framebuffers;

    bool quit;
    int32_t curFrame;
    int32_t frameCount;
    bool validate;
    bool use_break;
    PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
    PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
    VkDebugReportCallbackEXT msg_callback;
    PFN_vkDebugReportMessageEXT DebugReportMessage;

    uint32_t current_buffer;
    uint32_t queue_count;
};

// Forward declaration:
static void demo_resize(struct demo *demo);

static bool memory_type_from_properties(struct demo *demo, uint32_t typeBits,
                                        VkFlags requirements_mask,
                                        uint32_t *typeIndex) {
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < 32; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((demo->memory_properties.memoryTypes[i].propertyFlags &
                 requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

static void demo_flush_init_cmd(struct demo *demo) {
    VkResult U_ASSERT_ONLY err;

    if (demo->cmd == VK_NULL_HANDLE)
        return;

    err = vkEndCommandBuffer(demo->cmd);
    assert(!err);

    const VkCommandBuffer cmd_bufs[] = {demo->cmd};
    VkFence nullFence = VK_NULL_HANDLE;
    VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = NULL,
                                .waitSemaphoreCount = 0,
                                .pWaitSemaphores = NULL,
                                .pWaitDstStageMask = NULL,
                                .commandBufferCount = 1,
                                .pCommandBuffers = cmd_bufs,
                                .signalSemaphoreCount = 0,
                                .pSignalSemaphores = NULL};

    err = vkQueueSubmit(demo->queue, 1, &submit_info, nullFence);
    assert(!err);

    err = vkQueueWaitIdle(demo->queue);
    assert(!err);

    vkFreeCommandBuffers(demo->device, demo->cmd_pool, 1, cmd_bufs);
    demo->cmd = VK_NULL_HANDLE;
}

static void demo_set_image_layout(struct demo *demo, VkImage image,
                                  VkImageAspectFlags aspectMask,
                                  VkImageLayout old_image_layout,
                                  VkImageLayout new_image_layout,
                                  VkAccessFlagBits srcAccessMask) {
    VkResult U_ASSERT_ONLY err;

    if (demo->cmd == VK_NULL_HANDLE) {
        const VkCommandBufferAllocateInfo cmd = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = NULL,
            .commandPool = demo->cmd_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        err = vkAllocateCommandBuffers(demo->device, &cmd, &demo->cmd);
        assert(!err);

        VkCommandBufferInheritanceInfo cmd_buf_hinfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
            .pNext = NULL,
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .framebuffer = VK_NULL_HANDLE,
            .occlusionQueryEnable = VK_FALSE,
            .queryFlags = 0,
            .pipelineStatistics = 0,
        };
        VkCommandBufferBeginInfo cmd_buf_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = NULL,
            .flags = 0,
            .pInheritanceInfo = &cmd_buf_hinfo,
        };
        err = vkBeginCommandBuffer(demo->cmd, &cmd_buf_info);
        assert(!err);
    }

    VkImageMemoryBarrier image_memory_barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = srcAccessMask,
        .dstAccessMask = 0,
        .oldLayout = old_image_layout,
        .newLayout = new_image_layout,
        .image = image,
        .subresourceRange = {aspectMask, 0, 1, 0, 1}};

    if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        /* Make sure anything that was copying from this image has completed */
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    }

    if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        image_memory_barrier.dstAccessMask =
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        /* Make sure any Copy or CPU writes to image are flushed */
        image_memory_barrier.dstAccessMask =
            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    }

    VkImageMemoryBarrier *pmemory_barrier = &image_memory_barrier;

    VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    vkCmdPipelineBarrier(demo->cmd, src_stages, dest_stages, 0, 0, NULL, 0,
                         NULL, 1, pmemory_barrier);
}

static void demo_draw_build_cmd(struct demo *demo, VkCommandBuffer cmd_buf) {
    VkCommandBufferInheritanceInfo cmd_buf_hinfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
        .pNext = NULL,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .framebuffer = VK_NULL_HANDLE,
        .occlusionQueryEnable = VK_FALSE,
        .queryFlags = 0,
        .pipelineStatistics = 0,
    };
    const VkCommandBufferBeginInfo cmd_buf_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = &cmd_buf_hinfo,
    };
    const VkClearValue clear_values[2] = {
            [0] = {.color.float32 = {0.2f, 0.2f, 0.2f, 0.2f}},
            [1] = {.depthStencil = {1.0f, 0}},
    };
    const VkRenderPassBeginInfo rp_begin = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = NULL,
        .renderPass = demo->render_pass,
        .framebuffer = demo->framebuffers[demo->current_buffer],
        .renderArea.offset.x = 0,
        .renderArea.offset.y = 0,
        .renderArea.extent.width = demo->width,
        .renderArea.extent.height = demo->height,
        .clearValueCount = 2,
        .pClearValues = clear_values,
    };
    VkResult U_ASSERT_ONLY err;

    err = vkBeginCommandBuffer(cmd_buf, &cmd_buf_info);
    assert(!err);

    vkCmdBeginRenderPass(cmd_buf, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, demo->pipeline);
    vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            demo->pipeline_layout, 0, 1, &demo->desc_set, 0,
                            NULL);

    VkViewport viewport;
    memset(&viewport, 0, sizeof(viewport));
    viewport.height = (float)demo->height;
    viewport.width = (float)demo->width;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    vkCmdSetViewport(cmd_buf, 0, 1, &viewport);

    VkRect2D scissor;
    memset(&scissor, 0, sizeof(scissor));
    scissor.extent.width = demo->width;
    scissor.extent.height = demo->height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissor(cmd_buf, 0, 1, &scissor);

    vkCmdDraw(cmd_buf, 12 * 3, 1, 0, 0);
    vkCmdEndRenderPass(cmd_buf);

    VkImageMemoryBarrier prePresentBarrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

    prePresentBarrier.image = demo->buffers[demo->current_buffer].image;
    VkImageMemoryBarrier *pmemory_barrier = &prePresentBarrier;
    vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0,
                         NULL, 1, pmemory_barrier);

    err = vkEndCommandBuffer(cmd_buf);
    assert(!err);
}

void demo_update_data_buffer(struct demo *demo) {
    mat4x4 MVP, Model, VP;
    int matrixSize = sizeof(MVP);
    uint8_t *pData;
    VkResult U_ASSERT_ONLY err;

    mat4x4_mul(VP, demo->projection_matrix, demo->view_matrix);

    // Rotate 22.5 degrees around the Y axis
    mat4x4_dup(Model, demo->model_matrix);
    mat4x4_rotate(demo->model_matrix, Model, 0.0f, 1.0f, 0.0f,
                  (float)degreesToRadians(demo->spin_angle));
    mat4x4_mul(MVP, VP, demo->model_matrix);

    err = vkMapMemory(demo->device, demo->uniform_data.mem, 0,
                      demo->uniform_data.mem_alloc.allocationSize, 0,
                      (void **)&pData);
    assert(!err);

    memcpy(pData, (const void *)&MVP[0][0], matrixSize);

    vkUnmapMemory(demo->device, demo->uniform_data.mem);
}

static void demo_draw(struct demo *demo) {
    VkResult U_ASSERT_ONLY err;
    VkSemaphore presentCompleteSemaphore;
    VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };
    VkFence nullFence = VK_NULL_HANDLE;

    err = vkCreateSemaphore(demo->device, &presentCompleteSemaphoreCreateInfo,
                            NULL, &presentCompleteSemaphore);
    assert(!err);

    // Get the index of the next available swapchain image:
    err = demo->fpAcquireNextImageKHR(demo->device, demo->swapchain, UINT64_MAX,
                                      presentCompleteSemaphore,
                                      (VkFence)0, // TODO: Show use of fence
                                      &demo->current_buffer);
    if (err == VK_ERROR_OUT_OF_DATE_KHR) {
        // demo->swapchain is out of date (e.g. the window was resized) and
        // must be recreated:
        demo_resize(demo);
        demo_draw(demo);
        vkDestroySemaphore(demo->device, presentCompleteSemaphore, NULL);
        return;
    } else if (err == VK_SUBOPTIMAL_KHR) {
        // demo->swapchain is not as optimal as it could be, but the platform's
        // presentation engine will still present the image correctly.
    } else {
        assert(!err);
    }

    // Assume the command buffer has been run on current_buffer before so
    // we need to set the image layout back to COLOR_ATTACHMENT_OPTIMAL
    demo_set_image_layout(demo, demo->buffers[demo->current_buffer].image,
                          VK_IMAGE_ASPECT_COLOR_BIT,
                          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                          0);
    demo_flush_init_cmd(demo);

    // Wait for the present complete semaphore to be signaled to ensure
    // that the image won't be rendered to until the presentation
    // engine has fully released ownership to the application, and it is
    // okay to render to the image.

    // FIXME/TODO: DEAL WITH VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    VkPipelineStageFlags pipe_stage_flags =
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = NULL,
                                .waitSemaphoreCount = 1,
                                .pWaitSemaphores = &presentCompleteSemaphore,
                                .pWaitDstStageMask = &pipe_stage_flags,
                                .commandBufferCount = 1,
                                .pCommandBuffers =
                                    &demo->buffers[demo->current_buffer].cmd,
                                .signalSemaphoreCount = 0,
                                .pSignalSemaphores = NULL};

    err = vkQueueSubmit(demo->queue, 1, &submit_info, nullFence);
    assert(!err);

    VkPresentInfoKHR present = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .swapchainCount = 1,
        .pSwapchains = &demo->swapchain,
        .pImageIndices = &demo->current_buffer,
    };

    // TBD/TODO: SHOULD THE "present" PARAMETER BE "const" IN THE HEADER?
    err = demo->fpQueuePresentKHR(demo->queue, &present);
    if (err == VK_ERROR_OUT_OF_DATE_KHR) {
        // demo->swapchain is out of date (e.g. the window was resized) and
        // must be recreated:
        demo_resize(demo);
    } else if (err == VK_SUBOPTIMAL_KHR) {
        // demo->swapchain is not as optimal as it could be, but the platform's
        // presentation engine will still present the image correctly.
    } else {
        assert(!err);
    }

    err = vkQueueWaitIdle(demo->queue);
    assert(err == VK_SUCCESS);

    vkDestroySemaphore(demo->device, presentCompleteSemaphore, NULL);
}

static void demo_prepare_buffers(struct demo *demo) {
    VkResult U_ASSERT_ONLY err;
    VkSwapchainKHR oldSwapchain = demo->swapchain;

    // Check the surface capabilities and formats
    VkSurfaceCapabilitiesKHR surfCapabilities;
    err = demo->fpGetPhysicalDeviceSurfaceCapabilitiesKHR(
        demo->gpu, demo->surface, &surfCapabilities);
    assert(!err);

    uint32_t presentModeCount;
    err = demo->fpGetPhysicalDeviceSurfacePresentModesKHR(
        demo->gpu, demo->surface, &presentModeCount, NULL);
    assert(!err);
    VkPresentModeKHR *presentModes =
        (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
    assert(presentModes);
    err = demo->fpGetPhysicalDeviceSurfacePresentModesKHR(
        demo->gpu, demo->surface, &presentModeCount, presentModes);
    assert(!err);

    VkExtent2D swapchainExtent;
    // width and height are either both -1, or both not -1.
    if (surfCapabilities.currentExtent.width == (uint32_t)-1) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchainExtent.width = demo->width;
        swapchainExtent.height = demo->height;
    } else {
        // If the surface size is defined, the swap chain size must match
        swapchainExtent = surfCapabilities.currentExtent;
        demo->width = surfCapabilities.currentExtent.width;
        demo->height = surfCapabilities.currentExtent.height;
    }

    // If mailbox mode is available, use it, as is the lowest-latency non-
    // tearing mode.  If not, try IMMEDIATE which will usually be available,
    // and is fastest (though it tears).  If not, fall back to FIFO which is
    // always available.
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (size_t i = 0; i < presentModeCount; i++) {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
        if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
            (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
            swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        }
    }

    // Determine the number of VkImage's to use in the swap chain (we desire to
    // own only 1 image at a time, besides the images being displayed and
    // queued for display):
    uint32_t desiredNumberOfSwapchainImages =
        surfCapabilities.minImageCount + 1;
    if ((surfCapabilities.maxImageCount > 0) &&
        (desiredNumberOfSwapchainImages > surfCapabilities.maxImageCount)) {
        // Application must settle for fewer images than desired:
        desiredNumberOfSwapchainImages = surfCapabilities.maxImageCount;
    }

    VkSurfaceTransformFlagsKHR preTransform;
    if (surfCapabilities.supportedTransforms &
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfCapabilities.currentTransform;
    }

    const VkSwapchainCreateInfoKHR swapchain = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .surface = demo->surface,
        .minImageCount = desiredNumberOfSwapchainImages,
        .imageFormat = demo->format,
        .imageColorSpace = demo->color_space,
        .imageExtent =
            {
             .width = swapchainExtent.width, .height = swapchainExtent.height,
            },
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = preTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .imageArrayLayers = 1,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
        .presentMode = swapchainPresentMode,
        .oldSwapchain = oldSwapchain,
        .clipped = true,
    };
    uint32_t i;

    err = demo->fpCreateSwapchainKHR(demo->device, &swapchain, NULL,
                                     &demo->swapchain);
    assert(!err);

    // If we just re-created an existing swapchain, we should destroy the old
    // swapchain at this point.
    // Note: destroying the swapchain also cleans up all its associated
    // presentable images once the platform is done with them.
    if (oldSwapchain != VK_NULL_HANDLE) {
        demo->fpDestroySwapchainKHR(demo->device, oldSwapchain, NULL);
    }

    err = demo->fpGetSwapchainImagesKHR(demo->device, demo->swapchain,
                                        &demo->swapchainImageCount, NULL);
    assert(!err);

    VkImage *swapchainImages =
        (VkImage *)malloc(demo->swapchainImageCount * sizeof(VkImage));
    assert(swapchainImages);
    err = demo->fpGetSwapchainImagesKHR(demo->device, demo->swapchain,
                                        &demo->swapchainImageCount,
                                        swapchainImages);
    assert(!err);

    demo->buffers = (SwapchainBuffers *)malloc(sizeof(SwapchainBuffers) *
                                               demo->swapchainImageCount);
    assert(demo->buffers);

    for (i = 0; i < demo->swapchainImageCount; i++) {
        VkImageViewCreateInfo color_image_view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL,
            .format = demo->format,
            .components =
                {
                 .r = VK_COMPONENT_SWIZZLE_R,
                 .g = VK_COMPONENT_SWIZZLE_G,
                 .b = VK_COMPONENT_SWIZZLE_B,
                 .a = VK_COMPONENT_SWIZZLE_A,
                },
            .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                 .baseMipLevel = 0,
                                 .levelCount = 1,
                                 .baseArrayLayer = 0,
                                 .layerCount = 1},
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .flags = 0,
        };

        demo->buffers[i].image = swapchainImages[i];

        // Render loop will expect image to have been used before and in
        // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        // layout and will change to COLOR_ATTACHMENT_OPTIMAL, so init the image
        // to that state
        demo_set_image_layout(
            demo, demo->buffers[i].image, VK_IMAGE_ASPECT_COLOR_BIT,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            0);

        color_image_view.image = demo->buffers[i].image;

        err = vkCreateImageView(demo->device, &color_image_view, NULL,
                                &demo->buffers[i].view);
        assert(!err);
    }

    if (NULL != presentModes) {
        free(presentModes);
    }
}

static void demo_prepare_depth(struct demo *demo) {
    const VkFormat depth_format = VK_FORMAT_D16_UNORM;
    const VkImageCreateInfo image = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = NULL,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = depth_format,
        .extent = {demo->width, demo->height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .flags = 0,
    };

    VkImageViewCreateInfo view = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .image = VK_NULL_HANDLE,
        .format = depth_format,
        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                             .baseMipLevel = 0,
                             .levelCount = 1,
                             .baseArrayLayer = 0,
                             .layerCount = 1},
        .flags = 0,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
    };

    VkMemoryRequirements mem_reqs;
    VkResult U_ASSERT_ONLY err;
    bool U_ASSERT_ONLY pass;

    demo->depth.format = depth_format;

    /* create image */
    err = vkCreateImage(demo->device, &image, NULL, &demo->depth.image);
    assert(!err);

    vkGetImageMemoryRequirements(demo->device, demo->depth.image, &mem_reqs);
    assert(!err);

    demo->depth.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    demo->depth.mem_alloc.pNext = NULL;
    demo->depth.mem_alloc.allocationSize = mem_reqs.size;
    demo->depth.mem_alloc.memoryTypeIndex = 0;

    pass = memory_type_from_properties(demo, mem_reqs.memoryTypeBits,
                                       0, /* No requirements */
                                       &demo->depth.mem_alloc.memoryTypeIndex);
    assert(pass);

    /* allocate memory */
    err = vkAllocateMemory(demo->device, &demo->depth.mem_alloc, NULL,
                           &demo->depth.mem);
    assert(!err);

    /* bind memory */
    err =
        vkBindImageMemory(demo->device, demo->depth.image, demo->depth.mem, 0);
    assert(!err);

    demo_set_image_layout(demo, demo->depth.image, VK_IMAGE_ASPECT_DEPTH_BIT,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                          0);

    /* create image view */
    view.image = demo->depth.image;
    err = vkCreateImageView(demo->device, &view, NULL, &demo->depth.view);
    assert(!err);
}

/* Load a ppm file into memory */
bool loadTexture(const char *filename, uint8_t *rgba_data,
                 VkSubresourceLayout *layout, int32_t *width, int32_t *height) {
    FILE *fPtr = fopen(filename, "rb");
    char header[256], *cPtr, *tmp;

    if (!fPtr)
        return false;

    cPtr = fgets(header, 256, fPtr); // P6
    if (cPtr == NULL || strncmp(header, "P6\n", 3)) {
        fclose(fPtr);
        return false;
    }

    do {
        cPtr = fgets(header, 256, fPtr);
        if (cPtr == NULL) {
            fclose(fPtr);
            return false;
        }
    } while (!strncmp(header, "#", 1));

    sscanf(header, "%u %u", height, width);
    if (rgba_data == NULL) {
        fclose(fPtr);
        return true;
    }
    tmp = fgets(header, 256, fPtr); // Format
    (void)tmp;
    if (cPtr == NULL || strncmp(header, "255\n", 3)) {
        fclose(fPtr);
        return false;
    }

    for (int y = 0; y < *height; y++) {
        uint8_t *rowPtr = rgba_data;
        for (int x = 0; x < *width; x++) {
            size_t s = fread(rowPtr, 3, 1, fPtr);
            (void)s;
            rowPtr[3] = 255; /* Alpha of 1 */
            rowPtr += 4;
        }
        rgba_data += layout->rowPitch;
    }
    fclose(fPtr);
    return true;
}

static void demo_prepare_texture_image(struct demo *demo, const char *filename,
                                       struct texture_object *tex_obj,
                                       VkImageTiling tiling,
                                       VkImageUsageFlags usage,
                                       VkFlags required_props) {
    const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
    int32_t tex_width;
    int32_t tex_height;
    VkResult U_ASSERT_ONLY err;
    bool U_ASSERT_ONLY pass;

    if (!loadTexture(filename, NULL, NULL, &tex_width, &tex_height)) {
        printf("Failed to load textures\n");
        fflush(stdout);
        exit(1);
    }

    tex_obj->tex_width = tex_width;
    tex_obj->tex_height = tex_height;

    const VkImageCreateInfo image_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = NULL,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = tex_format,
        .extent = {tex_width, tex_height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .flags = 0,
        .initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
    };

    VkMemoryRequirements mem_reqs;

    err =
        vkCreateImage(demo->device, &image_create_info, NULL, &tex_obj->image);
    assert(!err);

    vkGetImageMemoryRequirements(demo->device, tex_obj->image, &mem_reqs);

    tex_obj->mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    tex_obj->mem_alloc.pNext = NULL;
    tex_obj->mem_alloc.allocationSize = mem_reqs.size;
    tex_obj->mem_alloc.memoryTypeIndex = 0;

    pass = memory_type_from_properties(demo, mem_reqs.memoryTypeBits,
                                       required_props,
                                       &tex_obj->mem_alloc.memoryTypeIndex);
    assert(pass);

    /* allocate memory */
    err = vkAllocateMemory(demo->device, &tex_obj->mem_alloc, NULL,
                           &(tex_obj->mem));
    assert(!err);

    /* bind memory */
    err = vkBindImageMemory(demo->device, tex_obj->image, tex_obj->mem, 0);
    assert(!err);

    if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        const VkImageSubresource subres = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .arrayLayer = 0,
        };
        VkSubresourceLayout layout;
        void *data;

        vkGetImageSubresourceLayout(demo->device, tex_obj->image, &subres,
                                    &layout);

        err = vkMapMemory(demo->device, tex_obj->mem, 0,
                          tex_obj->mem_alloc.allocationSize, 0, &data);
        assert(!err);

        if (!loadTexture(filename, data, &layout, &tex_width, &tex_height)) {
            fprintf(stderr, "Error loading texture: %s\n", filename);
        }

        vkUnmapMemory(demo->device, tex_obj->mem);
    }

    tex_obj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    demo_set_image_layout(demo, tex_obj->image, VK_IMAGE_ASPECT_COLOR_BIT,
                          VK_IMAGE_LAYOUT_PREINITIALIZED, tex_obj->imageLayout,
                          VK_ACCESS_HOST_WRITE_BIT);
    /* setting the image layout does not reference the actual memory so no need
     * to add a mem ref */
}

static void demo_destroy_texture_image(struct demo *demo,
                                       struct texture_object *tex_objs) {
    /* clean up staging resources */
    vkFreeMemory(demo->device, tex_objs->mem, NULL);
    vkDestroyImage(demo->device, tex_objs->image, NULL);
}

static void demo_prepare_textures(struct demo *demo) {
    const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormatProperties props;
    uint32_t i;

    vkGetPhysicalDeviceFormatProperties(demo->gpu, tex_format, &props);

    for (i = 0; i < DEMO_TEXTURE_COUNT; i++) {
        VkResult U_ASSERT_ONLY err;

        if ((props.linearTilingFeatures &
             VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) &&
            !demo->use_staging_buffer) {
            /* Device can texture using linear textures */
            demo_prepare_texture_image(demo, tex_files[i], &demo->textures[i],
                                       VK_IMAGE_TILING_LINEAR,
                                       VK_IMAGE_USAGE_SAMPLED_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        } else if (props.optimalTilingFeatures &
                   VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
            /* Must use staging buffer to copy linear texture to optimized */
            struct texture_object staging_texture;

            memset(&staging_texture, 0, sizeof(staging_texture));
            demo_prepare_texture_image(demo, tex_files[i], &staging_texture,
                                       VK_IMAGE_TILING_LINEAR,
                                       VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

            demo_prepare_texture_image(
                demo, tex_files[i], &demo->textures[i], VK_IMAGE_TILING_OPTIMAL,
                (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            demo_set_image_layout(demo, staging_texture.image,
                                  VK_IMAGE_ASPECT_COLOR_BIT,
                                  staging_texture.imageLayout,
                                  VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                  0);

            demo_set_image_layout(demo, demo->textures[i].image,
                                  VK_IMAGE_ASPECT_COLOR_BIT,
                                  demo->textures[i].imageLayout,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  0);

            VkImageCopy copy_region = {
                .srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                .srcOffset = {0, 0, 0},
                .dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                .dstOffset = {0, 0, 0},
                .extent = {staging_texture.tex_width,
                           staging_texture.tex_height, 1},
            };
            vkCmdCopyImage(
                demo->cmd, staging_texture.image,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, demo->textures[i].image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

            demo_set_image_layout(demo, demo->textures[i].image,
                                  VK_IMAGE_ASPECT_COLOR_BIT,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  demo->textures[i].imageLayout,
                                  0);

            demo_flush_init_cmd(demo);

            demo_destroy_texture_image(demo, &staging_texture);
        } else {
            /* Can't support VK_FORMAT_R8G8B8A8_UNORM !? */
            assert(!"No support for R8G8B8A8_UNORM as texture image format");
        }

        const VkSamplerCreateInfo sampler = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = NULL,
            .magFilter = VK_FILTER_NEAREST,
            .minFilter = VK_FILTER_NEAREST,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_FALSE,
            .maxAnisotropy = 1,
            .compareOp = VK_COMPARE_OP_NEVER,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
            .unnormalizedCoordinates = VK_FALSE,
        };

        VkImageViewCreateInfo view = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = NULL,
            .image = VK_NULL_HANDLE,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = tex_format,
            .components =
                {
                 VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                 VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
                },
            .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
            .flags = 0,
        };

        /* create sampler */
        err = vkCreateSampler(demo->device, &sampler, NULL,
                              &demo->textures[i].sampler);
        assert(!err);

        /* create image view */
        view.image = demo->textures[i].image;
        err = vkCreateImageView(demo->device, &view, NULL,
                                &demo->textures[i].view);
        assert(!err);
    }
}

void demo_prepare_cube_data_buffer(struct demo *demo) {
    VkBufferCreateInfo buf_info;
    VkMemoryRequirements mem_reqs;
    uint8_t *pData;
    int i;
    mat4x4 MVP, VP;
    VkResult U_ASSERT_ONLY err;
    bool U_ASSERT_ONLY pass;
    struct vktexcube_vs_uniform data;

    mat4x4_mul(VP, demo->projection_matrix, demo->view_matrix);
    mat4x4_mul(MVP, VP, demo->model_matrix);
    memcpy(data.mvp, MVP, sizeof(MVP));
    //    dumpMatrix("MVP", MVP);

    for (i = 0; i < 12 * 3; i++) {
        data.position[i][0] = g_vertex_buffer_data[i * 3];
        data.position[i][1] = g_vertex_buffer_data[i * 3 + 1];
        data.position[i][2] = g_vertex_buffer_data[i * 3 + 2];
        data.position[i][3] = 1.0f;
        data.attr[i][0] = g_uv_buffer_data[2 * i];
        data.attr[i][1] = g_uv_buffer_data[2 * i + 1];
        data.attr[i][2] = 0;
        data.attr[i][3] = 0;
    }

    memset(&buf_info, 0, sizeof(buf_info));
    buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    buf_info.size = sizeof(data);
    err =
        vkCreateBuffer(demo->device, &buf_info, NULL, &demo->uniform_data.buf);
    assert(!err);

    vkGetBufferMemoryRequirements(demo->device, demo->uniform_data.buf,
                                  &mem_reqs);

    demo->uniform_data.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    demo->uniform_data.mem_alloc.pNext = NULL;
    demo->uniform_data.mem_alloc.allocationSize = mem_reqs.size;
    demo->uniform_data.mem_alloc.memoryTypeIndex = 0;

    pass = memory_type_from_properties(
        demo, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        &demo->uniform_data.mem_alloc.memoryTypeIndex);
    assert(pass);

    err = vkAllocateMemory(demo->device, &demo->uniform_data.mem_alloc, NULL,
                           &(demo->uniform_data.mem));
    assert(!err);

    err = vkMapMemory(demo->device, demo->uniform_data.mem, 0,
                      demo->uniform_data.mem_alloc.allocationSize, 0,
                      (void **)&pData);
    assert(!err);

    memcpy(pData, &data, sizeof data);

    vkUnmapMemory(demo->device, demo->uniform_data.mem);

    err = vkBindBufferMemory(demo->device, demo->uniform_data.buf,
                             demo->uniform_data.mem, 0);
    assert(!err);

    demo->uniform_data.buffer_info.buffer = demo->uniform_data.buf;
    demo->uniform_data.buffer_info.offset = 0;
    demo->uniform_data.buffer_info.range = sizeof(data);
}

static void demo_prepare_descriptor_layout(struct demo *demo) {
    const VkDescriptorSetLayoutBinding layout_bindings[2] = {
            [0] =
                {
                 .binding = 0,
                 .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                 .descriptorCount = 1,
                 .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                 .pImmutableSamplers = NULL,
                },
            [1] =
                {
                 .binding = 1,
                 .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                 .descriptorCount = DEMO_TEXTURE_COUNT,
                 .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                 .pImmutableSamplers = NULL,
                },
    };
    const VkDescriptorSetLayoutCreateInfo descriptor_layout = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .bindingCount = 2,
        .pBindings = layout_bindings,
    };
    VkResult U_ASSERT_ONLY err;

    err = vkCreateDescriptorSetLayout(demo->device, &descriptor_layout, NULL,
                                      &demo->desc_layout);
    assert(!err);

    const VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .setLayoutCount = 1,
        .pSetLayouts = &demo->desc_layout,
    };

    err = vkCreatePipelineLayout(demo->device, &pPipelineLayoutCreateInfo, NULL,
                                 &demo->pipeline_layout);
    assert(!err);
}

static void demo_prepare_render_pass(struct demo *demo) {
    const VkAttachmentDescription attachments[2] = {
            [0] =
                {
                 .format = demo->format,
                 .samples = VK_SAMPLE_COUNT_1_BIT,
                 .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                 .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                 .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                 .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                 .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                 .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                },
            [1] =
                {
                 .format = demo->depth.format,
                 .samples = VK_SAMPLE_COUNT_1_BIT,
                 .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                 .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                 .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                 .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                 .initialLayout =
                     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                 .finalLayout =
                     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                },
    };
    const VkAttachmentReference color_reference = {
        .attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    const VkAttachmentReference depth_reference = {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    const VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .flags = 0,
        .inputAttachmentCount = 0,
        .pInputAttachments = NULL,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_reference,
        .pResolveAttachments = NULL,
        .pDepthStencilAttachment = &depth_reference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = NULL,
    };
    const VkRenderPassCreateInfo rp_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = NULL,
        .attachmentCount = 2,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 0,
        .pDependencies = NULL,
    };
    VkResult U_ASSERT_ONLY err;

    err = vkCreateRenderPass(demo->device, &rp_info, NULL, &demo->render_pass);
    assert(!err);
}

static VkShaderModule
demo_prepare_shader_module(struct demo *demo, const void *code, size_t size) {
    VkShaderModule module;
    VkShaderModuleCreateInfo moduleCreateInfo;
    VkResult U_ASSERT_ONLY err;

    moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleCreateInfo.pNext = NULL;

    moduleCreateInfo.codeSize = size;
    moduleCreateInfo.pCode = code;
    moduleCreateInfo.flags = 0;
    err = vkCreateShaderModule(demo->device, &moduleCreateInfo, NULL, &module);
    assert(!err);

    return module;
}

char *demo_read_spv(const char *filename, size_t *psize) {
    long int size;
    size_t U_ASSERT_ONLY retval;
    void *shader_code;

    FILE *fp = fopen(filename, "rb");
    if (!fp)
        return NULL;

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);

    shader_code = malloc(size);
    retval = fread(shader_code, size, 1, fp);
    assert(retval == 1);

    *psize = size;

    fclose(fp);
    return shader_code;
}

static VkShaderModule demo_prepare_vs(struct demo *demo) {
    void *vertShaderCode;
    size_t size;

    vertShaderCode = demo_read_spv("cube-vert.spv", &size);

    demo->vert_shader_module =
        demo_prepare_shader_module(demo, vertShaderCode, size);

    free(vertShaderCode);

    return demo->vert_shader_module;
}

static VkShaderModule demo_prepare_fs(struct demo *demo) {
    void *fragShaderCode;
    size_t size;

    fragShaderCode = demo_read_spv("cube-frag.spv", &size);

    demo->frag_shader_module =
        demo_prepare_shader_module(demo, fragShaderCode, size);

    free(fragShaderCode);

    return demo->frag_shader_module;
}

static void demo_prepare_pipeline(struct demo *demo) {
    VkGraphicsPipelineCreateInfo pipeline;
    VkPipelineCacheCreateInfo pipelineCache;
    VkPipelineVertexInputStateCreateInfo vi;
    VkPipelineInputAssemblyStateCreateInfo ia;
    VkPipelineRasterizationStateCreateInfo rs;
    VkPipelineColorBlendStateCreateInfo cb;
    VkPipelineDepthStencilStateCreateInfo ds;
    VkPipelineViewportStateCreateInfo vp;
    VkPipelineMultisampleStateCreateInfo ms;
    VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
    VkPipelineDynamicStateCreateInfo dynamicState;
    VkResult U_ASSERT_ONLY err;

    memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
    memset(&dynamicState, 0, sizeof dynamicState);
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStateEnables;

    memset(&pipeline, 0, sizeof(pipeline));
    pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline.layout = demo->pipeline_layout;

    memset(&vi, 0, sizeof(vi));
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    memset(&ia, 0, sizeof(ia));
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    memset(&rs, 0, sizeof(rs));
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_BACK_BIT;
    rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.depthClampEnable = VK_FALSE;
    rs.rasterizerDiscardEnable = VK_FALSE;
    rs.depthBiasEnable = VK_FALSE;

    memset(&cb, 0, sizeof(cb));
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    VkPipelineColorBlendAttachmentState att_state[1];
    memset(att_state, 0, sizeof(att_state));
    att_state[0].colorWriteMask = 0xf;
    att_state[0].blendEnable = VK_FALSE;
    cb.attachmentCount = 1;
    cb.pAttachments = att_state;

    memset(&vp, 0, sizeof(vp));
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount = 1;
    dynamicStateEnables[dynamicState.dynamicStateCount++] =
        VK_DYNAMIC_STATE_VIEWPORT;
    vp.scissorCount = 1;
    dynamicStateEnables[dynamicState.dynamicStateCount++] =
        VK_DYNAMIC_STATE_SCISSOR;

    memset(&ds, 0, sizeof(ds));
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable = VK_TRUE;
    ds.depthWriteEnable = VK_TRUE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    ds.depthBoundsTestEnable = VK_FALSE;
    ds.back.failOp = VK_STENCIL_OP_KEEP;
    ds.back.passOp = VK_STENCIL_OP_KEEP;
    ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
    ds.stencilTestEnable = VK_FALSE;
    ds.front = ds.back;

    memset(&ms, 0, sizeof(ms));
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.pSampleMask = NULL;
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Two stages: vs and fs
    pipeline.stageCount = 2;
    VkPipelineShaderStageCreateInfo shaderStages[2];
    memset(&shaderStages, 0, 2 * sizeof(VkPipelineShaderStageCreateInfo));

    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = demo_prepare_vs(demo);
    shaderStages[0].pName = "main";

    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = demo_prepare_fs(demo);
    shaderStages[1].pName = "main";

    memset(&pipelineCache, 0, sizeof(pipelineCache));
    pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

    err = vkCreatePipelineCache(demo->device, &pipelineCache, NULL,
                                &demo->pipelineCache);
    assert(!err);

    pipeline.pVertexInputState = &vi;
    pipeline.pInputAssemblyState = &ia;
    pipeline.pRasterizationState = &rs;
    pipeline.pColorBlendState = &cb;
    pipeline.pMultisampleState = &ms;
    pipeline.pViewportState = &vp;
    pipeline.pDepthStencilState = &ds;
    pipeline.pStages = shaderStages;
    pipeline.renderPass = demo->render_pass;
    pipeline.pDynamicState = &dynamicState;

    pipeline.renderPass = demo->render_pass;

    err = vkCreateGraphicsPipelines(demo->device, demo->pipelineCache, 1,
                                    &pipeline, NULL, &demo->pipeline);
    assert(!err);

    vkDestroyShaderModule(demo->device, demo->frag_shader_module, NULL);
    vkDestroyShaderModule(demo->device, demo->vert_shader_module, NULL);
}

static void demo_prepare_descriptor_pool(struct demo *demo) {
    const VkDescriptorPoolSize type_counts[2] = {
            [0] =
                {
                 .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                 .descriptorCount = 1,
                },
            [1] =
                {
                 .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                 .descriptorCount = DEMO_TEXTURE_COUNT,
                },
    };
    const VkDescriptorPoolCreateInfo descriptor_pool = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .maxSets = 1,
        .poolSizeCount = 2,
        .pPoolSizes = type_counts,
    };
    VkResult U_ASSERT_ONLY err;

    err = vkCreateDescriptorPool(demo->device, &descriptor_pool, NULL,
                                 &demo->desc_pool);
    assert(!err);
}

static void demo_prepare_descriptor_set(struct demo *demo) {
    VkDescriptorImageInfo tex_descs[DEMO_TEXTURE_COUNT];
    VkWriteDescriptorSet writes[2];
    VkResult U_ASSERT_ONLY err;
    uint32_t i;

    VkDescriptorSetAllocateInfo alloc_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = demo->desc_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &demo->desc_layout};
    err = vkAllocateDescriptorSets(demo->device, &alloc_info, &demo->desc_set);
    assert(!err);

    memset(&tex_descs, 0, sizeof(tex_descs));
    for (i = 0; i < DEMO_TEXTURE_COUNT; i++) {
        tex_descs[i].sampler = demo->textures[i].sampler;
        tex_descs[i].imageView = demo->textures[i].view;
        tex_descs[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    }

    memset(&writes, 0, sizeof(writes));

    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = demo->desc_set;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[0].pBufferInfo = &demo->uniform_data.buffer_info;

    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = demo->desc_set;
    writes[1].dstBinding = 1;
    writes[1].descriptorCount = DEMO_TEXTURE_COUNT;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writes[1].pImageInfo = tex_descs;

    vkUpdateDescriptorSets(demo->device, 2, writes, 0, NULL);
}

static void demo_prepare_framebuffers(struct demo *demo) {
    VkImageView attachments[2];
    attachments[1] = demo->depth.view;

    const VkFramebufferCreateInfo fb_info = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = NULL,
        .renderPass = demo->render_pass,
        .attachmentCount = 2,
        .pAttachments = attachments,
        .width = demo->width,
        .height = demo->height,
        .layers = 1,
    };
    VkResult U_ASSERT_ONLY err;
    uint32_t i;

    demo->framebuffers = (VkFramebuffer *)malloc(demo->swapchainImageCount *
                                                 sizeof(VkFramebuffer));
    assert(demo->framebuffers);

    for (i = 0; i < demo->swapchainImageCount; i++) {
        attachments[0] = demo->buffers[i].view;
        err = vkCreateFramebuffer(demo->device, &fb_info, NULL,
                                  &demo->framebuffers[i]);
        assert(!err);
    }
}

static void demo_prepare(struct demo *demo) {
    VkResult U_ASSERT_ONLY err;

    const VkCommandPoolCreateInfo cmd_pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .queueFamilyIndex = demo->graphics_queue_node_index,
        .flags = 0,
    };
    err = vkCreateCommandPool(demo->device, &cmd_pool_info, NULL,
                              &demo->cmd_pool);
    assert(!err);

    const VkCommandBufferAllocateInfo cmd = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = demo->cmd_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    demo_prepare_buffers(demo);
    demo_prepare_depth(demo);
    demo_prepare_textures(demo);
    demo_prepare_cube_data_buffer(demo);

    demo_prepare_descriptor_layout(demo);
    demo_prepare_render_pass(demo);
    demo_prepare_pipeline(demo);

    for (uint32_t i = 0; i < demo->swapchainImageCount; i++) {
        err =
            vkAllocateCommandBuffers(demo->device, &cmd, &demo->buffers[i].cmd);
        assert(!err);
    }

    demo_prepare_descriptor_pool(demo);
    demo_prepare_descriptor_set(demo);

    demo_prepare_framebuffers(demo);

    for (uint32_t i = 0; i < demo->swapchainImageCount; i++) {
        demo->current_buffer = i;
        demo_draw_build_cmd(demo, demo->buffers[i].cmd);
    }

    /*
     * Prepare functions above may generate pipeline commands
     * that need to be flushed before beginning the render loop.
     */
    demo_flush_init_cmd(demo);

    demo->current_buffer = 0;
    demo->prepared = true;
}

static void demo_cleanup(struct demo *demo) {
    uint32_t i;

    demo->prepared = false;

    for (i = 0; i < demo->swapchainImageCount; i++) {
        vkDestroyFramebuffer(demo->device, demo->framebuffers[i], NULL);
    }
    free(demo->framebuffers);
    vkDestroyDescriptorPool(demo->device, demo->desc_pool, NULL);

    vkDestroyPipeline(demo->device, demo->pipeline, NULL);
    vkDestroyPipelineCache(demo->device, demo->pipelineCache, NULL);
    vkDestroyRenderPass(demo->device, demo->render_pass, NULL);
    vkDestroyPipelineLayout(demo->device, demo->pipeline_layout, NULL);
    vkDestroyDescriptorSetLayout(demo->device, demo->desc_layout, NULL);

    for (i = 0; i < DEMO_TEXTURE_COUNT; i++) {
        vkDestroyImageView(demo->device, demo->textures[i].view, NULL);
        vkDestroyImage(demo->device, demo->textures[i].image, NULL);
        vkFreeMemory(demo->device, demo->textures[i].mem, NULL);
        vkDestroySampler(demo->device, demo->textures[i].sampler, NULL);
    }
    demo->fpDestroySwapchainKHR(demo->device, demo->swapchain, NULL);

    vkDestroyImageView(demo->device, demo->depth.view, NULL);
    vkDestroyImage(demo->device, demo->depth.image, NULL);
    vkFreeMemory(demo->device, demo->depth.mem, NULL);

    vkDestroyBuffer(demo->device, demo->uniform_data.buf, NULL);
    vkFreeMemory(demo->device, demo->uniform_data.mem, NULL);

    for (i = 0; i < demo->swapchainImageCount; i++) {
        vkDestroyImageView(demo->device, demo->buffers[i].view, NULL);
        vkFreeCommandBuffers(demo->device, demo->cmd_pool, 1,
                             &demo->buffers[i].cmd);
    }
    free(demo->buffers);

    free(demo->queue_props);

    vkDestroyCommandPool(demo->device, demo->cmd_pool, NULL);
    vkDestroyDevice(demo->device, NULL);
    if (demo->validate) {
        demo->DestroyDebugReportCallback(demo->inst, demo->msg_callback, NULL);
    }
    vkDestroySurfaceKHR(demo->inst, demo->surface, NULL);
    vkDestroyInstance(demo->inst, NULL);

#ifndef _WIN32
    xcb_destroy_window(demo->connection, demo->window);
    xcb_disconnect(demo->connection);
    free(demo->atom_wm_delete_window);
#endif // _WIN32
}

static void demo_resize(struct demo *demo) {
    uint32_t i;

    // Don't react to resize until after first initialization.
    if (!demo->prepared) {
        return;
    }
    // In order to properly resize the window, we must re-create the swapchain
    // AND redo the command buffers, etc.
    //
    // First, perform part of the demo_cleanup() function:
    demo->prepared = false;

    for (i = 0; i < demo->swapchainImageCount; i++) {
        vkDestroyFramebuffer(demo->device, demo->framebuffers[i], NULL);
    }
    free(demo->framebuffers);
    vkDestroyDescriptorPool(demo->device, demo->desc_pool, NULL);

    vkDestroyPipeline(demo->device, demo->pipeline, NULL);
    vkDestroyPipelineCache(demo->device, demo->pipelineCache, NULL);
    vkDestroyRenderPass(demo->device, demo->render_pass, NULL);
    vkDestroyPipelineLayout(demo->device, demo->pipeline_layout, NULL);
    vkDestroyDescriptorSetLayout(demo->device, demo->desc_layout, NULL);

    for (i = 0; i < DEMO_TEXTURE_COUNT; i++) {
        vkDestroyImageView(demo->device, demo->textures[i].view, NULL);
        vkDestroyImage(demo->device, demo->textures[i].image, NULL);
        vkFreeMemory(demo->device, demo->textures[i].mem, NULL);
        vkDestroySampler(demo->device, demo->textures[i].sampler, NULL);
    }

    vkDestroyImageView(demo->device, demo->depth.view, NULL);
    vkDestroyImage(demo->device, demo->depth.image, NULL);
    vkFreeMemory(demo->device, demo->depth.mem, NULL);

    vkDestroyBuffer(demo->device, demo->uniform_data.buf, NULL);
    vkFreeMemory(demo->device, demo->uniform_data.mem, NULL);

    for (i = 0; i < demo->swapchainImageCount; i++) {
        vkDestroyImageView(demo->device, demo->buffers[i].view, NULL);
        vkFreeCommandBuffers(demo->device, demo->cmd_pool, 1,
                             &demo->buffers[i].cmd);
    }
    vkDestroyCommandPool(demo->device, demo->cmd_pool, NULL);
    free(demo->buffers);

    // Second, re-perform the demo_prepare() function, which will re-create the
    // swapchain:
    demo_prepare(demo);
}

// On MS-Windows, make this a global, so it's available to WndProc()
struct demo demo;

#ifdef _WIN32
static void demo_run(struct demo *demo) {
    if (!demo->prepared)
        return;
    // Wait for work to finish before updating MVP.
    vkDeviceWaitIdle(demo->device);
    demo_update_data_buffer(demo);

    demo_draw(demo);

    // Wait for work to finish before updating MVP.
    vkDeviceWaitIdle(demo->device);

    demo->curFrame++;
    if (demo->frameCount != INT_MAX && demo->curFrame == demo->frameCount) {
        PostQuitMessage(validation_error);
    }
}

// MS-Windows event handling function:
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
        PostQuitMessage(validation_error);
        break;
    case WM_PAINT:
        demo_run(&demo);
        break;
    case WM_SIZE:
        // Resize the application to the new window size, except when
        // it was minimized. Vulkan doesn't support images or swapchains
        // with width=0 and height=0.
        if (wParam != SIZE_MINIMIZED) {
            demo.width = lParam & 0xffff;
            demo.height = lParam & 0xffff0000 >> 16;
            demo_resize(&demo);
        }
        break;
    default:
        break;
    }
    return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}

static void demo_create_window(struct demo *demo) {
    WNDCLASSEX win_class;

    // Initialize the window class structure:
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WndProc;
    win_class.cbClsExtra = 0;
    win_class.cbWndExtra = 0;
    win_class.hInstance = demo->connection; // hInstance
    win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    win_class.lpszMenuName = NULL;
    win_class.lpszClassName = demo->name;
    win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
    // Register window class:
    if (!RegisterClassEx(&win_class)) {
        // It didn't work, so try to give a useful error:
        printf("Unexpected error trying to start the application!\n");
        fflush(stdout);
        exit(1);
    }
    // Create window with the registered class:
    RECT wr = {0, 0, demo->width, demo->height};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    demo->window = CreateWindowEx(0,
                                  demo->name,           // class name
                                  demo->name,           // app name
                                  WS_OVERLAPPEDWINDOW | // window style
                                      WS_VISIBLE | WS_SYSMENU,
                                  100, 100,           // x/y coords
                                  wr.right - wr.left, // width
                                  wr.bottom - wr.top, // height
                                  NULL,               // handle to parent
                                  NULL,               // handle to menu
                                  demo->connection,   // hInstance
                                  NULL);              // no extra parameters
    if (!demo->window) {
        // It didn't work, so try to give a useful error:
        printf("Cannot create a window in which to draw!\n");
        fflush(stdout);
        exit(1);
    }
}
#else  // _WIN32
static void demo_handle_event(struct demo *demo,
                              const xcb_generic_event_t *event) {
    uint8_t event_code = event->response_type & 0x7f;
    switch (event_code) {
    case XCB_EXPOSE:
        // TODO: Resize window
        break;
    case XCB_CLIENT_MESSAGE:
        if ((*(xcb_client_message_event_t *)event).data.data32[0] ==
            (*demo->atom_wm_delete_window).atom) {
            demo->quit = true;
        }
        break;
    case XCB_KEY_RELEASE: {
        const xcb_key_release_event_t *key =
            (const xcb_key_release_event_t *)event;

        switch (key->detail) {
        case 0x9: // Escape
            demo->quit = true;
            break;
        case 0x71: // left arrow key
            demo->spin_angle += demo->spin_increment;
            break;
        case 0x72: // right arrow key
            demo->spin_angle -= demo->spin_increment;
            break;
        case 0x41:
            demo->pause = !demo->pause;
            break;
        }
    } break;
    case XCB_CONFIGURE_NOTIFY: {
        const xcb_configure_notify_event_t *cfg =
            (const xcb_configure_notify_event_t *)event;
        if ((demo->width != cfg->width) || (demo->height != cfg->height)) {
            demo->width = cfg->width;
            demo->height = cfg->height;
            demo_resize(demo);
        }
    } break;
    default:
        break;
    }
}

static void demo_run(struct demo *demo) {
    xcb_flush(demo->connection);

    while (!demo->quit) {
        xcb_generic_event_t *event;

        if (demo->pause) {
            event = xcb_wait_for_event(demo->connection);
        } else {
            event = xcb_poll_for_event(demo->connection);
        }
        if (event) {
            demo_handle_event(demo, event);
            free(event);
        }

        // Wait for work to finish before updating MVP.
        vkDeviceWaitIdle(demo->device);
        demo_update_data_buffer(demo);

        demo_draw(demo);

        // Wait for work to finish before updating MVP.
        vkDeviceWaitIdle(demo->device);
        demo->curFrame++;
        if (demo->frameCount != INT32_MAX && demo->curFrame == demo->frameCount)
            demo->quit = true;
    }
}

static void demo_create_window(struct demo *demo) {
    uint32_t value_mask, value_list[32];

    demo->window = xcb_generate_id(demo->connection);

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = demo->screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE |
                    XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    xcb_create_window(demo->connection, XCB_COPY_FROM_PARENT, demo->window,
                      demo->screen->root, 0, 0, demo->width, demo->height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, demo->screen->root_visual,
                      value_mask, value_list);

    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_cookie_t cookie =
        xcb_intern_atom(demo->connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *reply =
        xcb_intern_atom_reply(demo->connection, cookie, 0);

    xcb_intern_atom_cookie_t cookie2 =
        xcb_intern_atom(demo->connection, 0, 16, "WM_DELETE_WINDOW");
    demo->atom_wm_delete_window =
        xcb_intern_atom_reply(demo->connection, cookie2, 0);

    xcb_change_property(demo->connection, XCB_PROP_MODE_REPLACE, demo->window,
                        (*reply).atom, 4, 32, 1,
                        &(*demo->atom_wm_delete_window).atom);
    free(reply);

    xcb_map_window(demo->connection, demo->window);

    // Force the x/y coordinates to 100,100 results are identical in consecutive
    // runs
    const uint32_t coords[] = {100, 100};
    xcb_configure_window(demo->connection, demo->window,
                         XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, coords);
}
#endif // _WIN32

/*
 * Return 1 (true) if all layer names specified in check_names
 * can be found in given layer properties.
 */
static VkBool32 demo_check_layers(uint32_t check_count, char **check_names,
                                  uint32_t layer_count,
                                  VkLayerProperties *layers) {
    for (uint32_t i = 0; i < check_count; i++) {
        VkBool32 found = 0;
        for (uint32_t j = 0; j < layer_count; j++) {
            if (!strcmp(check_names[i], layers[j].layerName)) {
                found = 1;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "Cannot find layer: %s\n", check_names[i]);
            return 0;
        }
    }
    return 1;
}

static void demo_init_vk(struct demo *demo) {
    VkResult err;
    uint32_t instance_extension_count = 0;
    uint32_t instance_layer_count = 0;
    uint32_t device_validation_layer_count = 0;
    char **instance_validation_layers = NULL;
    demo->enabled_extension_count = 0;
    demo->enabled_layer_count = 0;

    char *instance_validation_layers_alt1[] = {
        "VK_LAYER_LUNARG_standard_validation"
    };

    char *instance_validation_layers_alt2[] = {
        "VK_LAYER_GOOGLE_threading",     "VK_LAYER_LUNARG_parameter_validation",
        "VK_LAYER_LUNARG_device_limits", "VK_LAYER_LUNARG_object_tracker",
        "VK_LAYER_LUNARG_image",         "VK_LAYER_LUNARG_core_validation",
        "VK_LAYER_LUNARG_swapchain",     "VK_LAYER_GOOGLE_unique_objects"
    };

    /* Look for validation layers */
    VkBool32 validation_found = 0;
    if (demo->validate) {

        err = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
        assert(!err);

        instance_validation_layers = instance_validation_layers_alt1;
        if (instance_layer_count > 0) {
            VkLayerProperties *instance_layers =
                    malloc(sizeof (VkLayerProperties) * instance_layer_count);
            err = vkEnumerateInstanceLayerProperties(&instance_layer_count,
                    instance_layers);
            assert(!err);


            validation_found = demo_check_layers(
                    ARRAY_SIZE(instance_validation_layers_alt1),
                    instance_validation_layers, instance_layer_count,
                    instance_layers);
            if (validation_found) {
                demo->enabled_layer_count = ARRAY_SIZE(instance_validation_layers_alt1);
                demo->device_validation_layers[0] = "VK_LAYER_LUNARG_standard_validation";
                device_validation_layer_count = 1;
            } else {
                // use alternative set of validation layers
                instance_validation_layers = instance_validation_layers_alt2;
                demo->enabled_layer_count = ARRAY_SIZE(instance_validation_layers_alt2);
                validation_found = demo_check_layers(
                    ARRAY_SIZE(instance_validation_layers_alt2),
                    instance_validation_layers, instance_layer_count,
                    instance_layers);
                device_validation_layer_count =
                        ARRAY_SIZE(instance_validation_layers_alt2);
                for (uint32_t i = 0; i < device_validation_layer_count; i++) {
                    demo->device_validation_layers[i] =
                            instance_validation_layers[i];
                }
            }
            free(instance_layers);
        }

        if (!validation_found) {
            ERR_EXIT("vkEnumerateInstanceLayerProperties failed to find"
                    "required validation layer.\n\n"
                    "Please look at the Getting Started guide for additional "
                    "information.\n",
                    "vkCreateInstance Failure");
        }
    }

    /* Look for instance extensions */
    VkBool32 surfaceExtFound = 0;
    VkBool32 platformSurfaceExtFound = 0;
    memset(demo->extension_names, 0, sizeof(demo->extension_names));

    err = vkEnumerateInstanceExtensionProperties(
        NULL, &instance_extension_count, NULL);
    assert(!err);

    if (instance_extension_count > 0) {
        VkExtensionProperties *instance_extensions =
            malloc(sizeof(VkExtensionProperties) * instance_extension_count);
        err = vkEnumerateInstanceExtensionProperties(
            NULL, &instance_extension_count, instance_extensions);
        assert(!err);
        for (uint32_t i = 0; i < instance_extension_count; i++) {
            if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME,
                        instance_extensions[i].extensionName)) {
                surfaceExtFound = 1;
                demo->extension_names[demo->enabled_extension_count++] =
                    VK_KHR_SURFACE_EXTENSION_NAME;
            }
#ifdef _WIN32
            if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
                        instance_extensions[i].extensionName)) {
                platformSurfaceExtFound = 1;
                demo->extension_names[demo->enabled_extension_count++] =
                    VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
            }
#else  // _WIN32
            if (!strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME,
                        instance_extensions[i].extensionName)) {
                platformSurfaceExtFound = 1;
                demo->extension_names[demo->enabled_extension_count++] =
                    VK_KHR_XCB_SURFACE_EXTENSION_NAME;
            }
#endif // _WIN32
            if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
                        instance_extensions[i].extensionName)) {
                if (demo->validate) {
                    demo->extension_names[demo->enabled_extension_count++] =
                        VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
                }
            }
            assert(demo->enabled_extension_count < 64);
        }

        free(instance_extensions);
    }

    if (!surfaceExtFound) {
        ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
                 "the " VK_KHR_SURFACE_EXTENSION_NAME
                 " extension.\n\nDo you have a compatible "
                 "Vulkan installable client driver (ICD) installed?\nPlease "
                 "look at the Getting Started guide for additional "
                 "information.\n",
                 "vkCreateInstance Failure");
    }
    if (!platformSurfaceExtFound) {
#ifdef _WIN32
        ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
                 "the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
                 " extension.\n\nDo you have a compatible "
                 "Vulkan installable client driver (ICD) installed?\nPlease "
                 "look at the Getting Started guide for additional "
                 "information.\n",
                 "vkCreateInstance Failure");
#else  // _WIN32
        ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
                 "the " VK_KHR_XCB_SURFACE_EXTENSION_NAME
                 " extension.\n\nDo you have a compatible "
                 "Vulkan installable client driver (ICD) installed?\nPlease "
                 "look at the Getting Started guide for additional "
                 "information.\n",
                 "vkCreateInstance Failure");
#endif // _WIN32
    }
    const VkApplicationInfo app = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = APP_SHORT_NAME,
        .applicationVersion = 0,
        .pEngineName = APP_SHORT_NAME,
        .engineVersion = 0,
        .apiVersion = VK_API_VERSION_1_0,
    };
    VkInstanceCreateInfo inst_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .pApplicationInfo = &app,
        .enabledLayerCount = demo->enabled_layer_count,
        .ppEnabledLayerNames = (const char *const *)instance_validation_layers,
        .enabledExtensionCount = demo->enabled_extension_count,
        .ppEnabledExtensionNames = (const char *const *)demo->extension_names,
    };

    /*
     * This is info for a temp callback to use during CreateInstance.
     * After the instance is created, we use the instance-based
     * function to register the final callback.
     */
    VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
    if (demo->validate) {
        dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        dbgCreateInfo.pNext = NULL;
        dbgCreateInfo.pfnCallback = demo->use_break ? BreakCallback : dbgFunc;
        dbgCreateInfo.pUserData = NULL;
        dbgCreateInfo.flags =
            VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        inst_info.pNext = &dbgCreateInfo;
    }

    uint32_t gpu_count;

    err = vkCreateInstance(&inst_info, NULL, &demo->inst);
    if (err == VK_ERROR_INCOMPATIBLE_DRIVER) {
        ERR_EXIT("Cannot find a compatible Vulkan installable client driver "
                 "(ICD).\n\nPlease look at the Getting Started guide for "
                 "additional information.\n",
                 "vkCreateInstance Failure");
    } else if (err == VK_ERROR_EXTENSION_NOT_PRESENT) {
        ERR_EXIT("Cannot find a specified extension library"
                 ".\nMake sure your layers path is set appropriately.\n",
                 "vkCreateInstance Failure");
    } else if (err) {
        ERR_EXIT("vkCreateInstance failed.\n\nDo you have a compatible Vulkan "
                 "installable client driver (ICD) installed?\nPlease look at "
                 "the Getting Started guide for additional information.\n",
                 "vkCreateInstance Failure");
    }

    /* Make initial call to query gpu_count, then second call for gpu info*/
    err = vkEnumeratePhysicalDevices(demo->inst, &gpu_count, NULL);
    assert(!err && gpu_count > 0);

    if (gpu_count > 0) {
        VkPhysicalDevice *physical_devices = malloc(sizeof(VkPhysicalDevice) * gpu_count);
        err = vkEnumeratePhysicalDevices(demo->inst, &gpu_count, physical_devices);
        assert(!err);
        /* For cube demo we just grab the first physical device */
        demo->gpu = physical_devices[0];
        free(physical_devices);
    } else {
        ERR_EXIT("vkEnumeratePhysicalDevices reported zero accessible devices.\n\n"
                 "Do you have a compatible Vulkan installable client driver (ICD) "
                 "installed?\nPlease look at the Getting Started guide for "
                 "additional information.\n",
                 "vkEnumeratePhysicalDevices Failure");
    }

    /* Look for validation layers */
    validation_found = 0;
    demo->enabled_layer_count = 0;
    uint32_t device_layer_count = 0;
    err =
        vkEnumerateDeviceLayerProperties(demo->gpu, &device_layer_count, NULL);
    assert(!err);

    if (device_layer_count > 0) {
        VkLayerProperties *device_layers =
            malloc(sizeof(VkLayerProperties) * device_layer_count);
        err = vkEnumerateDeviceLayerProperties(demo->gpu, &device_layer_count,
                                               device_layers);
        assert(!err);

        if (demo->validate) {
            validation_found = demo_check_layers(device_validation_layer_count,
                                                 demo->device_validation_layers,
                                                 device_layer_count,
                                                 device_layers);
            demo->enabled_layer_count = device_validation_layer_count;
        }

        free(device_layers);
    }

    if (demo->validate && !validation_found) {
        ERR_EXIT("vkEnumerateDeviceLayerProperties failed to find"
                 "a required validation layer.\n\n"
                 "Please look at the Getting Started guide for additional "
                 "information.\n",
                 "vkCreateDevice Failure");
    }

    /* Look for device extensions */
    uint32_t device_extension_count = 0;
    VkBool32 swapchainExtFound = 0;
    demo->enabled_extension_count = 0;
    memset(demo->extension_names, 0, sizeof(demo->extension_names));

    err = vkEnumerateDeviceExtensionProperties(demo->gpu, NULL,
                                               &device_extension_count, NULL);
    assert(!err);

    if (device_extension_count > 0) {
        VkExtensionProperties *device_extensions =
            malloc(sizeof(VkExtensionProperties) * device_extension_count);
        err = vkEnumerateDeviceExtensionProperties(
            demo->gpu, NULL, &device_extension_count, device_extensions);
        assert(!err);

        for (uint32_t i = 0; i < device_extension_count; i++) {
            if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                        device_extensions[i].extensionName)) {
                swapchainExtFound = 1;
                demo->extension_names[demo->enabled_extension_count++] =
                    VK_KHR_SWAPCHAIN_EXTENSION_NAME;
            }
            assert(demo->enabled_extension_count < 64);
        }

        free(device_extensions);
    }

    if (!swapchainExtFound) {
        ERR_EXIT("vkEnumerateDeviceExtensionProperties failed to find "
                 "the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
                 " extension.\n\nDo you have a compatible "
                 "Vulkan installable client driver (ICD) installed?\nPlease "
                 "look at the Getting Started guide for additional "
                 "information.\n",
                 "vkCreateInstance Failure");
    }

    if (demo->validate) {
        demo->CreateDebugReportCallback =
            (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
                demo->inst, "vkCreateDebugReportCallbackEXT");
        demo->DestroyDebugReportCallback =
            (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
                demo->inst, "vkDestroyDebugReportCallbackEXT");
        if (!demo->CreateDebugReportCallback) {
            ERR_EXIT(
                "GetProcAddr: Unable to find vkCreateDebugReportCallbackEXT\n",
                "vkGetProcAddr Failure");
        }
        if (!demo->DestroyDebugReportCallback) {
            ERR_EXIT(
                "GetProcAddr: Unable to find vkDestroyDebugReportCallbackEXT\n",
                "vkGetProcAddr Failure");
        }
        demo->DebugReportMessage =
            (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(
                demo->inst, "vkDebugReportMessageEXT");
        if (!demo->DebugReportMessage) {
            ERR_EXIT("GetProcAddr: Unable to find vkDebugReportMessageEXT\n",
                     "vkGetProcAddr Failure");
        }

        VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
        PFN_vkDebugReportCallbackEXT callback;
        callback = demo->use_break ? BreakCallback : dbgFunc;
        dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        dbgCreateInfo.pNext = NULL;
        dbgCreateInfo.pfnCallback = callback;
        dbgCreateInfo.pUserData = NULL;
        dbgCreateInfo.flags =
            VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        err = demo->CreateDebugReportCallback(demo->inst, &dbgCreateInfo, NULL,
                                              &demo->msg_callback);
        switch (err) {
        case VK_SUCCESS:
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            ERR_EXIT("CreateDebugReportCallback: out of host memory\n",
                     "CreateDebugReportCallback Failure");
            break;
        default:
            ERR_EXIT("CreateDebugReportCallback: unknown failure\n",
                     "CreateDebugReportCallback Failure");
            break;
        }
    }
    vkGetPhysicalDeviceProperties(demo->gpu, &demo->gpu_props);

    /* Call with NULL data to get count */
    vkGetPhysicalDeviceQueueFamilyProperties(demo->gpu, &demo->queue_count,
                                             NULL);
    assert(demo->queue_count >= 1);

    demo->queue_props = (VkQueueFamilyProperties *)malloc(
        demo->queue_count * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(demo->gpu, &demo->queue_count,
                                             demo->queue_props);
    // Find a queue that supports gfx
    uint32_t gfx_queue_idx = 0;
    for (gfx_queue_idx = 0; gfx_queue_idx < demo->queue_count;
         gfx_queue_idx++) {
        if (demo->queue_props[gfx_queue_idx].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            break;
    }
    assert(gfx_queue_idx < demo->queue_count);
    // Query fine-grained feature support for this device.
    //  If app has specific feature requirements it should check supported
    //  features based on this query
    VkPhysicalDeviceFeatures physDevFeatures;
    vkGetPhysicalDeviceFeatures(demo->gpu, &physDevFeatures);

    GET_INSTANCE_PROC_ADDR(demo->inst, GetPhysicalDeviceSurfaceSupportKHR);
    GET_INSTANCE_PROC_ADDR(demo->inst, GetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_INSTANCE_PROC_ADDR(demo->inst, GetPhysicalDeviceSurfaceFormatsKHR);
    GET_INSTANCE_PROC_ADDR(demo->inst, GetPhysicalDeviceSurfacePresentModesKHR);
    GET_INSTANCE_PROC_ADDR(demo->inst, GetSwapchainImagesKHR);
}

static void demo_create_device(struct demo *demo) {
    VkResult U_ASSERT_ONLY err;
    float queue_priorities[1] = {0.0};
    const VkDeviceQueueCreateInfo queue = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .queueFamilyIndex = demo->graphics_queue_node_index,
        .queueCount = 1,
        .pQueuePriorities = queue_priorities};

    VkDeviceCreateInfo device = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue,
        .enabledLayerCount = demo->enabled_layer_count,
        .ppEnabledLayerNames =
            (const char *const *)((demo->validate)
                                      ? demo->device_validation_layers
                                      : NULL),
        .enabledExtensionCount = demo->enabled_extension_count,
        .ppEnabledExtensionNames = (const char *const *)demo->extension_names,
        .pEnabledFeatures =
            NULL, // If specific features are required, pass them in here
    };

    err = vkCreateDevice(demo->gpu, &device, NULL, &demo->device);
    assert(!err);
}

static void demo_init_vk_swapchain(struct demo *demo) {
    VkResult U_ASSERT_ONLY err;
    uint32_t i;

// Create a WSI surface for the window:
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.hinstance = demo->connection;
    createInfo.hwnd = demo->window;

    err =
        vkCreateWin32SurfaceKHR(demo->inst, &createInfo, NULL, &demo->surface);

#else  // _WIN32
    VkXcbSurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.connection = demo->connection;
    createInfo.window = demo->window;

    err = vkCreateXcbSurfaceKHR(demo->inst, &createInfo, NULL, &demo->surface);
#endif // _WIN32

    // Iterate over each queue to learn whether it supports presenting:
    VkBool32 *supportsPresent =
        (VkBool32 *)malloc(demo->queue_count * sizeof(VkBool32));
    for (i = 0; i < demo->queue_count; i++) {
        demo->fpGetPhysicalDeviceSurfaceSupportKHR(demo->gpu, i, demo->surface,
                                                   &supportsPresent[i]);
    }

    // Search for a graphics and a present queue in the array of queue
    // families, try to find one that supports both
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex = UINT32_MAX;
    for (i = 0; i < demo->queue_count; i++) {
        if ((demo->queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (graphicsQueueNodeIndex == UINT32_MAX) {
                graphicsQueueNodeIndex = i;
            }

            if (supportsPresent[i] == VK_TRUE) {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            }
        }
    }
    if (presentQueueNodeIndex == UINT32_MAX) {
        // If didn't find a queue that supports both graphics and present, then
        // find a separate present queue.
        for (uint32_t i = 0; i < demo->queue_count; ++i) {
            if (supportsPresent[i] == VK_TRUE) {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }
    free(supportsPresent);

    // Generate error if could not find both a graphics and a present queue
    if (graphicsQueueNodeIndex == UINT32_MAX ||
        presentQueueNodeIndex == UINT32_MAX) {
        ERR_EXIT("Could not find a graphics and a present queue\n",
                 "Swapchain Initialization Failure");
    }

    // TODO: Add support for separate queues, including presentation,
    //       synchronization, and appropriate tracking for QueueSubmit.
    // NOTE: While it is possible for an application to use a separate graphics
    //       and a present queues, this demo program assumes it is only using
    //       one:
    if (graphicsQueueNodeIndex != presentQueueNodeIndex) {
        ERR_EXIT("Could not find a common graphics and a present queue\n",
                 "Swapchain Initialization Failure");
    }

    demo->graphics_queue_node_index = graphicsQueueNodeIndex;

    demo_create_device(demo);

    GET_DEVICE_PROC_ADDR(demo->device, CreateSwapchainKHR);
    GET_DEVICE_PROC_ADDR(demo->device, DestroySwapchainKHR);
    GET_DEVICE_PROC_ADDR(demo->device, GetSwapchainImagesKHR);
    GET_DEVICE_PROC_ADDR(demo->device, AcquireNextImageKHR);
    GET_DEVICE_PROC_ADDR(demo->device, QueuePresentKHR);

    vkGetDeviceQueue(demo->device, demo->graphics_queue_node_index, 0,
                     &demo->queue);

    // Get the list of VkFormat's that are supported:
    uint32_t formatCount;
    err = demo->fpGetPhysicalDeviceSurfaceFormatsKHR(demo->gpu, demo->surface,
                                                     &formatCount, NULL);
    assert(!err);
    VkSurfaceFormatKHR *surfFormats =
        (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
    err = demo->fpGetPhysicalDeviceSurfaceFormatsKHR(demo->gpu, demo->surface,
                                                     &formatCount, surfFormats);
    assert(!err);
    // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
    // the surface has no preferred format.  Otherwise, at least one
    // supported format will be returned.
    if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) {
        demo->format = VK_FORMAT_B8G8R8A8_UNORM;
    } else {
        assert(formatCount >= 1);
        demo->format = surfFormats[0].format;
    }
    demo->color_space = surfFormats[0].colorSpace;

    demo->quit = false;
    demo->curFrame = 0;

    // Get Memory information and properties
    vkGetPhysicalDeviceMemoryProperties(demo->gpu, &demo->memory_properties);
}

static void demo_init_connection(struct demo *demo) {
#ifndef _WIN32
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    demo->connection = xcb_connect(NULL, &scr);
    if (demo->connection == NULL) {
        printf("Cannot find a compatible Vulkan installable client driver "
               "(ICD).\nExiting ...\n");
        fflush(stdout);
        exit(1);
    }

    setup = xcb_get_setup(demo->connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0)
        xcb_screen_next(&iter);

    demo->screen = iter.data;
#endif // _WIN32
}

static void demo_init(struct demo *demo, int argc, char **argv) {
    vec3 eye = {0.0f, 3.0f, 5.0f};
    vec3 origin = {0, 0, 0};
    vec3 up = {0.0f, 1.0f, 0.0};

    memset(demo, 0, sizeof(*demo));
    demo->frameCount = INT32_MAX;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--use_staging") == 0) {
            demo->use_staging_buffer = true;
            continue;
        }
        if (strcmp(argv[i], "--break") == 0) {
            demo->use_break = true;
            continue;
        }
        if (strcmp(argv[i], "--validate") == 0) {
            demo->validate = true;
            continue;
        }
        if (strcmp(argv[i], "--c") == 0 && demo->frameCount == INT32_MAX &&
            i < argc - 1 && sscanf(argv[i + 1], "%d", &demo->frameCount) == 1 &&
            demo->frameCount >= 0) {
            i++;
            continue;
        }

        fprintf(stderr, "Usage:\n  %s [--use_staging] [--validate] [--break] "
                        "[--c <framecount>]\n",
                APP_SHORT_NAME);
        fflush(stderr);
        exit(1);
    }

    demo_init_connection(demo);
    demo_init_vk(demo);

    demo->width = 500;
    demo->height = 500;

    demo->spin_angle = 0.01f;
    demo->spin_increment = 0.01f;
    demo->pause = false;

    mat4x4_perspective(demo->projection_matrix, (float)degreesToRadians(45.0f),
                       1.0f, 0.1f, 100.0f);
    mat4x4_look_at(demo->view_matrix, eye, origin, up);
    mat4x4_identity(demo->model_matrix);
}

#ifdef _WIN32
// Include header required for parsing the command line options.
#include <shellapi.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine,
                   int nCmdShow) {
    MSG msg;   // message
    bool done; // flag saying when app is complete
    int argc;
    char **argv;

    // Use the CommandLine functions to get the command line arguments.
    // Unfortunately, Microsoft outputs
    // this information as wide characters for Unicode, and we simply want the
    // Ascii version to be compatible
    // with the non-Windows side.  So, we have to convert the information to
    // Ascii character strings.
    LPWSTR *commandLineArgs = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (NULL == commandLineArgs) {
        argc = 0;
    }

    if (argc > 0) {
        argv = (char **)malloc(sizeof(char *) * argc);
        if (argv == NULL) {
            argc = 0;
        } else {
            for (int iii = 0; iii < argc; iii++) {
                size_t wideCharLen = wcslen(commandLineArgs[iii]);
                size_t numConverted = 0;

                argv[iii] = (char *)malloc(sizeof(char) * (wideCharLen + 1));
                if (argv[iii] != NULL) {
                    wcstombs_s(&numConverted, argv[iii], wideCharLen + 1,
                               commandLineArgs[iii], wideCharLen + 1);
                }
            }
        }
    } else {
        argv = NULL;
    }

    demo_init(&demo, argc, argv);

    // Free up the items we had to allocate for the command line arguments.
    if (argc > 0 && argv != NULL) {
        for (int iii = 0; iii < argc; iii++) {
            if (argv[iii] != NULL) {
                free(argv[iii]);
            }
        }
        free(argv);
    }

    demo.connection = hInstance;
    strncpy(demo.name, "cube", APP_NAME_STR_LEN);
    demo_create_window(&demo);
    demo_init_vk_swapchain(&demo);

    demo_prepare(&demo);

    done = false; // initialize loop condition variable

    // main message loop
    while (!done) {
        PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
        if (msg.message == WM_QUIT) // check for a quit message
        {
            done = true; // if found, quit app
        } else {
            /* Translate and dispatch to event queue*/
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        RedrawWindow(demo.window, NULL, NULL, RDW_INTERNALPAINT);
    }

    demo_cleanup(&demo);

    return (int)msg.wParam;
}
#else  // _WIN32
int main(int argc, char **argv) {
    struct demo demo;

    demo_init(&demo, argc, argv);
    demo_create_window(&demo);
    demo_init_vk_swapchain(&demo);

    demo_prepare(&demo);
    demo_run(&demo);

    demo_cleanup(&demo);

    return validation_error;
}
#endif // _WIN32
