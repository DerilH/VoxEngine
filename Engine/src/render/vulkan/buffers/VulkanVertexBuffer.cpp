//
// Created by deril on 2/18/26.
//

#include "VoxEngine/render/vulkan/VulkanCommandBuffer.h"
#include "VoxEngine/render/vulkan/buffers/VulkanVertexBuffer.h"
#include "VoxEngine/render/vulkan/VulkanDevice.h"

VULKAN_NS
    VulkanVertexBuffer VulkanVertexBuffer::Create(const VulkanDevice &device, VkDeviceSize size) {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferCreateInfo.size = size;
        VmaAllocation allocation{};

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        VmaAllocationInfo info{};
        VkBuffer buffer = device.allocateBuffer(bufferCreateInfo, allocCreateInfo, true, allocation, info);

        return {buffer, allocation, info};
    }

    VulkanVertexBuffer::VulkanVertexBuffer(const VkBuffer buffer, const VmaAllocation alloc, const VmaAllocationInfo &allocInfo) :RenderBuffer(BufferUsage::VERTEX), VertexBuffer(), VulkanRenderBuffer(buffer, alloc, allocInfo) {}

//    bool VertexBuffer::isTransferDst() {
//            return true;
//    }
//
//
//    void VertexBuffer::bind(const CommandBuffer &cmdBuffer) const{
//        VkDeviceSize offsets[] = {0};
//        VkBuffer v[] = {mHandle};
//        vkCmdBindVertexBuffers(cmdBuffer.getHandle(), 0, 1, v, offsets);
//    }
NS_END



