//
// Created by deril on 2/18/26.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include "RenderBuffer.h"
#include "VoxEngine/render/vulkan/CommandBuffer.h"
#include <vector>
VULKAN_NS
    class LogicalDevice;

    class VertexBuffer : public RenderBuffer{
        friend class LogicalDevice;
        static VertexBuffer Create(const LogicalDevice& device, VkDeviceSize size);
        VertexBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo &allocInfo);

    public:
        bool isTransferDst() override;
        void bind(const CommandBuffer &cmdBuffer) const;
    };
NS_END