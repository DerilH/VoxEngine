//
// Created by deril on 2/18/26.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include "RenderBuffer.h"
#include "VoxEngine/render/vulkan/VulkanCommandBuffer.h"
#include <vector>
VULKAN_NS
    class VulkanDevice;

    class VertexBuffer : public RenderBuffer{
        friend class VulkanDevice;
        static VertexBuffer Create(const VulkanDevice& device, VkDeviceSize size);
        VertexBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo &allocInfo);

    public:
        bool isTransferDst() override;
        void bind(const CommandBuffer &cmdBuffer) const;
    };
NS_END