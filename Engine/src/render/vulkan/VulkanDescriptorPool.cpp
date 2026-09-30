//
// Created by deril on 3/22/26.
//
#include <VoxEngine/render/vulkan/VulkanDescriptorPool.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include <VoxEngine/render/vulkan/VulkanUtil.h>

VULKAN_NS
    VulkanDescriptorPool VulkanDescriptorPool::Create(const VulkanDevice& device, const uint32_t maxSets, const ArrayView<VkDescriptorPoolSize> sizes) {
        VkDescriptorPoolCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.maxSets = maxSets;
        createInfo.poolSizeCount = sizes.size();
        createInfo.pPoolSizes = sizes.pData;

        VkDescriptorPool pool;
        VK_CHECK(vkCreateDescriptorPool(device, &createInfo, nullptr, &pool), "Cannot create descriptor pool");

        return {pool};
    }

    VulkanDescriptorPool::VulkanDescriptorPool(VkDescriptorPool handle) : VulkanObject(handle) {
    }
NS_END