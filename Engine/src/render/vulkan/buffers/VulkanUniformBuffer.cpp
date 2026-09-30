//
// Created by deril on 3/22/26.
//

#include <VoxEngine/render/vulkan/buffers/VulkanUniformBuffer.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>

VULKAN_NS
    VulkanUniformBuffer VulkanUniformBuffer::Create(const VulkanDevice& device, VkDeviceSize size) {

        VkBufferCreateInfo bufferCreateInfo{};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferCreateInfo.size = size;
        VmaAllocation allocation{};

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
        allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
        allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        VmaAllocationInfo info{};
        VkBuffer buffer = device.allocateBuffer(bufferCreateInfo, allocCreateInfo, true, allocation, info);
        return {buffer, allocation, info};
    }

    VulkanUniformBuffer::VulkanUniformBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo& allocInfo) : RenderBuffer(BufferUsage::UNIFORM), VulkanRenderBuffer(buffer, alloc, allocInfo), UniformBuffer(UniformBinding(0,0)) {
    }

    void VulkanUniformBuffer::write(const void* data, uint32_t size) {
        void* mappedData = mAllocInfo.pMappedData;
        if (mappedData) {
            memcpy(mappedData, data, std::min((uint32_t)mAllocInfo.size, size));
        }
    }

NS_END
