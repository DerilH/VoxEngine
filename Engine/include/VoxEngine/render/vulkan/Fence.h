//
// Created by deril on 2/16/26.
//

#pragma once
#include <vulkan/vulkan_core.h>
#include "VulkanObject.h"
#include "VoxCore/containers/Buffer.h"
VULKAN_NS
    class LogicalDevice;

    class Fence : public VulkanObject<VkFence> {
        friend class LogicalDevice;

        explicit Fence(VkFence fence);
        static Fence Create(const LogicalDevice& device);
public:
        inline static void wait(const LogicalDevice& device, const Buffer<VkFence>&& wait, VkBool32 waitCount = VK_TRUE, uint64_t timeout = 0);
    };

NS_END
