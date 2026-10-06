//
// Created by deril on 3/1/26.
//

#pragma once

#include <vk_mem_alloc.h>
#include "VoxEngine/render/RenderBackend.h"
#include "VoxCore/render/Enums.h"
#include "VulkanDevice.h"

VULKAN_NS
    class VulkanBackend : public RenderBackend {
        VkInstance mInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;

        void createInstance();

        void init() override;

        explicit VulkanBackend(RenderAPI api);

        explicit VulkanBackend() = delete;

    public:
        virtual ~VulkanBackend() = default;

        int32_t beginFrame() override;

        void endFrame() override;

        RenderTargetRef createWindowTarget(Ref<Window> window) const override;
        RenderTargetRef createTextureTarget(Extent extent, Format format) const override;
        CommandPoolRef createCommandPool() override;

        TextureRef createTexture(Format format, Extent extent) const override;

        PipelineStateRef createPSO(const PipelineStateDesc& desc) override;

        IndexBufferRef createIndexBuffer(const void* data, uint32_t size, IndexType type) override;

        VertexBufferRef createVertexBuffer(const void* data, uint32_t size, BufferUsage usage) override;
        UniformBufferRef createUniformBuffer(uint32_t size) override;

        VmaAllocator createAllocator(const VulkanDevice& device);

        inline VkInstance getVkInstance() const { return mInstance; }

        static RenderBackend* Create() {
            return new VulkanBackend(RenderAPI::VULKAN_API);
        }

        inline bool isInitialized() const {
            return mInstance != nullptr;
        }
    };

NS_END
