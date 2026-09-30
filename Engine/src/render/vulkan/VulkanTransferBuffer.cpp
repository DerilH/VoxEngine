//
// Created by deril on 2/18/26.
//

#include "VoxEngine/render/vulkan/buffers/VulkanTransferBuffer.h"

#include "VoxEngine/render/vulkan/VulkanDevice.h"
#include "VoxEngine/render/vulkan/VulkanResourceCast.h"

VULKAN_NS
    VulkanTransferBuffer VulkanTransferBuffer::Create(const VulkanDevice& device, VkDeviceSize size) {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.size = size;
        VmaAllocation allocation{};

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
        allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        VmaAllocationInfo info{};
        const VkBuffer buffer = device.allocateBuffer(bufferCreateInfo, allocCreateInfo, true, allocation, info);

        return VulkanTransferBuffer(buffer, allocation, info);
    }

    VulkanTransferBuffer::VulkanTransferBuffer(const VkBuffer buffer, const VmaAllocation alloc, const VmaAllocationInfo& allocInfo) : RenderBuffer(BufferUsage::TRANSFER_SRC), VulkanRenderBuffer(buffer, alloc, allocInfo), TransferBuffer() {
        VOX_ASSERT_PTR(allocInfo.pMappedData, "buffer not mapped")
    }

    void VulkanTransferBuffer::write(const void* data, size_t size) {
        memcpy(mAllocInfo.pMappedData, data, size);
    }

    void VulkanTransferBuffer::copy(const CommandBufferRef cmdBuffer, const RenderBufferRef buffer, VkDeviceSize size) {
        VOX_ASSERT(buffer->sizeInBytes() >= size, "Size of copy region is more than dst buffer size")
        auto dstBuffer = ResourceCast(buffer);

        VkBufferCopy copyRegion{};
        copyRegion.dstOffset = 0;
        copyRegion.srcOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(*ResourceCast(cmdBuffer), mHandle, *dstBuffer, 1, &copyRegion);
    }

NS_END
