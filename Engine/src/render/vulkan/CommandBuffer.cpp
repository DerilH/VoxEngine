//
// Created by deril on 2/23/26.
//

#include "VoxEngine/render/vulkan/CommandBuffer.h"

VULKAN_NS
    CommandBuffer::CommandBuffer(VkCommandBuffer handle) : VulkanObject(handle) {
        VOX_ASSERT_PTR(handle, "Command buffer is nullptr")
    }

    void CommandBuffer::reset(VkCommandBufferResetFlags flags) {
        VK_CHECK(vkResetCommandBuffer(mHandle, flags), "Cannot reset command buffer");
    }

    void CommandBuffer::begin(VkCommandBufferUsageFlags flags) {
        VkCommandBufferBeginInfo beginInfo{};

        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VK_CHECK(vkBeginCommandBuffer(mHandle, &beginInfo), "Cannot begin command buffer");
    }

    void CommandBuffer::end() {
        VK_CHECK(vkEndCommandBuffer(mHandle), "Cannot end command buffer");
    }

NS_END
