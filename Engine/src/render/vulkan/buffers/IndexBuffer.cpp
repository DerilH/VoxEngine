//
// Created by deril on 2/18/26.
//

#include <VoxEngine/render/vulkan/buffers/IndexBuffer.h>
#include <vk_mem_alloc.h>

VULKAN_NS
    IndexBuffer IndexBuffer::Create(const LogicalDevice &device, VkDeviceSize size, VkIndexType indexType) {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferCreateInfo.size = size;
        VmaAllocation allocation{};

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        VmaAllocationInfo info{};
        const VkBuffer buffer = AllocateBuffer(device, bufferCreateInfo, allocCreateInfo, true, allocation, info);
        int count = size / IndexTypeSize(indexType);

        return {buffer, allocation, info, indexType, count};
    }

    IndexBuffer::IndexBuffer(const VkBuffer buffer, const VmaAllocation alloc, const VmaAllocationInfo &allocInfo, VkIndexType indexType, int count) : RenderBuffer(buffer, alloc, allocInfo), mIndexType(indexType), mCount(count) {}

    bool IndexBuffer::isTransferDst() {
            return true;
    }


    void IndexBuffer::bind(const CommandBuffer &cmdBuffer) const {
        vkCmdBindIndexBuffer(cmdBuffer.getHandle(), mHandle, 0, mIndexType);
    }

    VkIndexType IndexBuffer::getIndexType() const{
        return mIndexType;
    }

    int IndexBuffer::getCount() const {
        return mCount;
    }
NS_END


