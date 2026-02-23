//
// Created by deril on 2/18/26.
//

#include <VoxEngine/render/vulkan/buffers/VertexBuffer.h>
#include <vk_mem_alloc.h>
#include <initializer_list>
#include <cstdarg>
#include <vector>
#include "VoxEngine/render/vulkan/CommandBuffer.h"

VULKAN_NS
    VertexBuffer VertexBuffer::Create(const LogicalDevice &device, VkDeviceSize size) {
        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferCreateInfo.size = size;
        VmaAllocation allocation{};

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        VmaAllocationInfo info{};
        const VkBuffer buffer = AllocateBuffer(device, bufferCreateInfo, allocCreateInfo, true, allocation, info);

        return {buffer, allocation, info};
    }

    VertexBuffer::VertexBuffer(const VkBuffer buffer, const VmaAllocation alloc, const VmaAllocationInfo &allocInfo) : RenderBuffer(buffer, alloc, allocInfo) {}

    bool VertexBuffer::isTransferDst() {
            return true;
    }


    void VertexBuffer::bind(const CommandBuffer &cmdBuffer) const{
        VkDeviceSize offsets[] = {0};
        VkBuffer v[] = {mHandle};
        vkCmdBindVertexBuffers(cmdBuffer.getHandle(), 0, 1, v, offsets);
    }
NS_END


