//
// Created by deril on 2/18/26.
//

#include <VoxEngine/render/vulkan/buffers/VulkanIndexBuffer.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include "VoxEngine/render/vulkan/VulkanUtil.h"
#include <vk_mem_alloc.h>

VULKAN_NS
    VulkanIndexBuffer VulkanIndexBuffer::Create(const VulkanDevice& device, VkDeviceSize size, IndexType indexType) {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferCreateInfo.size = size;
        VmaAllocation allocation{};

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        VmaAllocationInfo info{};
        VkBuffer buffer = device.allocateBuffer(bufferCreateInfo, allocCreateInfo, true, allocation, info);
        uint32_t count = size / IndexTypeSize(indexType);

        return {buffer, allocation, info, indexType, count};
    }

    VulkanIndexBuffer::VulkanIndexBuffer(const VkBuffer buffer, const VmaAllocation alloc, const VmaAllocationInfo& allocInfo, IndexType indexType, uint32_t count) : RenderBuffer(BufferUsage::INDEX), IndexBuffer(indexType, count), VulkanRenderBuffer(buffer, alloc, allocInfo) {}
NS_END



