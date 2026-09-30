//
// Created by deril on 2/18/26.
//

#pragma once

#include <vk_mem_alloc.h>
#include "VoxEngine/render/buffers/RenderBuffer.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "VoxEngine/render/buffers/TransferBuffer.h"
#include "VulkanRenderBuffer.h"

VULKAN_NS
    class VulkanDevice;

    class VulkanTransferBuffer : public VulkanRenderBuffer, public TransferBuffer {
        friend class VulkanDevice;

        static VulkanTransferBuffer Create(const VulkanDevice &device, VkDeviceSize size);

        NO_COPY_MOVE_DEFAULT(VulkanTransferBuffer)

        explicit VulkanTransferBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo &allocInfo);
    public:
        void write(const void* data, size_t size);
        void copy(const CommandBufferRef cmdBuffer, const RenderBufferRef buffer, VkDeviceSize size);
    };

NS_END
