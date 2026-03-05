//
// Created by deril on 2/23/26.
//

#include "VoxEngine/render/vulkan/VulkanCommandBuffer.h"

VULKAN_NS
//    VulkanCommandBuffer::CommandBuffer(VkCommandBuffer handle) : VulkanObject(handle) {
//        VOX_ASSERT_PTR(handle, "Command buffer is nullptr")
//    }
//
//    void VulkanCommandBuffer::reset(VkCommandBufferResetFlags flags) {
//        VK_CHECK(vkResetCommandBuffer(mHandle, flags), "Cannot reset command buffer");
//    }
//
//    void VulkanCommandBuffer::begin(VkCommandBufferUsageFlags flags) {
//        VkCommandBufferBeginInfo beginInfo{};
//
//        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//        VK_CHECK(vkBeginCommandBuffer(mHandle, &beginInfo), "Cannot begin command buffer");
//    }
//
//    void VulkanCommandBuffer::end() {
//        VK_CHECK(vkEndCommandBuffer(mHandle), "Cannot end command buffer");
//    }

NS_END
