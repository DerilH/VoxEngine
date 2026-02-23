//
// Created by deril on 2/18/26.
//

#include "VoxEngine/render/vulkan/StagingBuffer.h"

#include "VoxEngine/render/vulkan/LogicalDevice.h"

VULKAN_NS
    StagingBuffer StagingBuffer::Create(const LogicalDevice &device, VkDeviceSize size) {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferCreateInfo.size = size;
        VmaAllocation allocation{};

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
        VmaAllocationInfo info{};
        const VkBuffer buffer = AllocateBuffer(device, bufferCreateInfo, allocCreateInfo, true, allocation, info);

        return StagingBuffer(buffer, allocation, info);
    }

    StagingBuffer::StagingBuffer(const VkBuffer buffer, const VmaAllocation alloc, const VmaAllocationInfo &allocInfo) : RenderBuffer(buffer, alloc, allocInfo) {
        VOX_ASSERT_PTR(allocInfo.pMappedData, "buffer not mapped")
    }

    void StagingBuffer::write(void *data, size_t size) {
        memcpy(mAllocInfo.pMappedData,data, size);
    }

    void StagingBuffer::copy(const VkCommandBuffer &cmdBuffer, const RenderBuffer &buffer, VkDeviceSize size) {
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(cmdBuffer, mHandle, buffer.getHandle(), 1, &copyRegion);
    }

    bool StagingBuffer::isTransferDst() {
        return false;
    }

NS_END
