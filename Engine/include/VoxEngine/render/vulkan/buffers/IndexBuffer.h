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

    constexpr int IndexTypeSize(VkIndexType type) {
        switch(type) {
            case VK_INDEX_TYPE_NONE_KHR: case VK_INDEX_TYPE_MAX_ENUM: return 0;

            case VK_INDEX_TYPE_UINT32: return 4;
            case VK_INDEX_TYPE_UINT16: return 2;
            case VK_INDEX_TYPE_UINT8: return 1;
        }
        return 0;
    }

    class IndexBuffer : public RenderBuffer{
        friend class LogicalDevice;
        VkIndexType mIndexType;
        int mCount;

    private:
        static IndexBuffer Create(const LogicalDevice& device, VkDeviceSize size, VkIndexType indexType);
        IndexBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo &allocInfo, VkIndexType indexType, int count);

    public:
        bool isTransferDst() override;
        void bind(const CommandBuffer &cmdBuffer) const;
        VkIndexType getIndexType() const;
        int getCount() const;
    };
NS_END