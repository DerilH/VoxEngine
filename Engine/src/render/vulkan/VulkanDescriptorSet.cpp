#include <VoxCore/Assert.h>
#include <VoxCore/Define.h>
#include <VoxEngine/render/vulkan/VulkanDescriptorSet.h>
#include <VoxEngine/render/vulkan/VulkanTypes.h>
#include <VoxEngine/render/vulkan/VulkanDescriptorPool.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>
//
// Created by deril on 3/22/26.
//
VULKAN_NS
    VulkanDescriptorSet VulkanDescriptorSet::Create(const VulkanDevice &device, VulkanDescriptorPoolRef pool, VkDescriptorSetLayout layout) {
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = *pool;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &layout;

        VkDescriptorSet set;
        VK_CHECK(vkAllocateDescriptorSets(device, &allocateInfo, &set), "Cannot allocate descriptor set");
        return {set, layout};
    }

    VkDescriptorSetLayout VulkanDescriptorSet::getLayouts() const {
        return mLayout;
    }
NS_END
