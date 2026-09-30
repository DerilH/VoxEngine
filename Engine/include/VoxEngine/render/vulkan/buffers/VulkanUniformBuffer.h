//
// Created by deril on 2/18/26.
//

#pragma once


#include "VoxEngine/render/buffers/UniformBuffer.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "VulkanRenderBuffer.h"

VULKAN_NS
    class VulkanDevice;

    class VulkanUniformBuffer : public VulkanRenderBuffer, public UniformBuffer {
        friend class VulkanDevice;

    private:
        static VulkanUniformBuffer Create(const VulkanDevice& device, VkDeviceSize size);

        VulkanUniformBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo& allocInfo);
    };
NS_END