//
// Created by deril on 2/18/26.
//

#pragma once


#include "VoxEngine/render/buffers/UniformBuffer.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "VoxEngine/render/vulkan/VulkanTypes.h"
#include "VulkanRenderBuffer.h"

VULKAN_NS
    class VulkanDevice;

    class VulkanUniformBuffer : public VulkanRenderBuffer, public UniformBuffer {
        friend class VulkanDevice;

    protected:
        VulkanDescriptorSetRef mDescriptorSet = nullptr;

        static VulkanUniformBuffer Create(const VulkanDevice& device, VkDeviceSize size);

        VulkanUniformBuffer(VkBuffer buffer, VmaAllocation alloc, const VmaAllocationInfo& allocInfo);

    public:
        void write(const void* data, uint32_t size) override;
        void* getInternalDescriptorSet() const override { return mDescriptorSet; }
        void setDescriptorSet(VulkanDescriptorSetRef set) { mDescriptorSet = set; }
    };
NS_END