//
// Created by deril on 3/21/26.
//

#pragma once


#include "VoxCore/Define.h"
#include "VoxEngine/render/buffers/RenderBuffer.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"

VULKAN_NS
class VulkanRenderBuffer : virtual public RenderBuffer, public VulkanAllocated<VkBuffer> {
protected:
    VulkanRenderBuffer(const VkBuffer buffer, const VmaAllocation alloc, const VmaAllocationInfo &allocInfo) : RenderBuffer(BufferUsage::VERTEX), VulkanAllocated<VkBuffer>(buffer, alloc, allocInfo) {}
public:
    inline uint32_t sizeInBytes() override {
        return mAllocInfo.size;
    }
};
NS_END
