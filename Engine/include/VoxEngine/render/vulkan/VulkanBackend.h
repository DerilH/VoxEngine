//
// Created by deril on 3/1/26.
//

#pragma once

#include <vk_mem_alloc.h>
#include "VoxEngine/render/RenderBackend.h"
#include "VoxEngine/render/Enums.h"
#include "VulkanDevice.h"

VULKAN_NS
    class VulkanBackend : public RenderBackend {
        VkInstance mInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;

        void createInstance();

        void init() override;

        explicit VulkanBackend() = default;

    public:

        void beginFrame() override;

        void endFrame() override;

        RenderTargetRef createWindowTarget(Extent extent, void* windowHandle) override;

        VmaAllocator createAllocator(const VulkanDevice& device);

        inline VkInstance getVkInstance() const { return mInstance; }

        inline static VulkanBackend* Get() {
            VOX_ASSERT(Initialized(), "Render backend not initialized");
            return (VulkanBackend*) RenderBackend::Get();
        }

        static RenderBackend* Create() {
            return new VulkanBackend();
        }
    };

NS_END
