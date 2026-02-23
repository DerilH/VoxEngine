//
// Created by deril on 2/23/26.
//

#pragma once


#include "VulkanObject.h"

VULKAN_NS
    class CommandBuffer : public VulkanObject<VkCommandBuffer> {
    public:

        explicit CommandBuffer(VkCommandBuffer handle);

        void reset(VkCommandBufferResetFlags flags = 0);

        void begin(VkCommandBufferUsageFlags flags = 0);

        void end();
    };
NS_END
