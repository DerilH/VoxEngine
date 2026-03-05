//
// Created by deril on 2/16/26.
//

#pragma once
#include <vulkan/vulkan_core.h>
#include "VulkanObject.h"
#include "VoxCore/containers/ArrayView.h"
VULKAN_NS
    class VulkanDevice;

    class Fence : public VulkanObject<VkFence> {
        friend class VulkanDevice;

        explicit Fence(VkFence fence);
        static Fence Create(const VulkanDevice& device);
public:
        inline static void wait(const VulkanDevice& device, const ArrayView<VkFence>&& wait, VkBool32 waitCount = VK_TRUE, uint64_t timeout = 0);
    };

NS_END
