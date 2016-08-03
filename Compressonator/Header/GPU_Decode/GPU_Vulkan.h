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
/// \file GPU_Vulkan.h
//
//=====================================================================



#ifndef H_GPU_VULKAN
#define H_GPU_VULKAN


#include "GPU_DecodeBase.h"

#include <vulkan/vulkan.h>
#include <glm/gtc/matrix_transform.hpp>
#include "vulkanswapchain.hpp"
#include "vulkandebug.h"
#include <assert.h>
#include <string>
#include <array>

using namespace std;

#define FENCE_TIMEOUT 100000000
//---------------------------------------------------------------------------------
namespace GPU_Decode
{
 

    class GPU_Vulkan : public RenderWindow
    {
    public:
        GPU_Vulkan(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback);
        ~GPU_Vulkan();

        virtual int WINAPI Decompress(
            const CMP_Texture* pSourceTexture,
            CMP_Texture* pDestTexture
            );

    private:
        void VkRender();
        VkFormat MIP2VK_Format(const CMP_Texture* pSourceTexture);

        // Set to true if want to enable validation layers
        bool        m_tenableValidation = false;
        bool        m_prepared          = false;
        float       m_zoom              = -2.0f;
        uint32_t    m_width;
        uint32_t    m_height;

        VulkanSwapChain swapChain;

        vkTools::UniformData uniformDataVS;

        // Active frame buffer index
        uint32_t currentBuffer = 0;
        // Handle to the device graphics queue that command buffers are submitted to
        VkQueue queue;
        // Color buffer format
        VkFormat colorformat = VK_FORMAT_B8G8R8A8_UNORM;
        // Depth format is selected during Vulkan initialization
        VkFormat depthFormat;
        // Physical device (GPU) that Vulkan will ise
        VkPhysicalDevice physicalDevice;
        // Stores all available memory (type) properties for the physical device
        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        // Logical device, application's view of the physical device (GPU)
        VkDevice device;
        // Vulkan instance, stores all per-application states
        VkInstance instance;
        // Command buffer pool
        VkCommandPool cmdPool;
        // Command buffer used for setup
        VkCommandBuffer setupCmdBuffer = VK_NULL_HANDLE;
        // Command buffer for submitting a post present image barrier
        VkCommandBuffer postPresentCmdBuffer = VK_NULL_HANDLE;
        // Command buffer for submitting a pre present image barrier
        VkCommandBuffer prePresentCmdBuffer = VK_NULL_HANDLE;
        // Pipeline stage flags for the submit info structure
        VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        // Contains command buffers and semaphores to be presented to the queue
        VkSubmitInfo submitInfo;
        // Pipeline cache object
        VkPipelineCache pipelineCache;
        // Global render pass for frame buffer writes
        VkRenderPass renderPass;
        // Pipeline layout and descriptors
        VkPipelineLayout pipelineLayout;
        VkDescriptorSet descriptorSet;
        VkDescriptorSetLayout descriptorSetLayout;
        // List of available frame buffers (same as number of swap chain images)
        std::vector<VkFramebuffer>frameBuffers;
        // Command buffers used for rendering
        std::vector<VkCommandBuffer> drawCmdBuffers;
        // Descriptor set pool
        VkDescriptorPool descriptorPool;
        // List of shader modules created (stored for cleanup)
        std::vector<VkShaderModule> shaderModules;

        // List of functions used for loading compressed texture and decode view in compressonator gui
        VkResult createInstance(bool enableValidation);
        VkResult createDevice(VkDeviceQueueCreateInfo requestedQueues, bool enableValidation);
        void initVulkan(bool enableValidation);
        void submitPostPresentBarrier(VkImage image);
        void submitPrePresentBarrier(VkImage image);
        void draw();
        void createCommandPool();
        void createSetupCommandBuffer();
        void createCommandBuffers();
        VkBool32 getMemoryType(uint32_t typeBits, VkFlags properties, uint32_t * typeIndex);
        void setupDepthStencil();
        void setupRenderPass();
        void createPipelineCache();
        void setupFrameBuffer();
        void flushSetupCommandBuffer();
        VkBool32 createBuffer(VkBufferUsageFlags usage, VkDeviceSize size, void * data, VkBuffer *buffer, VkDeviceMemory *memory);
        VkBool32 createBuffer(VkBufferUsageFlags usage, VkDeviceSize size, void * data, VkBuffer * buffer, VkDeviceMemory * memory, VkDescriptorBufferInfo * descriptor);
        void updateUniformBuffers();
        void setImageLayout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout);
        void loadTexture(const CMP_Texture* pSourceTexture);
        VkPipelineShaderStageCreateInfo loadShader(const char * fileName, VkShaderStageFlagBits stage);
        void prepare(const CMP_Texture* pSourceTexture);
        void destroyCommandBuffers();
        void clean();
        bool memory_type_from_properties(uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
        void set_image_layout(VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout, VkImageLayout new_image_layout);
        void write(const CMP_Texture* pDestTexture);
    };
}

#endif
