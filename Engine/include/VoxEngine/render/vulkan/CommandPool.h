//
// Created by deril on 2/14/26.
//

#pragma once
#include <vulkan/vulkan_core.h>
#include "QueueFamilyRepository.h"
#include "CommandBuffer.h"
#include "VulkanObject.h"

VULKAN_NS
    class LogicalDevice;
    class Queue;

    class CommandPool : public VulkanObject<VkCommandPool > {
        friend class LogicalDevice;
        const LogicalDevice& mDevice;
        CommandBuffer* mOneTimeBuffer = nullptr;

        CommandPool(VkCommandPool handle, const LogicalDevice &device);
        static CommandPool Create(const LogicalDevice &device, const QueueFamily &family);
    public:
        CommandBuffer allocateBuffer() const;
        const CommandBuffer& startTemp();
        void submitTemp(const Queue &queue);
    };
NS_END
