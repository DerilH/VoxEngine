//
// Created by deril on 2/18/26.
//

#pragma once
#include "VoxEngine/render/vulkan/buffers/RenderBuffer.h"

VULKAN_NS
    class LogicalDevice;

    class StagingBuffer : public RenderBuffer {
        friend class LogicalDevice;

        static StagingBuffer Create(const LogicalDevice &device, VkDeviceSize size);

        NO_COPY_MOVE_DEFAULT(StagingBuffer)

        explicit StagingBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo &allocInfo);
    public:
        void write(void* data, size_t size);
        void copy(const VkCommandBuffer &cmdBuffer, const RenderBuffer &buffer, VkDeviceSize size);

        bool isTransferDst() override;
    };

NS_END
