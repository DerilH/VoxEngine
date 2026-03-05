//
// Created by deril on 2/14/26.
//

#pragma once
#include <vulkan/vulkan_core.h>
#include "QueueFamilyRepository.h"
#include "VulkanCommandBuffer.h"
#include "VulkanObject.h"
#include "VoxEngine/render/CommandPool.h"

VULKAN_NS
    class VulkanDevice;
    class Queue;

    class VulkanCommandPool : public CommandPool, public VulkanObject<VkCommandPool > {
        friend class VulkanDevice;
        const VulkanDevice& mDevice;
        CommandBuffer* mOneTimeBuffer = nullptr;

        VulkanCommandPool(VkCommandPool handle, const VulkanDevice &device);
        static VulkanCommandPool Create(const VulkanDevice &device, const QueueFamily &family);
    public:
        CommandBufferRef allocBuffer() override;
    };
NS_END
