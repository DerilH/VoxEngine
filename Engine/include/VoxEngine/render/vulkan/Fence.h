//
// Created by deril on 2/16/26.
//

#pragma once
#include <vulkan/vulkan_core.h>
#include "VulkanObject.h"

VULKAN_NS
    class LogicalDevice;

    class Fence : public VulkanObject<VkFence> {
        friend class LogicalDevice;

        explicit Fence(VkFence fence);
        static Fence Create(const LogicalDevice& device);
public:
    };

NS_END
