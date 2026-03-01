//
// Created by deril on 2/18/26.
//

#pragma once
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>
#include "VoxEngine/render/vulkan/VulkanObject.h"

VULKAN_NS
    class LogicalDevice;

    class RenderBuffer : public VulkanObject<VkBuffer>{
    protected:
        VmaAllocation mAllocation;
        VmaAllocationInfo mAllocInfo;

        RenderBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo &allocInfo);

        static VkBuffer AllocateBuffer(const LogicalDevice &device, VkBufferCreateInfo &bufferCreateInfo, const VmaAllocationCreateInfo &allocInfo, bool exclusive, VmaAllocation &allocation, VmaAllocationInfo &info);

        NO_COPY_MOVE_DEFAULT(RenderBuffer)

    public:
        VmaAllocation getAllocation() const;
        virtual bool isTransferDst() = 0;
        VkDeviceSize getSize() const;
    };

NS_END
