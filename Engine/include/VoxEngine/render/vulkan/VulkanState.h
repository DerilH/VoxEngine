#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>
#include <VoxEngine/render/shaders/ShaderRepository.h>
#include <VoxEngine/render/vulkan/Surface.h>
#include <VoxEngine/render/vulkan/LogicalDevice.h>
#include "VoxEngine/render/vulkan/buffers/VertexBuffer.h"
#include "VoxEngine/render/vulkan/buffers/IndexBuffer.h"
#include "Vertex.h"

namespace Vox::Render::Vulkan {
    class VulkanState {
    public:
        Shader::ShaderRepository mShaderRepository;

        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

        PhysicalDevice *physicalDevice = VK_NULL_HANDLE;
        std::vector<PhysicalDevice> physicalDevices;

        GraphicsPipeline* pipeline = nullptr;

        RenderPass* renderPass = VK_NULL_HANDLE;

        LogicalDevice *device = VK_NULL_HANDLE;

        VertexBuffer* quadBuffer = VK_NULL_HANDLE;
        IndexBuffer* indexBuffer;
        const int mApiVersion;
        NON_COPYABLE_NON_MOVABLE(VulkanState)

        void createInstance();

        VmaAllocator createAllocator(const LogicalDevice &device) const;

        VertexBuffer* createVertexBuffer(std::vector<Vertex> vertices);
        IndexBuffer* createIndexBuffer(std::vector<uint32_t> indices);


        void recordCommandBuffer(VkCommandBuffer commandBuffer, VkExtent2D extent, VkFramebuffer framebuffer, RenderPass pass, std::function<void()> misc);

        void cleanup();

        static VulkanState *Get();

        static void initialize(int apiVersion, const Shader::ShaderRepository &shaders);
    private:
        void init();
        static VulkanState* sInstance;

        explicit VulkanState(int apiVersion, const Shader::ShaderRepository &repository);
    };
}
