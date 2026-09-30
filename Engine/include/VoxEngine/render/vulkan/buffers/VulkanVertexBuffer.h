//
// Created by deril on 2/18/26.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include "VoxEngine/render/vulkan/VulkanCommandBuffer.h"
#include "VoxEngine/render/buffers/VertexBuffer.h"
#include "VulkanRenderBuffer.h"

VULKAN_NS
    class VulkanDevice;

    class VulkanVertexBuffer : public VulkanRenderBuffer, public VertexBuffer {
        friend class VulkanDevice;
        static VulkanVertexBuffer Create(const VulkanDevice& device, VkDeviceSize size);
        VulkanVertexBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo &allocInfo);
    };
NS_END