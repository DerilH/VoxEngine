//
// Created by deril on 3/22/26.
//

#pragma once


#include <VoxCore/containers/ArrayView.h>
#include <vulkan/vulkan_core.h>
#include "VoxCore/Define.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"

VULKAN_NS
class VulkanDevice;
class VulkanDescriptorPool : public VulkanObject<VkDescriptorPool> {
    friend class VulkanDevice;
    VulkanDescriptorPool(VkDescriptorPool handle);
public:
    static VulkanDescriptorPool Create(const VulkanDevice& device, uint32_t maxSets, ArrayView<VkDescriptorPoolSize> sizes);
    NO_COPY_MOVE_DEFAULT(VulkanDescriptorPool);
};
NS_END
