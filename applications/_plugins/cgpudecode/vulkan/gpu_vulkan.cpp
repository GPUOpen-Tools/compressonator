//=====================================================================
// Copyright (c) 2016    Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//
/// \file GPU_Vulkan.cpp
//
// Vulkan Example - Texture loading (and display) example (including mip maps)
//
// Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
//
// This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
//=====================================================================

#include "gpu_vulkan.h"

#pragma comment(lib, "opengl32.lib")  // Open GL Link requirements in Base Code

using namespace GPU_Decode;

#define VK_VERSION_MAJOR(version) ((uint32_t)(version) >> 22)
#define VK_VERSION_MINOR(version) (((uint32_t)(version) >> 12) & 0x3ff)
// Synchronization semaphores
struct
{
    VkSemaphore presentDone;
    VkSemaphore renderDone;
} semaphores;

struct
{
    VkImage        image;
    VkDeviceMemory mem;
    VkImageView    view;
} depthStencil;

// Vertex layout
struct Vertex
{
    float coordinate[3];
    float uv[2];
};

struct
{
    VkBuffer                                       buffer;
    VkDeviceMemory                                 memory;
    VkPipelineVertexInputStateCreateInfo           inputState;
    std::vector<VkVertexInputBindingDescription>   bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
} vertices;

struct
{
    int            count;
    VkBuffer       buffer;
    VkDeviceMemory memory;
} indices;

struct
{
    glm::mat4 projection;
    glm::mat4 model;
    float     lodBias = 0.0f;
} uboVS;

struct Texture
{
    VkSampler      sampler;
    VkImage        image;
    VkImageLayout  imageLayout;
    VkDeviceMemory deviceMemory;
    VkImageView    view;
    uint32_t       width, height;
    uint32_t       mipLevels;
} texture;

struct
{
    VkPipeline solid;
} pipelines;

VkClearColorValue defaultClearColor = {{0.025f, 0.025f, 0.025f, 1.0f}};

GPU_Vulkan::GPU_Vulkan(std::uint32_t Width, std::uint32_t Height, WNDPROC callback)
    : RenderWindow("Vulkan")
{
    m_initOk = false;
    //set default width and height if is 0
    if (Width <= 0)
        Width = 640;
    if (Height <= 0)
        Height = 480;

    m_descriptorPool = VK_NULL_HANDLE;

    if (FAILED(InitWindow(Width, Height, callback)))
    {
        fprintf(stderr, "[Vulkan] Failed to initialize Window. Please make sure Vulkan SDK is available.\n");
        assert(0);
    }

    EnableWindowContext(m_hWnd, &m_hDC, &m_hRC);
}

GPU_Vulkan::~GPU_Vulkan()
{
    if (m_initOk)
        clean();
}

//--------------------------------------------------------------------------------------
void GPU_Vulkan::VkRender()
{
    if (!m_prepared)
        return;
    vkDeviceWaitIdle(device);
    draw();
    vkDeviceWaitIdle(device);
}

VkResult GPU_Vulkan::createInstance(bool enableValidation)
{
    m_tenableValidation = enableValidation;

    VkApplicationInfo appInfo = {};
    appInfo.sType             = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName  = "vulkanview";
    appInfo.pEngineName       = "vulkanview";

    appInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> enabledExtensions = {VK_KHR_SURFACE_EXTENSION_NAME};

#ifdef _WIN32
    enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType                = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext                = NULL;
    instanceCreateInfo.pApplicationInfo     = &appInfo;
    if (enabledExtensions.size() > 0)
    {
        if (enableValidation)
        {
            enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }
        instanceCreateInfo.enabledExtensionCount   = (uint32_t)enabledExtensions.size();
        instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
    }
    if (enableValidation)
    {
        instanceCreateInfo.enabledLayerCount   = vkDebug::validationLayerCount;
        instanceCreateInfo.ppEnabledLayerNames = vkDebug::validationLayerNames;
    }
    return vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
}

VkResult GPU_Vulkan::createDevice(VkDeviceQueueCreateInfo requestedQueues, bool enableValidation)
{
    std::vector<const char*> enabledExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkDeviceCreateInfo deviceCreateInfo   = {};
    deviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext                = NULL;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos    = &requestedQueues;
    deviceCreateInfo.pEnabledFeatures     = NULL;

    if (enabledExtensions.size() > 0)
    {
        deviceCreateInfo.enabledExtensionCount   = (uint32_t)enabledExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
    }
    if (enableValidation)
    {
        deviceCreateInfo.enabledLayerCount   = vkDebug::validationLayerCount;
        deviceCreateInfo.ppEnabledLayerNames = vkDebug::validationLayerNames;
    }

    return vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
}

bool GPU_Vulkan::initVulkan(bool enableValidation)
{
    VkResult err;

    m_initOk = false;

    // Vulkan instance
    err = createInstance(enableValidation);
    if (err)
    {
        fprintf(stderr, "\nCould not create Vulkan instance : %s Vulkan not Supported!\n", vkTools::errorString(err));
        vkTools::exitFatal("Could not create Vulkan instance : \n" + vkTools::errorString(err), "Vulkan not Supported!");
        return false;
    }

    // Physical device
    // Get number of available physical devices
    err = vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
    if (err != VK_SUCCESS)
    {
        fprintf(stderr, "\n[Vulkan Error] Could not get Vulkan devices: %s\n", vkTools::errorString(err));
        return false;
    }

    if (gpuCount == 0)
        fprintf(stderr, "\n[Vulkan Error] no GPU available\n");

    assert(gpuCount > 0);

    // Enumerate devices
    physicalDevices.resize(gpuCount);
    err = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
    if (err != VK_SUCCESS)
    {
        fprintf(stderr, "\n[Vulkan Error] Could not enumerate phyiscal devices: %s\n", vkTools::errorString(err));
        vkTools::exitFatal("Could not enumerate phyiscal devices : \n" + vkTools::errorString(err), "Vulkan not Supported!");
        return false;
    }

    // use the first physical device reported,
    // change the vector index if you have multiple Vulkan devices and want to use another one
    physicalDevice = physicalDevices[0];

    // Find a queue that supports graphics operations
    uint32_t graphicsQueueIndex = 0;
    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);

    if (queueCount == 0)
        fprintf(stderr, "\n[Vulkan Error] queueCount = 0\n");

    assert(queueCount >= 1);

    std::vector<VkQueueFamilyProperties> queueProps;
    queueProps.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());

    for (graphicsQueueIndex = 0; graphicsQueueIndex < queueCount; graphicsQueueIndex++)
    {
        if (queueProps[graphicsQueueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            break;
    }

    if (graphicsQueueIndex > queueCount)
        fprintf(stderr, "\n[Vulkan Error] graphicsQueueIndex > queueCount\n");
    assert(graphicsQueueIndex < queueCount);

    // Vulkan device
    std::array<float, 1>    queuePriorities = {0.0f};
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex        = graphicsQueueIndex;
    queueCreateInfo.queueCount              = 1;
    queueCreateInfo.pQueuePriorities        = queuePriorities.data();

    err = createDevice(queueCreateInfo, enableValidation);
    if (err != VK_SUCCESS)
    {
        fprintf(stderr, "\n[Vulkan Error] Could not createDevice: %s\n", vkTools::errorString(err));
        return false;
    }

    // Gather physical device memory properties
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

    // Get the graphics queue
    vkGetDeviceQueue(device, graphicsQueueIndex, 0, &queue);

    // Find a suitable depth format
    VkBool32 validDepthFormat = vkTools::getSupportedDepthFormat(physicalDevice, &depthFormat);
    if (validDepthFormat == 0)
        fprintf(stderr, "\n[Vulkan Error] validDepthFormat = 0\n");
    assert(validDepthFormat);

    swapChain.connect(instance, physicalDevice, device);

    // Create synchronization objects
    VkSemaphoreCreateInfo semaphoreCreateInfo = vkTools::initializers::semaphoreCreateInfo();
    // Create a semaphore used to synchronize image presentation
    // Ensures that the image is displayed before we start submitting new commands to the queu
    err = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores.presentDone);
    if (err != VK_SUCCESS)
    {
        fprintf(stderr, "\n[Vulkan Error] vkCreateSemaphore present: %s\n", vkTools::errorString(err));
        return false;
    }

    // Create a semaphore used to synchronize command submission
    // Ensures that the image is not presented until all commands have been sumbitted and executed
    err = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores.renderDone);
    if (err != VK_SUCCESS)
    {
        fprintf(stderr, "\n[Vulkan Error] vkCreateSemaphore render: %s\n", vkTools::errorString(err));
        return false;
    }

    // Set up submit info structure
    // Semaphores will stay the same during application lifetime
    // Command buffer submission info is set by each example
    submitInfo                      = vkTools::initializers::submitInfo();
    submitInfo.pWaitDstStageMask    = &submitPipelineStages;
    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = &semaphores.presentDone;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = &semaphores.renderDone;

    m_initOk = true;
    return true;
}

void GPU_Vulkan::submitPostPresentBarrier(VkImage image)
{
    VkCommandBufferBeginInfo cmdBufInfo = vkTools::initializers::commandBufferBeginInfo();

    VkResult vkRes = vkBeginCommandBuffer(postPresentCmdBuffer, &cmdBufInfo);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkBeginCommandBuffer = %d\n", vkRes);
    assert(!vkRes);

    VkImageMemoryBarrier postPresentBarrier = vkTools::initializers::imageMemoryBarrier();
    postPresentBarrier.srcAccessMask        = 0;
    postPresentBarrier.dstAccessMask        = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    postPresentBarrier.oldLayout            = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    postPresentBarrier.newLayout            = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    postPresentBarrier.srcQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
    postPresentBarrier.dstQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
    postPresentBarrier.subresourceRange     = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    postPresentBarrier.image                = image;

    vkCmdPipelineBarrier(postPresentCmdBuffer,
                         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         0,
                         0,
                         nullptr,  // No memory barriers,
                         0,
                         nullptr,  // No buffer barriers,
                         1,
                         &postPresentBarrier);

    vkRes = vkEndCommandBuffer(postPresentCmdBuffer);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkEndCommandBuffer = %d\n", vkRes);
    assert(!vkRes);

    VkSubmitInfo submitInfo       = vkTools::initializers::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &postPresentCmdBuffer;

    vkRes = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkQueueSubmit = %d\n", vkRes);
    assert(!vkRes);
}

void GPU_Vulkan::submitPrePresentBarrier(VkImage image)
{
    VkCommandBufferBeginInfo cmdBufInfo = vkTools::initializers::commandBufferBeginInfo();

    VkResult vkRes = vkBeginCommandBuffer(prePresentCmdBuffer, &cmdBufInfo);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkBeginCommandBuffer = %d\n", vkRes);
    assert(!vkRes);

    VkImageMemoryBarrier prePresentBarrier = vkTools::initializers::imageMemoryBarrier();
    prePresentBarrier.srcAccessMask        = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    prePresentBarrier.dstAccessMask        = 0;
    prePresentBarrier.oldLayout            = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    prePresentBarrier.newLayout            = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    prePresentBarrier.srcQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.dstQueueFamilyIndex  = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.subresourceRange     = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    prePresentBarrier.image                = image;

    vkCmdPipelineBarrier(prePresentCmdBuffer,
                         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_FLAGS_NONE,
                         0,
                         nullptr,  // No memory barriers,
                         0,
                         nullptr,  // No buffer barriers,
                         1,
                         &prePresentBarrier);

    vkRes = vkEndCommandBuffer(prePresentCmdBuffer);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkEndCommandBuffer = %d\n", vkRes);
    assert(!vkRes);

    VkSubmitInfo submitInfo       = vkTools::initializers::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &prePresentCmdBuffer;

    vkRes = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkQueueSubmit = %d\n", vkRes);
    assert(!vkRes);
}

void GPU_Vulkan::draw()
{
    VkResult err;

    // Get next image in the swap chain (back/front buffer)
    err = swapChain.acquireNextImage(semaphores.presentDone, &currentBuffer);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] acquireNextImage = %d\n", err);
    assert(!err);

    // Command buffer to be sumitted to the queue
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &drawCmdBuffers[currentBuffer];

    // Submit to queue
    err = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkQueueSubmit = %d\n", err);
    assert(!err);

    err = swapChain.queuePresent(queue, currentBuffer, semaphores.renderDone);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] queuePresent = %d\n", err);
    assert(!err);

    err = vkQueueWaitIdle(queue);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkQueueWaitIdle = %d\n", err);
    assert(!err);
}

void GPU_Vulkan::createCommandPool()
{
    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex        = swapChain.queueNodeIndex;
    cmdPoolInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VkResult vkRes                      = vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &cmdPool);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkQueueWaitIdle = %d\n", vkRes);
    assert(!vkRes);
}

void GPU_Vulkan::createSetupCommandBuffer()
{
    if (setupCmdBuffer != VK_NULL_HANDLE)
    {
        vkFreeCommandBuffers(device, cmdPool, 1, &setupCmdBuffer);
        setupCmdBuffer = VK_NULL_HANDLE;
    }

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = vkTools::initializers::commandBufferAllocateInfo(cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

    VkResult vkRes = vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &setupCmdBuffer);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkAllocateCommandBuffers = %d\n", vkRes);

    assert(!vkRes);

    VkCommandBufferBeginInfo cmdBufInfo = {};
    cmdBufInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkRes = vkBeginCommandBuffer(setupCmdBuffer, &cmdBufInfo);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkBeginCommandBuffer = %d\n", vkRes);

    assert(!vkRes);
}

void GPU_Vulkan::createCommandBuffers()
{
    // Create one command buffer per frame buffer in the swap chain

    drawCmdBuffers.resize(swapChain.m_imageCount);

    VkCommandBufferAllocateInfo cmdBufAllocateInfo =
        vkTools::initializers::commandBufferAllocateInfo(cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, (uint32_t)drawCmdBuffers.size());

    VkResult vkRes = vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, drawCmdBuffers.data());
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkAllocateCommandBuffers = %d\n", vkRes);
    assert(!vkRes);
}

VkBool32 GPU_Vulkan::getMemoryType(uint32_t typeBits, VkFlags properties, uint32_t* typeIndex)
{
    for (uint32_t i = 0; i < 32; i++)
    {
        if ((typeBits & 1) == 1)
        {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    return false;
}

void GPU_Vulkan::setupDepthStencil()
{
    VkImageCreateInfo image = {};
    image.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image.pNext             = NULL;
    image.imageType         = VK_IMAGE_TYPE_2D;
    image.format            = depthFormat;
    image.extent            = {m_width, m_height, 1};
    image.mipLevels         = 1;
    image.arrayLayers       = 1;
    image.samples           = VK_SAMPLE_COUNT_1_BIT;
    image.tiling            = VK_IMAGE_TILING_OPTIMAL;
    image.usage             = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    image.flags             = 0;

    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext                = NULL;
    mem_alloc.allocationSize       = 0;
    mem_alloc.memoryTypeIndex      = 0;

    VkImageViewCreateInfo depthStencilView           = {};
    depthStencilView.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthStencilView.pNext                           = NULL;
    depthStencilView.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format                          = depthFormat;
    depthStencilView.flags                           = 0;
    depthStencilView.subresourceRange                = {};
    depthStencilView.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthStencilView.subresourceRange.baseMipLevel   = 0;
    depthStencilView.subresourceRange.levelCount     = 1;
    depthStencilView.subresourceRange.baseArrayLayer = 0;
    depthStencilView.subresourceRange.layerCount     = 1;

    VkMemoryRequirements memReqs;
    VkResult             err;

    err = vkCreateImage(device, &image, nullptr, &depthStencil.image);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreateImage = %d\n", err);
    assert(!err);
    vkGetImageMemoryRequirements(device, depthStencil.image, &memReqs);
    mem_alloc.allocationSize = memReqs.size;
    getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);
    err = vkAllocateMemory(device, &mem_alloc, nullptr, &depthStencil.mem);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkAllocateMemory = %d\n", err);
    assert(!err);

    err = vkBindImageMemory(device, depthStencil.image, depthStencil.mem, 0);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkBindImageMemory = %d\n", err);
    assert(!err);

    depthStencilView.image = depthStencil.image;
    err                    = vkCreateImageView(device, &depthStencilView, nullptr, &depthStencil.view);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreateImageView = %d\n", err);
    assert(!err);
}

void GPU_Vulkan::setupRenderPass()
{
    // Color buffer format
    VkFormat colorformat = VK_FORMAT_B8G8R8A8_UNORM;

    if ((m_pSourceTexture->format == CMP_FORMAT_BC4_S) || (m_pSourceTexture->format == CMP_FORMAT_BC5_S) ||
        (m_pSourceTexture->format == CMP_FORMAT_RGBA_8888_S))
        colorformat = VK_FORMAT_B8G8R8A8_SNORM;

    VkAttachmentDescription attachments[2];
    attachments[0].format         = colorformat;
    attachments[0].samples        = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachments[1].format         = depthFormat;
    attachments[1].samples        = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[1].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {};
    colorReference.attachment            = 0;
    colorReference.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment            = 1;
    depthReference.layout                = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass    = {};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.flags                   = 0;
    subpass.inputAttachmentCount    = 0;
    subpass.pInputAttachments       = NULL;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorReference;
    subpass.pResolveAttachments     = NULL;
    subpass.pDepthStencilAttachment = &depthReference;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments    = NULL;

    // Subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass      = 0;
    dependencies[0].srcStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass      = 0;
    dependencies[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask    = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.pNext                  = NULL;
    renderPassInfo.attachmentCount        = 2;
    renderPassInfo.pAttachments           = attachments;
    renderPassInfo.subpassCount           = 1;
    renderPassInfo.pSubpasses             = &subpass;
    renderPassInfo.dependencyCount        = 2;
    renderPassInfo.pDependencies          = dependencies.data();

    VkResult err;

    err = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreateRenderPass = %d\n", err);
    assert(!err);
}

void GPU_Vulkan::createPipelineCache()
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType                     = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    VkResult err                                      = vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreatePipelineCache = %d\n", err);
    assert(!err);
}

void GPU_Vulkan::setupFrameBuffer()
{
    VkImageView attachments[2];

    // Depth/Stencil attachment is the same for all frame buffers
    attachments[1] = depthStencil.view;

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.pNext                   = NULL;
    frameBufferCreateInfo.renderPass              = renderPass;
    frameBufferCreateInfo.attachmentCount         = 2;
    frameBufferCreateInfo.pAttachments            = attachments;
    frameBufferCreateInfo.width                   = m_width;
    frameBufferCreateInfo.height                  = m_height;
    frameBufferCreateInfo.layers                  = 1;

    // Create frame buffers for every swap chain image
    frameBuffers.resize(swapChain.m_imageCount);
    for (uint32_t i = 0; i < frameBuffers.size(); i++)
    {
        attachments[0] = swapChain.buffers[i].view;
        VkResult err   = vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &frameBuffers[i]);
        if (err != VK_SUCCESS)
            fprintf(stderr, "\n[Vulkan Error] vkCreateFramebuffer = %d\n", err);
        assert(!err);
    }
}

void GPU_Vulkan::flushSetupCommandBuffer()
{
    VkResult err;

    if (setupCmdBuffer == VK_NULL_HANDLE)
        return;

    err = vkEndCommandBuffer(setupCmdBuffer);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkEndCommandBuffer = %d\n", err);
    assert(!err);

    VkSubmitInfo submitInfo       = {};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &setupCmdBuffer;

    err = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkQueueSubmit = %d\n", err);
    assert(!err);

    err = vkQueueWaitIdle(queue);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkQueueWaitIdle = %d\n", err);
    assert(!err);

    vkFreeCommandBuffers(device, cmdPool, 1, &setupCmdBuffer);
    setupCmdBuffer = VK_NULL_HANDLE;
}

VkBool32 GPU_Vulkan::createBuffer(VkBufferUsageFlags usage, VkDeviceSize size, void* data, VkBuffer* buffer, VkDeviceMemory* memory)
{
    VkMemoryRequirements memReqs;
    VkMemoryAllocateInfo memAlloc         = vkTools::initializers::memoryAllocateInfo();
    VkBufferCreateInfo   bufferCreateInfo = vkTools::initializers::bufferCreateInfo(usage, size);

    VkResult err = vkCreateBuffer(device, &bufferCreateInfo, nullptr, buffer);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreateBuffer = %d\n", err);
    assert(!err);
    vkGetBufferMemoryRequirements(device, *buffer, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAlloc.memoryTypeIndex);
    err = vkAllocateMemory(device, &memAlloc, nullptr, memory);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkAllocateMemory = %d\n", err);
    assert(!err);
    if (data != nullptr)
    {
        void* mapped;
        err = vkMapMemory(device, *memory, 0, size, 0, &mapped);
        if (err != VK_SUCCESS)
            fprintf(stderr, "\n[Vulkan Error] vkMapMemory = %d\n", err);
        assert(!err);
        memcpy(mapped, data, (std::size_t)size);
        vkUnmapMemory(device, *memory);
    }
    err = vkBindBufferMemory(device, *buffer, *memory, 0);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkBindBufferMemory = %d\n", err);
    assert(!err);
    return true;
}

VkBool32 GPU_Vulkan::createBuffer(VkBufferUsageFlags      usage,
                                  VkDeviceSize            size,
                                  void*                   data,
                                  VkBuffer*               buffer,
                                  VkDeviceMemory*         memory,
                                  VkDescriptorBufferInfo* descriptor)
{
    VkBool32 res = createBuffer(usage, size, data, buffer, memory);
    if (res)
    {
        descriptor->offset = 0;
        descriptor->buffer = *buffer;
        descriptor->range  = size;
        return true;
    }
    else
    {
        return false;
    }
}

void GPU_Vulkan::updateUniformBuffers()
{
    // Vertex shader- use default
    uint8_t* pData;
    VkResult err = vkMapMemory(device, uniformDataVS.memory, 0, sizeof(uboVS), 0, (void**)&pData);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkMapMemory = %d\n", err);
    assert(!err);
    memcpy(pData, &uboVS, sizeof(uboVS));
    vkUnmapMemory(device, uniformDataVS.memory);
}

// Create an image memory barrier for changing the layout of an image and put it into an active command buffer
void GPU_Vulkan::setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
{
    // Create an image barrier object
    VkImageMemoryBarrier imageMemoryBarrier = vkTools::initializers::imageMemoryBarrier();
    ;
    imageMemoryBarrier.oldLayout                     = oldImageLayout;
    imageMemoryBarrier.newLayout                     = newImageLayout;
    imageMemoryBarrier.image                         = image;
    imageMemoryBarrier.subresourceRange.aspectMask   = aspectMask;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount   = 1;
    imageMemoryBarrier.subresourceRange.layerCount   = 1;

    // New layout is transfer destination (copy, blit)
    // Make sure any reads from and writes to the image have been finished
    if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    // New layout is shader read (sampler, input attachment)
    // Make sure any writes to the image have been finished
    if (newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }

    // New layout is transfer source
    // Make sure any reads from and writes to the image have been finished
    if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    // Put barrier on top
    VkPipelineStageFlags srcStageFlags  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    // Put barrier inside setup command buffer
    vkCmdPipelineBarrier(setupCmdBuffer, srcStageFlags, destStageFlags, VK_FLAGS_NONE, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

// Create an image memory barrier for changing the layout of
// an image and put it into an active command buffer
void GPU_Vulkan::setImageLayout(VkCommandBuffer         cmdBuffer,
                                VkImage                 image,
                                VkImageAspectFlags      aspectMask,
                                VkImageLayout           oldImageLayout,
                                VkImageLayout           newImageLayout,
                                VkImageSubresourceRange subresourceRange)
{
    // Create an image barrier object
    VkImageMemoryBarrier imageMemoryBarrier = vkTools::initializers::imageMemoryBarrier();
    ;
    imageMemoryBarrier.oldLayout        = oldImageLayout;
    imageMemoryBarrier.newLayout        = newImageLayout;
    imageMemoryBarrier.image            = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    // Only sets masks for layouts used in this example
    // For a more complete version that can be used with other layouts see vkTools::setImageLayout

    // Source layouts (old)
    switch (oldImageLayout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        // Only valid as initial layout, memory contents are not preserved
        // Can be accessed directly, no source dependency required
        imageMemoryBarrier.srcAccessMask = 0;
        break;
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        // Only valid as initial layout for linear images, preserves memory contents
        // Make sure host writes to the image have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Old layout is transfer destination
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    }

    // Target layouts (new)
    switch (newImageLayout)
    {
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Transfer source (copy, blit)
        // Make sure any reads from the image have been finished
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Transfer destination (copy, blit)
        // Make sure any writes to the image have been finished
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Shader read (sampler, input attachment)
        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    }

    // Put barrier on top of pipeline
    VkPipelineStageFlags srcStageFlags  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    // Put barrier inside setup command buffer
    vkCmdPipelineBarrier(cmdBuffer, srcStageFlags, destStageFlags, VK_FLAGS_NONE, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

VkFormat GPU_Vulkan::MIP2VK_Format(const CMP_Texture* pSourceTexture)
{
    VkFormat m_VKnum;
    switch (pSourceTexture->format)
    {
    case CMP_FORMAT_BC1:
    case CMP_FORMAT_DXT1:
        m_VKnum = VK_FORMAT_BC1_RGB_UNORM_BLOCK;
        break;
    case CMP_FORMAT_BC2:
    case CMP_FORMAT_DXT3:
        m_VKnum = VK_FORMAT_BC2_UNORM_BLOCK;
        break;
    case CMP_FORMAT_BC3:
    case CMP_FORMAT_DXT5:
        m_VKnum = VK_FORMAT_BC3_UNORM_BLOCK;
        break;
    case CMP_FORMAT_BC4:
    case CMP_FORMAT_ATI1N:
        m_VKnum = VK_FORMAT_BC4_UNORM_BLOCK;
        break;
    case CMP_FORMAT_BC4_S:
        m_VKnum = VK_FORMAT_BC4_SNORM_BLOCK;
        break;
    case CMP_FORMAT_BC5:
    case CMP_FORMAT_ATI2N:
    case CMP_FORMAT_ATI2N_XY:
    case CMP_FORMAT_ATI2N_DXT5:
        m_VKnum = VK_FORMAT_BC5_UNORM_BLOCK;
        break;
    case CMP_FORMAT_BC5_S:
        m_VKnum = VK_FORMAT_BC5_SNORM_BLOCK;
        break;
    case CMP_FORMAT_BC6H:
        m_VKnum = VK_FORMAT_BC6H_UFLOAT_BLOCK;
        break;
    case CMP_FORMAT_BC6H_SF:
        m_VKnum = VK_FORMAT_BC6H_SFLOAT_BLOCK;
        break;
    case CMP_FORMAT_BC7:
        m_VKnum = VK_FORMAT_BC7_UNORM_BLOCK;
        break;
    case CMP_FORMAT_ETC_RGB:
    case CMP_FORMAT_ETC2_RGB:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        break;
    case CMP_FORMAT_ETC2_SRGB:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
        break;
    case CMP_FORMAT_ETC2_RGBA:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
        break;
    case CMP_FORMAT_ETC2_RGBA1:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
        break;
    case CMP_FORMAT_ETC2_SRGBA:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
        break;
    case CMP_FORMAT_ETC2_SRGBA1:
        m_VKnum = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
        break;
    case CMP_FORMAT_ASTC:
        if ((pSourceTexture->nBlockWidth == 4) && (pSourceTexture->nBlockHeight == 4))
            m_VKnum = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 5) && (pSourceTexture->nBlockHeight == 4))
            m_VKnum = VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 5) && (pSourceTexture->nBlockHeight == 5))
            m_VKnum = VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 6) && (pSourceTexture->nBlockHeight == 5))
            m_VKnum = VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 6) && (pSourceTexture->nBlockHeight == 6))
            m_VKnum = VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 8) && (pSourceTexture->nBlockHeight == 5))
            m_VKnum = VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 8) && (pSourceTexture->nBlockHeight == 6))
            m_VKnum = VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 8) && (pSourceTexture->nBlockHeight == 8))
            m_VKnum = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 10) && (pSourceTexture->nBlockHeight == 5))
            m_VKnum = VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 10) && (pSourceTexture->nBlockHeight == 6))
            m_VKnum = VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 10) && (pSourceTexture->nBlockHeight == 8))
            m_VKnum = VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 10) && (pSourceTexture->nBlockHeight == 10))
            m_VKnum = VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 12) && (pSourceTexture->nBlockHeight == 10))
            m_VKnum = VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
        else if ((pSourceTexture->nBlockWidth == 12) && (pSourceTexture->nBlockHeight == 12))
            m_VKnum = VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        else
            m_VKnum = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        break;
    default:
        m_VKnum = VK_FORMAT_UNDEFINED;
        break;
    }
    return m_VKnum;
}

CMP_ERROR GPU_Vulkan::loadTexture(const CMP_Texture* pSourceTexture)
{
    VkResult res;

    VkFormat format = MIP2VK_Format(pSourceTexture);

    if (format == VK_FORMAT_UNDEFINED)
        return (CMP_ERR_UNSUPPORTED_SOURCE_FORMAT);

    VkFormatProperties formatProperties;

    texture.width     = static_cast<uint32_t>(pSourceTexture->dwWidth);
    texture.height    = static_cast<uint32_t>(pSourceTexture->dwHeight);
    texture.mipLevels = 1;

    // Get device properites for the requested texture format
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
    VkMemoryAllocateInfo memAllocInfo = vkTools::initializers::memoryAllocateInfo();
    VkMemoryRequirements memReqs      = {};

    {
        // Create a host-visible staging buffer that contains the raw image data
        VkBuffer       stagingBuffer;
        VkDeviceMemory stagingMemory;

        // This buffer is used as a transfer source for the buffer copy
        VkBufferCreateInfo bufferCreateInfo = vkTools::initializers::bufferCreateInfo(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, pSourceTexture->dwDataSize);
        bufferCreateInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

        res = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &stagingBuffer);
        if (res != VK_SUCCESS)
        {
            return (CMP_ERR_GENERIC);
        }

        // Get memory requirements for the staging buffer (alignment, memory type bits)
        vkGetBufferMemoryRequirements(device, stagingBuffer, &memReqs);

        memAllocInfo.allocationSize = memReqs.size;
        // Get memory type that can be mapped to host memory
        getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);

        res = vkAllocateMemory(device, &memAllocInfo, nullptr, &stagingMemory);
        if (res != VK_SUCCESS)
        {
            return (CMP_ERR_GENERIC);
        }

        res = vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0);
        if (res != VK_SUCCESS)
        {
            return (CMP_ERR_GENERIC);
        }

        // Copy texture data into staging buffer
        uint8_t* data;
        res = vkMapMemory(device, stagingMemory, 0, memReqs.size, 0, (void**)&data);
        if (res != VK_SUCCESS)
        {
            return (CMP_ERR_GENERIC);
        }

        memcpy(data, pSourceTexture->pData, pSourceTexture->dwDataSize);
        vkUnmapMemory(device, stagingMemory);

        std::vector<VkBufferImageCopy> bufferCopyRegions;
        // Setup buffer copy regions for each mip level
        // Can loop this section of code for miplevels
        {
            VkBufferImageCopy bufferCopyRegion               = {};
            bufferCopyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            bufferCopyRegion.imageSubresource.mipLevel       = 0;
            bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
            bufferCopyRegion.imageSubresource.layerCount     = 1;
            bufferCopyRegion.imageExtent.width               = static_cast<uint32_t>(pSourceTexture->dwWidth);
            bufferCopyRegion.imageExtent.height              = static_cast<uint32_t>(pSourceTexture->dwHeight);
            bufferCopyRegion.imageExtent.depth               = 1;
            bufferCopyRegion.bufferOffset                    = 0;
            bufferCopyRegions.push_back(bufferCopyRegion);
        }

        // Create optimal tiled target image
        VkImageCreateInfo imageCreateInfo = vkTools::initializers::imageCreateInfo();
        imageCreateInfo.imageType         = VK_IMAGE_TYPE_2D;
        imageCreateInfo.format            = format;
        imageCreateInfo.mipLevels         = texture.mipLevels;
        imageCreateInfo.arrayLayers       = 1;
        imageCreateInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.tiling            = VK_IMAGE_TILING_OPTIMAL;
        imageCreateInfo.usage             = VK_IMAGE_USAGE_SAMPLED_BIT;
        imageCreateInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;  //VK_IMAGE_LAYOUT_PREINITIALIZED
        imageCreateInfo.extent            = {texture.width, texture.height, 1};
        imageCreateInfo.usage             = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

        res = vkCreateImage(device, &imageCreateInfo, nullptr, &texture.image);
        if (res != VK_SUCCESS)
        {
            return (CMP_ERR_GENERIC);
        }

        vkGetImageMemoryRequirements(device, texture.image, &memReqs);

        memAllocInfo.allocationSize = memReqs.size;
        getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);

        res = vkAllocateMemory(device, &memAllocInfo, nullptr, &texture.deviceMemory);
        if (res != VK_SUCCESS)
        {
            return (CMP_ERR_GENERIC);
        }

        res = vkBindImageMemory(device, texture.image, texture.deviceMemory, 0);
        if (res != VK_SUCCESS)
        {
            return (CMP_ERR_GENERIC);
        }

        VkCommandBuffer copyCmd = createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

        // Image barrier for optimal image

        // The sub resource range describes the regions of the image we will be transition
        VkImageSubresourceRange subresourceRange = {};
        // Image only contains color data
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // Start at first mip level
        subresourceRange.baseMipLevel = 0;
        // We will transition on all mip levels
        subresourceRange.levelCount = texture.mipLevels;
        // The 2D texture only has one layer
        subresourceRange.layerCount = 1;

        // Optimal image will be used as destination for the copy, so we must transfer from our
        // initial undefined image layout to the transfer destination layout
        setImageLayout(copyCmd,
                       texture.image,
                       VK_IMAGE_ASPECT_COLOR_BIT,
                       VK_IMAGE_LAYOUT_UNDEFINED,  // VK_IMAGE_LAYOUT_PREINITIALIZED,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       subresourceRange);

        // Copy mip levels from staging buffer
        vkCmdCopyBufferToImage(copyCmd,
                               stagingBuffer,
                               texture.image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                               static_cast<uint32_t>(bufferCopyRegions.size()),
                               bufferCopyRegions.data());

        // Change texture image layout to shader read after all mip levels have been copied
        texture.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        setImageLayout(copyCmd, texture.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, texture.imageLayout, subresourceRange);

        flushCommandBuffer(copyCmd, queue, true);

        // Clean up staging resources
        vkFreeMemory(device, stagingMemory, nullptr);
        vkDestroyBuffer(device, stagingBuffer, nullptr);
    }

    // Create sampler
    // In Vulkan textures are accessed by samplers
    // This separates all the sampling information from the
    // texture data
    // This means you could have multiple sampler objects
    // for the same texture with different settings
    // Similar to the samplers available with OpenGL 3.3
    VkSamplerCreateInfo sampler = vkTools::initializers::samplerCreateInfo();
    sampler.magFilter           = VK_FILTER_LINEAR;
    sampler.minFilter           = VK_FILTER_LINEAR;
    sampler.mipmapMode          = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler.addressModeU        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.addressModeV        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.addressModeW        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler.mipLodBias          = 0.0f;
    sampler.compareOp           = VK_COMPARE_OP_NEVER;
    sampler.minLod              = 0.0f;
    // Set max level-of-detail to mip level count of the texture
    sampler.maxLod           = (float)texture.mipLevels;
    sampler.maxAnisotropy    = 1.0;  // anisotropic filtering
    sampler.anisotropyEnable = VK_FALSE;
    sampler.borderColor      = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    res                      = vkCreateSampler(device, &sampler, nullptr, &texture.sampler);
    if (res != VK_SUCCESS)
    {
        return (CMP_ERR_GENERIC);
    }

    // Create image view
    // Textures are not directly accessed by the shaders and
    // are abstracted by image views containing additional
    // information and sub resource ranges
    VkImageViewCreateInfo view = vkTools::initializers::imageViewCreateInfo();
    view.image                 = VK_NULL_HANDLE;
    view.viewType              = VK_IMAGE_VIEW_TYPE_2D;
    view.format                = format;
    view.components            = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
    // The subresource range describes the set of mip levels (and array layers) that can be accessed through this image view
    // It's possible to create multiple image views for a single image referring to different (and/or overlapping) ranges of the image
    view.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    view.subresourceRange.baseMipLevel   = 0;
    view.subresourceRange.baseArrayLayer = 0;
    view.subresourceRange.layerCount     = 1;
    // Linear tiling usually won't support mip maps
    // Only set mip map count if optimal tiling is used
    view.subresourceRange.levelCount = texture.mipLevels;
    view.image                       = texture.image;
    res                              = vkCreateImageView(device, &view, nullptr, &texture.view);
    if (res != VK_SUCCESS)
    {
        return (CMP_ERR_GENERIC);
    }

    return (CMP_OK);
}

//==============================================================

VkCommandBuffer GPU_Vulkan::createCommandBuffer(VkCommandBufferLevel level, bool begin)
{
    VkCommandBuffer cmdBuffer;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = vkTools::initializers::commandBufferAllocateInfo(cmdPool, level, 1);

    VK_CHECK_RESULT_ERR_PTR(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &cmdBuffer));

    // If requested, also start the new command buffer
    if (begin)
    {
        VkCommandBufferBeginInfo cmdBufInfo = vkTools::initializers::commandBufferBeginInfo();
        VK_CHECK_RESULT_ERR_PTR(vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo));
    }

    return cmdBuffer;
}

void GPU_Vulkan::flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free = true)
{
    if (commandBuffer == VK_NULL_HANDLE)
    {
        return;
    }

    VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

    VkSubmitInfo submitInfo       = {};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceInfo = vkTools::initializers::fenceCreateInfo(VK_FLAGS_NONE);
    VkFence           fence;
    VkResult          vkRes = vkCreateFence(device, &fenceInfo, nullptr, &fence);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreateFence = %d\n", vkRes);
    assert(!vkRes);

    // Submit to the queue
    vkRes = vkQueueSubmit(queue, 1, &submitInfo, fence);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkQueueSubmit = %d\n", vkRes);
    assert(!vkRes);

    // Wait for the fence to signal that command buffer has finished executing
    vkRes = vkWaitForFences(device, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT);
    if (vkRes != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkWaitForFences = %d\n", vkRes);
    assert(!vkRes);

    vkDestroyFence(device, fence, nullptr);

    if (free)
    {
        vkFreeCommandBuffers(device, cmdPool, 1, &commandBuffer);
    }
}

//==============================================================

VkPipelineShaderStageCreateInfo GPU_Vulkan::loadShader(const char* fileName, VkShaderStageFlagBits stage)
{
    VkPipelineShaderStageCreateInfo shaderStage = {};
    shaderStage.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStage.stage                           = stage;
    shaderStage.module                          = vkTools::loadShader(fileName, device, stage);
    shaderStage.pName                           = "main";
    if (shaderStage.module == NULL)
        fprintf(stderr, "\n[Vulkan Error] shaderStage.module is NULL\n");
    assert(shaderStage.module != NULL);
    shaderModules.push_back(shaderStage.module);
    return shaderStage;
}

// ToDo remove asserts and replace with cmp_res

CMP_ERROR GPU_Vulkan::prepare(const CMP_Texture* pSourceTexture)
{
    if (m_tenableValidation)
    {
        vkDebug::setupDebugging(instance, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, NULL);
    }

    createCommandPool();
    swapChain.create(&m_width, &m_height);
    createCommandBuffers();
    setupDepthStencil();
    setupRenderPass();
    createPipelineCache();
    setupFrameBuffer();

    // Setup vertices for a single uv-mapped quad
#define dim 1.0f
    std::vector<Vertex> vertexBuffer = {
        {{dim, dim, 0.0f}, {1.0f, 1.0f}}, {{-dim, dim, 0.0f}, {0.0f, 1.0f}}, {{-dim, -dim, 0.0f}, {0.0f, 0.0f}}, {{dim, -dim, 0.0f}, {1.0f, 0.0f}}};
#undef dim
    createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBuffer.size() * sizeof(Vertex), vertexBuffer.data(), &vertices.buffer, &vertices.memory);

    // Setup indices
    std::vector<uint32_t> indexBuffer = {0, 1, 2, 2, 3, 0};
    indices.count                     = (int)indexBuffer.size();

    createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBuffer.size() * sizeof(uint32_t), indexBuffer.data(), &indices.buffer, &indices.memory);

    // setupVertexDescriptions
    // Binding description
    vertices.bindingDescriptions.resize(1);
    vertices.bindingDescriptions[0] = vkTools::initializers::vertexInputBindingDescription(0,  //vertex buffer bind id
                                                                                           sizeof(Vertex),
                                                                                           VK_VERTEX_INPUT_RATE_VERTEX);

    // Attribute descriptions
    // Describes memory layout and shader positions
    vertices.attributeDescriptions.resize(2);
    // Location 0 : Position
    vertices.attributeDescriptions[0] = vkTools::initializers::vertexInputAttributeDescription(0,  //vertex buffer bind id
                                                                                               0,
                                                                                               VK_FORMAT_R32G32B32_SFLOAT,
                                                                                               0);
    // Location 1 : Texture coordinates
    vertices.attributeDescriptions[1] = vkTools::initializers::vertexInputAttributeDescription(0,  //vertex buffer bind id
                                                                                               1,
                                                                                               VK_FORMAT_R32G32_SFLOAT,
                                                                                               sizeof(float) * 3);

    vertices.inputState                                 = vkTools::initializers::pipelineVertexInputStateCreateInfo();
    vertices.inputState.vertexBindingDescriptionCount   = (uint32_t)vertices.bindingDescriptions.size();
    vertices.inputState.pVertexBindingDescriptions      = vertices.bindingDescriptions.data();
    vertices.inputState.vertexAttributeDescriptionCount = (uint32_t)vertices.attributeDescriptions.size();
    vertices.inputState.pVertexAttributeDescriptions    = vertices.attributeDescriptions.data();

    // Vertex shader uniform buffer block
    createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(uboVS), &uboVS, &uniformDataVS.buffer, &uniformDataVS.memory, &uniformDataVS.descriptor);

    updateUniformBuffers();

    CMP_ERROR cmp_res = loadTexture(pSourceTexture);

    if (cmp_res != CMP_OK)
    {
        return (cmp_res);
    }

    //===========================
    //setupDescriptorSetLayout
    //===========================
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
        // Binding 0 : Vertex shader uniform buffer
        vkTools::initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
        // Binding 1 : Fragment shader image sampler
        vkTools::initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)};

    VkDescriptorSetLayoutCreateInfo descriptorLayout =
        vkTools::initializers::descriptorSetLayoutCreateInfo(setLayoutBindings.data(), (uint32_t)setLayoutBindings.size());

    VkResult err = vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &descriptorSetLayout);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreateDescriptorSetLayout = %d\n", err);

    assert(!err);

    //===========================
    // preparePipelines
    //===========================
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = vkTools::initializers::pipelineLayoutCreateInfo(&descriptorSetLayout, 1);

    err = vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreatePipelineLayout = %d\n", err);
    assert(!err);

    // preparePipelines
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        vkTools::initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

    VkPipelineRasterizationStateCreateInfo rasterizationState =
        vkTools::initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE, 0);

    VkPipelineColorBlendAttachmentState blendAttachmentState = vkTools::initializers::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

    VkPipelineColorBlendStateCreateInfo colorBlendState = vkTools::initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        vkTools::initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);

    VkPipelineViewportStateCreateInfo viewportState = vkTools::initializers::pipelineViewportStateCreateInfo(1, 1, 0);

    VkPipelineMultisampleStateCreateInfo multisampleState = vkTools::initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, 0);

    std::vector<VkDynamicState>      dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState =
        vkTools::initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables.data(), (uint32_t)dynamicStateEnables.size(), (uint32_t)0);

    // Load shaders
    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages;

    shaderStages[0] = loadShader("texture.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    shaderStages[1] = loadShader("texture.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = vkTools::initializers::pipelineCreateInfo(pipelineLayout, renderPass, 0);

    pipelineCreateInfo.pVertexInputState   = &vertices.inputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState    = &colorBlendState;
    pipelineCreateInfo.pMultisampleState   = &multisampleState;
    pipelineCreateInfo.pViewportState      = &viewportState;
    pipelineCreateInfo.pDepthStencilState  = &depthStencilState;
    pipelineCreateInfo.pDynamicState       = &dynamicState;
    pipelineCreateInfo.stageCount          = (uint32_t)shaderStages.size();
    pipelineCreateInfo.pStages             = shaderStages.data();

    err = vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipelines.solid);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreateGraphicsPipelines = %d\n", err);
    assert(!err);

    //setupDescriptorPool
    // Example uses one ubo and one image sampler
    std::vector<VkDescriptorPoolSize> poolSizes = {vkTools::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1),
                                                   vkTools::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)};

    VkDescriptorPoolCreateInfo descriptorPoolInfo = vkTools::initializers::descriptorPoolCreateInfo((uint32_t)poolSizes.size(), poolSizes.data(), (uint32_t)2);

    VkResult vkRes = vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &m_descriptorPool);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreateDescriptorPool = %d\n", err);
    assert(!vkRes);

    //setupDescriptorSet
    VkDescriptorSetAllocateInfo allocInfo = vkTools::initializers::descriptorSetAllocateInfo(m_descriptorPool, &descriptorSetLayout, 1);

    vkRes = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
    if (err != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkAllocateDescriptorSets = %d\n", err);
    assert(!vkRes);

    // Image descriptor for the color map texture
    VkDescriptorImageInfo texDescriptor = vkTools::initializers::descriptorImageInfo(texture.sampler, texture.view, VK_IMAGE_LAYOUT_GENERAL);

    std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
        // Binding 0 : Vertex shader uniform buffer
        vkTools::initializers::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &uniformDataVS.descriptor),
        // Binding 1 : Fragment shader texture sampler
        vkTools::initializers::writeDescriptorSet(descriptorSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &texDescriptor)};

    vkUpdateDescriptorSets(device, (uint32_t)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, NULL);

    //buildCommandBuffers
    VkCommandBufferBeginInfo cmdBufInfo = vkTools::initializers::commandBufferBeginInfo();

    VkClearValue clearValues[2];
    clearValues[0].color        = defaultClearColor;
    clearValues[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo renderPassBeginInfo    = vkTools::initializers::renderPassBeginInfo();
    renderPassBeginInfo.renderPass               = renderPass;
    renderPassBeginInfo.renderArea.offset.x      = 0;
    renderPassBeginInfo.renderArea.offset.y      = 0;
    renderPassBeginInfo.renderArea.extent.width  = m_width;
    renderPassBeginInfo.renderArea.extent.height = m_height;
    renderPassBeginInfo.clearValueCount          = 2;
    renderPassBeginInfo.pClearValues             = clearValues;

    for (size_t i = 0; i < drawCmdBuffers.size(); ++i)
    {
        // Set target frame buffer
        renderPassBeginInfo.framebuffer = frameBuffers[i];

        err = vkBeginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo);
        if (err != VK_SUCCESS)
            fprintf(stderr, "\n[Vulkan Error] vkBeginCommandBuffer = %d\n", err);
        assert(!err);

        vkCmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport = vkTools::initializers::viewport((float)m_width, (float)m_height, 0.0f, 1.0f);
        vkCmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

        VkRect2D scissor = vkTools::initializers::rect2D(m_width, m_height, 0, 0);
        vkCmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

        vkCmdBindDescriptorSets(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);
        vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.solid);

        VkDeviceSize offsets[1] = {0};
        vkCmdBindVertexBuffers(drawCmdBuffers[i], 0, 1, &vertices.buffer, offsets);
        vkCmdBindIndexBuffer(drawCmdBuffers[i], indices.buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(drawCmdBuffers[i], indices.count, 1, 0, 0, 0);

        vkCmdEndRenderPass(drawCmdBuffers[i]);

        err = vkEndCommandBuffer(drawCmdBuffers[i]);
        if (err != VK_SUCCESS)
            fprintf(stderr, "\n[Vulkan Error] vkEndCommandBuffer = %d\n", err);
        assert(!err);
    }

    m_prepared = true;

    return (cmp_res);
}

void GPU_Vulkan::destroyCommandBuffers()
{
    vkFreeCommandBuffers(device, cmdPool, (uint32_t)drawCmdBuffers.size(), drawCmdBuffers.data());
}

void GPU_Vulkan::clean()
{
    // Clean up Vulkan resources
    swapChain.cleanup();
    if (m_descriptorPool != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
    if (setupCmdBuffer != VK_NULL_HANDLE)
    {
        vkFreeCommandBuffers(device, cmdPool, 1, &setupCmdBuffer);
    }
    destroyCommandBuffers();
    vkDestroyRenderPass(device, renderPass, nullptr);
    for (uint32_t i = 0; i < frameBuffers.size(); i++)
    {
        vkDestroyFramebuffer(device, frameBuffers[i], nullptr);
    }

    for (auto& shaderModule : shaderModules)
    {
        vkDestroyShaderModule(device, shaderModule, nullptr);
    }
    vkDestroyImageView(device, depthStencil.view, nullptr);
    vkDestroyImage(device, depthStencil.image, nullptr);
    vkFreeMemory(device, depthStencil.mem, nullptr);

    vkDestroyPipelineCache(device, pipelineCache, nullptr);

    vkDestroyCommandPool(device, cmdPool, nullptr);

    vkDestroySemaphore(device, semaphores.presentDone, nullptr);
    vkDestroySemaphore(device, semaphores.renderDone, nullptr);

    vkDestroyDevice(device, nullptr);

    if (m_tenableValidation)
    {
        vkDebug::freeDebugCallback(instance);
    }

    vkDestroyInstance(instance, nullptr);
}

bool GPU_Vulkan::memory_type_from_properties(uint32_t typeBits, VkFlags requirements_mask, uint32_t* typeIndex)
{
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((typeBits & 1) == 1)
        {
            // Type is available, does it match user properties?
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask)
            {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

void GPU_Vulkan::set_image_layout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout, VkImageLayout new_image_layout)
{
    /* DEPENDS on info.cmd and info.queue initialized */

    if (postPresentCmdBuffer == VK_NULL_HANDLE)
        fprintf(stderr, "\n[Vulkan Error] postPresentCmdBuffer = VK_NULL_HANDLE\n");
    assert(postPresentCmdBuffer != VK_NULL_HANDLE);
    if (queue == VK_NULL_HANDLE)
        fprintf(stderr, "\n[Vulkan Error] queue = VK_NULL_HANDLE\n");
    assert(queue != VK_NULL_HANDLE);

    VkImageMemoryBarrier image_memory_barrier            = {};
    image_memory_barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_memory_barrier.pNext                           = NULL;
    image_memory_barrier.srcAccessMask                   = 0;
    image_memory_barrier.dstAccessMask                   = 0;
    image_memory_barrier.oldLayout                       = old_image_layout;
    image_memory_barrier.newLayout                       = new_image_layout;
    image_memory_barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    image_memory_barrier.image                           = image;
    image_memory_barrier.subresourceRange.aspectMask     = aspectMask;
    image_memory_barrier.subresourceRange.baseMipLevel   = 0;
    image_memory_barrier.subresourceRange.levelCount     = 1;
    image_memory_barrier.subresourceRange.baseArrayLayer = 0;
    image_memory_barrier.subresourceRange.layerCount     = 1;

    if (old_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    }

    if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    if (old_image_layout == VK_IMAGE_LAYOUT_PREINITIALIZED)
    {
        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
    }

    if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    }

    if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    VkPipelineStageFlags src_stages  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    vkCmdPipelineBarrier(postPresentCmdBuffer, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
}

void GPU_Vulkan::write(const CMP_Texture* pDestTexture)
{
    VkResult res;

    VkImageCreateInfo image_create_info     = {};
    image_create_info.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.pNext                 = NULL;
    image_create_info.imageType             = VK_IMAGE_TYPE_2D;
    image_create_info.format                = swapChain.colorFormat;
    image_create_info.extent.width          = m_width;
    image_create_info.extent.height         = m_height;
    image_create_info.extent.depth          = 1;
    image_create_info.mipLevels             = 1;
    image_create_info.arrayLayers           = 1;
    image_create_info.samples               = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling                = VK_IMAGE_TILING_LINEAR;
    image_create_info.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage                 = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    image_create_info.queueFamilyIndexCount = 0;
    image_create_info.pQueueFamilyIndices   = NULL;
    image_create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.flags                 = 0;

    VkMemoryAllocateInfo mem_alloc = {};
    mem_alloc.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc.pNext                = NULL;
    mem_alloc.allocationSize       = 0;
    mem_alloc.memoryTypeIndex      = 0;

    VkImage        mappableImage;
    VkDeviceMemory mappableMemory;

    //Create a mappable image
    res = vkCreateImage(device, &image_create_info, NULL, &mappableImage);
    if (res != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkCreateImage = %d\n", res);
    assert(res == VK_SUCCESS);

    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(device, mappableImage, &mem_reqs);

    mem_alloc.allocationSize = mem_reqs.size;

    //Find the memory type that is host mappable
    bool pass = memory_type_from_properties(
        mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &mem_alloc.memoryTypeIndex);

    assert(pass && "No mappable, coherent memory");

    //allocate memory
    res = vkAllocateMemory(device, &mem_alloc, NULL, &(mappableMemory));
    if (res != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkAllocateMemory = %d\n", res);
    assert(res == VK_SUCCESS);

    //bind memory
    res = vkBindImageMemory(device, mappableImage, mappableMemory, 0);
    if (res != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkBindImageMemory = %d\n", res);
    assert(res == VK_SUCCESS);

    VkCommandBuffer copyCmd = createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    VkImageMemoryBarrier imageMemoryBarrier = vkTools::initializers::imageMemoryBarrier();

    // Transition destination image to transfer destination layout
    vkTools::insertImageMemoryBarrier(copyCmd,
                                      mappableImage,
                                      0,
                                      VK_ACCESS_TRANSFER_WRITE_BIT,
                                      VK_IMAGE_LAYOUT_UNDEFINED,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

    // Transition swapchain image from present to transfer source layout
    vkTools::insertImageMemoryBarrier(copyCmd,
                                      swapChain.buffers[currentBuffer].image,
                                      VK_ACCESS_MEMORY_READ_BIT,
                                      VK_ACCESS_TRANSFER_READ_BIT,
                                      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

    VkImageCopy copy_region;
    copy_region.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_region.srcSubresource.mipLevel       = 0;
    copy_region.srcSubresource.baseArrayLayer = 0;
    copy_region.srcSubresource.layerCount     = 1;
    copy_region.srcOffset.x                   = 0;
    copy_region.srcOffset.y                   = 0;
    copy_region.srcOffset.z                   = 0;
    copy_region.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_region.dstSubresource.mipLevel       = 0;
    copy_region.dstSubresource.baseArrayLayer = 0;
    copy_region.dstSubresource.layerCount     = 1;
    copy_region.dstOffset.x                   = 0;
    copy_region.dstOffset.y                   = 0;
    copy_region.dstOffset.z                   = 0;
    copy_region.extent.width                  = m_width;
    copy_region.extent.height                 = m_height;
    copy_region.extent.depth                  = 1;

    //issue copy command
    vkCmdCopyImage(copyCmd,
                   swapChain.buffers[currentBuffer].image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   mappableImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &copy_region);

    // Transition destination image to general layout, which is the required layout for mapping the image memory later on
    vkTools::insertImageMemoryBarrier(copyCmd,
                                      mappableImage,
                                      VK_ACCESS_TRANSFER_WRITE_BIT,
                                      VK_ACCESS_MEMORY_READ_BIT,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                      VK_IMAGE_LAYOUT_GENERAL,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

    // Transition back the swap chain image
    vkTools::insertImageMemoryBarrier(copyCmd,
                                      swapChain.buffers[currentBuffer].image,
                                      VK_ACCESS_TRANSFER_READ_BIT,
                                      VK_ACCESS_MEMORY_READ_BIT,
                                      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      VK_PIPELINE_STAGE_TRANSFER_BIT,
                                      VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1});

    flushCommandBuffer(copyCmd, queue);

    VkImageSubresource subres = {};
    subres.aspectMask         = VK_IMAGE_ASPECT_COLOR_BIT;
    subres.mipLevel           = 0;
    subres.arrayLayer         = 0;
    VkSubresourceLayout sr_layout;
    vkGetImageSubresourceLayout(device, mappableImage, &subres, &sr_layout);

    char* ptr;
    res = vkMapMemory(device, mappableMemory, 0, pDestTexture->dwDataSize, 0, (void**)&ptr);
    if (res != VK_SUCCESS)
        fprintf(stderr, "\n[Vulkan Error] vkMapMemory = %d\n", res);
    assert(res == VK_SUCCESS);

    ptr += sr_layout.offset;
    int* pData = (int*)pDestTexture->pData;

    size_t x, y;

    for (y = 0; y < m_height; y++)
    {
        const int* row = (const int*)ptr;
        int        swapped;

        if (swapChain.colorFormat == VK_FORMAT_B8G8R8A8_UNORM || swapChain.colorFormat == VK_FORMAT_B8G8R8A8_SRGB)
        {
            for (x = 0; x < m_width; x++)
            {
                swapped = (*row & 0xff00ff00) | (*row & 0x000000ff) << 16 | (*row & 0x00ff0000) >> 16;
                *pData  = *row;
                row++;
                pData++;
            }
        }
        else if (swapChain.colorFormat == VK_FORMAT_R8G8B8A8_UNORM)
        {
            // change uint to sint buffer
            if (pDestTexture->format == CMP_FORMAT_RGBA_8888_S)
            {
                union _sdata
                {
                    char sb[4];
                    int  data;
                } sdata;

                union _udata
                {
                    char ub[4];
                    int  data;
                } udata;

                for (x = 0; x < m_width; x++)
                {
                     udata.data = *row;
                     sdata.sb[0] = udata.ub[0] - 127;
                     sdata.sb[1] = udata.ub[1] - 127;
                     sdata.sb[2] = udata.ub[2] - 127;
                     sdata.sb[3] = udata.ub[3] - 127;
                     *pData      = sdata.data;

                    row++;
                    pData++;
                }
            }
            else
            for (x = 0; x < m_width; x++)
            {
                *pData = *row;
                row++;
                pData++;
            }
        }
        else if (swapChain.colorFormat == VK_FORMAT_R16G16B16A16_SFLOAT)
        {
            for (x = 0; x < m_width; x++)
            {
                //only "int" type work, use int type and copy twice for this case
                *pData = *row;
                row++;
                pData++;
                *pData = *row;
                row++;
                pData++;
            }
        }
        else
        {
            //printf("Unrecognized image format - will not write image files");
            break;
        }

        ptr += sr_layout.rowPitch;
    }

    vkUnmapMemory(device, mappableMemory);
    vkFreeMemory(device, mappableMemory, NULL);
    vkDestroyImage(device, mappableImage, NULL);
}

bool GPU_Vulkan::isSupportVersion()
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    uint32_t driverVersion = deviceProperties.driverVersion;
    uint32_t major         = VK_VERSION_MAJOR(driverVersion);
    uint32_t minor         = VK_VERSION_MINOR(driverVersion);
    if (major > 1 || minor > 6)
        return false;
    else
        return true;
}

bool GPU_Vulkan::isSupportASTC()
{
    VkPhysicalDeviceFeatures deviceFeature;
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeature);

    if (deviceFeature.textureCompressionASTC_LDR == VK_FALSE)
        return false;
    else
        return true;
}

//--------------------------------------------------------------------------------------
#pragma warning(suppress : 6262)

CMP_ERROR WINAPI GPU_Vulkan::Decompress(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture)
{
    assert(pSourceTexture);

    CMP_ERROR cmp_res;

    m_pSourceTexture = pSourceTexture;
    m_width          = pSourceTexture->dwWidth;
    m_height         = pSourceTexture->dwHeight;

    if (!initVulkan(m_tenableValidation))
        return CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB;

    // Check for device feature for ASTC decode only
    if (pSourceTexture->format == CMP_FORMAT_ASTC)
    {
        if (!isSupportASTC())
        {
            m_initOk = false;
            return CMP_ERR_UNSUPPORTED_GPU_ASTC_DECODE;
        }
    }

    swapChain.initSurface(m_hInstance, m_hWnd, pDestTexture->format);

    cmp_res = prepare(pSourceTexture);

    if (cmp_res != CMP_OK)
    {
        swapChain.cleanup();
        return (cmp_res);
    }

#ifdef SHOW_WINDOW
    // Activate the window -- for debug view purpose
    ShowWindow(m_hWnd, SW_SHOW);
#endif
    //  Wait in Main message loop, until render is complete!!
    //  then exit

    MSG msg = {0};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            VkRender();
            write(pDestTexture);
            break;
        }
    }

    return CMP_OK;
}
