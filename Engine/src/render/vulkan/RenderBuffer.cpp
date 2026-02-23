//
// Created by deril on 2/18/26.
//
#include <vk_mem_alloc.h>
#include "VoxEngine/render/vulkan/buffers/RenderBuffer.h"
#include "VoxEngine/render/vulkan/LogicalDevice.h"

VULKAN_NS
    RenderBuffer::RenderBuffer(const VkBuffer buffer, const VmaAllocation alloc, const VmaAllocationInfo &allocInfo) : mHandle(buffer), mAllocation(alloc), mAllocInfo(std::move(allocInfo)) {
    }

    VkBuffer RenderBuffer::AllocateBuffer(const LogicalDevice &device, VkBufferCreateInfo &bufferCreateInfo, const VmaAllocationCreateInfo &allocInfo, const bool exclusive, VmaAllocation &allocation, VmaAllocationInfo &info) {
        bufferCreateInfo.sharingMode = exclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

        VkBuffer buffer = VK_NULL_HANDLE;
        VK_CHECK(vmaCreateBuffer(device.getAllocator(), &bufferCreateInfo, &allocInfo, &buffer, &allocation, &info), "failed to allocate buffers");
        return buffer;
    }

    VkBuffer RenderBuffer::getHandle() const {
        return mHandle;
    }

    VmaAllocation RenderBuffer::getAllocation() const {
        return mAllocation;
    }

    VkDeviceSize RenderBuffer::getSize() const{
        return mAllocInfo.size;
    }
NS_END
