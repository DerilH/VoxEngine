//
// Created by deril on 3/22/26.
//

#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/containers/ArrayView.h>

#include "VulkanObject.h"
#include "VulkanTypes.h"

VULKAN_NS
    class VulkanDevice;
    class VulkanUniformBuffer;

    class VulkanDescriptorSet : public VulkanObject<VkDescriptorSet> {
        friend class VulkanDevice;
        VkDescriptorSetLayout mLayout;

        static VulkanDescriptorSet Create(const VulkanDevice& device, VulkanDescriptorPoolRef pool, VkDescriptorSetLayout layout);
        VulkanDescriptorSet(VkDescriptorSet handle, VkDescriptorSetLayout layout) : VulkanObject(handle), mLayout(layout) {}
    public:
        NO_COPY_MOVE_DEFAULT(VulkanDescriptorSet);
        VkDescriptorSetLayout getLayouts() const;

        void update(const VulkanDevice& device, uint32_t binding, const VulkanUniformBuffer& buffer);
    };

NS_END
