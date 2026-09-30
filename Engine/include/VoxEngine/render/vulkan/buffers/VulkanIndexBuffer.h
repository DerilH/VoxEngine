//
// Created by deril on 2/18/26.
//

#pragma once


#include "VoxEngine/render/buffers/IndexBuffer.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "VulkanRenderBuffer.h"

VULKAN_NS
    class VulkanDevice;

    class VulkanIndexBuffer : public VulkanRenderBuffer, public IndexBuffer {
        friend class VulkanDevice;

    private:
        static VulkanIndexBuffer Create(const VulkanDevice& device, VkDeviceSize size, IndexType indexType);

        VulkanIndexBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo& allocInfo, IndexType indexType, uint32_t count);

    };
NS_END